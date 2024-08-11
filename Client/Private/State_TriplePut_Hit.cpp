#include "pch.h"
#include "State_TriplePut_Hit.h"

#include "GameObject.h"
#include "Transform.h"

#include "Monster_TriplePut.h"
#include "Camera_Attach.h"

#include "UI_Fade.h"
#include "UI_Manager.h"

#include "Player.h"

CState_TriplePut_Hit::CState_TriplePut_Hit(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_TriplePut_Hit::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_TriplePut_Hit::Tick(_float fTimeDelta)
{
	// 죽었으면 State death로
	if (STATE::STATE_DEATH == m_eState)
	{
		// 데스 타임 계산해서 사라지도록!!!!
		m_fDeathTime += fTimeDelta;

		if (!m_isFade && 1.f <= m_fDeathTime)
		{
			shared_ptr<CUI> pUIFade = CUI_Manager::GetInstance()->GetUI(TEXT("GameObject_UI_Fade"));
			dynamic_pointer_cast<CUI_Fade>(pUIFade)->FadeOut(2.f);

			m_isFade = true;
		}

		if (3.f <= m_fDeathTime)
		{
			// 배틀 내의 마지막으로 죽은 애면
			if (1 == m_pGameInstance->Get_Layer_Objects(m_pGameInstance->Get_CurLevel(), LAYER_MONSTER).size())
			{
				dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player())->SetBattleEnd(true);

				shared_ptr<CUI> pUIFade = CUI_Manager::GetInstance()->GetUI(TEXT("GameObject_UI_Fade"));
				dynamic_pointer_cast<CUI_Fade>(pUIFade)->FadeIn(2.f);
			}

			m_pOwnerObject.lock()->SetAlive(false);
		}

		return STATE::STATE_DEATH;
	}

	// 피격 판정
	if (dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->IsHit())
	{
		dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->SetHit(false);
		Enter_State();
		return STATE::STATE_TRIPLEPUT_HIT;
	}

	// 넉다운 피격이면 일어나는것까지 해야하고`
	if (m_isDowned && HIT_TRHOW == m_eHitType)
	{
		if (0.028 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio() && 0.5 >= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
		{
			// 이거 뭐임..?
			// 넉다운 피격이고 누워있을 때,,,,,
			// 날라가는거구나
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
				m_pOwnerModel.lock()->Set_Animation(STATE::STATE_TRIPLEPUT_DOWN_B_OUT - (_uint)STATE_ARMOR_END - 1, false, 3.0f, 0.1f, 0);
				break;
			case Engine::DIR_F:
				m_pOwnerModel.lock()->Set_Animation(STATE::STATE_TRIPLEPUT_DOWN_F_OUT - (_uint)STATE_ARMOR_END - 1, false, 3.0f, 0.1f, 0);
				break;
			}
			m_pOwnerCollider.lock()->SetOnCollide(true);
		}

		return STATE::STATE_TRIPLEPUT_HIT;
	}

	// 나머지는 끝날 때 까지 X 0.7?? 정도로 해서 끝나면 IDLE로 넘어가게 하기
	if (0.8 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
	{
		m_pOwnerCollider.lock()->SetOnCollide(true);
		return STATE::STATE_TRIPLEPUT_IDLE;
	}

	else
		return STATE::STATE_TRIPLEPUT_HIT;
}

STATE CState_TriplePut_Hit::Late_Tick(_float fTimeDelta)
{

	return STATE::STATE_TRIPLEPUT_HIT;
}

void CState_TriplePut_Hit::Reset_State()
{
}

void CState_TriplePut_Hit::Enter_State()
{
	if (!dynamic_pointer_cast<CMonster_TriplePut>(m_pOwnerObject.lock())->SetDamage())
	{
		m_pOwnerModel.lock()->Set_Animation(STATE_TRIPLEPUT_STAND_DEATH - (_uint)STATE_ARMOR_END - 1, false, 3.0f, 0.1f, 0);
		m_eState = STATE::STATE_DEATH;
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
			eHitState = STATE_TRIPLEPUT_HIT_HEAVY;
			break;
		/*case Engine::DIR_F:
			eHitState = STATE_TRIPLEPUT_HIT_HEAVY_F;
			break;
		case Engine::DIR_L:
			eHitState = STATE_TRIPLEPUT_HIT_HEAVY_L;
			break;
		case Engine::DIR_R:
			eHitState = STATE_TRIPLEPUT_HIT_HEAVY_R;
			break;*/
		default:
			m_iHitRand = rand() % 3 + 115;
			eHitState = (STATE)m_iHitRand;
			break;
		}
		break;
	case HIT_LIGHT:
		// Light Hit
		switch (m_eHitDir)
		{
		case Engine::DIR_B:
			eHitState = STATE_TRIPLEPUT_HIT;
			break;
		/*case Engine::DIR_F:
			eHitState = STATE_TRIPLEPUT_HIT_F;
			break;
		case Engine::DIR_L:
			eHitState = STATE_TRIPLEPUT_HIT_L;
			break;
		case Engine::DIR_R:
			eHitState = STATE_TRIPLEPUT_HIT_R;
			break;*/
		default:
			m_iHitRand = rand() % 3 + 119;
			eHitState = (STATE)m_iHitRand;
			break;
		}
		break;
	case HIT_DEFAULT:
		// Default Hit
		switch (m_eHitDir)
		{
		case Engine::DIR_B:
			eHitState = STATE_TRIPLEPUT_HIT;
			break;
		/*case Engine::DIR_F:
			eHitState = STATE_TRIPLEPUT_HIT_F;
			break;
		case Engine::DIR_L:
			eHitState = STATE_TRIPLEPUT_HIT_L;
			break;
		case Engine::DIR_R:
			eHitState = STATE_TRIPLEPUT_HIT_R;
			break;*/
		default:
			m_iHitRand = rand() % 3 + 119;
			eHitState = (STATE)m_iHitRand;
			break;
		}
		break;
	case HIT_TRHOW:
		eHitState = STATE_TRIPLEPUT_THROW;
		break;
	default:
		break;
	}

	m_pOwnerModel.lock()->Set_Animation((_uint)eHitState - (_uint)STATE_ARMOR_END - 1, false, 3.0f, 0.1f, 0);
	m_eState = eHitState;
}

shared_ptr<CState> CState_TriplePut_Hit::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_TriplePut_Hit> pInstance = make_shared<CState_TriplePut_Hit>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_TriplePut_Hit");
		pInstance.reset();
	}

	return pInstance;
}

void CState_TriplePut_Hit::Free()
{
}
