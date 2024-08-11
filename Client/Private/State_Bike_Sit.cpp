#include "pch.h"
#include "State_Bike_Sit.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"

#include "Bike.h"
#include "Player.h"
#include "Camera_Attach.h"

CState_Bike_Sit::CState_Bike_Sit(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Bike_Sit::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Bike_Sit::Tick(_float fTimeDelta)
{
	// 미니게임으로 이동했을 때
	if (m_pBike.lock()->IsOnMiniGameNow() || m_pBike.lock()->IsOnMiniGame())
	{
		m_pBike.lock()->Set_Attached(true);
		return STATE::STATE_BIKE_MINIGAME_IDLE;
	}

	if (m_isIn)
	{
		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_isIn = false;
			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_IDLE_SIT - (_uint)STATE_BONE_END - 1, true, 2.0f, 0.2f,  0);
		}

		return STATE::STATE_BIKE_IDLE_SIT;
	}

	if (m_isOut)
	{
		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_isOut = false;
			return STATE::STATE_BIKE_IDLE;
		}

		return STATE::STATE_BIKE_IDLE_SIT;
	}

	if (m_isIdling)
	{
		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_isIdling = false;
			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_IDLE_SIT - (_uint)STATE_BONE_END - 1, true, 2.0f, 0.2f,  0);
		}
	}

	/*if (!m_pBike.lock()->Get_Attached())
	{
		if (m_pGameInstance->Is_KeyDown(DIK_RIGHT))
		{
			m_iAnimTest++;

			if (m_iAnimTest == (_uint)STATE_BIKE_END)
				m_iAnimTest = (_uint)STATE_BIKE_ATTACK_IN;

			m_pOwnerModel.lock()->Set_Animation(m_iAnimTest - (_uint)STATE_BONE_END - 1, true, 2.0f, 0.2f,  0);
		}

		return STATE::STATE_BIKE_IDLE_SIT;
	}*/

	_float fDist = XMVectorGetX(XMVector4Length(m_pGameInstance->Get_Player()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION)
							- m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION)));

	// 플레이어랑 거리 계산해서 Sit이었다가 Idle이었다가 반복
	if (10.f >= fDist)
	{
		CSound_Manager::GetInstance()->PlaySound(L"Cat_Meow_Short.wav", CSound_Manager::CHANNELID::CAT, 1.f);

		m_isOut = true;
		m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_IDLE_SIT_OUT - (_uint)STATE_BONE_END - 1, false, 2.0f, 0.2f,  0);

		return STATE::STATE_BIKE_IDLE_SIT;
	}

	// 10보다 멀면 Idling 동작 가능
	else if (!m_isIdling)
	{
		m_fSitTime += fTimeDelta;
		if (m_fSitTime >= 5.f)
		{
			m_iIdlingCnt++;

			switch (m_iIdlingCnt % 3)
			{
			case 0:
				m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_IDLE_SIT_LOOK_AROUND - (_uint)STATE_BONE_END - 1, false, 2.0f, 0.2f,  0);
				break;
			case 1:
				m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_IDLE_YAWN - (_uint)STATE_BONE_END - 1, false, 2.0f, 0.2f,  0);
				break;
			case 2:
				m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_IDLE_FACEWASH - (_uint)STATE_BONE_END - 1, false, 2.0f, 0.2f,  0);
				break;
			}
			m_isIdling = true;
			m_fSitTime = 0.f;
		}
	}


	return m_eState;
}

STATE CState_Bike_Sit::Late_Tick(_float fTimeDelta)
{
	return m_eState;
}

void CState_Bike_Sit::Reset_State()
{
}

void CState_Bike_Sit::Enter_State()
{
	m_isOut = false;
	m_isIdling = false;

	m_pBike = dynamic_pointer_cast<CBike>(m_pOwnerObject.lock());

	// 가까웠다가 멀어진 경우
	if (STATE::STATE_BIKE_IDLE == m_pOwnerMachine.lock()->Get_PreState())
	{
		m_isIn = true;
		m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_IDLE_SIT_IN - (_uint)STATE_BONE_END - 1, false, 2.0f, 0.2f,  0);
	}

	else
	{
		m_isIn = false;
		m_fSitTime = 10.f;
		m_pOwnerModel.lock()->Set_Animation((_uint)STATE_BIKE_IDLE_SIT - (_uint)STATE_BONE_END - 1, true, 2.0f, 0.2f,  0);
	}

	m_iAnimTest = (_uint)STATE_BIKE_IDLE_SIT;
	m_eState = STATE::STATE_BIKE_IDLE_SIT;
}

shared_ptr<CState> CState_Bike_Sit::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Bike_Sit> pInstance = make_shared<CState_Bike_Sit>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Bike_Sit");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Bike_Sit::Free()
{
}
