#include "pch.h"
#include "State_Bike_Run.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"

#include "Bike.h"
#include "Player.h"
#include "Camera_Attach.h"

CState_Bike_Run::CState_Bike_Run(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Bike_Run::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Bike_Run::Tick(_float fTimeDelta)
{
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

	m_eState = Key_Input(fTimeDelta);

	return m_eState;
}

STATE CState_Bike_Run::Late_Tick(_float fTimeDelta)
{

	return m_eState;
}

void CState_Bike_Run::Reset_State()
{
	m_pBike.lock()->SetRun(false);
}

void CState_Bike_Run::Enter_State()
{
	m_pBike = dynamic_pointer_cast<CBike>(m_pOwnerObject.lock());
	m_pBike.lock()->SetRun(true);

	m_eState = STATE::STATE_BIKE_RUN;

	m_isOut = false;

	if (STATE::STATE_BIKE_IDLE == m_pOwnerMachine.lock()->Get_PreState())
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

STATE CState_Bike_Run::Key_Input(_float fTimeDelta)
{
	// ºÎ½ºÅÍ
	if (m_pGameInstance->Is_MouseDown(DIM_LB))
	{
		return STATE::STATE_BIKE_NITRO;
	}

	m_isPressed = false;

	if (m_pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		m_pBike.lock()->Get_TransformCom()->Go_Straight(fTimeDelta, m_pNavigation);

		m_isPressed = true;
	}

	if (m_isPressed)
	{
		if (m_pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
		{
			m_pBike.lock()->Get_TransformCom()->Turn({ 0.f, 1.f, 0.f, 0.f }, -fTimeDelta);
		}

		if (m_pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
		{
			m_pBike.lock()->Get_TransformCom()->Turn({ 0.f, 1.f, 0.f, 0.f }, fTimeDelta);
		}

		CSound_Manager::GetInstance()->PlaySound(L"npc_am0000_mvmt_fs_walk_b_01_3.wav", CSound_Manager::CHANNELID::CAT_STEP, 1.f);

		return STATE::STATE_BIKE_RUN;
	}

	else
		return STATE::STATE_BIKE_IDLE;
}

shared_ptr<CState> CState_Bike_Run::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Bike_Run> pInstance = make_shared<CState_Bike_Run>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Bike_Run");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Bike_Run::Free()
{
}
