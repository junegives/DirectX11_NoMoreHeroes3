#include "pch.h"
#include "State_Guard.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"

#include "Player.h"
#include "Camera_Attach.h"

CState_Guard::CState_Guard(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Guard::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Guard::Tick(_float fTimeDelta)
{
	// �ǰݸ��
	if (dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->IsHit())
	{

		switch (dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->GetHitType())
		{
		case HIT_LIGHT:
		case HIT_DEFAULT:
		case HIT_HEAVY:
			dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation(STATE_GUARD_HIT, false, 2.0f, 0.1f, 0);
			m_isHit = true;

			break;
		case HIT_STUN:
			// ����� �ٲٰ� �Ǵ� ���� ����
			dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation(STATE_SKILLLOCK, false, 2.0f, 0.1f, 0);
			m_isHit = true;

			break;
		case HIT_KNOCKDOWN:
			// ��ǵ� �ٲٰ� �ǵ� ���
			dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetHit(false);
			dynamic_pointer_cast<CCamera_Attach>(m_pGameInstance->GetCamera(CCamera_Manager::CAM_ATTACH))->SetLock(false);
			return STATE::STATE_HIT;
			break;
		default:
			break;
		}

		dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetHit(false);
		m_pOwnerCollider.lock()->SetOnCollide(true);

		m_isF = false;
		m_isB = false;
		m_isL = false;
		m_isR = false;
	}



	// Shift�� ������ �������� ��� Ȯ���ؾ��ϰ�

	m_eState = Key_Input(fTimeDelta);

	return m_eState;
}

STATE CState_Guard::Late_Tick(_float fTimeDelta)
{

	return m_eState;
}

void CState_Guard::Reset_State()
{
}

void CState_Guard::Enter_State()
{
	// �ٰ� �־��� �� ���� ���� �ִϸ��̼� �ǳʶٰ� �ٷ� GuardWalk �ִϸ��̼�����
	if (STATE::STATE_RUN == m_pOwnerMachine.lock()->Get_PreState())
	{
		dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_GUARDWALK, true, 3.0f, 0.2f,  0);
		m_eState = STATE::STATE_GUARD;
		m_isGuardIn = false;
		m_isWalking = true;
	}

	// �켱 ���� ����(Guard In) �ִϸ��̼�����
	else
	{
		dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_GUARD, false, 5.0f, 0.1f, 0);
		m_eState = STATE::STATE_GUARD;
		m_isGuardIn = true;
		m_isWalking = false;
	}

	m_isF = false;
	m_isFL = false;
	m_isFR = false;
	m_isB = false;
	m_isBL = false;
	m_isBR = false;
	m_isL = false;
	m_isR = false;
	m_isWalkingOut = false;
	m_isGuardEnd = false;

	m_pTarget = dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SearchTarget();
}

