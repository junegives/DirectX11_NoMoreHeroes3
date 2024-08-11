#include "pch.h"
#include "State_Bike_MiniGame_Run.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"

#include "Bike.h"
#include "Player.h"
#include "Camera_Attach.h"

CState_Bike_MiniGame_Run::CState_Bike_MiniGame_Run(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Bike_MiniGame_Run::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	m_isLine = 1;
	m_fLineX[0] = -6.f;
	m_fLineX[1] = -0.f;
	m_fLineX[2] = 6.f;

	return S_OK;
}

STATE CState_Bike_MiniGame_Run::Tick(_float fTimeDelta)
{
	if (850.f <= m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION).m128_f32[2])
	{
		return STATE::STATE_BIKE_MINIGAME_IDLE;
	}

	if (m_isIn)
	{
		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_isIn = false;
			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_RUN - (_uint)STATE_BONE_END - 1, true, 7.0f, 0.1f, 0);
		}
	}

	if (m_isOut)
	{
		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_isOut = false;
			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_IDLE - (_uint)STATE_BONE_END - 1, true, 2.0f, 0.1f, 0);
			return STATE::STATE_BIKE_IDLE;
		}
	}

	m_pBike.lock()->Get_TransformCom()->Go_Straight(fTimeDelta, nullptr);

	if (m_isJump)
	{
		if (0.5 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
		{
			m_isJump = false;
			m_isAvoidL = false;
			m_isAvoidR = false;

			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_RUN - (_uint)STATE_BONE_END - 1, true, 7.0f, 0.1f, 0);
		}

		return STATE::STATE_BIKE_MINIGAME_RUN;
	}

	if (m_isAvoidL)
	{
		_float fFrameRatio = m_pOwnerModel.lock()->Get_CurKeyFrameRatio();

		if (0.75 >= fFrameRatio)
		{
			m_pOwnerTransform.lock()->Go_LeftXZ(fTimeDelta * 0.6f);
		}

		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_isJump = false;
			m_isAvoidL = false;
			m_isAvoidR = false;

			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_RUN - (_uint)STATE_BONE_END - 1, true, 7.0f, 0.1f, 0);
		}

		return STATE::STATE_BIKE_MINIGAME_RUN;
	}

	if (m_isAvoidR)
	{
		_float fFrameRatio = m_pOwnerModel.lock()->Get_CurKeyFrameRatio();

		if (0.75 >= fFrameRatio)
		{
			m_pOwnerTransform.lock()->Go_RightXZ(fTimeDelta * 0.6f);
		}

		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_isJump = false;
			m_isAvoidL = false;
			m_isAvoidR = false;

			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_RUN - (_uint)STATE_BONE_END - 1, true, 7.0f, 0.1f, 0);
		}

		return STATE::STATE_BIKE_MINIGAME_RUN;
	}

	m_eState = Key_Input(fTimeDelta);

	return m_eState;
}

STATE CState_Bike_MiniGame_Run::Late_Tick(_float fTimeDelta)
{

	return m_eState;
}

void CState_Bike_MiniGame_Run::Reset_State()
{
	m_pBike.lock()->SetRun(false);
}

void CState_Bike_MiniGame_Run::Enter_State()
{
	m_pBike = dynamic_pointer_cast<CBike>(m_pOwnerObject.lock());
	m_pBike.lock()->SetRun(true);

	m_eState = STATE::STATE_BIKE_MINIGAME_RUN;

	m_isOut = false;

	if (STATE::STATE_BIKE_MINIGAME_IDLE == m_pOwnerMachine.lock()->Get_PreState())
	{
		m_isIn = true;
		m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_RUN_IN - (_uint)STATE_BONE_END - 1, false, 7.0f, 0.0f, 0);
	}

	else
	{
		m_isIn = false;
		m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_RUN - (_uint)STATE_BONE_END - 1, true, 7.0f, 0.1f, 0);
	}
}

STATE CState_Bike_MiniGame_Run::Key_Input(_float fTimeDelta)
{
	// ºÎ½ºÅÍ
	if (m_pGameInstance->Is_MouseDown(DIM_LB))
	{
		return STATE::STATE_BIKE_NITRO;
	}

	if (m_pGameInstance->Is_KeyDown(DIK_SPACE))
	{
		m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_JUMP - (_uint)STATE_BONE_END - 1, false, 1.0f, 0.0f, 0);
		m_isJump = true;
		m_isAvoidL = false;
		m_isAvoidR = false;

		return STATE::STATE_BIKE_MINIGAME_RUN;
	}

	if (m_pGameInstance->Is_KeyDown(DIK_A))
	{
		if (0 == m_isLine)
			return STATE::STATE_BIKE_MINIGAME_RUN;

		dynamic_pointer_cast<CPlayer>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, LAYER_PLAYER, TEXT("GameObject_Player")))->SetJump(true);
		m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_AVOID_L - (_uint)STATE_BONE_END - 1, false, 3.0f, 0.0f, 0);
		m_isJump = false;
		m_isAvoidL = true;
		m_isAvoidR = false;

		return STATE::STATE_BIKE_MINIGAME_RUN;
	}

	if (m_pGameInstance->Is_KeyDown(DIK_D))
	{
		if (2 == m_isLine)
			return STATE::STATE_BIKE_MINIGAME_RUN;

		dynamic_pointer_cast<CPlayer>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, LAYER_PLAYER, TEXT("GameObject_Player")))->SetJump(true);
		m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_AVOID_R - (_uint)STATE_BONE_END - 1, false, 3.0f, 0.0f, 0);
		m_isJump = false;
		m_isAvoidL = false;
		m_isAvoidR = true;

		return STATE::STATE_BIKE_MINIGAME_RUN;
	}

	return STATE::STATE_BIKE_MINIGAME_RUN;
}

shared_ptr<CState> CState_Bike_MiniGame_Run::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Bike_MiniGame_Run> pInstance = make_shared<CState_Bike_MiniGame_Run>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Bike_MiniGame_Run");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Bike_MiniGame_Run::Free()
{
}
