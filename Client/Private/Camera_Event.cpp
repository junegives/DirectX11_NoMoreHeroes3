#include "pch.h"
#include "Camera_Event.h"

CCamera_Event::CCamera_Event(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Event::CCamera_Event(const CCamera_Event& rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Event::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	CAMERA_DESC* pCameraDesc = (CAMERA_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_isZoom = false;
	m_isMove = false;
	m_isRotation = false;
	m_isLook = false;
	m_isAttach = false;

	m_fPlayTime = 0.f;
	
	m_vTargetPos = { 0.f, 0.f, 0.f, 1.f };
	m_vTargetLook = { 0.f, 0.f, 0.f, 0.f };
	m_fTargetZoom = 1.f;
	m_fTargetRotation = XMConvertToRadians(360);

	m_matPivot = XMMatrixIdentity();

	return S_OK;
}

void CCamera_Event::Priority_Tick(_float fTimeDelta)
{
	if (m_isAttach)
	{
		if (m_pGameInstance->Is_KeyPressing(DIK_RIGHT))
		{
			m_vLocalTrans.m128_f32[2] += 0.1f;
		}

		if (m_pGameInstance->Is_KeyPressing(DIK_LEFT))
		{
			m_vLocalTrans.m128_f32[2] -= 0.1f;
		}

		if (m_pGameInstance->Is_KeyPressing(DIK_UP))
		{
			m_vLocalTrans.m128_f32[0] += 0.1f;
		}

		if (m_pGameInstance->Is_KeyPressing(DIK_DOWN))
		{
			m_vLocalTrans.m128_f32[0] -= 0.1f;
		}

		if (m_pGameInstance->Is_KeyPressing(DIK_PGUP))
		{
			m_vLocalTrans.m128_f32[1] += 0.1f;
		}

		if (m_pGameInstance->Is_KeyPressing(DIK_PGDN))
		{
			m_vLocalTrans.m128_f32[1] -= 0.1f;
		}

		if (m_pGameInstance->Is_KeyPressing(DIK_J))
		{
			m_iX += 10;
			m_matPivot *= XMMatrixRotationX(XMConvertToRadians(10));
		}

		if (m_pGameInstance->Is_KeyPressing(DIK_K))
		{
			m_iY += 10;
			m_matPivot *= XMMatrixRotationY(XMConvertToRadians(10));
		}

		if (m_pGameInstance->Is_KeyPressing(DIK_L))
		{
			m_iZ += 10;
			m_matPivot *= XMMatrixRotationZ(XMConvertToRadians(10));
		}

		_float4x4	matParentWorld;

		_matrix matBone = XMLoadFloat4x4(m_matAttach) * m_matPivot;
		matBone.r[3] += m_vLocalTrans;
		_matrix matParent = m_pAttachObject->Get_TransformCom()->Get_WorldMatrix();

		XMStoreFloat4x4(&matParentWorld, matBone * m_pAttachObject->Get_TransformCom()->Get_WorldMatrix());

		m_pTransformCom->Set_WorldMatrix(matParentWorld);
	}

	if (m_isMove)
	{
		m_fMoveTime -= fTimeDelta;

		m_pTransformCom->MoveTo(m_vTargetPos, 1.f, fTimeDelta);

		if (0 >= m_fMoveTime)
			m_isMove = false;
	}

	if (m_isRotation)
	{
		m_fRotationTime -= fTimeDelta;



		if (0 >= m_fRotationTime)
			m_isRotation = false;
	}

	if (m_isZoom)
	{
		m_fZoomTime -= fTimeDelta;



		if (0 >= m_fZoomTime)
			m_isRotation = false;
	}

	if (!m_isAttach && !m_isMove && !m_isRotation && !m_isZoom)
		m_pGameInstance->SetMainCam(CCamera_Manager::CAM_ATTACH);

	if (FAILED(SetUp_TransformMatrices()))
		return;
}

void CCamera_Event::Tick(_float fTimeDelta)
{
}

void CCamera_Event::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCamera_Event::Render()
{
	return S_OK;
}

void CCamera_Event::SettingBefore(_vector vPos, _vector vLook)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->SetLook(vLook);
}

void CCamera_Event::SettingBefore(shared_ptr<CGameObject> pAttachObject, _float4x4* matBone, _matrix matPivot, _vector vLocalTrans)
{
	m_isAttach = true;

	m_pAttachObject = pAttachObject;

	m_matAttach = matBone;
	m_vLocalTrans = vLocalTrans;
	m_matPivot = matPivot;

	_float4x4	matParentWorld;

	XMStoreFloat4x4(&matParentWorld, XMLoadFloat4x4(m_matAttach) * m_pAttachObject->Get_TransformCom()->Get_WorldMatrix());

	m_pTransformCom->Set_WorldMatrix(matParentWorld);
}

void CCamera_Event::CamZoom(_float fPlayTime, _float fSpeed, _float fZoomAmount)
{
	m_isZoom = true;
	m_fZoomTime = fPlayTime;

	m_fTargetZoom = fZoomAmount;
}

void CCamera_Event::CamMove(_float fPlayTime, _float fSpeed, _vector vTargetPos)
{
	m_isMove = true;
	m_fMoveSpeed = fSpeed;

	m_pTransformCom->SetSpeedPerSec(m_fMoveSpeed);

	m_fMoveTime = fPlayTime;

	m_vTargetPos = vTargetPos;
}

void CCamera_Event::CamRotation(_float fPlayTime, _float fSpeed, _float fRadian)
{
	m_isRotation = true;
	m_fRotationSpeed = fSpeed;
	m_fRotationTime = fPlayTime;

	m_fTargetRotation = fRadian;
}

void CCamera_Event::CamLook(_float fPlayTime, _float fSpeed, _vector vTargetLook)
{
	m_isLook = true;
	m_fPlayTime = fPlayTime;

	m_vTargetLook = vTargetLook;
}

shared_ptr<CCamera_Event> CCamera_Event::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
	shared_ptr<CCamera_Event> pInstance = make_shared<CCamera_Event>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CCamera_Event");
		pInstance.reset();
	}

	return pInstance;
}

void CCamera_Event::Free()
{
}