STATE CState_Guard::Key_Input(_float fTimeDelta)
{
	if (m_isHit)
	{
		if (!m_pOwnerModel.lock()->isAnimStop())
		{
			Lock_On();
			return STATE::STATE_GUARD;
		}
		else
			m_isHit = false;
	}

	dynamic_pointer_cast<CCamera_Attach>(m_pGameInstance->GetCamera(CCamera_Manager::CAM_ATTACH))->SetLock(false);

	if (m_pGameInstance->Get_DIMouseState(MOUSEKEYSTATE::DIM_LB) & 0x80)
	{
		return STATE_ATTACK;
	}
	if (m_pGameInstance->Get_DIMouseState(MOUSEKEYSTATE::DIM_RB) & 0x80)
	{
		return STATE::STATE_HEAVYATTACK;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_1) & 0x80)
	{
		return STATE::STATE_KILLERSLASH;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_2) & 0x80)
	{
		return STATE::STATE_DEATHKICK;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_3) & 0x80)
	{
		return STATE::STATE_DASHATTACK;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_E) & 0x80)
	{
		return STATE::STATE_DODGE;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		return STATE::STATE_JUMP;
	}

	//// �����ٰ� �ٽ� ������ ��
	//if (m_pGameInstance->Is_KeyDown(DIK_LSHIFT))
	//{
	//	// �ٰ� �־��� ��
	//	if (m_isWalking)
	//	{
	//		dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_GUARDWALK, true, 2.0f, 0.2f,  0);
	//		return STATE::STATE_GUARD;
	//	}

	//	// �ȶٰ� �־��� ��
	//	else
	//	{
	//		dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_GUARD, false, 2.0f, 0.2f,  0);
	//		m_isGuardEnd = true;
	//		return STATE::STATE_GUARD;
	//		// return STATE::STATE_WALK_OUT;
	//	}
	//}

	// �̹� ����Ʈ ���� Guard End �ִϸ��̼� ���� ��
	if (m_isGuardEnd)
	{
		// Guard_Out�� �ִϸ��̼��� ������ �����ϰ� IDLE�� �Ѿ���Ѵ�.
		if (m_pOwnerModel.lock()->isAnimStop())
			return STATE::STATE_IDLE;

		return STATE::STATE_GUARD;
	}

	// ����Ʈ ���� ��
	if (m_pGameInstance->Is_KeyUp(DIK_LSHIFT))
	{
		m_isGuardIn = false;

		// �ٰ� �־��� ��
		if (m_isWalking)
		{
			m_isWalking = false;
			return STATE::STATE_RUN;
		}

		// �ȶٰ� �־��� ��
		else
		{
			dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_GUARD_OUT, false, 5.0f, 0.1f, 0);
			m_isGuardEnd = true;
			//return STATE::STATE_IDLE;
			 return STATE::STATE_GUARD;
		}
	}

	// �� ����Ʈ ������ ���� ��
	Lock_On();

	// ���� In ���� ���� �� (Idle ����) -> ����Ű ������ �������� GuardWalk�� �Ѿ��
	if (m_isGuardIn)
	{
		if (m_pOwnerModel.lock()->isAnimStop())
		{
			dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_GUARD_LOOP, true, 3.0f, 0.1f, 0);
			m_isGuardIn = false;
		}
	}

	// Walk���� �ƴ��� �Ǻ��ϱ�
	m_isPressed = false;
	m_isWalking = false;
	m_isUnPressed = false;

	// 4����Ű�� ������ ���� ��
	if (m_pGameInstance->Is_KeyPressing(DIK_A))
	{
		// ó�� ������ �� (�ִϸ��̼� ����)
		if (!m_isL)
		{
			dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_GUARDWALK_L, true, 3.0f, 0.1f, 0);
			m_isL = true;
		}
		m_isPressed = true;
	}

	if (m_pGameInstance->Is_KeyPressing(DIK_D))
	{
		// ó�� ������ �� (�ִϸ��̼� ����)
		if (!m_isR)
		{
			dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_GUARDWALK_R, true, 3.0f, 0.1f, 0);
			m_isR = true;
		}
		m_isPressed = true;
	}

	if (m_pGameInstance->Is_KeyPressing(DIK_S))
	{
		// ó�� ������ �� (�ִϸ��̼� ����)
		if (!m_isB)
		{
			dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_GUARDWALK, true, 3.0f, 0.1f, 0);
			m_isB = true;
		}
		m_isPressed = true;
	}

	if (m_pGameInstance->Is_KeyPressing(DIK_W))
	{
		// ó�� ������ �� (�ִϸ��̼� ����)
		if (!m_isF)
		{
			dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_GUARDWALK_F, true, 3.0f, 0.1f, 0);
			m_isF = true;
		}
		m_isPressed = true;
	}

	// 4����Ű ���� ��
	if (m_pGameInstance->Is_KeyUp(DIK_A))
	{
		m_isL = false;
		m_isUnPressed = true;
	}

	if (m_pGameInstance->Is_KeyUp(DIK_D))
	{
		m_isR = false;
		m_isUnPressed = true;
	}

	if (m_pGameInstance->Is_KeyUp(DIK_W))
	{
		m_isF = false;
		m_isUnPressed = true;
	}

	if (m_pGameInstance->Is_KeyUp(DIK_S))
	{
		m_isB = false;
		m_isUnPressed = true;
	}

	// ����Ű �� �ϳ��� ������ ���� ��
	// ���� �̵���Ű��
	if (m_isL || m_isR || m_isF || m_isB)
	{
		m_isWalking = true;

		if (m_isL)
		{		
			if (m_isF)
			{
				m_pOwnerTransform.lock()->Go_Straight(fTimeDelta * 0.5f * 0.5f, m_pNavigation);
				m_pOwnerTransform.lock()->Go_Left(fTimeDelta * 0.5f * 0.5f, m_pNavigation);
			}

			else if (m_isB)
			{
				m_pOwnerTransform.lock()->Go_Backward(fTimeDelta * 0.5f * 0.5f, m_pNavigation);
				m_pOwnerTransform.lock()->Go_Left(fTimeDelta * 0.5f * 0.5f, m_pNavigation);
			}

			else
				m_pOwnerTransform.lock()->Go_Left(fTimeDelta * 0.5f, m_pNavigation);
		}

		else if (m_isR)
		{
			if (m_isF)
			{
				m_pOwnerTransform.lock()->Go_Straight(fTimeDelta * 0.5f * 0.5f, m_pNavigation);
				m_pOwnerTransform.lock()->Go_Right(fTimeDelta * 0.5f * 0.5f, m_pNavigation);
			}

			else if (m_isB)
			{
				m_pOwnerTransform.lock()->Go_Backward(fTimeDelta * 0.5f * 0.5f, m_pNavigation);
				m_pOwnerTransform.lock()->Go_Right(fTimeDelta * 0.5f * 0.5f, m_pNavigation);
			}

			else
				m_pOwnerTransform.lock()->Go_Right(fTimeDelta * 0.5f, m_pNavigation);
		}

		else if (m_isF)
		{
			m_pOwnerTransform.lock()->Go_Straight(fTimeDelta * 0.5f, m_pNavigation);
		}

		else if (m_isB)
		{
			m_pOwnerTransform.lock()->Go_Backward(fTimeDelta * 0.5f, m_pNavigation);
		}

		return STATE::STATE_GUARD;
	}

	// ����Ű �ƹ��͵� �ȴ����� ��
	else
	{
		// �ƹ��͵� �ȴ�����, ��� ���� ��
		if (m_isUnPressed)
		{
			dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_GUARD_LOOP, true, 3.0f, 0.2f,  0);
		}

		m_isWalking = false;
		m_isPressed = false;
		return STATE::STATE_GUARD;
	}
	//	// ����Ʈ�� ������ ������ ����Ű�� ������ ���� ���� ��
	//	if (!m_isWalkingOut)
	//	{
	//		// ���� In ����� ��
	//		if (!m_isWalking)
	//		{
	//			// ���� In ��� �������� Ȯ���ϰ� �� �� true��
	//			if (m_pOwnerModel.lock()->isAnimStop())
	//			{
	//				dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_GUARD_LOOP, false, 3.0f, 0.1f, 0);
	//				m_isWalking = true;
	//			}
	//		}

	//		dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_GUARDWALK_OUT, false, 3.0f, 0.1f, 0);
	//		m_isWalkingOut = true;

	//		return STATE::STATE_GUARD;
	//	}

	//	return STATE::STATE_GUARD;
	//}

	//// ����Ʈ �ȴ����� ���� ��
	//if (m_isGuardEnd)
	//{
	//	m_isWalkingOut = false;
	//	if (m_pOwnerModel.lock()->isAnimStop())
	//		return STATE::STATE_IDLE;
	//}

	//else
	//{
	//	dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_GUARD_OUT, false, 8.0f, 0.1f, 0);
	//	m_isGuardEnd = true;

	//	return STATE::STATE_GUARD;
	//}
}

