#include "pch.h"
#include "Effect_Pillar.h"

CEffect_Pillar::CEffect_Pillar(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CEffect(pDevice, pContext)
{
}

CEffect_Pillar::CEffect_Pillar(const CEffect_Pillar& rhs)
	: CEffect(rhs)
{
}

CEffect_Pillar::~CEffect_Pillar()
{
}

HRESULT CEffect_Pillar::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

void CEffect_Pillar::Priority_Tick(_float fTimeDelta)
{
}

void CEffect_Pillar::Tick(_float fTimeDelta)
{
	m_vUVPivot.y += m_EffectDesc.fMoveSpeed;

	_float fDist = XMVectorGetX(XMVector4Length(m_pGameInstance->Get_Player()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

	if (fDist > 15.f)
		m_fAlpha = 1.f;

	else if (fDist >= 7.f && fDist <= 15.f)
	{
		m_fAlpha = (fDist - 5.f) / 10.f;
	}

	else
		m_fAlpha = 0.2f;

	m_fAlpha /= 1.5f;
}

void CEffect_Pillar::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, shared_from_this())))
		return;
}

HRESULT CEffect_Pillar::Render()
{
	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix;

	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrix());
	XMStoreFloat4x4(&ViewMatrix, m_pGameInstance->Get_Transform_Matrix(CPipeline::D3DTS_VIEW));
	XMStoreFloat4x4(&ProjMatrix, m_pGameInstance->Get_Transform_Matrix(CPipeline::D3DTS_PROJ));

	_float4		vCamPos = m_pGameInstance->Get_CamPosition();

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	if (m_EffectDesc.isUseMask)
		if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
			return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVPivot", &m_vUVPivot, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vFrontColor", &m_EffectDesc.vColor, sizeof(_float4))))
		return E_FAIL;

	_float4		vBackColor = { 1.f, 0.431f, 0.659f, 1.f };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vBackColor", &vBackColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_EffectDesc.iShaderPass)))
		return E_FAIL;

	if (FAILED(m_pModelCom->Render(0)))
		return E_FAIL;

	return S_OK;
}

void CEffect_Pillar::Play(_uint iNum, _vector vCenter, _vector vScale, _vector vUp, _vector vLook)
{
}

void CEffect_Pillar::Play(_uint iNum, _vector vCenter, _bool isRotate, _vector vAxis, _vector vRotate)
{
}

void CEffect_Pillar::Play(shared_ptr<CTransform> pParentTransform)
{
}

HRESULT CEffect_Pillar::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Pillar_Wave"),
		TEXT("Com_Texture"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pMaskTextureCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effect_Trigger_Pillar"),
		TEXT("Com_Model"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pModelCom))))
		return E_FAIL;
}

shared_ptr<CEffect_Pillar> CEffect_Pillar::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
	shared_ptr<CEffect_Pillar> pInstance = make_shared<CEffect_Pillar>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CEffect_Pillar");
		pInstance.reset();
	}

	return pInstance;
}

void CEffect_Pillar::Free()
{
	__super::Free();
}