#include "pch.h"
#include "State_TriplePut_Spin.h"

#include "GameObject.h"
#include "Transform.h"
#include "Monster_TriplePut.h"

#include "Player.h"

CState_TriplePut_Spin::CState_TriplePut_Spin(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_TriplePut_Spin::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_TriplePut_Spin::Tick(_float fTimeDelta)
{
	// 피격 판정
	if (dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->IsHit())
	{
		dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->SetHit(false);
		if (!dynamic_pointer_cast<CMonster_TriplePut>(m_pOwnerObject.lock())->SetDamage())
			return STATE::STATE_TRIPLEPUT_HIT;
	}

	m_eState = Check_State();

	return m_eState;
}

STATE CState_TriplePut_Spin::Late_Tick(_float fTimeDelta)
{
	return m_eState;
}

void CState_TriplePut_Spin::Reset_State()
{
}

void CState_TriplePut_Spin::Enter_State()
{
	m_pOwnerModel.lock()->Set_Animation((_uint)STATE_TRIPLEPUT_SPIN - (_uint)STATE_ARMOR_END - 1, false, 3.0f, 0.3f, 0);
	m_eState = STATE::STATE_TRIPLEPUT_SPIN;

	m_isSetSuperDodgeTime = false;
	m_isOnAttack = false;
	m_isEndAttack = false;
}

STATE CState_TriplePut_Spin::Check_State()
{
	// 공격 충돌 종료
	if (!m_isEndAttack && 0.55 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
	{
		m_isEndAttack = true;
		m_pOwnerAttackCollider.lock()->SetActive(false);
		m_pOwnerAttackCollider.lock()->SetOnCollide(false);
	}

	// 공격 충돌 시작
	else if (!m_isOnAttack && 0.33 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
	{
		dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player())->SetOnPerfectDodge(false);

		m_isOnAttack = true;
		m_pOwnerAttackCollider.lock()->SetActive(true);
		m_pOwnerAttackCollider.lock()->SetOnCollide(true);
	}

	// 플레이어 회피 타이밍
	else if (!m_isSetSuperDodgeTime && 0.33 >= m_pOwnerModel.lock()->Get_CurKeyFrameRatio()
		&& 6.f >= dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->GetDistanceToPlayer())
	{
		m_isSetSuperDodgeTime = true;
		dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player())->SetOnPerfectDodge(true);
	}

	if (!m_pOwnerModel.lock()->isAnimStop())
		return STATE::STATE_TRIPLEPUT_SPIN;

	return STATE::STATE_TRIPLEPUT_IDLE;
}

shared_ptr<CState> CState_TriplePut_Spin::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_TriplePut_Spin> pInstance = make_shared<CState_TriplePut_Spin>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_TriplePut_Spin");
		pInstance.reset();
	}

	return pInstance;
}

void CState_TriplePut_Spin::Free()
{
}
