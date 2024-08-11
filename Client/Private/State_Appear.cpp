#include "pch.h"
#include "State_Appear.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"

#include "Monster_Leopardon.h"
#include "Monster_TriplePut.h"
#include "Monster_Bone.h"

#include "Player.h"
#include "Weapon.h"

#include "Camera_Manager.h"
#include "Camera_Attach.h"
#include "Camera_Event.h"

#include "UI_Fade.h"
#include "UI_Manager.h"

#include "Sound_Manager.h"

CState_Appear::CState_Appear(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Appear::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Appear::Tick(_float fTimeDelta)
{
	shared_ptr<CPlayer> pPlayer = dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player());
	shared_ptr<CWeapon>			pWeapon = dynamic_pointer_cast<CWeapon>(pPlayer->GetWeapon());
	shared_ptr<CBinaryModel>	pWeaponModel = dynamic_pointer_cast<CBinaryModel>(pWeapon->Find_Component(TEXT("Com_Model")));

	_float4x4* pSocekt = pWeaponModel->Get_CombinedBoneMatrixPtr("gimmick_03");

	if (m_isIn)
	{
		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_isIn = false;
			m_isLoop = true;
			dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_APPEAR_LOOP, true, 3.0f, 0.0f, 0);
		}

		return STATE::STATE_APPEAR_IN;
	}

	if (m_isLoop)
	{
		if (m_pGameInstance->Is_KeyDown(DIK_SPACE))
		{
			shared_ptr<CCamera_Event>	pEventCam = dynamic_pointer_cast<CCamera_Event>(m_pGameInstance->GetCamera(CCamera_Manager::CAM_EVENT));
			//pEventCam->CamZoom(0.1f, 1.f, 0.f);

			CSound_Manager::GetInstance()->PlaySound(L"Weapon_Start.wav", CSound_Manager::CHANNELID::PLAYER_WEAPON, 1.f);

			m_isLoop = false;
			m_isOut = true;
			dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_APPEAR_OUT, false, 3.0f, 0.1f, 0);
			pWeapon->SetWeaponGlow(true);
		}

		return STATE::STATE_APPEAR_IN;
	}

	if (m_isOut)
	{
		if (!m_isWeaponAttached && 0.2 <= m_pOwnerModel.lock()->Get_CurKeyFrameTime())
		{
			m_isWeaponAttached = true;
			//dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player())->SetWeaponAttach(true);

			return STATE::STATE_APPEAR_IN;
		}

		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_isIn = false;
			m_isLoop = false;
			m_isOut = false;

			CMonster::MONSTER_DESC			MonsterDesc{};

			m_pGameInstance->SetMainCam(CCamera_Manager::CAM_ATTACH);

			// 레벨에 따라 몬스터 출현할 차례
			switch (m_eNextLevel)
			{
			case Client::LEVEL_BATTLE1:

				if (m_isTriplePut)
				{
					m_pGameInstance->Remove_Object(LEVEL_BATTLE1, LAYER_MONSTER, TEXT("GameObject_Monster_TriplePut"));
					m_isTriplePut = false;
				}

				MonsterDesc.iStartCell = 1;
				m_pGameInstance->Add_GameObject(LEVEL_BATTLE1, LAYER_MONSTER, TEXT("GameObject_Monster_TriplePut"),
					CMonster_TriplePut::Create(m_pDevice, m_pContext, &MonsterDesc));

				m_isTriplePut = true;

				if (m_isBattle1Bone1)
				{
					m_pGameInstance->Remove_Object(LEVEL_BATTLE1, LAYER_MONSTER, TEXT("GameObject_Monster_Battle1Bone1"));
					m_isBattle1Bone1 = false;
				}

				MonsterDesc.iStartCell = 2;
				m_pGameInstance->Add_GameObject(LEVEL_BATTLE1, LAYER_MONSTER, TEXT("GameObject_Monster_Battle1Bone1"),
					CMonster_Bone::Create(m_pDevice, m_pContext, &MonsterDesc));

				m_isBattle1Bone1 = true;

				if (m_isBattle1Bone2)
				{
					m_pGameInstance->Remove_Object(LEVEL_BATTLE1, LAYER_MONSTER, TEXT("GameObject_Monster_Battle1Bone2"));
					m_isBattle1Bone2 = false;
				}

				MonsterDesc.iStartCell = 3;
				m_pGameInstance->Add_GameObject(LEVEL_BATTLE1, LAYER_MONSTER, TEXT("GameObject_Monster_Battle1Bone2"),
					CMonster_Bone::Create(m_pDevice, m_pContext, &MonsterDesc));

				m_isBattle1Bone2 = true;

				break;
			case Client::LEVEL_BATTLE2:

				if (m_isLeopardon)
				{
					m_pGameInstance->Remove_Object(LEVEL_BATTLE2, LAYER_MONSTER, TEXT("GameObject_Monster_Leopardon"));
					m_isLeopardon = false;
				}

				MonsterDesc.iStartCell = 3;
				m_pGameInstance->Add_GameObject(LEVEL_BATTLE2, LAYER_MONSTER, TEXT("GameObject_Monster_Leopardon"),
					CMonster_Leopardon::Create(m_pDevice, m_pContext, &MonsterDesc));

				m_isLeopardon = true;

				if (m_isBattle1Bone1)
				{
					m_pGameInstance->Remove_Object(LEVEL_BATTLE2, LAYER_MONSTER, TEXT("GameObject_Monster_Battle2Bone1"));
					m_isBattle1Bone1 = false;
				}

				MonsterDesc.iStartCell = 5;
				m_pGameInstance->Add_GameObject(LEVEL_BATTLE2, LAYER_MONSTER, TEXT("GameObject_Monster_Battle2Bone1"),
					CMonster_Bone::Create(m_pDevice, m_pContext, &MonsterDesc));

				m_isBattle1Bone1 = true;

				if (m_isBattle1Bone2)
				{
					m_pGameInstance->Remove_Object(LEVEL_BATTLE2, LAYER_MONSTER, TEXT("GameObject_Monster_Battle2Bone2"));
					m_isBattle1Bone2 = false;
				}

				MonsterDesc.iStartCell = 15;
				m_pGameInstance->Add_GameObject(LEVEL_BATTLE2, LAYER_MONSTER, TEXT("GameObject_Monster_Battle2Bone2"),
					CMonster_Bone::Create(m_pDevice, m_pContext, &MonsterDesc));

				m_isBattle1Bone2 = true;
				break;
			case Client::LEVEL_MINIGAME:
				break;
			default:
				break;
			}

			return STATE::STATE_IDLE;
		}
	}

	return STATE::STATE_APPEAR_IN;
}

