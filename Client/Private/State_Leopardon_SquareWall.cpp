#include "pch.h"
#include "State_Leopardon_SquareWall.h"

#include "GameObject.h"
#include "Transform.h"
#include "Monster_Leopardon.h"
#include "Projectile_Leopardon_Cube.h"

CState_Leopardon_SquareWall::CState_Leopardon_SquareWall(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Leopardon_SquareWall::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	m_pCube = CProjectile_Leopardon_Cube::Create(m_pDevice, m_pContext);

	m_pGameInstance->Add_GameObject(LEVEL_BATTLE2, LAYER_MONSTER_ATTACK, TEXT("GameObject_Projectile_Leopardon_Cube"), m_pCube);

	return S_OK;
}

STATE CState_Leopardon_SquareWall::Tick(_float fTimeDelta)
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

STATE CState_Leopardon_SquareWall::Late_Tick(_float fTimeDelta)
{
	return m_eState;
}

void CState_Leopardon_SquareWall::Reset_State()
{
	dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->SetYChange(false);
}

void CState_Leopardon_SquareWall::Enter_State()
{
	m_pOwnerModel.lock()->Set_Animation((_uint)STATE_LEOPARDON_SQUAREWALL - (_uint)STATE_TRIPLEPUT_END - 1, false, 4.f, 0.2f,  0);
	m_eState = STATE::STATE_LEOPARDON_SQUAREWALL;

	m_isCreate = false;
	m_isThrow = false;

	m_pCube->GenerateCube();
}

STATE CState_Leopardon_SquareWall::Check_State()
{
	// �ǰ� ����

	if (!m_isCreate && 0.1 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
	{
		// �簢�� ���� �ڵ� (��ǻ� �̹� ������ �� visible)
		m_isCreate = true;
		m_pCube->SetVisible(true);
		m_pCube->GenerateCube();

		m_pCube->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION));

		// �÷��̾� ��ġ ã�� ������ �ش� ��ġ�� look ����
		shared_ptr<CTransform> pPlayerTransform = dynamic_pointer_cast<CTransform>(m_pGameInstance->Find_Component(LEVEL_STATIC, LAYER_PLAYER, g_strTransformTag));
		m_pCube->Get_TransformCom()->LookAt_ForLandObject(pPlayerTransform->Get_State(CTransform::STATE_POSITION));

		_vector vLook = m_pCube->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);

		// ť�� ��ġ ����
		m_pCube->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION) + vLook * 2.f);
	}

	// Ư�� �ִϸ��̼� Ű�������� ��,
	else if (!m_isThrow && 0.8 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
	{
		// �簢�� �߻� �ڵ�
		m_isThrow = true;
		m_pCube->ThrowCube();
	}

	// �� ���Ŀ��� ��� ������ idle��
	else if (m_pOwnerModel.lock()->isAnimStop())
	{
		return STATE::STATE_LEOPARDON_IDLE;
	}

	return STATE::STATE_LEOPARDON_SQUAREWALL;
}

shared_ptr<CState> CState_Leopardon_SquareWall::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Leopardon_SquareWall> pInstance = make_shared<CState_Leopardon_SquareWall>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Leopardon_SquareWall");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Leopardon_SquareWall::Free()
{
}
