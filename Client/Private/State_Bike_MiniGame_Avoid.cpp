#include "pch.h"
#include "State_Bike_MiniGame_Avoid.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"

#include "Bike.h"
#include "Player.h"
#include "Camera_Attach.h"

CState_Bike_MiniGame_Avoid::CState_Bike_MiniGame_Avoid(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Bike_MiniGame_Avoid::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Bike_MiniGame_Avoid::Tick(_float fTimeDelta)
{
	if (m_pBike.lock()->IsGameStart())
	{
		return STATE_BIKE_MINIGAME_RUN;
	}

	m_eState = Key_Input(fTimeDelta);

	return m_eState;
}

STATE CState_Bike_MiniGame_Avoid::Late_Tick(_float fTimeDelta)
{
	return m_eState;
}

void CState_Bike_MiniGame_Avoid::Reset_State()
{
}

void CState_Bike_MiniGame_Avoid::Enter_State()
{
	m_pBike = dynamic_pointer_cast<CBike>(m_pOwnerObject.lock());

	m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_AVOID - (_uint)STATE_BONE_END - 1, true, 2.0f, 0.2f,  0);

	m_eState = STATE::STATE_BIKE_AVOID;
}

STATE CState_Bike_MiniGame_Avoid::Key_Input(_float fTimeDelta)
{
	if (m_pGameInstance->Is_KeyDown(DIK_SPACE))
	{
		m_pBike.lock()->SetGameStart(true);
	}

	return STATE::STATE_BIKE_MINIGAME_IDLE;
}

shared_ptr<CState> CState_Bike_MiniGame_Avoid::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Bike_MiniGame_Avoid> pInstance = make_shared<CState_Bike_MiniGame_Avoid>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Bike_MiniGame_Avoid");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Bike_MiniGame_Avoid::Free()
{
}