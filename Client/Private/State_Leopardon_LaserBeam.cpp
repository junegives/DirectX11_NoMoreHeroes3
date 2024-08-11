#include "pch.h"
#include "State_Leopardon_LaserBeam.h"

#include "GameObject.h"
#include "Transform.h"
#include "Monster_Leopardon.h"


CState_Leopardon_LaserBeam::CState_Leopardon_LaserBeam(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Leopardon_LaserBeam::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Leopardon_LaserBeam::Tick(_float fTimeDelta)
{
	if (dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->IsHit())
	{
		dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->SetHit(false);
		m_pOwnerCollider.lock()->SetOnCollide(true);
		if (!dynamic_pointer_cast<CMonster_Leopardon>(m_pOwnerObject.lock())->SetDamage())
			return STATE::STATE_LEOPARDON_HIT;
	}

	shared_ptr<CTransform> pPlayerTransform = dynamic_pointer_cast<CTransform>(m_pGameInstance->Find_Component(LEVEL_STATIC, LAYER_PLAYER, g_strTransformTag));

	// 플레이어 위치 찾은 다음에 해당 위치로 look 변경
	m_pOwnerTransform.lock()->LookAt_ForLandObject(pPlayerTransform->Get_State(CTransform::STATE_POSITION));

	m_eState = Check_State(fTimeDelta);

	return m_eState;
}

STATE CState_Leopardon_LaserBeam::Late_Tick(_float fTimeDelta)
{

	return m_eState;
}

void CState_Leopardon_LaserBeam::Reset_State()
{
}

void CState_Leopardon_LaserBeam::Enter_State()
{
	m_pOwnerModel.lock()->Set_Animation((_uint)STATE_LEOPARDON_LASERBEAM - (_uint)STATE_TRIPLEPUT_END - 1, false, 2.3f, 0.2f,  0);
	m_eState = STATE_LEOPARDON_LASERBEAM;

	m_isIn = true;
	m_isLoop = false;
	m_isOut = false;

	m_fFocusTime = 0.f;
	m_fFocusMaxTime = 2.f;
}

STATE CState_Leopardon_LaserBeam::Check_State(_float fTimeDelta)
{
	if (m_isIn)
	{
		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_isIn = false;
			m_isLoop = true;
			m_isOut = false;

			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_LEOPARDON_LASERBEAM_LOOP - (_uint)STATE_TRIPLEPUT_END - 1, false, 2.3f, 0.2f,  0);
		}

		return STATE::STATE_LEOPARDON_LASERBEAM;
	}

	else if (m_isLoop)
	{
		m_fFocusTime += fTimeDelta;

		if (m_fFocusMaxTime <= m_fFocusTime)
		{
			m_isIn = false;
			m_isLoop = false;
			m_isOut = true;

			m_fFocusTime = 0.f;
			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_LEOPARDON_LASERBEAM_OUT - (_uint)STATE_TRIPLEPUT_END - 1, false, 2.3f, 0.2f,  0);
		}

		return STATE::STATE_LEOPARDON_LASERBEAM;
	}

	else if (m_isOut)
	{
		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_isIn = false;
			m_isLoop = false;
			m_isOut = false;

			return STATE::STATE_LEOPARDON_IDLE;
		}

		return STATE::STATE_LEOPARDON_LASERBEAM;
	}

	return STATE::STATE_LEOPARDON_LASERBEAM;
}

shared_ptr<CState> CState_Leopardon_LaserBeam::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Leopardon_LaserBeam> pInstance = make_shared<CState_Leopardon_LaserBeam>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Leopardon_LaserBeam");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Leopardon_LaserBeam::Free()
{
}
