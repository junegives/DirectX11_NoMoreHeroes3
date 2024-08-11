#include "pch.h"
#include "State_TriplePut_Invisible.h"

#include "GameObject.h"
#include "Transform.h"
#include "Monster_TriplePut.h"

CState_TriplePut_Invisible::CState_TriplePut_Invisible(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_TriplePut_Invisible::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_TriplePut_Invisible::Tick(_float fTimeDelta)
{
	m_eState = Check_State();

	return m_eState;
}

STATE CState_TriplePut_Invisible::Late_Tick(_float fTimeDelta)
{
	return m_eState;
}

void CState_TriplePut_Invisible::Reset_State()
{
}

STATE CState_TriplePut_Invisible::Check_State()
{
	if (m_isIn)
	{
		// 모션 끝나는지 체크하고 Loop 모션으로 넘어가
		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_isIn = false;
			m_isLoop = true;
			m_isOut = false;
			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_TRIPLEPUT_INVISIBLE_LOOP, false, 1.0f, 0.3f, 0);
			return STATE::STATE_TRIPLEPUT_INVISIBLE;
		}
	}

	else if (m_isLoop)
	{
		// 모션 끝나는지 체크하고 Out 모션으로 넘어가
		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_isIn = false;
			m_isLoop = false;
			m_isOut = true;
			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_TRIPLEPUT_INVISIBLE_OUT, false, 1.0f, 0.3f, 0);
			return STATE::STATE_TRIPLEPUT_INVISIBLE;
		}
	}

	else if (m_isOut)
	{
		// 모션 끝나는지 체크하고 WALK 모션으로 넘어가
		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_isIn = false;
			m_isLoop = false;
			m_isOut = true;
			return STATE::STATE_TRIPLEPUT_IDLE;
		}
	}

	return STATE::STATE_TRIPLEPUT_INVISIBLE;
}

void CState_TriplePut_Invisible::Enter_State()
{
	m_pOwnerModel.lock()->Set_Animation((_uint)STATE_TRIPLEPUT_INVISIBLE, false, 1.0f, 0.3f, 0);
	m_eState = STATE::STATE_TRIPLEPUT_INVISIBLE;

	m_isIn = true;
	m_isLoop = false;
	m_isOut = false;
}

STATE CState_TriplePut_Invisible::Key_Input(_float fTimeDelta)
{
	return m_eState;
}

shared_ptr<CState> CState_TriplePut_Invisible::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_TriplePut_Invisible> pInstance = make_shared<CState_TriplePut_Invisible>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_TriplePut_Invisible");
		pInstance.reset();
	}

	return pInstance;
}

void CState_TriplePut_Invisible::Free()
{
}
