#include "pch.h"
#include "State_IDLE.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"

#include "Player.h"
#include "Camera_Attach.h"

CState_Idle::CState_Idle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Idle::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Idle::Tick(_float fTimeDelta)
{
	if (m_iOtherAnim)
	{
		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_iOtherAnim = false;
			dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_IDLE, true, 2.0f, 0.2f,  0);
		}
	}

	if (dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->IsSceneChange())
	{
		dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetSceneChange(false);
		return STATE::STATE_RISING;
	}

	if (dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->IsOnBattleNow())
	{
		dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetOnBattleNow (false);
		return STATE::STATE_APPEAR_IN;
	}


	if (dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->IsHit())
	{
		dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetHit(false);
		return STATE::STATE_HIT;
	}

	m_eState = Key_Input(fTimeDelta);

	return m_eState;
}

STATE CState_Idle::Late_Tick(_float fTimeDelta)
{
	return m_eState;
}

void CState_Idle::Reset_State()
{
}

void CState_Idle::Enter_State()
{
	_float fAnimLerpTime = dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->GetAnimLerp();

	if (0 <= fAnimLerpTime)
		dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_IDLE, true, 2.0f, fAnimLerpTime, 0);
	else
		dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_IDLE, true, 2.0f, 0.2f,  0);

	m_eState = STATE::STATE_IDLE;

	m_pTarget = dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SearchTarget();

	m_iOtherAnim = false;
}

STATE CState_Idle::Key_Input(_float fTimeDelta)
{
	_bool isOnBattle = dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->IsOnBattle();

	if (isOnBattle && m_pGameInstance->Is_KeyPressing(DIK_LSHIFT))
	{
		//Lock_On();
	}
	else
	{
		dynamic_pointer_cast<CCamera_Attach>(m_pGameInstance->GetCamera(CCamera_Manager::CAM_ATTACH))->SetLock(false);
	}

	if (m_pGameInstance->Get_DIMouseState(DIM_LB) & 0x80)
	{
		// 오토바이 타기
		if (!isOnBattle)
		{
			// 오토바이랑 거리 체크
			shared_ptr<CGameObject> pBike = m_pGameInstance->Find_GameObject(LEVEL_STATIC, LAYER_VEHICLE, TEXT("GameObject_Vehicle_Bike"));

			if (pBike)
			{
				_vector vBikePos = pBike->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
				if (4.f > XMVectorGetX(XMVector4Length(vBikePos - m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION))))
				{
					dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetOnBike(true);
					dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetOnBikeNow(true);

					return STATE_VEHICLE_IDLE;
				}
			}
		}

		else
		{
			return STATE_ATTACK;
		}

	}
	if (m_pGameInstance->Get_DIMouseState(MOUSEKEYSTATE::DIM_RB) & 0x80)
	{
		// 오토바이 부르기
		if (!isOnBattle)
		{
			m_iOtherAnim = true;
			dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_VEHICLE_CALL, false, 2.0f, 0.2f,  0);
			return STATE_IDLE;
		}

		else
		{
			return STATE::STATE_HEAVYATTACK;
		}
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_Z) & 0x80)
	{
		return STATE::STATE_HIT;
	}
	/*if (m_pGameInstance->Get_DIKeyState(DIK_T) & 0x80)
	{
		return STATE::STATE_TRANSFORM;
	}*/

	if (m_pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		return STATE::STATE_JUMP;
	}

	if (isOnBattle)
	{
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
			// 퍼펙트 닷지 상황이면
			if (dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player())->IsOnPerfectDodge())
			{
				dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player())->SetTarget(m_pTarget);
				return STATE::STATE_PERFECTDODGE;
			}

			return STATE::STATE_DODGE;
		}

		/*if (m_pGameInstance->Get_DIKeyState(DIK_R) & 0x80)
		{
			dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player())->SetOnPerfectDodge(true);
			return STATE::STATE_PERFECTDODGE;
		}*/

		if (m_pGameInstance->Get_DIKeyState(DIK_F) & 0x80)
		{
			if (m_pTarget)
				return STATE::STATE_THROW_LEOPARDON;
		}

		if (m_pGameInstance->Get_DIKeyState(DIK_LSHIFT) & 0x80)
		{
			m_pTarget = dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SearchTarget();
			wstring outputString = L"Get_DIKeyState(DIK_LSHIFT)\n";
			OutputDebugStringW(outputString.c_str());
			return STATE::STATE_GUARD;
		}
	}
	
	m_isPressed = false;

	if (m_pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		m_isPressed = true;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
	{
		m_isPressed = true;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
	{
		m_isPressed = true;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		m_isPressed = true;
	}

	if (m_isPressed)
	{
		return STATE::STATE_RUN;
	}

	return STATE::STATE_IDLE;
}

void CState_Idle::Lock_On()
{
	m_pTarget = dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->GetTarget();

	shared_ptr<CGameObject>		pCamera = m_pGameInstance->GetCamera(CCamera_Manager::CAM_ATTACH);
	_vector vDir = { 0.f, 0.f, 0.f, 0.f };

	if (m_pTarget)
	{
		vDir = m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION) - m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION);

		dynamic_pointer_cast<CCamera_Attach>(pCamera)->ChangeTargetCamLerp(XMVector3Normalize(vDir), 0.1f, m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));
	}
}

shared_ptr<CState> CState_Idle::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Idle> pInstance = make_shared<CState_Idle>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Run");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Idle::Free()
{
}
