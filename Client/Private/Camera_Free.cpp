#include "pch.h"
#include "Camera_Free.h"

CCamera_Free::CCamera_Free(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CCamera(pDevice, pContext)
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& rhs)
    : CCamera(rhs)
{
}

HRESULT CCamera_Free::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    CAMERA_FREE_DESC* pCameraDesc = (CAMERA_FREE_DESC*)pArg;

    m_fMouseSensor = pCameraDesc->fMouseSensor;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

	//_float4 InitPos = { 0, 66, 146, 1 };

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&InitPos));

    return S_OK;
}

void CCamera_Free::Priority_Tick(_float fTimeDelta)
{
	// 카메라 잠금
	{
		if (!m_bPressed && GetKeyState('Q') < 0)
		{
			m_bPressed = true;
			m_bCameraLock = !m_bCameraLock;
		}
		else if (GetKeyState('Q') >= 0 && m_bPressed)
		{
			m_bPressed = false;
		}
	}

	if (m_bCameraLock)
	{
		// 상하좌우 이동
		if (GetKeyState('W') & 0x8000)
		{
			m_pTransformCom->Go_StraightXZ(fTimeDelta);
		}

		if (GetKeyState('S') & 0x8000)
		{
			m_pTransformCom->Go_BackwardXZ(fTimeDelta);
		}
	}

	else
	{
		// 상하좌우 이동
		if (GetKeyState('W') & 0x8000)
		{
			m_pTransformCom->Go_Straight(fTimeDelta);
		}

		if (GetKeyState('S') & 0x8000)
		{
			m_pTransformCom->Go_Backward(fTimeDelta);
		}

		// look 변경
		_long MouseMove = { 0 };

		if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);
		}

		if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * m_fMouseSensor);
		}
	}


	if (GetKeyState('A') & 0x8000)
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}

	if (GetKeyState('D') & 0x8000)
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}

	if (GetAsyncKeyState('F') & 0x8000)
	{
		m_bCameraLock = !m_bCameraLock;
	}

	if (FAILED(SetUp_TransformMatrices()))
		return;
}

void CCamera_Free::Tick(_float fTimeDelta)
{
}

void CCamera_Free::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCamera_Free::Render()
{
    return S_OK;
}

shared_ptr<CCamera_Free> CCamera_Free::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
	shared_ptr<CCamera_Free> pInstance = make_shared<CCamera_Free>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CCamera_Free");
		pInstance.reset();
	}

	return pInstance;
}

void CCamera_Free::Free()
{
}
