#include "pch.h"
#include "State_Leopardon_BeamSword.h"

#include "GameObject.h"
#include "Transform.h"
#include "Monster_Leopardon.h"
#include "Player.h"


CState_Leopardon_BeamSword::CState_Leopardon_BeamSword(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Leopardon_BeamSword::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Leopardon_BeamSword::Tick(_float fTimeDelta)
{
	if (dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->IsHit())
	{
		dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->SetHit(false);
		m_pOwnerCollider.lock()->SetOnCollide(true);
		if (!dynamic_pointer_cast<CMonster_Leopardon>(m_pOwnerObject.lock())->SetDamage())
			return STATE::STATE_LEOPARDON_HIT;
	}

	m_eState = Check_State();

	return m_eState;
}

STATE CState_Leopardon_BeamSword::Late_Tick(_float fTimeDelta)
{
	return m_eState;
}

void CState_Leopardon_BeamSword::Reset_State()
{
}

void CState_Leopardon_BeamSword::Enter_State()
{
	m_pOwnerModel.lock()->Set_Animation((_uint)STATE_LEOPARDON_BEAMSWORD - (_uint)STATE_TRIPLEPUT_END - 1, false, 3.0f, 0.2f,  0);
	m_eState = STATE_LEOPARDON_BEAMSWORD;

	m_isSetSuperDodgeTime = false;
	m_isStartAttack = false;
	m_isEndAttack = false;
}

STATE CState_Leopardon_BeamSword::Check_State()
{
	if (m_isEndAttack && m_pOwnerModel.lock()->isAnimStop())
		return STATE::STATE_LEOPARDON_IDLE;

	else if (!m_isEndAttack && 0.6 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
	{
		m_isEndAttack = true;

		m_pOwnerAttackCollider.lock()->SetActive(false);
		m_pOwnerAttackCollider.lock()->SetOnCollide(false);
	}

	else if (!m_isStartAttack && 0.35 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
	{
		dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player())->SetOnPerfectDodge(false);

		m_isStartAttack = true;

		m_pOwnerAttackCollider.lock()->SetActive(true);
		m_pOwnerAttackCollider.lock()->SetOnCollide(true);
	}

	else if (!m_isSetSuperDodgeTime && 0.35 >= m_pOwnerModel.lock()->Get_CurKeyFrameRatio()
				&& 7.f >= dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->GetDistanceToPlayer())
	{
		m_isSetSuperDodgeTime = true;
		dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player())->SetOnPerfectDodge(true);
	}

	return STATE::STATE_LEOPARDON_BEAMSWORD;
}

shared_ptr<CState> CState_Leopardon_BeamSword::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Leopardon_BeamSword> pInstance = make_shared<CState_Leopardon_BeamSword>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Leopardon_BeamSword");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Leopardon_BeamSword::Free()
{
}
