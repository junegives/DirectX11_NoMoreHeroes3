#include "pch.h"
#include "State_Leopardon_Warp.h"

#include "GameObject.h"
#include "Transform.h"
#include "Monster_Leopardon.h"
#include "LandObject_Leopardon_Teleport.h"


CState_Leopardon_Warp::CState_Leopardon_Warp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Leopardon_Warp::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Leopardon_Warp::Tick(_float fTimeDelta)
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

STATE CState_Leopardon_Warp::Late_Tick(_float fTimeDelta)
{

	return m_eState;
}

void CState_Leopardon_Warp::Reset_State()
{
}

void CState_Leopardon_Warp::Enter_State()
{
	m_pOwnerModel.lock()->Set_Animation((_uint)STATE_LEOPARDON_WARP - (_uint)STATE_TRIPLEPUT_END - 1, false, 4.f, 0.2f,  0);
	m_eState = STATE_LEOPARDON_WARP;

	m_isWarped = false;

	m_isIn = true;
	m_isOut = false;
}

STATE CState_Leopardon_Warp::Check_State()
{
	if (m_isIn)
	{
		// 워프 이동
		if (!m_isWarped && m_pOwnerModel.lock()->isAnimStop())
		{
			m_isIn = false;
			m_isOut = true;

			m_isWarped = true;

			// 랜덤 텔레포트로 이동 말고 그냥 다음 텔레포트로 이동합시다
			m_iCurTeleportIdx++;
			if (m_iCurTeleportIdx >= dynamic_pointer_cast<CMonster_Leopardon>(m_pOwnerObject.lock())->GetTeleportVec().size())
				m_iCurTeleportIdx = 0;

			shared_ptr<CLandObject_Leopardon_Teleport> pTeleport = dynamic_pointer_cast<CMonster_Leopardon>(m_pOwnerObject.lock())->GetTeleportVec()[m_iCurTeleportIdx];
			pTeleport->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

			_vector vPos = pTeleport->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

			m_pOwnerObject.lock()->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vPos);

			shared_ptr<CTransform> pPlayerTransform = dynamic_pointer_cast<CTransform>(m_pGameInstance->Find_Component(LEVEL_STATIC, LAYER_PLAYER, g_strTransformTag));

			// 플레이어 위치 찾은 다음에 해당 위치로 look 변경
			m_pOwnerTransform.lock()->LookAt_ForLandObject(pPlayerTransform->Get_State(CTransform::STATE_POSITION));

			dynamic_pointer_cast<CMonster_Leopardon>(m_pOwnerObject.lock())->IsValidPos(vPos);

			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_LEOPARDON_WARP_OUT - (_uint)STATE_TRIPLEPUT_END - 1, false, 2.3f, 0.2f,  0);
		}

		return STATE::STATE_LEOPARDON_WARP;
	}

	if (m_isOut)
	{
		if (m_pOwnerModel.lock()->isAnimStop())
			return STATE::STATE_LEOPARDON_IDLE;
	}

	return STATE::STATE_LEOPARDON_WARP;
}

shared_ptr<CState> CState_Leopardon_Warp::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Leopardon_Warp> pInstance = make_shared<CState_Leopardon_Warp>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Leopardon_Warp");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Leopardon_Warp::Free()
{
}
