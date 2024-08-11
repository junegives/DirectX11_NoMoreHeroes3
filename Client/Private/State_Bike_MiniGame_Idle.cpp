#include "pch.h"
#include "State_Bike_MiniGame_Idle.h"

#include "UI_QuestBoard.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"

#include "Bike.h"
#include "Player.h"

#include "Camera_Manager.h"
#include "Camera_Event.h"

#include "UI_Fade.h"
#include "UI_Manager.h"

CState_Bike_MiniGame_Idle::CState_Bike_MiniGame_Idle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Bike_MiniGame_Idle::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	m_isLine = 1;
	m_fLineX[0] = -6.3f;
	m_fLineX[1] = -0.2f;
	m_fLineX[2] = 6.f;

	return S_OK;
}

STATE CState_Bike_MiniGame_Idle::Tick(_float fTimeDelta)
{
	if (m_isIn)
	{
		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_isIn = false;

			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_IDLE - (_uint)STATE_BONE_END - 1, true, 2.0f, 0.2f,  0);
		}

		return STATE_BIKE_MINIGAME_IDLE;
	}
	if (m_isOut)
	{
		if (!m_isFade && 0.8 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
		{
			shared_ptr<CUI> pUIFade = CUI_Manager::GetInstance()->GetUI(TEXT("GameObject_UI_Fade"));
			dynamic_pointer_cast<CUI_Fade>(pUIFade)->FadeOut(2.f);

			m_isFade = true;
		}

		if (m_pOwnerModel.lock()->isAnimStop())
		{
			shared_ptr<CPlayer> pPlayer = dynamic_pointer_cast<CPlayer>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, LAYER_PLAYER, TEXT("GameObject_Player")));
			
			m_isOut = false;

			pPlayer->SetBattleEnd(true);

			shared_ptr<CUI> pUIFade = CUI_Manager::GetInstance()->GetUI(TEXT("GameObject_UI_Fade"));
			dynamic_pointer_cast<CUI_Fade>(pUIFade)->FadeIn(2.f);

			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_IDLE - (_uint)STATE_BONE_END - 1, true, 2.0f, 0.2f, 0);

			m_pBike.lock()->SetOnMiniGame(false);
			m_pBike.lock()->Set_Attached(false);
			/*m_pGameInstance->Set_CurLevel(LEVEL_VILLAGE);
			pPlayer->Change_NaviCom(LEVEL_VILLAGE);
			dynamic_pointer_cast<CUI_QuestBoard>(m_pGameInstance->Find_GameObject(LEVEL_VILLAGE, LAYER_UI, TEXT("GameObject_UI_QuestBoard")))->SetQuest(CUI_QuestBoard::QUEST_BOSS);*/
			pPlayer->SetOnBike(false);

			return STATE::STATE_BIKE_IDLE;
		}

		return STATE_BIKE_MINIGAME_IDLE;
	}

	if (m_pBike.lock()->IsGameStart())
	{
		return STATE_BIKE_MINIGAME_RUN;
	}

	m_eState = Key_Input(fTimeDelta);

	return m_eState;
}

STATE CState_Bike_MiniGame_Idle::Late_Tick(_float fTimeDelta)
{
	return m_eState;
}

void CState_Bike_MiniGame_Idle::Reset_State()
{
}

void CState_Bike_MiniGame_Idle::Enter_State()
{
	m_pBike = dynamic_pointer_cast<CBike>(m_pOwnerObject.lock());

	if (m_pBike.lock()->IsOnMiniGameNow())
	{
		m_isIn = true;

		shared_ptr<CUI> pUIFade = CUI_Manager::GetInstance()->GetUI(TEXT("GameObject_UI_Fade"));
		dynamic_pointer_cast<CUI_Fade>(pUIFade)->FadeIn(2.f);

		CSound_Manager::GetInstance()->StopAll();
		CSound_Manager::GetInstance()->PlaySound(L"Cat_Sigh.wav", CSound_Manager::CHANNELID::CAT, 1.f);

		_vector vPos = m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION);
		vPos.m128_f32[0] = m_fLineX[1];
		m_pOwnerTransform.lock()->Set_State(CTransform::STATE_POSITION, vPos);
		//m_pOwnerTransform.lock()->SetLook();
		m_pOwnerTransform.lock()->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));
		m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_IDLE_STRETCHING - (_uint)STATE_BONE_END - 1, false, 2.0f, 0.2f,  0);

		m_pBike.lock()->SetOnMiniGameNow(false);

		// 이벤트 카메라
		{
			shared_ptr<CCamera_Event> pEventCam = dynamic_pointer_cast<CCamera_Event>(m_pGameInstance->GetCamera(CCamera_Manager::CAM_EVENT));

			vPos.m128_f32[0] += 5.f;
			vPos.m128_f32[1] += 2.f;
			vPos.m128_f32[2] += 5.f;

			pEventCam->SettingBefore(vPos, XMVectorSet(0.f, -0.2f, -1.f, 0.f));

			vPos.m128_f32[0] -= 10.f;
			pEventCam->CamMove(5.f, 1.f, vPos);

			m_pGameInstance->SetMainCam(CCamera_Manager::CAM_EVENT);
		}
	}

	else
	{
		if (STATE::STATE_BIKE_MINIGAME_RUN == m_pOwnerMachine.lock()->Get_PreState())
		{
			m_isOut = true;
			CSound_Manager::GetInstance()->StopAll();
			CSound_Manager::GetInstance()->PlaySound(L"Cat_Meow.wav", CSound_Manager::CHANNELID::CAT, 1.f);

			// 이벤트 카메라
			{
				_vector vPos = m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION);

				shared_ptr<CCamera_Event> pEventCam = dynamic_pointer_cast<CCamera_Event>(m_pGameInstance->GetCamera(CCamera_Manager::CAM_EVENT));

				vPos.m128_f32[0] += 2.f;
				vPos.m128_f32[1] += 1.f;
				vPos.m128_f32[2] += 5.f;

				pEventCam->SettingBefore(vPos, XMVectorSet(-0.2f, 0.f, -1.f, 0.f));

				vPos.m128_f32[0] -= 2.f;
				vPos.m128_f32[1] += 3.f;
				vPos.m128_f32[2] -= 2.f;
				pEventCam->CamMove(5.f, 0.3f, vPos);

				m_pGameInstance->SetMainCam(CCamera_Manager::CAM_EVENT);
			}

			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_IDLE_YAWN - (_uint)STATE_BONE_END - 1, false, 2.0f, 0.2f, 0);
		}

		else
		{
			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_IDLE - (_uint)STATE_BONE_END - 1, true, 2.0f, 0.2f, 0);
		}
	}

	m_eState = STATE::STATE_BIKE_MINIGAME_IDLE;
}

STATE CState_Bike_MiniGame_Idle::Key_Input(_float fTimeDelta)
{
	if (m_pGameInstance->Is_KeyDown(DIK_SPACE))
	{
		m_pBike.lock()->SetGameStart(true);
	}

	return STATE::STATE_BIKE_MINIGAME_IDLE;
}

shared_ptr<CState> CState_Bike_MiniGame_Idle::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Bike_MiniGame_Idle> pInstance = make_shared<CState_Bike_MiniGame_Idle>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Bike_MiniGame_Idle");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Bike_MiniGame_Idle::Free()
{
}