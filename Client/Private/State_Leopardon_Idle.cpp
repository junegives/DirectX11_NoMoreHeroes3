#include "pch.h"
#include "State_Leopardon_Idle.h"

#include "GameObject.h"
#include "Transform.h"
#include "Monster_Leopardon.h"

CState_Leopardon_Idle::CState_Leopardon_Idle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Leopardon_Idle::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	m_fIdleTime = 0.f;

	return S_OK;
}

STATE CState_Leopardon_Idle::Tick(_float fTimeDelta)
{
	shared_ptr<CTransform> pPlayerTransform = dynamic_pointer_cast<CTransform>(m_pGameInstance->Find_Component(LEVEL_STATIC, LAYER_PLAYER, g_strTransformTag));

	// 플레이어 위치 찾은 다음에 해당 위치로 look 변경
	m_pOwnerTransform.lock()->LookAt_ForLandObject(pPlayerTransform->Get_State(CTransform::STATE_POSITION));

	m_fIdleTime += fTimeDelta;
	//m_iRandomSkill = 1;

	m_eState = Check_State();

	return m_eState;
}

STATE CState_Leopardon_Idle::Late_Tick(_float fTimeDelta)
{
	return m_eState;
}

void CState_Leopardon_Idle::Reset_State()
{
	dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->SetYChange(false);
}

void CState_Leopardon_Idle::Enter_State()
{
	m_pOwnerModel.lock()->Set_Animation((_uint)STATE_LEOPARDON_IDLE - (_uint)STATE_TRIPLEPUT_END - 1, true, 2.3f, 0.2f,  0);
	m_eState = STATE::STATE_LEOPARDON_IDLE;

	m_fIdleWaitTime = 0.7f;
}

STATE CState_Leopardon_Idle::Check_State()
{
	// 피격 판정
	if (dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->IsHit())
	{
		dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->SetHit(false);
		return STATE::STATE_LEOPARDON_HIT;
	}

	// 1. 플레이어와 거리 계산
	_float fDist = dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->GetDistanceToPlayer();

	if (3.f < fDist && m_fIdleWaitTime > m_fIdleTime)
		return STATE::STATE_LEOPARDON_IDLE;

	m_fIdleTime = 0.f;

	// 랜덤 스킬 번호
	m_iRandomSkill++;

	//m_iRandomSkill = 0;

	if (10.f < fDist)
	{
		// 거리가 10보다 먼 경우, 워프 이동과 레이저 검은 사용 못한다.
		if (m_iRandomSkill >= 4)
			m_iRandomSkill = 1;
	}
	else if (5.f < fDist)
	{
		// 거리가 5보다 먼 경우, 워프 이동은 사용 못한다.
		if (m_iRandomSkill >= 5)
			m_iRandomSkill = 1;
	}
	else
	{
		if (m_iRandomSkill >= 6)
			m_iRandomSkill = 5;
	}

	/*if (m_iRandomSkill == 1)
		m_iRandomSkill = 2;

	if (m_iRandomSkill > 2)
		m_iRandomSkill = 0;*/

	//// 워프 개수 체크해서 워프 생성
	if (m_iRandomSkill % 2 == 0 && 2 > dynamic_pointer_cast<CMonster_Leopardon>(m_pOwnerObject.lock())->GetNumWarp())
		return STATE::STATE_LEOPARDON_OCCURRENCE;

	// 근거리 스킬 (모두 다 쓴다)
	// 훌라후프, 레이저 검, 워프 이동, 레이저 빔, 큐브 벽

	switch (m_iRandomSkill)
	{
	case 0:
		// 레이저 빔
		return STATE::STATE_LEOPARDON_LASERBEAM;
		break;

	case 1:
		// 큐브 벽
		return STATE::STATE_LEOPARDON_SQUAREWALL;
		break;

	case 2:
		// 훌라후프
		return STATE::STATE_LEOPARDON_SPREADRING;
		break;

	case 3:
		// 훌라후프
		return STATE::STATE_LEOPARDON_SPREADRING;
		break;

	case 4:
		//// 레이저 검
		//return STATE::STATE_LEOPARDON_BEAMSWORD;
		// 워프 이동
		if (0 < dynamic_pointer_cast<CMonster_Leopardon>(m_pOwnerObject.lock())->GetNumWarp())
			return STATE::STATE_LEOPARDON_WARP;
		break;

	case 5:
		// 워프 이동
		if (0 < dynamic_pointer_cast<CMonster_Leopardon>(m_pOwnerObject.lock())->GetNumWarp())
			return STATE::STATE_LEOPARDON_WARP;
		break;

	default:
		return STATE::STATE_LEOPARDON_IDLE;
		break;
	}

	return STATE::STATE_LEOPARDON_IDLE;
}

shared_ptr<CState> CState_Leopardon_Idle::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Leopardon_Idle> pInstance = make_shared<CState_Leopardon_Idle>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Leopardon_Idle");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Leopardon_Idle::Free()
{
}
