#include "pch.h"
#include "State_Leopardon_Ring.h"

#include "GameObject.h"
#include "Transform.h"
#include "Monster_Leopardon.h"
#include "Projectile_Leopardon_Ring.h"

CState_Leopardon_Ring::CState_Leopardon_Ring(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Leopardon_Ring::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Leopardon_Ring::Tick(_float fTimeDelta)
{
	if (dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->IsHit())
	{
		dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->SetHit(false);
		m_pOwnerCollider.lock()->SetOnCollide(true);
		if (!dynamic_pointer_cast<CMonster_Leopardon>(m_pOwnerObject.lock())->SetDamage())
			return STATE::STATE_LEOPARDON_HIT;
	}

	m_eState = Check_State(fTimeDelta);

	return m_eState;
}

STATE CState_Leopardon_Ring::Late_Tick(_float fTimeDelta)
{
	return m_eState;
}

void CState_Leopardon_Ring::Reset_State()
{
	dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->SetYChange(false);
}

void CState_Leopardon_Ring::Enter_State()
{
	m_pOwnerModel.lock()->Set_Animation((_uint)STATE_LEOPARDON_SPREADRING - (_uint)STATE_TRIPLEPUT_END - 1, false, 3.f, 0.2f,  0);
	m_eState = STATE::STATE_LEOPARDON_SPREADRING;

	m_isCreate = false;
	m_isSpreadFirst = false;
	m_isSpreadSecond = false;
}

STATE CState_Leopardon_Ring::Check_State(_float fTimeDelta)
{
	// 피격 판정

	if (!m_isCreate && 0.35 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
	{
		for (size_t i = 0; i < 9; i++)
		{
			shared_ptr<CProjectile_Leopardon_Ring> pRing = CProjectile_Leopardon_Ring::Create(m_pDevice, m_pContext);

			m_isCreate = true;

			_vector vPos = m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION);

			pRing->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vPos);
			dynamic_pointer_cast<CNavigation>(pRing->Find_Component(TEXT("Com_Navigation")))->Compute_CurrentIdx(vPos);

			pRing->SetAttackPower(1);
			pRing->SetIdx((_int)i);

			// 플레이어 위치 찾은 다음에 해당 위치로 look 변경
			shared_ptr<CTransform> pPlayerTransform = m_pGameInstance->Get_Player()->Get_TransformCom();
			pRing->Get_TransformCom()->SetLook(pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION));

			_vector vLook = pRing->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
			vLook.m128_f32[1] = 0.f;

			// 링 위치 설정
			pRing->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION) + vLook * 0.2f + XMVectorSet(0.f, 1.f, 0.f, 0.f));
			
			dynamic_pointer_cast<CMonster_Leopardon>(m_pOwnerObject.lock())->AddRing(pRing);
		}

		return STATE::STATE_LEOPARDON_SPREADRING;
	}

	// 3개로 퍼지게
	else if (!m_isSpreadFirst && 0.4 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
	{
		m_isSpreadFirst = true;
		vector<shared_ptr<CProjectile_Leopardon_Ring>> pRingVec = dynamic_pointer_cast<CMonster_Leopardon>(m_pOwnerObject.lock())->GetRingVec();

		for (auto& pRing : pRingVec)
		{
			pRing->SetSpreadFirst();
			pRing->SetAttackPower(3);
		}

		return STATE::STATE_LEOPARDON_SPREADRING;
	}

	// 9개로 퍼지게
	else if (!m_isSpreadSecond && 0.55 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
	{
		m_isSpreadSecond = true;
		vector<shared_ptr<CProjectile_Leopardon_Ring>> pRingVec = dynamic_pointer_cast<CMonster_Leopardon>(m_pOwnerObject.lock())->GetRingVec();

		for (auto& pRing : pRingVec)
		{
			pRing->SetSpreadSecond();
			pRing->SetAttackPower(9);
		}

		return STATE::STATE_LEOPARDON_SPREADRING;
	}

	// 그 이후에는 모션 끝나면 idle로
	else if (m_pOwnerModel.lock()->isAnimStop())
	{
		return STATE::STATE_LEOPARDON_IDLE;
	}

	return STATE::STATE_LEOPARDON_SPREADRING;
}

shared_ptr<CState> CState_Leopardon_Ring::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Leopardon_Ring> pInstance = make_shared<CState_Leopardon_Ring>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Leopardon_Ring");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Leopardon_Ring::Free()
{
}
