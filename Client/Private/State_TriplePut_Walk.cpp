#include "pch.h"
#include "State_TriplePut_Walk.h"

#include "GameObject.h"
#include "Transform.h"
#include "Monster_TriplePut.h"

CState_TriplePut_Walk::CState_TriplePut_Walk(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_TriplePut_Walk::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_TriplePut_Walk::Tick(_float fTimeDelta)
{
	// 피격 판정
	if (dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->IsHit())
	{
		dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->SetHit(false);
		if (!dynamic_pointer_cast<CMonster_TriplePut>(m_pOwnerObject.lock())->SetDamage())
			return STATE::STATE_TRIPLEPUT_HIT;
	}

	shared_ptr<CTransform> pPlayerTransform = dynamic_pointer_cast<CTransform>(m_pGameInstance->Find_Component(LEVEL_STATIC, LAYER_PLAYER, g_strTransformTag));

	// 플레이어 위치 찾은 다음에 해당 위치로 look 변경
	m_pOwnerTransform.lock()->LookAt_ForLandObject(pPlayerTransform->Get_State(CTransform::STATE_POSITION));

	m_eState = Check_State();

	return m_eState;
}

STATE CState_TriplePut_Walk::Late_Tick(_float fTimeDelta)
{
	return m_eState;
}

void CState_TriplePut_Walk::Reset_State()
{
	dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->SetYChange(false);
}

void CState_TriplePut_Walk::Enter_State()
{
	m_pOwnerModel.lock()->Set_Animation((_uint)STATE_TRIPLEPUT_WALK - (_uint)STATE_ARMOR_END - 1, false, 2.5f, 0.2f,  0);
	m_eState = STATE::STATE_TRIPLEPUT_WALK;
	dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->SetYChange(true);
	m_iWalkTimes = 0;
}

STATE CState_TriplePut_Walk::Check_State()
{
	// 점프 동작 한 번 다 돌때까지 기다려
	if (0.7 > m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
	{
		m_isAnimSet = false;
		return STATE::STATE_TRIPLEPUT_WALK;
	}

	// 1. 플레이어와 거리 계산해서
	_float fDist = dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->GetDistanceToPlayer();

	// 점프 3번 뛰었으면 다음 모션으로 넘어갈 수 있음
	if (2 <= m_iWalkTimes)
	{
		// 점프 동작 한 번 다 돌았고, 점프도 3번 이상 했을 때 비로소 다른 상태로 넘어간다.

		// 2-1. 일정 거리 이하면 spin attack
		if (6.f >= fDist)
		{
			return STATE::STATE_TRIPLEPUT_SPIN;
		}
		// 2-2. 일정 거리 이상이면 rush attack
		if (15.f >= fDist && 6.f <= fDist)
		{
			return STATE::STATE_TRIPLEPUT_RUSH;
		}
	}

	// 점프 횟수 카운트하고
	m_iWalkTimes++;

	// 다음 점프 무슨 점프할건지 정하고 계속 점프 진행합시다.
	if (m_isAnimSet)
		return STATE::STATE_TRIPLEPUT_WALK;

	// 10보다 멀면 다가오게
	if (15.f <= fDist)
		m_pOwnerModel.lock()->Set_Animation((_uint)STATE_TRIPLEPUT_WALK_F - (_uint)STATE_ARMOR_END - 1, false, 2.5f, 0.2f,  0);

	// 3보다 멀면 좌우 점프만 하게
	else if (6.f <= fDist)
	{
		// 랜덤으로 좌 우 결정해주기
		m_isRandom = !m_isRandom;

		if (m_isRandom)
			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_TRIPLEPUT_WALK_L - (_uint)STATE_ARMOR_END - 1, false, 2.5f, 0.2f,  0);

		else
			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_TRIPLEPUT_WALK_R - (_uint)STATE_ARMOR_END - 1, false, 2.5f, 0.2f,  0);
	}
	// 3보다 가까우면 뒤로 점프
	else
	{
		m_pOwnerModel.lock()->Set_Animation((_uint)STATE_TRIPLEPUT_WALK - (_uint)STATE_ARMOR_END - 1, false, 2.5f, 0.2f,  0);
	}

	m_isAnimSet = true;

	return STATE::STATE_TRIPLEPUT_WALK;
}

shared_ptr<CState> CState_TriplePut_Walk::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_TriplePut_Walk> pInstance = make_shared<CState_TriplePut_Walk>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_TriplePut_Walk");
		pInstance.reset();
	}

	return pInstance;
}

void CState_TriplePut_Walk::Free()
{
}
