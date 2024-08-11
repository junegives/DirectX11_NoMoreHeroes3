#include "pch.h"
#include "State_Vehicle_Run.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"

#include "Bike.h"
#include "Player.h"
#include "Camera_Attach.h"

CState_Vehicle_Run::CState_Vehicle_Run(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Vehicle_Run::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Vehicle_Run::Tick(_float fTimeDelta)
{
	if (!m_pBike.lock()->IsRun())
	{
		return STATE_VEHICLE_IDLE;
	}

	if (dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->IsJump())
		return STATE_VEHICLE_FALL;

	if (m_isIn)
	{
		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_isIn = false;
			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_VEHICLE_RUN, true, 3.f, 0.0f, 0);
		}
	}

	//// 내리는 모션
	//if (m_isOut)
	//{
	//	if (0.0 == m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
	//		m_pBike.lock()->Set_Attached(false);

	//	if (m_pOwnerModel.lock()->isAnimStop())
	//	{
	//		m_isOut = false;
	//		dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetAnimLerp(0.f);
	//		dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetAnimLerpChange(true);
	//		dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetNextTickTurn({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(90));
	//		return STATE::STATE_IDLE;
	//	}

	//	return STATE::STATE_VEHICLE_IDLE;
	//}

	//if (!m_isOut)
		//m_eState = Key_Input(fTimeDelta);

	return m_eState;
}

STATE CState_Vehicle_Run::Late_Tick(_float fTimeDelta)
{
	return m_eState;
}

void CState_Vehicle_Run::Reset_State()
{
}

void CState_Vehicle_Run::Enter_State()
{
	if (!m_pBike.lock())
		m_pBike = dynamic_pointer_cast<CBike>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, LAYER_VEHICLE, TEXT("GameObject_Vehicle_Bike")));

	if (STATE::STATE_VEHICLE_FALL == m_pOwnerMachine.lock()->Get_PreState())
	{
		m_isIn = false;
		m_isOut = false;
		m_isTurned = false;
		m_pOwnerModel.lock()->Set_Animation((_uint)STATE_VEHICLE_RUN, true, 3.f, 0.3f, 0);

		m_eState = STATE::STATE_VEHICLE_RUN;
	}

	else
	{
		m_isIn = true;
		m_isOut = false;
		m_isTurned = false;
		dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_VEHICLE_RUN_START, false, 3.0f, 0.2f, 0);

		m_eState = STATE::STATE_VEHICLE_RUN;
	}

}

STATE CState_Vehicle_Run::Key_Input(_float fTimeDelta)
{
	if (m_pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		m_isOut = true;
		m_isIn = false;
		dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_VEHICLE_OUT_R, false, 3.0f, 0.2f,  0);
		return STATE_VEHICLE_IDLE;
	}

	return STATE::STATE_VEHICLE_IDLE;
}

shared_ptr<CState> CState_Vehicle_Run::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Vehicle_Run> pInstance = make_shared<CState_Vehicle_Run>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Vehicle_Run");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Vehicle_Run::Free()
{
}
