#include "pch.h"
#include "State_TriplePut_Rush.h"

#include "GameObject.h"
#include "Transform.h"
#include "Monster_TriplePut.h"
#include "RushTrail.h"

CState_TriplePut_Rush::CState_TriplePut_Rush(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_TriplePut_Rush::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_TriplePut_Rush::Tick(_float fTimeDelta)
{
	// 피격 판정
	if (dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->IsHit())
	{
		dynamic_pointer_cast<CMonster>(m_pOwnerObject.lock())->SetHit(false);
		if (!dynamic_pointer_cast<CMonster_TriplePut>(m_pOwnerObject.lock())->SetDamage())
			return STATE::STATE_TRIPLEPUT_HIT;
	}

	m_eState = Check_State(fTimeDelta);

	return m_eState;
}

STATE CState_TriplePut_Rush::Late_Tick(_float fTimeDelta)
{
	return m_eState;
}

void CState_TriplePut_Rush::Reset_State()
{
	m_pTrailHandL->ClearTrail();
	m_pTrailHandR->ClearTrail();
	m_pTrailFootL->ClearTrail();
	m_pTrailFootR->ClearTrail();
}

void CState_TriplePut_Rush::Enter_State()
{
	m_pOwnerModel.lock()->Set_Animation((_uint)STATE_TRIPLEPUT_RUSH - (_uint)STATE_ARMOR_END - 1, false, 3.0f, 0.3f, 0);
	m_eState = STATE::STATE_TRIPLEPUT_RUSH;
	m_isIn = true;
	m_isLoop = false;
	m_isOut = false;

	shared_ptr<CTransform> pPlayerTransform = dynamic_pointer_cast<CTransform>(m_pGameInstance->Find_Component(LEVEL_STATIC, LAYER_PLAYER, g_strTransformTag));

	m_vDest = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	m_pOwnerTransform.lock()->LookAt_ForLandObject(pPlayerTransform->Get_State(CTransform::STATE_POSITION));

	m_fPrevDist = 100.f;
	m_iFarCnt = 0;
}

STATE CState_TriplePut_Rush::Check_State(_float fTimeDelta)
{
	if (m_isIn)
	{
		if (0.8 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
		{
			shared_ptr<CTransform> pPlayerTransform = dynamic_pointer_cast<CTransform>(m_pGameInstance->Find_Component(LEVEL_STATIC, LAYER_PLAYER, g_strTransformTag));

			m_pOwnerTransform.lock()->LookAt_ForLandObject(pPlayerTransform->Get_State(CTransform::STATE_POSITION));
		}

		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_isIn = false;
			m_isLoop = true;
			m_isOut = false;

			CSound_Manager::GetInstance()->StopSound(CSound_Manager::CHANNELID::MONSTER2_ATTACK);
			CSound_Manager::GetInstance()->PlaySound(L"TriplePut_Rush.wav", CSound_Manager::CHANNELID::MONSTER2_ATTACK, 1.f);

			m_pOwnerAttackCollider.lock()->SetActive(true);
			m_pOwnerAttackCollider.lock()->SetOnCollide(true);

			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_TRIPLEPUT_RUSH_LOOP - (_uint)STATE_ARMOR_END - 1, false, 2.3f, 0.0f, 0);
		}

		return STATE::STATE_TRIPLEPUT_RUSH;
	}
	else if (m_isLoop)
	{
		{
			m_vUpperPos = { m_pTrailHandR1Matrix->m[3][0], m_pTrailHandR1Matrix->m[3][1] , m_pTrailHandR1Matrix->m[3][2] };
			m_vLowerPos = { m_pTrailHandR2Matrix->m[3][0], m_pTrailHandR2Matrix->m[3][1] , m_pTrailHandR2Matrix->m[3][2] };
			m_pTrailHandR->UpdateTrail(m_vUpperPos, m_vLowerPos, m_pOwnerTransform.lock()->Get_WorldMatrix());
			m_pTrailHandR->SetColor(XMVectorSet(1.f, 0.094f, 0.094f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 0.f));
			m_pTrailHandR->Tick(fTimeDelta);

			m_vUpperPos = { m_pTrailHandL1Matrix->m[3][0], m_pTrailHandL1Matrix->m[3][1] , m_pTrailHandL1Matrix->m[3][2] };
			m_vLowerPos = { m_pTrailHandL2Matrix->m[3][0], m_pTrailHandL2Matrix->m[3][1] , m_pTrailHandL2Matrix->m[3][2] };
			m_pTrailHandL->UpdateTrail(m_vUpperPos, m_vLowerPos, m_pOwnerTransform.lock()->Get_WorldMatrix());
			m_pTrailHandL->SetColor(XMVectorSet(1.f, 0.094f, 0.094f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 0.f));
			m_pTrailHandL->Tick(fTimeDelta);

			m_vUpperPos = { m_pTrailHandR2Matrix->m[3][0], m_pTrailHandR2Matrix->m[3][1] , m_pTrailHandR2Matrix->m[3][2] };
			m_vLowerPos = { m_pTrailFootRMatrix->m[3][0], m_pTrailFootRMatrix->m[3][1] , m_pTrailFootRMatrix->m[3][2] };
			m_pTrailFootR->UpdateTrail(m_vUpperPos, m_vLowerPos, m_pOwnerTransform.lock()->Get_WorldMatrix());
			m_pTrailFootR->SetColor(XMVectorSet(1.f, 0.094f, 0.094f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 0.f));
			m_pTrailFootR->Tick(fTimeDelta);

			m_vUpperPos = { m_pTrailHandL2Matrix->m[3][0], m_pTrailHandL2Matrix->m[3][1] , m_pTrailHandL2Matrix->m[3][2] };
			m_vLowerPos = { m_pTrailFootLMatrix->m[3][0], m_pTrailFootLMatrix->m[3][1] , m_pTrailFootLMatrix->m[3][2] };
			m_pTrailHandL->UpdateTrail(m_vUpperPos, m_vLowerPos, m_pOwnerTransform.lock()->Get_WorldMatrix());
			m_pTrailHandL->SetColor(XMVectorSet(1.f, 0.094f, 0.094f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 0.f));
			m_pTrailHandL->Tick(fTimeDelta);
		}

		if (1 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
		{
			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_TRIPLEPUT_RUSH_LOOP - (_uint)STATE_ARMOR_END - 1, false, 2.3f, 0.0f, 0);
		}

		_vector vCurPos = m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION);
		_float fDist = XMVectorGetX(XMVector4Length(m_vDest - vCurPos));

		if (m_fPrevDist <= fDist)
		{
			m_iFarCnt++;

			if (13 < m_iFarCnt)
			{
				m_isIn = false;
				m_isLoop = false;
				m_isOut = true;

				m_pOwnerAttackCollider.lock()->SetActive(false);
				m_pOwnerAttackCollider.lock()->SetOnCollide(false);

				CSound_Manager::GetInstance()->StopSound(CSound_Manager::CHANNELID::MONSTER2_ATTACK);
				m_pOwnerModel.lock()->Set_Animation((_uint)STATE_TRIPLEPUT_RUSH_OUT - (_uint)STATE_ARMOR_END - 1, false, 2.3f, 0.0f, 0);
				return STATE::STATE_TRIPLEPUT_RUSH;
			}
		}

		m_fPrevDist = fDist;

		/*if (1.f >= XMVectorGetX(XMVector4Length(m_vDest - vCurPos)))
		{
			m_isIn = false;
			m_isLoop = false;
			m_isOut = true;

			m_pOwnerModel.lock()->Set_Animation((_uint)STATE_TRIPLEPUT_RUSH_OUT - (_uint)STATE_ARMOR_END - 1, false, 2.3f, 0.0f, 0);
		}*/

		return STATE::STATE_TRIPLEPUT_RUSH;
	}
	else if (m_isOut)
	{
		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_isIn = false;
			m_isLoop = false;
			m_isOut = false;

			return STATE::STATE_TRIPLEPUT_IDLE;
		}
	}

	return STATE::STATE_TRIPLEPUT_RUSH;
}