void CState_Guard::Lock_On()
{
	shared_ptr<CGameObject>		pCamera = m_pGameInstance->GetCamera(CCamera_Manager::CAM_ATTACH);
	_vector vDir = { 0.f, 0.f, 0.f, 0.f };

	if (m_pTarget)
	{
		vDir = m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION) - m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION);

		dynamic_pointer_cast<CCamera_Attach>(pCamera)->ChangeTargetCamLerp(XMVector3Normalize(vDir), 0.1f, m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));

		//_float3 targetSphereCoord = { /* ��ǥ ��ġ�� ���� ��ǥ �� */ };

		//// ���� ��ġ�� ��ǥ ��ġ�� ���� �����մϴ�.
		//_float3 interpolatedSphereCoord = Lerp(currentSphereCoord, targetSphereCoord, 0.01);

		//// ���� ��ǥ�� ī�׽þ� ��ǥ�� ��ȯ�մϴ�.
		//_float3 interpolatedCartesianCoord = m_pSphereicalCoord->Rotate(interpolatedSphereCoord.x, interpolatedSphereCoord.y).toCartesian();

		//// ��ǥ ��ġ���� �ε巴�� �̵��ϴ� ī�޶� ��ġ�� �����մϴ�.
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(interpolatedCartesianCoord.x, interpolatedCartesianCoord.y, interpolatedCartesianCoord.z, 1.f));
		//
		//pCamera->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, );
	}

	else
	{
		_vector vCamLook = pCamera->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);

		vCamLook.m128_f32[1] = 0.f;

		vDir += vCamLook;
	}

	// Lerp
	_float fRadian = XMVectorGetX(XMVector3Dot(m_pOwnerTransform.lock()->Get_State(CTransform::STATE_LOOK), vDir));

	if (-0.7 > fRadian)
	{
		vDir = XMVectorLerp(m_pOwnerTransform.lock()->Get_State(CTransform::STATE_LOOK), vDir, 0.5f);
		m_pOwnerTransform.lock()->SetLook_ForLandObject(XMVector3Normalize(vDir));
	}
	else if (-0.1 > fRadian)
	{
		vDir = XMVectorLerp(m_pOwnerTransform.lock()->Get_State(CTransform::STATE_LOOK), vDir, 0.3f);
		m_pOwnerTransform.lock()->SetLook_ForLandObject(XMVector3Normalize(vDir));
	}
	else
	{
		vDir = XMVectorLerp(m_pOwnerTransform.lock()->Get_State(CTransform::STATE_LOOK), vDir, 0.2f);
		m_pOwnerTransform.lock()->SetLook_ForLandObject(XMVector3Normalize(vDir));
	}
}

void CState_Guard::Check_Guard_State()
{
	_bool					m_isWalking = false;

	_bool					m_isF = false;
	_bool					m_isFL = false;
	_bool					m_isFR = false;

	_bool					m_isB = false;
	_bool					m_isBL = false;
	_bool					m_isBR = false;

	_bool					m_isL = false;
	_bool					m_isR = false;

	// ���� Ǯ�� ���
	_bool					m_isGuardEnd = false;

	// �Ȱ� �־��� ��
	if (m_isWalking)
	{

	}
	
	// ������ ���־��� ��
	{

	}
}

shared_ptr<CState> CState_Guard::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Guard> pInstance = make_shared<CState_Guard>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Guard");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Guard::Free()
{
}
