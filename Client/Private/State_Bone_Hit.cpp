#include "pch.h"
#include "State_Bone_Hit.h"

#include "GameObject.h"
#include "Transform.h"

#include "Monster_Bone.h"
#include "Camera_Attach.h"

#include "UI_Fade.h"
#include "UI_Manager.h"

#include "Player.h"

CState_Bone_Hit::CState_Bone_Hit(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Bone_Hit::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Bone_Hit::Tick(_float fTimeDelta)
{
	// �׾����� State death��
	if (STATE::STATE_BONE_DOWN_DEATH == m_eState)
	{
		// ��Ʋ ���� ���������� ���� �ָ�
		if (1 == m_pGameInstance->Get_Layer_Objects(m_pGameInstance->Get_CurLevel(), LAYER_MONSTER).size())
		{
			dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player())->SetBattleEnd(true);
		}

		// ���� Ÿ�� ����ؼ� ���������!!!!
		m_fDeathTime += fTimeDelta;

		if (2.f <= m_fDeathTime)
		{
			m_pOwnerObject.lock()->SetAlive(false);
		}

		return STATE::STATE_BONE_DOWN_DEATH;
	}

	// �ǰ� ����
	if (dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->IsHit())
	{
		dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->SetHit(false);
		Enter_State();
		return STATE::STATE_BONE_HIT_LIGHT;
	}

	// �˴ٿ� �ǰ��̸� �Ͼ�°ͱ��� �ؾ��ϰ�`
	if (m_isDowned && HIT_TRHOW == m_eHitType)
	{
		if (0.028 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio() && 0.5 >= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
		{
			// �̰� ����..?
			// �˴ٿ� �ǰ��̰� �������� ��,,,,,
			// ���󰡴°ű���
			switch (m_eHitDir)
			{
			case Engine::DIR_B:
				m_pOwnerTransform.lock()->Go_Straight_Slide(fTimeDelta, m_pNavigation, ((0.5f - m_pOwnerModel.lock()->Get_CurKeyFrameRatio()) * 8));
				break;
			case Engine::DIR_F:
				m_pOwnerTransform.lock()->Go_Straight_Slide(fTimeDelta, m_pNavigation, -((0.5f - m_pOwnerModel.lock()->Get_CurKeyFrameRatio()) * 8));
				break;
			}

			//m_pOwnerTransform.lock()->MoveTo();
		}


		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_isDowned = false;

			switch (m_eHitDir)
			{
			case Engine::DIR_B:
				m_pOwnerModel.lock()->Set_Animation(STATE::STATE_BONE_DOWN_OUT_B - (_uint)STATE_LEOPARDON_END - 1, false, 3.0f, 0.1f, 0);
				break;
			case Engine::DIR_F:
				m_pOwnerModel.lock()->Set_Animation(STATE::STATE_BONE_DOWN_OUT - (_uint)STATE_LEOPARDON_END - 1, false, 3.0f, 0.1f, 0);
				break;
			}
			m_pOwnerCollider.lock()->SetOnCollide(true);
		}

		return STATE::STATE_BONE_HIT_LIGHT;
	}

	// �������� ���� �� ���� X 0.7?? ������ �ؼ� ������ IDLE�� �Ѿ�� �ϱ�
	if (0.8 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
	{
		m_pOwnerCollider.lock()->SetOnCollide(true);
		return STATE::STATE_BONE_IDLE;
	}

	else
		return STATE::STATE_BONE_HIT_LIGHT;
}

STATE CState_Bone_Hit::Late_Tick(_float fTimeDelta)
{

	return STATE::STATE_BONE_HIT_LIGHT;
}

void CState_Bone_Hit::Reset_State()
{
}

void CState_Bone_Hit::Enter_State()
{
	if (!dynamic_pointer_cast<CMonster_Bone>(m_pOwnerObject.lock())->SetDamage())
	{
		m_pOwnerModel.lock()->Set_Animation(STATE_BONE_DOWN_DEATH - (_uint)STATE_LEOPARDON_END - 1, false, 3.0f, 0.03f, 0);
		m_eState = STATE::STATE_BONE_DOWN_DEATH;
		return;
	}
	
	m_isFade = false;

	dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->SetHit(false);

	m_eHitType = dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->GetHitType();
	m_eHitDir = dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->GetHitDir();

	STATE eHitState;

	switch (m_eHitType)
	{
	case HIT_HEAVY:
		// Heavy Hit
		switch (m_eHitDir)
		{
		case Engine::DIR_B:
			eHitState = STATE_BONE_HIT_HEAVY_B;
			break;
		default:
			m_iHitRand = rand() % 3 + 186;
			eHitState = (STATE)m_iHitRand;
			break;
		}
		m_pOwnerModel.lock()->Set_Animation((_uint)eHitState - (_uint)STATE_LEOPARDON_END - 1, false, 3.0f, 0.03f, 0);
		break;
	case HIT_LIGHT:
		// Light Hit
		switch (m_eHitDir)
		{
		case Engine::DIR_B:
			eHitState = STATE_BONE_HIT_DEFAULT_B;
			break;
		default:
			m_iHitRand = rand() % 3 + 183;
			eHitState = (STATE)m_iHitRand;
			m_pOwnerModel.lock()->Set_Animation((_uint)eHitState - (_uint)STATE_LEOPARDON_END - 1, false, 3.0f, 0.03f, 0);
			break;
		}
		break;
	case HIT_DEFAULT:
		// Default Hit
		switch (m_eHitDir)
		{
		case Engine::DIR_B:
			eHitState = STATE_BONE_HIT_DEFAULT_B;
			break;
		default:
			m_iHitRand = rand() % 3 + 187;
			eHitState = (STATE)m_iHitRand;
			m_pOwnerModel.lock()->Set_Animation((_uint)eHitState - (_uint)STATE_LEOPARDON_END - 1, false, 3.0f, 0.03f, 0);
			break;
		}
		break;
	case HIT_TRHOW:
		eHitState = STATE_BONE_THROW;
		m_pOwnerModel.lock()->Set_Animation((_uint)eHitState - (_uint)STATE_LEOPARDON_END - 1, false, 3.0f, 0.1f, 0);
		break;
	}

	m_eState = eHitState;
}

shared_ptr<CState> CState_Bone_Hit::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Bone_Hit> pInstance = make_shared<CState_Bone_Hit>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Bone_Hit");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Bone_Hit::Free()
{
}