STATE CState_Appear::Late_Tick(_float fTimeDelta)
{
	return m_eState;
}

void CState_Appear::Reset_State()
{
}

void CState_Appear::Enter_State()
{
	shared_ptr<CPlayer> pPlayer = dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player());
	shared_ptr<CWeapon> pWeapon = dynamic_pointer_cast<CWeapon>(pPlayer->GetWeapon());

	m_eNextLevel = pPlayer->GetNextLevel();

	m_isIn = true;
	m_isLoop = false;
	m_isOut = false;
	dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_APPEAR_IN, false, 3.0f, 0.2f,  0);
	
	pWeapon->SetWeaponGlow(false);

	dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player())->SetWeaponAttach(true);
	m_isWeaponAttached = false;

	// 이벤트 카메라
	// 무기에다가 붙여볼까???
	{
		shared_ptr<CCamera_Event>	pEventCam = dynamic_pointer_cast<CCamera_Event>(m_pGameInstance->GetCamera(CCamera_Manager::CAM_EVENT));
		//shared_ptr<CWeapon>			pWeapon = dynamic_pointer_cast<CWeapon>(pPlayer->GetWeapon());
		//shared_ptr<CBinaryModel>	pWeaponModel = dynamic_pointer_cast<CBinaryModel>(pWeapon->Find_Component(TEXT("Com_Model")));

		//// 붙일 Socket Matrix
		//_vector		vLocalTrans = {-0.7f, 0.4f, 0.2f, 0.f};	

		//_matrix		matPivot = XMMatrixIdentity();

		//matPivot *= XMMatrixRotationX(XMConvertToRadians(120));
		//matPivot *= XMMatrixRotationY(XMConvertToRadians(270));
		//matPivot *= XMMatrixRotationZ(XMConvertToRadians(30));

		//pEventCam->SettingBefore(pWeapon, pWeaponModel->Get_CombinedBoneMatrixPtr("gimmick_03"), matPivot, vLocalTrans);

		_vector vPlayerLook = pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
		XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180));

		_vector vPlayerLookOpposite = XMVector3TransformCoord(vPlayerLook, XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180)));
		vPlayerLookOpposite.m128_f32[3] = 0.f;
		
		_vector vStartPos = pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION) + vPlayerLook * 1.f;

		pEventCam->SettingBefore(vStartPos + XMVectorSet(0.f, 0.3f, 0.f, 0.f), vPlayerLookOpposite);
		pEventCam->CamMove(100.f, 0.4f, vStartPos + XMVectorSet(0.f, 2.3f, 0.f, 0.f) + pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_RIGHT) * 0.5);
		m_pGameInstance->SetMainCam(CCamera_Manager::CAM_EVENT);
	}

	shared_ptr<CUI> pUIFade = CUI_Manager::GetInstance()->GetUI(TEXT("GameObject_UI_Fade"));
	dynamic_pointer_cast<CUI_Fade>(pUIFade)->FadeIn(2.f);

	m_eState = STATE::STATE_APPEAR_IN;
}

shared_ptr<CState> CState_Appear::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Appear> pInstance = make_shared<CState_Appear>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Appear");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Appear::Free()
{
}
