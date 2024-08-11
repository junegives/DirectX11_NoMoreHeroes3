#include "pch.h"
#include "State_Leopardon_Occurrence.h"

#include "GameObject.h"
#include "Transform.h"
#include "Monster_Leopardon.h"
#include "LandObject_Leopardon_Teleport.h"


CState_Leopardon_Occurrence::CState_Leopardon_Occurrence(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Leopardon_Occurrence::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Leopardon_Occurrence::Tick(_float fTimeDelta)
{
	if (dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->IsHit())
	{
		dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->SetHit(false);
		m_pOwnerCollider.lock()->SetOnCollide(true);
		if (!dynamic_pointer_cast<CMonster_Leopardon>(m_pOwnerObject.lock())->SetDamage())
			return STATE::STATE_LEOPARDON_HIT;
	}

	m_eState = Check_State();

	return m_eState;
}

STATE CState_Leopardon_Occurrence::Late_Tick(_float fTimeDelta)
{

	return m_eState;
}

void CState_Leopardon_Occurrence::Reset_State()
{
}

void CState_Leopardon_Occurrence::Enter_State()
{
	m_pOwnerModel.lock()->Set_Animation((_uint)STATE_LEOPARDON_OCCURRENCE - (_uint)STATE_TRIPLEPUT_END - 1, false, 4.f, 0.2f,  0);
	m_eState = STATE_LEOPARDON_OCCURRENCE;
}

STATE CState_Leopardon_Occurrence::Check_State()
{
	// 워프 생성하는 코드,,,,, 만들기 싫다,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,

	if (!m_isCreated && 0.3 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
	{
		for (size_t i = 0; i < 3; i++)
		{
			shared_ptr<CLandObject_Leopardon_Teleport> pTeleport = CLandObject_Leopardon_Teleport::Create(m_pDevice, m_pContext);

			//// 위치 랜덤으로,,, 설정,,,, 하기 싫다,,,,,,,,,,,,,,,,,,,,,,,,,
			//_vector vRandomPos = XMVectorSet(_float(rand() % 500 - 250) / 10.f, 3.f, _float(rand() % 300 - 200) / 10.f, 1.f);

			//if (!dynamic_pointer_cast<CLandObject_Leopardon_Teleport>(pTeleport)->IsValidPos(vRandomPos))
			//{
			//	i--;
			//	continue;
			//}

			_vector vRandomPos;

			if (0 == i)
				vRandomPos = XMVectorSet(18.4f, 3.6f, -4.5f, 1.f);

			else if (1 == i)
				vRandomPos = XMVectorSet(4.2f, 3.6f, -11.7f, 1.f);

			else
				vRandomPos = XMVectorSet(-13.7f, 3.6f, -1.0f, 1.f);

			pTeleport->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vRandomPos);

			dynamic_pointer_cast<CMonster_Leopardon>(m_pOwnerObject.lock())->AddTeleport(pTeleport);
		}


		m_isCreated = true;

		return STATE::STATE_LEOPARDON_OCCURRENCE;
	}

	if (m_pOwnerModel.lock()->isAnimStop())
		return STATE::STATE_LEOPARDON_IDLE;

	return STATE::STATE_LEOPARDON_OCCURRENCE;
}

shared_ptr<CState> CState_Leopardon_Occurrence::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Leopardon_Occurrence> pInstance = make_shared<CState_Leopardon_Occurrence>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Leopardon_Occurrence");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Leopardon_Occurrence::Free()
{
}
