#include "pch.h"
#include "UI.h"

#include "GameInstance.h"

CUI::CUI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI::CUI(const CUI& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI::Initialize(void* pArg)
{
	UIDESC* pUIDesc = (UIDESC*)pArg;

	m_UIDesc.fCX = pUIDesc->fCX;
	m_UIDesc.fCY = pUIDesc->fCY;
	m_UIDesc.fX = pUIDesc->fX;
	m_UIDesc.fY = pUIDesc->fY;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	m_pTransformCom->Set_Scale(m_UIDesc.fCX, m_UIDesc.fCY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.fX - g_iWinSizeX * 0.5f, -m_UIDesc.fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	return S_OK;
}

void CUI::Priority_Tick(_float fTimeDelta)
{
}

void CUI::Tick(_float fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;

	if (m_isMoving)
	{
		m_fMoveTime -= fTimeDelta;

		if (m_fMoveTime <= 0.f)
		{
			m_isMoving = false;

			return;
		}

		m_pTransformCom->MoveTo(m_vMovTarget, 0.1f, fTimeDelta);
	}
}

void CUI::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, shared_from_this())))
		return;
}

HRESULT CUI::Render()
{
	return S_OK;
}

_vector CUI::ChangeToWindowPos(_vector vPos)
{
	return XMVectorSet(vPos.m128_f32[0] + g_iWinSizeX * 0.5f, -vPos.m128_f32[1] + g_iWinSizeY * 0.5f, 0.f, 1.f);
}

void CUI::WorldToScreenPos(_vector vPos, _float fTop)
{

	_matrix matProj = m_pGameInstance->Get_Transform_Matrix(CPipeline::D3DTS_PROJ);


	_vector vViewPos = XMVector3TransformCoord(vPos, m_pGameInstance->Get_Transform_Matrix(CPipeline::D3DTS_VIEW));
	_vector vProjPos = XMVector3TransformCoord(vViewPos, m_pGameInstance->Get_Transform_Matrix(CPipeline::D3DTS_PROJ));

	_float fScreenX, fScreenY;

	fScreenX = (vProjPos.m128_f32[0] + 1.f) * g_iWinSizeX * 0.5f;
	fScreenY = (vProjPos.m128_f32[1] - 1.f) * g_iWinSizeY * -0.5f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(fScreenX - g_iWinSizeX * 0.5f, -fScreenY + g_iWinSizeY * 0.5f + fTop, 0.f, 1.f));
}

_vector CUI::ChangeToWorldPos(_vector vPos)
{
	return XMVectorSet(vPos.m128_f32[0] - g_iWinSizeX * 0.5f, -vPos.m128_f32[1] + g_iWinSizeY * 0.5f, 0.f, 1.f);
}

void CUI::MoveTo(_bool isDir, _fvector vTarget, _float fSpeed, _float fTime)
{
	m_isMoving = true;
	m_fMoveTime = fTime;
	m_isMovToDir = isDir;

	// 움직일 방향을 받아온 경우
	if (m_isMovToDir)
	{
		m_vMovTarget = ChangeToWorldPos(XMVector4Normalize(vTarget) * fSpeed);
		m_pTransformCom->SetSpeedPerSec(fSpeed);

		// vTarget 방향으로 이동 (현재 값에서 vTarget 더해줌)
	}
	
	// 움직일 목표 포지션을 받아온 경우
	else
	{
		m_vMovTarget = ChangeToWorldPos(vTarget);

		_float fDist = XMVectorGetX(XMVector4Length(m_vMovTarget - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
		fSpeed = fDist / fTime;
		m_pTransformCom->SetSpeedPerSec(fSpeed);
	}
}

void CUI::MoveTo(_fvector vPos)
{

}

void CUI::CalcCorner()
{
	_vector vWinPos = ChangeToWindowPos(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_fLT = { vWinPos.m128_f32[0] - m_UIDesc.fCX * 0.5f, vWinPos.m128_f32[1] - m_UIDesc.fCY * 0.5f };
	m_fRT = { vWinPos.m128_f32[0] + m_UIDesc.fCX * 0.5f, vWinPos.m128_f32[1] - m_UIDesc.fCY * 0.5f };
	m_fLB = { vWinPos.m128_f32[0] - m_UIDesc.fCX * 0.5f, vWinPos.m128_f32[1] + m_UIDesc.fCY * 0.5f };
	m_fRB = { vWinPos.m128_f32[0] + m_UIDesc.fCX * 0.5f, vWinPos.m128_f32[1] + m_UIDesc.fCY * 0.5f };
}

void CUI::CalcCorner(_vector vPos, _float3 fSize)
{
	_vector vWinPos = ChangeToWindowPos(vPos);

	m_fLT = { vWinPos.m128_f32[0] - fSize.x * 0.5f, vWinPos.m128_f32[1] - fSize.y * 0.5f };
	m_fRT = { vWinPos.m128_f32[0] + fSize.x * 0.5f, vWinPos.m128_f32[1] - fSize.y * 0.5f };
	m_fLB = { vWinPos.m128_f32[0] - fSize.x * 0.5f, vWinPos.m128_f32[1] + fSize.y * 0.5f };
	m_fRB = { vWinPos.m128_f32[0] + fSize.x * 0.5f, vWinPos.m128_f32[1] + fSize.y * 0.5f };
}

HRESULT CUI::Add_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CUI> CUI::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
	shared_ptr<CUI> pInstance = make_shared<CUI>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CUI");
		pInstance.reset();
	}

	return pInstance;
}

void CUI::Free()
{
	__super::Free();
}
