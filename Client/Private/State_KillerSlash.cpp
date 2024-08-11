#include "pch.h"
#include "State_KillerSlash.h"

#include "GameObject.h"
#include "Transform.h"
#include "Player.h"
#include "Body_Player.h"

CState_KillerSlash::CState_KillerSlash(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_KillerSlash::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_KillerSlash::Tick(_float fTimeDelta)
{
	// 타격 타이밍
	if (!m_isAttackColSet && 0.3 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
	{
		m_pOwnerAttackCollider.lock()->SetOnCollide(true);
		m_pOwnerAttackCollider.lock()->SetHitType(HIT_HEAVY);
		m_pOwnerAttackCollider.lock()->SetAP(5);
		m_isAttackColSet = true;
	}

	// 할게 없을 것 같다...???

	// 그냥 애니메이션 끝나면 IDLE 리턴해주면 될듯?


	//m_eState = Key_Input(fTimeDelta);
	if (m_pOwnerModel.lock()->isAnimStop())
	{
		m_pOwnerCollider.lock()->SetOnCollide(true);
		return STATE::STATE_IDLE;
	}

	else
		return m_eState;
}

STATE CState_KillerSlash::Late_Tick(_float fTimeDelta)
{

	return m_eState;
}

void CState_KillerSlash::Reset_State()
{
	m_pOwnerAttackCollider.lock()->SetOnCollide(false);
}

void CState_KillerSlash::Enter_State()
{
	m_isRandom = !m_isRandom;

	STATE eRandomState = m_isRandom ? STATE_KILLERSLASH : STATE_KILLERSLASH_L;
	dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)eRandomState, false, 1.0f, 0.1f, 0);
	m_eState = eRandomState;

	// 몬스터 앞으로 위치 보정해주기
	m_pTarget = dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SearchTarget();

	m_pOwnerTransform.lock()->Set_State(CTransform::STATE_POSITION, m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION) + (XMVector4Normalize(m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_LOOK)) * 3.f));
	m_pOwnerTransform.lock()->SetLook_ForLandObject(-m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_LOOK));

	// 얘는 왜있는거지?
	m_pOwnerCollider.lock()->SetOnCollide(false);

	m_isAttackColSet = false;
}

STATE CState_KillerSlash::Key_Input(_float fTimeDelta)
{
	return m_eState;
}

shared_ptr<CState> CState_KillerSlash::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_KillerSlash> pInstance = make_shared<CState_KillerSlash>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Dodge");
		pInstance.reset();
	}

	return pInstance;
}

void CState_KillerSlash::Free()
{
}
