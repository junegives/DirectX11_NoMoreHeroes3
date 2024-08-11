#include "pch.h"
#include "State_DashAttack.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"
#include "Player.h"
#include "Player.h"

CState_DashAttack::CState_DashAttack(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_DashAttack::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_DashAttack::Tick(_float fTimeDelta)
{
	// 타격 타이밍
	if (!m_isAttackColSet && 0.2 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
	{
		CSound_Manager::GetInstance()->StopSound(CSound_Manager::CHANNELID::PLAYER_ATTACK);
		CSound_Manager::GetInstance()->PlaySound(L"Player_Attack_Voice_H.wav", CSound_Manager::CHANNELID::PLAYER_ATTACK, 1.f);
		m_pOwnerAttackCollider.lock()->SetOnCollide(true);
		m_pOwnerAttackCollider.lock()->SetHitType(HIT_HEAVY);
		m_pOwnerAttackCollider.lock()->SetAP(5);
		m_isAttackColSet = true;
	}

	if (m_pOwnerModel.lock()->isAnimStop())
	{
		m_pOwnerCollider.lock()->SetOnCollide(true);
		return STATE::STATE_IDLE;
	}

	else
		return m_eState;
}

STATE CState_DashAttack::Late_Tick(_float fTimeDelta)
{

	return m_eState;
}

void CState_DashAttack::Reset_State()
{

	m_pOwnerAttackCollider.lock()->SetOnCollide(false);
}

void CState_DashAttack::Enter_State()
{
	dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_DASHATTACK, false, 2.0f, 0.05f, 0);
	m_eState = STATE_DASHATTACK;

	// 몬스터 앞으로 위치 보정해주기
	m_pTarget = dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SearchTarget();

	m_pOwnerTransform.lock()->Set_State(CTransform::STATE_POSITION, m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION) + (XMVector4Normalize(m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_LOOK)) * 5.3f));
	m_pOwnerTransform.lock()->SetLook_ForLandObject(-m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_LOOK));

	m_pOwnerCollider.lock()->SetOnCollide(false);

	m_isAttackColSet = false;
}

STATE CState_DashAttack::Key_Input(_float fTimeDelta)
{
	return m_eState;
}

shared_ptr<CState> CState_DashAttack::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_DashAttack> pInstance = make_shared<CState_DashAttack>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Dodge");
		pInstance.reset();
	}

	return pInstance;
}

void CState_DashAttack::Free()
{
}
