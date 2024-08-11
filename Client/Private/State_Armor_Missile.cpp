#include "pch.h"
#include "State_Armor_Missile.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"

#include "Player.h"

CState_Armor_Missile::CState_Armor_Missile(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Armor_Missile::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Armor_Missile::Tick(_float fTimeDelta)
{
	m_fWaitTime += fTimeDelta;

	m_eState = Key_Input(fTimeDelta);

	return m_eState;

	//if (m_pOwnerModel.lock()->isAnimStop())
	//	return STATE::STATE_IDLE;

	//else
	//	return STATE::STATE_DODGE;
}

STATE CState_Armor_Missile::Late_Tick(_float fTimeDelta)
{

	return m_eState;
}

void CState_Armor_Missile::Reset_State()
{
}

void CState_Armor_Missile::Enter_State()
{
	dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation_TransModel((_uint)STATE_ARMOR_MISSILE_IN - ((_uint)STATE_PLAYER_END + 1), false, 3.0f, 0.1f, 0);
	
	m_eState = STATE::STATE_ARMOR_MISSILE_IN;
	m_fWaitTime = 0.f;
	m_isIn = true;
	m_isLoop = false;
	m_isOut = false;

	m_pOwnerCollider.lock()->SetOnCollide(false);
}

STATE CState_Armor_Missile::Key_Input(_float fTimeDelta)
{

	// In 끝나면 Loop로,
	// Loop에서 F키 누르면 Out으로,
	// n초동안 F키 안누르면 자동으로 Out으로


	if (m_isIn)
	{
		if (m_pOwnerTransModel.lock()->isAnimStop())
		{
			m_isLoop = true;
			m_isIn = false;
			m_isOut = false;
			dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation_TransModel((_uint)STATE_ARMOR_MISSILE_LOOP - ((_uint)STATE_PLAYER_END + 1), true, 3.0f, 0.1f, 0);
		}

		return STATE::STATE_ARMOR_MISSILE_IN;
	}

	else if (m_isLoop)
	{
		if (m_pGameInstance->Is_KeyDown(DIK_F) || 5.f <= m_fWaitTime)
		{
			m_isIn = false;
			m_isLoop = false;
			m_isOut = true;
			dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation_TransModel((_uint)STATE_ARMOR_MISSILE_OUT - ((_uint)STATE_PLAYER_END + 1), false, 2.0f, 0.1f, 0);
		}

		return STATE::STATE_ARMOR_MISSILE_IN;
	}

	else if (m_isOut)
	{
		if (m_pOwnerTransModel.lock()->isAnimStop())
		{
			m_pOwnerCollider.lock()->SetOnCollide(true);

			m_isLoop = false;
			m_isIn = false;
			m_isOut = false;
			dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation_TransModel((_uint)STATE_ARMOR_IDLE - ((_uint)STATE_PLAYER_END + 1), false, 3.0f, 0.1f, 0);
			dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->ChangeModel(false);

			return STATE::STATE_IDLE;
		}
	}

	return STATE::STATE_ARMOR_MISSILE_IN;
}

shared_ptr<CState> CState_Armor_Missile::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Armor_Missile> pInstance = make_shared<CState_Armor_Missile>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Armor_Missile");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Armor_Missile::Free()
{
}
