#include "pch.h"
#include "State_TriplePut_Idle.h"

#include "GameObject.h"
#include "Transform.h"
#include "Monster_TriplePut.h"

CState_TriplePut_Idle::CState_TriplePut_Idle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_TriplePut_Idle::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_TriplePut_Idle::Tick(_float fTimeDelta)
{
	m_fIdleTime += fTimeDelta;

	m_eState = Check_State();

	return m_eState;
}

STATE CState_TriplePut_Idle::Late_Tick(_float fTimeDelta)
{
	return m_eState;
}

void CState_TriplePut_Idle::Reset_State()
{
}

void CState_TriplePut_Idle::Enter_State()
{
	m_pOwnerModel.lock()->Set_Animation((_uint)STATE_TRIPLEPUT_IDLE - (_uint)STATE_ARMOR_END - 1, true, 1.0f, 0.3f, 0);
	m_eState = STATE::STATE_TRIPLEPUT_IDLE;

	m_fIdleTime = 0.f;
}
STATE CState_TriplePut_Idle::Key_Input(_float fTimeDelta)
{
	return m_eState;
}


STATE CState_TriplePut_Idle::Check_State()
{
	// 피격 판정
	if (dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->IsHit())
	{
		return STATE::STATE_TRIPLEPUT_HIT;
		/*dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->SetHit(false);
		m_pOwnerCollider.lock()->SetOnCollide(true);*/
	}

	//// 1. 일정 피 이하면 invisible (m_isInvisible == false인 경우)
	//if (10 >= dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->GetHP()
	//	&& dynamic_pointer_cast<CMonster_TriplePut>(m_pOwnerObject.lock())->IsVisible())
	//	return STATE::STATE_TRIPLEPUT_INVISIBLE;

	// 2. Idle 1초동안 유지했으면 Walk로
	if (0.7f <= m_fIdleTime)
	{
		return STATE::STATE_TRIPLEPUT_WALK;
	}

	return STATE::STATE_TRIPLEPUT_IDLE;
}

shared_ptr<CState> CState_TriplePut_Idle::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_TriplePut_Idle> pInstance = make_shared<CState_TriplePut_Idle>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_TriplePut_Idle");
		pInstance.reset();
	}

	return pInstance;
}

void CState_TriplePut_Idle::Free()
{
}
