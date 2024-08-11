#include "pch.h"
#include "State_DeathKick.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"
#include "Player.h"

CState_DeathKick::CState_DeathKick(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_DeathKick::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_DeathKick::Tick(_float fTimeDelta)
{
	m_eState = State_Check();

	return m_eState;
}

STATE CState_DeathKick::Late_Tick(_float fTimeDelta)
{

	return m_eState;
}

void CState_DeathKick::Reset_State()
{
	m_pOwnerAttackCollider.lock()->SetOnCollide(false);
}

void CState_DeathKick::Enter_State()
{
	m_isRandom = !m_isRandom;

	dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_DEATHKICK, false, 5.0f, 0.2f,  0);
	m_eState = STATE_DEATHKICK;

	// 몬스터 앞으로 위치 보정해주기
	m_pTarget = dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SearchTarget();

	m_pOwnerTransform.lock()->Set_State(CTransform::STATE_POSITION, m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION) + (XMVector4Normalize(m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_LOOK)) * 1.8f));
	m_pOwnerTransform.lock()->SetLook_ForLandObject(-m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_LOOK));

	m_isIn = true;
	m_isOut = false;

	m_pOwnerCollider.lock()->SetOnCollide(false);

	m_pOwnerAttackCollider.lock()->SetOnCollide(true);
}

STATE CState_DeathKick::Key_Input(_float fTimeDelta)
{
	return m_eState;
}

STATE CState_DeathKick::State_Check()
{
	if (m_isIn)
	{

		// 모션 끝나는지 체크하고 Core 모션으로 넘어가
		if (0.8 < m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
		{
			m_isOut = true;
			m_isIn = false;
			dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_DEATHKICK_OUT, true, 2.0f, 0.2f,  0);
			return STATE::STATE_DEATHKICK_OUT;
		}
	}

	else if (m_isOut)
	{
		// 모션 끝나는지 체크하고 Idle로 넘어가
		if (0.5 < m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
		{
			m_pOwnerCollider.lock()->SetOnCollide(true);
			return STATE::STATE_IDLE;
		}
	}

	return m_eState;
}

shared_ptr<CState> CState_DeathKick::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_DeathKick> pInstance = make_shared<CState_DeathKick>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Dodge");
		pInstance.reset();
	}

	return pInstance;
}

void CState_DeathKick::Free()
{
}
