#include "pch.h"
#include "State_Bone_Idle.h"

#include "GameObject.h"
#include "Transform.h"
#include "Monster_Bone.h"

CState_Bone_Idle::CState_Bone_Idle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Bone_Idle::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	m_fIdleTime = 0.f;

	return S_OK;
}

STATE CState_Bone_Idle::Tick(_float fTimeDelta)
{
	shared_ptr<CTransform> pPlayerTransform = dynamic_pointer_cast<CTransform>(m_pGameInstance->Find_Component(LEVEL_STATIC, LAYER_PLAYER, g_strTransformTag));

	m_fIdleTime += fTimeDelta;

	if (m_isFinding)
	{
		if (0.2 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
		{
			m_pOwnerTransform.lock()->LookAt_ForLandObject(m_pGameInstance->Get_Player()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));
		}

		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_iFindingCnt++;
			m_isFinding = false;
			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BONE_IDLE - (_uint)STATE_LEOPARDON_END - 1, true, 2.3f, 0.2f,  0);
		}
	}

	m_eState = Check_State();

	return m_eState;
}

STATE CState_Bone_Idle::Late_Tick(_float fTimeDelta)
{
	return m_eState;
}

void CState_Bone_Idle::Reset_State()
{
}

void CState_Bone_Idle::Enter_State()
{
	m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BONE_IDLE - (_uint)STATE_LEOPARDON_END - 1, true, 2.3f, 0.2f,  0);
	m_eState = STATE::STATE_BONE_IDLE;

	m_isFinding = false;
	m_iFindingCnt = 0;

	m_fIdleWaitTime = 1.5f;
}

STATE CState_Bone_Idle::Check_State()
{
	// 피격 판정
	if (dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->IsHit())
	{
		dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->SetHit(false);
		return STATE::STATE_BONE_HIT_LIGHT;
	}

	if (m_fIdleWaitTime > m_fIdleTime)
		return STATE::STATE_BONE_IDLE;

	m_fIdleWaitTime = 1.f + (_float)(rand() % 50) / 100.f;

	m_fIdleTime = 0.f;

	//m_iRandomSkill = 0;

	// 1. 플레이어와 거리 계산
	_float fDist = dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->GetDistanceToPlayer();

	// 거리가 3보다 가까우면 Attack
	if (3 >= fDist)
	{
		return STATE::STATE_BONE_ATTACK;
	}

	// 거리가 17보다 가까운 경우 다가오기
	else if (17 >= fDist)
	{
		return STATE::STATE_BONE_WALK;
	}

	// 거리가 15보다 먼 경우, Find 모션 실행 (애니메이션만 실행)
	else if (17 <= fDist && !m_isFinding && 1 > m_iFindingCnt)
	{
		m_isFinding = true;
		m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BONE_IDLE_FIND2 - (_uint)STATE_LEOPARDON_END - 1, false, 2.3f, 0.2f,  0);
		return STATE::STATE_BONE_IDLE;
	}

	else if (0 < m_iFindingCnt)
	{
		return STATE::STATE_BONE_WALK;
	}

	return STATE::STATE_BONE_IDLE;
}

shared_ptr<CState> CState_Bone_Idle::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Bone_Idle> pInstance = make_shared<CState_Bone_Idle>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Bone_Idle");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Bone_Idle::Free()
{
}
