#include "pch.h"
#include "State_Bike_Idle.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"

#include "Bike.h"
#include "Player.h"
#include "Camera_Attach.h"

CState_Bike_Idle::CState_Bike_Idle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Bike_Idle::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Bike_Idle::Tick(_float fTimeDelta)
{
	// 미니게임으로 이동했을 때
	if (m_pBike.lock()->IsOnMiniGameNow() || m_pBike.lock()->IsOnMiniGame())
	{
		m_pBike.lock()->Set_Attached(true);
		return STATE::STATE_BIKE_MINIGAME_IDLE;
	}

	if (!m_pBike.lock()->Get_Attached())
	{
		_float fDist = XMVectorGetX(XMVector4Length(m_pGameInstance->Get_Player()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION)
			- m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION)));

		if (10.f <= fDist)
		{
			return STATE::STATE_BIKE_IDLE_SIT;
		}

		return STATE::STATE_BIKE_IDLE;
	}

	m_eState = Key_Input(fTimeDelta);

	return m_eState;
}

STATE CState_Bike_Idle::Late_Tick(_float fTimeDelta)
{
	return m_eState;
}

void CState_Bike_Idle::Reset_State()
{
}

void CState_Bike_Idle::Enter_State()
{
	m_pBike = dynamic_pointer_cast<CBike>(m_pOwnerObject.lock());

	m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_IDLE - (_uint)STATE_BONE_END - 1, true, 2.0f, 0.2f,  0);

	m_iAnimTest = (_uint)STATE_BIKE_IDLE;
	m_eState = STATE::STATE_BIKE_IDLE;

	m_isOut = false;
}

STATE CState_Bike_Idle::Key_Input(_float fTimeDelta)
{
	// 직진
	if (m_pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		m_pBike.lock()->Get_TransformCom()->Go_Straight(fTimeDelta, m_pNavigation);

		return STATE::STATE_BIKE_RUN;
	}

	return STATE::STATE_BIKE_IDLE;
}

shared_ptr<CState> CState_Bike_Idle::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Bike_Idle> pInstance = make_shared<CState_Bike_Idle>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Bike_Idle");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Bike_Idle::Free()
{
}
