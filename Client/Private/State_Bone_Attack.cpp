#include "pch.h"
#include "State_Bone_Attack.h"

#include "GameObject.h"
#include "Transform.h"
#include "Monster_Bone.h"
#include "Player.h"


CState_Bone_Attack::CState_Bone_Attack(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Bone_Attack::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Bone_Attack::Tick(_float fTimeDelta)
{
	// 피격 판정
	if (dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->IsHit())
	{
		dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->SetHit(false);
		return STATE::STATE_BONE_HIT_LIGHT;
	}

	m_eState = Check_State();

	return m_eState;
}

STATE CState_Bone_Attack::Late_Tick(_float fTimeDelta)
{
	return m_eState;
}

void CState_Bone_Attack::Reset_State()
{
}

void CState_Bone_Attack::Enter_State()
{
	m_iRandAttack = 1 - m_iRandAttack;
	m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BONE_ATTACK - (_uint)STATE_LEOPARDON_END - 1, false, 2.0f, 0.2f,  0);
	m_eState = STATE_BONE_ATTACK;

	m_isSetSuperDodgeTime = false;
	m_isStartAttack = false;
	m_isEndAttack = false;
}

STATE CState_Bone_Attack::Check_State()
{
	if (m_isEndAttack && m_pOwnerModel.lock()->isAnimStop())
		return STATE::STATE_BONE_IDLE;

	else if (!m_isEndAttack && 140 <= m_pOwnerModel.lock()->Get_CurKeyFrameTime())
	{
		m_isEndAttack = true;

		m_pOwnerAttackCollider.lock()->SetActive(false);
		m_pOwnerAttackCollider.lock()->SetOnCollide(false);
	}

	else if (!m_isStartAttack && 125 <= m_pOwnerModel.lock()->Get_CurKeyFrameTime())
	{
		dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player())->SetOnPerfectDodge(false);

		m_isStartAttack = true;

		m_pOwnerAttackCollider.lock()->SetActive(true);
		m_pOwnerAttackCollider.lock()->SetOnCollide(true);
	}

	else if (!m_isSetSuperDodgeTime && 100 <= m_pOwnerModel.lock()->Get_CurKeyFrameTime()
				&& 5.f >= dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->GetDistanceToPlayer())
	{
		m_isSetSuperDodgeTime = true;
		dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player())->SetOnPerfectDodge(true);
	}

	return STATE::STATE_BONE_ATTACK;
}

shared_ptr<CState> CState_Bone_Attack::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Bone_Attack> pInstance = make_shared<CState_Bone_Attack>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Bone_Attack");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Bone_Attack::Free()
{
}
