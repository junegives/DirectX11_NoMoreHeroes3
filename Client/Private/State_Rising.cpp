#include "pch.h"
#include "State_Rising.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"

#include "UI_Fade.h"
#include "UI_Manager.h"

#include "Player.h"
#include "Camera_Attach.h"

#include "Sound_Manager.h"

CState_Rising::CState_Rising(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Rising::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Rising::Tick(_float fTimeDelta)
{
	if (m_isIn)
	{
		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_isIn = false;
			dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_RISINGUP, false, 2.0f, 0.0f, 0);
		}

		return STATE::STATE_RISING;
	}

	if (m_pOwnerModel.lock()->isAnimStop())
	{
		// 레벨 전환할 차례
		m_pGameInstance->Set_CurLevel(m_eNextLevel);

		shared_ptr<CPlayer> pPlayer = dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player());
		pPlayer->Change_NaviCom(m_eNextLevel);

		if (m_eNextLevel == LEVEL_MINIGAME)
		{
			pPlayer->SetCurBGM(L"BGM_Nyan_Origin.mp3");
			pPlayer->SetOnBike(true);
			return STATE::STATE_VEHICLE_IDLE;
		}
		
		pPlayer->SetOnBattleNow(true);
		pPlayer->SetOnBattle(true);

		dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_IDLE, false, 3.0f, 0.0f, 0);
		return STATE::STATE_IDLE;
	}

	return STATE::STATE_RISING;
}

STATE CState_Rising::Late_Tick(_float fTimeDelta)
{
	return m_eState;
}

void CState_Rising::Reset_State()
{
}

void CState_Rising::Enter_State()
{
	m_eNextLevel = dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player())->GetNextLevel();

	shared_ptr<CUI> pUIFade = CUI_Manager::GetInstance()->GetUI(TEXT("GameObject_UI_Fade"));
	dynamic_pointer_cast<CUI_Fade>(pUIFade)->FadeOut(2.f);

	if (LEVEL_BATTLE1 == m_eNextLevel)
	{
		m_isIn = false;
		dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_INHALE, false, 2.0f, 0.2f,  0);
	}
	else if (LEVEL_BATTLE2 == m_eNextLevel)
	{
		m_isIn = true;
		dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_RISING, false, 2.0f, 0.2f,  0);
	}
	else if (LEVEL_MINIGAME == m_eNextLevel)
	{
		m_isIn = false;
		dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_ITEMGET, false, 1.0f, 0.2f,  0);
	}

	m_eState = STATE::STATE_RISING;
}

shared_ptr<CState> CState_Rising::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Rising> pInstance = make_shared<CState_Rising>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Rising");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Rising::Free()
{
}
