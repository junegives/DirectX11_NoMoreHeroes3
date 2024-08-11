#include "pch.h"
#include "State_Throw.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"

#include "Player.h"
#include "Monster.h"
#include "Camera_Attach.h"

CState_Throw::CState_Throw(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Throw::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Throw::Tick(_float fTimeDelta)
{
	// 던지기 중에 맞았을 때
	if (dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->IsHit())
	{
		// 애니메이션은 유지하고, 데미지만 입히자.
		dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetDamage();
		dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetHit(false);
		m_pOwnerCollider.lock()->SetOnCollide(true);
	}
	
	// 할게 없을 것 같다...???
	// 몬스터에게 같은 동작 하도록

	// 그냥 애니메이션 끝나면 IDLE 리턴해주면 될듯?
	dynamic_pointer_cast<CCamera_Attach>(m_pGameInstance->GetCamera(CCamera_Manager::CAM_ATTACH))->SetLock(false);

	//m_eState = Key_Input(fTimeDelta);
	if (0.9 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
		return STATE::STATE_IDLE;

	else
		return STATE::STATE_THROW_LEOPARDON;
}

STATE CState_Throw::Late_Tick(_float fTimeDelta)
{

	return m_eState;
}

void CState_Throw::Reset_State()
{
	//m_pOwnerTransform.lock()->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.0f));
	m_pOwnerTransform.lock()->SetLook_ForLandObject(-m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_LOOK));
	m_pOwnerCollider.lock()->SetOnCollide(true);
}

void CState_Throw::Enter_State()
{
	dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_THROW_LEOPARDON, false, 3.0f, 0.1f, 0);
	m_eState = STATE::STATE_THROW_LEOPARDON;

	m_pTarget = dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SearchTarget();

	m_pOwnerTransform.lock()->Set_State(CTransform::STATE_POSITION, m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION) - (XMVector4Normalize(m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_LOOK)) * 1.3f));
	m_pOwnerTransform.lock()->SetLook_ForLandObject(m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_LOOK));

	m_pOwnerCollider.lock()->SetOnCollide(false);

	dynamic_pointer_cast<CMonster>(m_pTarget)->SetHit(true);
	dynamic_pointer_cast<CMonster>(m_pTarget)->SetHitType(HIT_TRHOW);
	dynamic_pointer_cast<CMonster>(m_pTarget)->SetAP(5);
}

shared_ptr<CState> CState_Throw::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Throw> pInstance = make_shared<CState_Throw>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Throw");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Throw::Free()
{
}
