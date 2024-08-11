#include "pch.h"
#include "State_Vehicle_Idle.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"

#include "Bike.h"
#include "Player.h"
#include "Camera_Attach.h"

CState_Vehicle_Idle::CState_Vehicle_Idle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Vehicle_Idle::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Vehicle_Idle::Tick(_float fTimeDelta)
{
	if (m_pBike->IsRun())
	{
		return STATE_VEHICLE_RUN;
	}

	if (!dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->IsOnBike())
	{
		return STATE::STATE_IDLE;
	}

	/*if (dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->IsHit())
	{
		dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetHit(false);
		return STATE::STATE_HIT;
	}*/

	// 타는 모션
	if (m_isIn)
	{
		if (0.1 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio() && 0.34 >= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
		{
			m_pOwnerTransform.lock()->Set_State(CTransform::STATE_POSITION, (m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.05f, 0.f, 0.f)));
		}

		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_isIn = false;
			m_pBike->Set_Attached(true);
			dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_VEHICLE_IDLE, true, 2.0f, 0.0f, 0);
		}

		return STATE::STATE_VEHICLE_IDLE;
	}

	if (!m_isTurned)
	{
		m_isTurned = true;
		m_pOwnerTransform.lock()->TurnRadian({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(90));
	}

	// 내리는 모션
	if (m_isOut)
	{
		if (0.0 == m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
			m_pBike->Set_Attached(false);

		if (0.1 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio() && 0.34 >= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
		{
			m_pOwnerTransform.lock()->Set_State(CTransform::STATE_POSITION, (m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION) - XMVectorSet(0.f, 0.05f, 0.f, 0.f)));
		}

		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_isOut = false;
			dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetAnimLerp(0.f);
			dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetAnimLerpChange(true);
			dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetNextTickTurn({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(90));
			return STATE::STATE_IDLE;
		}

		return STATE::STATE_VEHICLE_IDLE;
	}


	if (!m_isIn && !m_isOut)
		m_eState = Key_Input(fTimeDelta);

	return m_eState;
}

STATE CState_Vehicle_Idle::Late_Tick(_float fTimeDelta)
{
	return m_eState;
}

void CState_Vehicle_Idle::Reset_State()
{
}

void CState_Vehicle_Idle::Enter_State()
{
	if (!m_pBike)
		m_pBike = dynamic_pointer_cast<CBike>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, LAYER_VEHICLE, TEXT("GameObject_Vehicle_Bike")));

	if (dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->IsOnBikeNow())
	{
		// 위치 보정
		m_isIn = true;
		m_isTurned = false;
		dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_VEHICLE_IN_R, false, 3.0f, 0.2f,  0);
		dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetOnBikeNow(false);
	}

	else
	{
		m_isIn = false;
		m_isTurned = true;
		m_pBike->Set_Attached(true);
		dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_VEHICLE_IDLE, true, 2.0f, 0.2f,  0);
	}

	m_eState = STATE::STATE_VEHICLE_IDLE;

	m_isOut = false;
}

STATE CState_Vehicle_Idle::Key_Input(_float fTimeDelta)
{
	if (m_pGameInstance->Get_DIMouseState(MOUSEKEYSTATE::DIM_LB) & 0x80)
	{
		return STATE_VEHICLE_RUN;
	}

	if ((m_pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80) && !m_pBike->IsOnMiniGame())
	{
		m_isOut = true;
		dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_VEHICLE_OUT_R, false, 3.0f, 0.2f,  0);
		return STATE_VEHICLE_IDLE;
	}

	return STATE::STATE_VEHICLE_IDLE;
}

shared_ptr<CState> CState_Vehicle_Idle::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Vehicle_Idle> pInstance = make_shared<CState_Vehicle_Idle>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Vehicle_Idle");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Vehicle_Idle::Free()
{
}
