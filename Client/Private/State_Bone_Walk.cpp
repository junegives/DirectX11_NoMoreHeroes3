#include "pch.h"
#include "State_Bone_Walk.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"
#include "Monster_Bone.h"


#include "Player.h"

CState_Bone_Walk::CState_Bone_Walk(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Bone_Walk::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Bone_Walk::Tick(_float fTimeDelta)
{
	if (dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->IsHit())
	{
		dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->SetHit(false);
		return STATE::STATE_BONE_HIT_LIGHT;
	}

	m_eState = Check_State(fTimeDelta);

	return m_eState;
}

STATE CState_Bone_Walk::Late_Tick(_float fTimeDelta)
{

	return m_eState;
}

void CState_Bone_Walk::Reset_State()
{
}

void CState_Bone_Walk::Enter_State()
{

	// 1. �÷��̾�� �Ÿ� ���
	_float fDist = dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->GetDistanceToPlayer();

	// �Ÿ��� 7���� ����� ��� WALK (�ִϸ��̼�)
	if (7 > fDist)
	{
		m_isRun = false;
		m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BONE_WALK - (_uint)STATE_LEOPARDON_END - 1, true, 2.5f, 0.1f, 0);
	}

	// �Ÿ��� 7���� �� ���, RUN
	else if (7 <= fDist)
	{
		m_isRun = true;
		m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BONE_RUN - (_uint)STATE_LEOPARDON_END - 1, true, 2.5f, 0.1f, 0);
	}

	m_eState = STATE::STATE_BONE_WALK;
}

STATE CState_Bone_Walk::Check_State(_float fTimeDelta)
{
	// �÷��̾� ��ġ ã�� ������ �ش� ��ġ�� look ����
	m_pOwnerTransform.lock()->LookAt_ForLandObject(m_pGameInstance->Get_Player()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));

	_float fDist = dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->GetDistanceToPlayer();

	// �ٴ� ���� ��� (�Ÿ� 7 �ۿ��� �÷����̿��� �پ���� ���)
	if (m_isRun)
	{
		if (7.f > fDist)
			// IDLE�� ����
			return STATE::STATE_BONE_IDLE;
	}

	// �ȴ� ���� ��� (�Ÿ� 7 �ȿ��� �÷��̾�� �ɾ���� ���)
	else
	{
		if (7.f < fDist)
		{
			// �ٴ� ������� ����
			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BONE_RUN - (_uint)STATE_LEOPARDON_END - 1, true, 2.5f, 0.1f, 0);
			m_isRun = true;
		}

		if (3.f >= fDist)
		{
			return STATE::STATE_BONE_ATTACK;
		}
	}

	return STATE::STATE_BONE_WALK;
}

shared_ptr<CState> CState_Bone_Walk::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Bone_Walk> pInstance = make_shared<CState_Bone_Walk>(pDevice, pContext);
	
	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Bone_Walk");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Bone_Walk::Free()
{
}
