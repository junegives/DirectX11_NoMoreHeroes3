#include "pch.h"
#include "State_Hit.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"

#include "Player.h"
#include "Camera_Attach.h"

CState_Hit::CState_Hit(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Hit::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Hit::Tick(_float fTimeDelta)
{
	// 죽었으면 State death로
	if (STATE::STATE_DEATH == m_eState)
		return STATE::STATE_DEATH;

	if (dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->IsHit())
	{
		dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetHit(false);
		Enter_State();
		return STATE::STATE_HIT;
	}

	dynamic_pointer_cast<CCamera_Attach>(m_pGameInstance->GetCamera(CCamera_Manager::CAM_ATTACH))->SetLock(false);

	// 넉다운 피격이면 일어나는것까지 해야하고`
	if (m_isDowned && HIT_KNOCKDOWN == m_eHitType)
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
				dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation(STATE::STATE_DOWN_B_OUT, false, 3.0f, 0.1f, 0);
				break;
			case Engine::DIR_F:
				dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation(STATE::STATE_DOWN_F_OUT, false, 3.0f, 0.1f, 0);
				break;
			}
			m_pOwnerCollider.lock()->SetOnCollide(true);
		}


		return STATE::STATE_HIT;
	}

	// 나머지는 끝날 때 까지 X 0.7?? 정도로 해서 끝나면 IDLE로 넘어가게 하기
	if (0.8 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
	{
		m_pOwnerCollider.lock()->SetOnCollide(true);
		return STATE::STATE_IDLE;
	}

	else

	return STATE::STATE_HIT;
}

STATE CState_Hit::Late_Tick(_float fTimeDelta)
{

	return STATE::STATE_HIT;
}

void CState_Hit::Reset_State()
{
}

void CState_Hit::Enter_State()
{
	if (!dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetDamage())
	{
		dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation(STATE_DEATH, false, 3.0f, 0.1f, 0);
		m_eState = STATE::STATE_DEATH;
		return;
	}

	dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetHit(false);

	m_eHitType = dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->GetHitType();
	m_eHitDir = dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->GetHitDir();

	STATE eHitState;

	switch (m_eHitType)
	{
	case HIT_HEAVY:
		// Heavy Hit
		switch (m_eHitDir)
		{
		case Engine::DIR_B:
			eHitState = STATE_HEAVY_HIT;
			break;
		case Engine::DIR_F:
			eHitState = STATE_HEAVY_HIT_F;
			break;
		case Engine::DIR_L:
			eHitState = STATE_HEAVY_HIT_L;
			break;
		case Engine::DIR_R:
			eHitState = STATE_HEAVY_HIT_R;
			break;
		default:
			eHitState = STATE_HEAVY_HIT;
			break;
		}
		break;
	case HIT_LIGHT:
		// Light Hit
		switch (m_eHitDir)
		{
		case Engine::DIR_B:
			eHitState = STATE_LIGHT_HIT;
			break;
		case Engine::DIR_F:
			eHitState = STATE_LIGHT_HIT;
			break;
		case Engine::DIR_L:
			eHitState = STATE_LIGHT_HIT_L;
			break;
		case Engine::DIR_R:
			eHitState = STATE_LIGHT_HIT_R;
			break;
		default:
			eHitState = STATE_LIGHT_HIT;
			break;
		}
		break;
	case HIT_DEFAULT:
		// Default Hit
		switch (m_eHitDir)
		{
		case Engine::DIR_B:
			eHitState = STATE_HIT;
			break;
		case Engine::DIR_F:
			eHitState = STATE_HIT_F;
			break;
		case Engine::DIR_L:
			eHitState = STATE_HIT_L;
			break;
		case Engine::DIR_R:
			eHitState = STATE_HIT_R;
			break;
		default:
			eHitState = STATE_HIT;
			break;
		}
		break;
	case HIT_KNOCKDOWN:
		// Knock Down
		m_isDowned = true;
		switch (m_eHitDir)
		{
		case Engine::DIR_B:
			eHitState = STATE_STRUCKDOWN;
			break;
		case Engine::DIR_F:
			eHitState = STATE_STRUCKDOWN_F;
			break;
		case Engine::DIR_L:
			eHitState = STATE_STRUCKDOWN_F;
			break;
		case Engine::DIR_R:
			eHitState = STATE_STRUCKDOWN_F;
			break;
		default:
			eHitState = STATE_STRUCKDOWN_F;
			break;
		}
		break;
	case 4:
		// Blown

		break;
	default:
		break;
	}

	dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)eHitState, false, 3.0f, 0.1f, 0);
	m_eState = eHitState;
}

STATE CState_Hit::Key_Input(_float fTimeDelta)
{
	if (m_pGameInstance->Is_KeyPressing(DIK_LSHIFT))
	{
		Lock_On();
		m_isLockEnter = false;
	}
	else
	{
		m_isLockEnter = true;
		dynamic_pointer_cast<CCamera_Attach>(m_pGameInstance->GetCamera(CCamera_Manager::CAM_ATTACH))->SetLock(false);
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		return STATE::STATE_JUMP;
	}

	if (m_pGameInstance->Get_DIMouseState(MOUSEKEYSTATE::DIM_LB) & 0x80)
	{
		return STATE::STATE_ATTACK;
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

	//_vector vDir = m_pOwnerTransform.lock()->Get_State(CTransform::STATE_LOOK);

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


		if (-0.6 > fRadian)
		{
			vDir = XMVectorLerp(m_pOwnerTransform.lock()->Get_State(CTransform::STATE_LOOK), vDir, 0.5f);
		}
		else if (-0.1 > fRadian)
		{
			vDir = XMVectorLerp(m_pOwnerTransform.lock()->Get_State(CTransform::STATE_LOOK), vDir, 0.4f);
		}
		else
		{
			vDir = XMVectorLerp(m_pOwnerTransform.lock()->Get_State(CTransform::STATE_LOOK), vDir, 0.3f);
		}

		m_pOwnerTransform.lock()->SetLook_ForLandObject(XMVector3Normalize(vDir));
		m_pOwnerTransform.lock()->Go_Straight(fTimeDelta);

		return STATE::STATE_RUN;
	}

	return STATE::STATE_IDLE;
}

void CState_Hit::Lock_On()
{
	if (m_isLockEnter)
	{
		m_pTarget = dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SearchTarget();
	}

	shared_ptr<CGameObject>		pCamera = m_pGameInstance->GetCamera(CCamera_Manager::CAM_ATTACH);
	_vector vDir = { 0.f, 0.f, 0.f, 0.f };

	if (m_pTarget)
	{
		vDir = m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION) - m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION);

		dynamic_pointer_cast<CCamera_Attach>(pCamera)->ChangeTargetCamLerp(XMVector3Normalize(vDir), 0.1f, m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));
	}
}

shared_ptr<CState> CState_Hit::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Hit> pInstance = make_shared<CState_Hit>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Dodge");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Hit::Free()
{
}
