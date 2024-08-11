#include "pch.h"
#include "State_Bike_Nitro.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"

#include "Bike.h"
#include "Player.h"
#include "Camera_Attach.h"

CState_Bike_Nitro::CState_Bike_Nitro(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Bike_Nitro::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Bike_Nitro::Tick(_float fTimeDelta)
{
	if (m_isIn)
	{
		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_pOwnerTransform.lock()->ChangeSpeedPerSec(10.f);
			m_isIn = false;
			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_NITRO - (_uint)STATE_BONE_END - 1, true, 3.0f, 0.0f, 0);
		}
	}

	if (m_isOut)
	{
		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_pOwnerTransform.lock()->ChangeSpeedPerSec(-10.f);
			m_isOut = false;
			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_RUN - (_uint)STATE_BONE_END - 1, true, 2.0f, 0.0f, 0);
			return STATE::STATE_BIKE_RUN;
		}
	}

	m_eState = Key_Input(fTimeDelta);

	return m_eState;
}

STATE CState_Bike_Nitro::Late_Tick(_float fTimeDelta)
{

	return m_eState;
}

void CState_Bike_Nitro::Reset_State()
{
	int a = 3;
}

void CState_Bike_Nitro::Enter_State()
{
	m_eState = STATE::STATE_BIKE_NITRO;

	m_isOut = false;

	if (STATE::STATE_BIKE_RUN == m_pOwnerMachine.lock()->Get_PreState())
	{
		m_isIn = true;
		m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_NITRO_IN - (_uint)STATE_BONE_END - 1, false, 3.0f, 0.0f, 0);
	}

	else
	{
		m_isIn = false;
		m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_NITRO - (_uint)STATE_BONE_END - 1, true, 3.0f, 0.1f, 0);
	}
}

STATE CState_Bike_Nitro::Key_Input(_float fTimeDelta)
{
	// 부스터 유지
	if (m_pGameInstance->Is_MousePressing(DIM_LB))
	{
		return STATE::STATE_BIKE_NITRO;
	}

	// 부스터 끔
	if (m_pGameInstance->Is_MouseUp(DIM_LB))
	{
		m_isOut = true;
		m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_NITRO_OUT - (_uint)STATE_BONE_END - 1, false, 5.0f, 0.0f, 0);

		return STATE::STATE_BIKE_NITRO;
	}

	return STATE::STATE_BIKE_NITRO;
}

shared_ptr<CState> CState_Bike_Nitro::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Bike_Nitro> pInstance = make_shared<CState_Bike_Nitro>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Bike_Nitro");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Bike_Nitro::Free()
{
}