void CState_TriplePut_Rush::SetTrail()
{
	m_pTrailHandR1Matrix = m_pOwnerModel.lock()->Get_CombinedBoneMatrixPtr("FX_PHY_hand_01_r");
	m_pTrailHandR2Matrix = m_pOwnerModel.lock()->Get_CombinedBoneMatrixPtr("FX_PHY_hand_02_r");

	m_pTrailHandL1Matrix = m_pOwnerModel.lock()->Get_CombinedBoneMatrixPtr("FX_PHY_hand_01_l");
	m_pTrailHandL2Matrix = m_pOwnerModel.lock()->Get_CombinedBoneMatrixPtr("FX_PHY_hand_02_l");

	m_pTrailFootRMatrix = m_pOwnerModel.lock()->Get_CombinedBoneMatrixPtr("FX_foot_l");
	m_pTrailFootLMatrix = m_pOwnerModel.lock()->Get_CombinedBoneMatrixPtr("FX_foot_r");

	m_pTrailHandL = CRushTrail::Create(m_pDevice, m_pContext, L"Prototype_Component_Texture_Mask_Rush");
	m_pTrailHandR = CRushTrail::Create(m_pDevice, m_pContext, L"Prototype_Component_Texture_Mask_Rush");
	m_pTrailFootL = CRushTrail::Create(m_pDevice, m_pContext, L"Prototype_Component_Texture_Mask_Rush");
	m_pTrailFootR = CRushTrail::Create(m_pDevice, m_pContext, L"Prototype_Component_Texture_Mask_Rush");
}

shared_ptr<CState> CState_TriplePut_Rush::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_TriplePut_Rush> pInstance = make_shared<CState_TriplePut_Rush>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_TriplePut_Rush");
		pInstance.reset();
	}

	return pInstance;
}

void CState_TriplePut_Rush::Free()
{
}
