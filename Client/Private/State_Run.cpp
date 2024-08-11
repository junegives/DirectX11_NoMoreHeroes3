#include "pch.h"
#include "State_Run.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"

#include "Player.h"

#include "Sound_Manager.h"

CState_Run::CState_Run(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Run::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Run::Tick(_float fTimeDelta)
{
	if (dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->IsHit())
	{
		dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetHit(false);
		return STATE::STATE_HIT;
	}

	m_eState = Key_Input(fTimeDelta);

	return m_eState;
}

STATE CState_Run::Late_Tick(_float fTimeDelta)
{

	return m_eState;
}

void CState_Run::Reset_State()
{
}

void CState_Run::Enter_State()
{
	dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_RUN, true, 2.0f, 0.1f, 0);
	m_eState = STATE::STATE_RUN;
}

STATE CState_Run::Key_Input(_float fTimeDelta)
{
	_bool isOnBattle = dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->IsOnBattle();

	if (isOnBattle)
	{

		if (m_pGameInstance->Get_DIMouseState(MOUSEKEYSTATE::DIM_LB) & 0x80)
		{
			return STATE::STATE_ATTACK;
		}
		if (m_pGameInstance->Get_DIMouseState(MOUSEKEYSTATE::DIM_RB) & 0x80)
		{
			return STATE::STATE_HEAVYATTACK;
		}

		if (m_pGameInstance->Get_DIKeyState(DIK_1) & 0x80)
		{
			return STATE::STATE_KILLERSLASH;
		}

		if (m_pGameInstance->Get_DIKeyState(DIK_2) & 0x80)
		{
			return STATE::STATE_DEATHKICK;
		}

		if (m_pGameInstance->Get_DIKeyState(DIK_3) & 0x80)
		{
			return STATE::STATE_DASHATTACK;
		}

		if (m_pGameInstance->Get_DIKeyState(DIK_E) & 0x80)
		{
			return STATE::STATE_DODGE;
		}

		if (m_pGameInstance->Get_DIKeyState(DIK_R) & 0x80)
		{
			return STATE::STATE_PERFECTDODGE;
		}
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		return STATE::STATE_JUMP;
	}

	if (isOnBattle && m_pGameInstance->Get_DIKeyState(DIK_LSHIFT) & 0x80)
	{
		wstring outputString = L"Get_DIKeyState(DIK_LSHIFT)\n";
		OutputDebugStringW(outputString.c_str());
		return STATE::STATE_GUARD;
	}

	// vLook을 바꿔준 다음에 (카메라의 look 기준)
	/*
		1. vDir에 카메라의 vLook 가져오기
		2. vDir에 대해 누른 키의 방향으로 최종 방향 설정하기
		3. 플레이어의 vLook에서 vDir로 방향 보간 (아마 RotationSpeed를 쓰면 될듯)
		4. 플레이어의 vLook을 보간된 벡터로 설정
	*/

	m_isPressed = false;

	shared_ptr<CGameObject> pCam = m_pGameInstance->GetCamera(CCamera_Manager::CAM_ATTACH);
	_vector vCamLook = pCam->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	vCamLook.m128_f32[1] = 0.f;
	_vector vCamRight = pCam->Get_TransformCom()->Get_State(CTransform::STATE_RIGHT);
	vCamRight.m128_f32[1] = 0.f;
	_vector vDir = { 0.f, 0.f, 0.f, 0.f };

	// Go Straight 하기 (플레이어 look 기준)
	/*
		Go_Straight 호출하면서 Navigation Component 가져와서 같이 넘겨주기
	*/

	if (m_pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		_matrix matY = XMMatrixRotationY(XMConvertToRadians(270.f));
		_vector vCamLeft = XMVector3TransformNormal(vCamLook, matY);
		vDir += vCamLeft;

		m_isPressed = true;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
	{
		vDir += vCamRight;

		m_isPressed = true;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
	{
		_matrix matY = XMMatrixRotationY(XMConvertToRadians(180.f));
		_vector vCamBack = XMVector3TransformNormal(vCamLook, matY);

		vDir += vCamBack;

		m_isPressed = true;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		vDir += vCamLook;

		m_isPressed = true;
	}

	if (m_isPressed)
	{
		_float fRadian = XMVectorGetX(XMVector3Dot(m_pOwnerTransform.lock()->Get_State(CTransform::STATE_LOOK), vDir));
		CSound_Manager::GetInstance()->PlaySound(L"foley_TRV_FT_Conc_Run_00.wav", CSound_Manager::CHANNELID::PLAYER_STEP, 1.f);

		/*if (-0.8 > fRadian)
		{
			wstring outputString = L"fRadian: " + std::to_wstring(fRadian) + L"\n";
			OutputDebugStringW(outputString.c_str());
			vDir = XMVectorLerp(m_pOwnerTransform.lock()->Get_State(CTransform::STATE_LOOK), vDir, 0.5);
		}*/

		if (-0.7 > fRadian)
		{
			/*wstring outputString = L"fRadian : " + std::to_wstring(fRadian) + L"\n";
			OutputDebugStringW(outputString.c_str());*/
			vDir = XMVectorLerp(m_pOwnerTransform.lock()->Get_State(CTransform::STATE_LOOK), vDir, 0.5f);
			m_pOwnerTransform.lock()->SetLook_ForLandObject(XMVector3Normalize(vDir));
			m_pOwnerTransform.lock()->Go_Straight(fTimeDelta * 0.1f, m_pNavigation);
		}
		else if (-0.1 > fRadian)
		{
			/*wstring outputString = L"-0.1 > fRadian : " + std::to_wstring(fRadian) + L"\n";
			OutputDebugStringW(outputString.c_str());*/
			vDir = XMVectorLerp(m_pOwnerTransform.lock()->Get_State(CTransform::STATE_LOOK), vDir, 0.3f);
			m_pOwnerTransform.lock()->SetLook_ForLandObject(XMVector3Normalize(vDir));
			m_pOwnerTransform.lock()->Go_Straight(fTimeDelta * 0.3f, m_pNavigation);
		}
		else
		{
			/*wstring outputString = L"else : " + std::to_wstring(fRadian) + L"\n";
			OutputDebugStringW(outputString.c_str());*/
			vDir = XMVectorLerp(m_pOwnerTransform.lock()->Get_State(CTransform::STATE_LOOK), vDir, 0.2f);
			m_pOwnerTransform.lock()->SetLook_ForLandObject(XMVector3Normalize(vDir));
			m_pOwnerTransform.lock()->Go_Straight(fTimeDelta, m_pNavigation);
		}

		/*m_pOwnerTransform.lock()->SetLook(XMVector3Normalize(vDir));
		m_pOwnerTransform.lock()->Go_Straight(fTimeDelta, m_pNavigation);*/

		return STATE::STATE_RUN;
	}

	return STATE::STATE_IDLE;
}

shared_ptr<CState> CState_Run::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Run> pInstance = make_shared<CState_Run>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Run");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Run::Free()
{
}
