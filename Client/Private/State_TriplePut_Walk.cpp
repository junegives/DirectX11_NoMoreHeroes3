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
	// �ǰ� ����
	if (dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->IsHit())
	{
		dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->SetHit(false);
		if (!dynamic_pointer_cast<CMonster_TriplePut>(m_pOwnerObject.lock())->SetDamage())
			return STATE::STATE_TRIPLEPUT_HIT;
	}

	shared_ptr<CTransform> pPlayerTransform = dynamic_pointer_cast<CTransform>(m_pGameInstance->Find_Component(LEVEL_STATIC, LAYER_PLAYER, g_strTransformTag));

	// �÷��̾� ��ġ ã�� ������ �ش� ��ġ�� look ����
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
	// ���� ���� �� �� �� �������� ��ٷ�
	if (0.7 > m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
	{
		m_isAnimSet = false;
		return STATE::STATE_TRIPLEPUT_WALK;
	}

	// 1. �÷��̾�� �Ÿ� ����ؼ�
	_float fDist = dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->GetDistanceToPlayer();

	// ���� 3�� �پ����� ���� ������� �Ѿ �� ����
	if (2 <= m_iWalkTimes)
	{
		// ���� ���� �� �� �� ���Ұ�, ������ 3�� �̻� ���� �� ��μ� �ٸ� ���·� �Ѿ��.

		// 2-1. ���� �Ÿ� ���ϸ� spin attack
		if (6.f >= fDist)
		{
			return STATE::STATE_TRIPLEPUT_SPIN;
		}
		// 2-2. ���� �Ÿ� �̻��̸� rush attack
		if (15.f >= fDist && 6.f <= fDist)
		{
			return STATE::STATE_TRIPLEPUT_RUSH;
		}
	}

	// ���� Ƚ�� ī��Ʈ�ϰ�
	m_iWalkTimes++;

	// ���� ���� ���� �����Ұ��� ���ϰ� ��� ���� �����սô�.
	if (m_isAnimSet)
		return STATE::STATE_TRIPLEPUT_WALK;

	// 10���� �ָ� �ٰ�����
	if (15.f <= fDist)
		m_pOwnerModel.lock()->Set_Animation((_uint)STATE_TRIPLEPUT_WALK_F - (_uint)STATE_ARMOR_END - 1, false, 2.5f, 0.2f,  0);

	// 3���� �ָ� �¿� ������ �ϰ�
	else if (6.f <= fDist)
	{
		// �������� �� �� �������ֱ�
		m_isRandom = !m_isRandom;

		if (m_isRandom)
			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_TRIPLEPUT_WALK_L - (_uint)STATE_ARMOR_END - 1, false, 2.5f, 0.2f,  0);

		else
			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_TRIPLEPUT_WALK_R - (_uint)STATE_ARMOR_END - 1, false, 2.5f, 0.2f,  0);
	}
	// 3���� ������ �ڷ� ����
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
