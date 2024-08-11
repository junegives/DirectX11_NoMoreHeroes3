#include "pch.h"
#include "State_HeavyAttack.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"

#include "Player.h"
#include "Camera_Attach.h"

CState_HeavyAttack::CState_HeavyAttack(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_HeavyAttack::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_HeavyAttack::Tick(_float fTimeDelta)
{
	if (dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->IsHit())
	{
		dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetHit(false);
		return STATE::STATE_HIT;
	}

	m_eState = Key_Input(fTimeDelta);

	Combo_Check();

	return m_eState;
}

STATE CState_HeavyAttack::Late_Tick(_float fTimeDelta)
{
	return m_eState;
}

void CState_HeavyAttack::Reset_State()
{
	m_isAttack2 = false;
	m_isAttack3 = false;

	m_pOwnerAttackCollider.lock()->SetOnCollide(false);
}

void CState_HeavyAttack::Enter_State()
{
	dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_HEAVYATTACK, false, 3.0f, 0.1f, 0);
	m_eState = STATE::STATE_HEAVYATTACK;

	m_isAttack1 = true;
	m_isAttack2 = false;
	m_isAttack3 = false;

	m_isStackCombo = false;
	m_isAttackColSet = false;

	m_pTarget = dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SearchTarget();
}

STATE CState_HeavyAttack::Key_Input(_float fTimeDelta)
{
	if (m_pGameInstance->Is_KeyPressing(DIK_LSHIFT))
	{
		Lock_On();
		m_isLockEnter = false;
	}
	else
	{
		m_isLockEnter = true;
		dynamic_pointer_cast<CCamera_Attach>(m_pGameInstance->GetCamera(CCamera_Manager::CAM_ATTACH))->SetLock(false);
	}

	// Ÿ�� Ÿ�̹� ����
	if (!m_isAttackColSet && !m_isStackCombo)
	{
		// �޺�3 ������
		if (m_isAttack3)
		{
			// Ÿ�̹�
			if (0.1 < m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
			{
				m_isAttackColSet = true;
				m_pOwnerAttackCollider.lock()->SetOnCollide(true);
				m_pOwnerAttackCollider.lock()->SetHitType(HIT_HEAVY);
				m_pOwnerAttackCollider.lock()->SetAP(3);
			}
		}
		// �޺�2 ������
		else if (m_isAttack2)
		{
			// Ÿ�̹�
			if (0.1 < m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
			{
				m_isAttackColSet = true;
				m_pOwnerAttackCollider.lock()->SetOnCollide(true);
				m_pOwnerAttackCollider.lock()->SetHitType(HIT_HEAVY);
				m_pOwnerAttackCollider.lock()->SetAP(2);
			}
		}
		// �޺�1 ������
		else if (m_isAttack1)
		{
			// Ÿ�̹�
			if (0.1 < m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
			{
				CSound_Manager::GetInstance()->StopSound(CSound_Manager::CHANNELID::PLAYER_ATTACK);
				CSound_Manager::GetInstance()->PlaySound(L"vo_TRV_ATK_M_Yah_01.wav", CSound_Manager::CHANNELID::PLAYER_ATTACK, 1.f);
				m_isAttackColSet = true;
				m_pOwnerAttackCollider.lock()->SetOnCollide(true);
				m_pOwnerAttackCollider.lock()->SetHitType(HIT_HEAVY);
				m_pOwnerAttackCollider.lock()->SetAP(2);
			}
		}
	}

	Block_RootMotion();

	// �̹� ���� �޺� ���� ���
	if (m_isStackCombo)
		return STATE::STATE_HEAVYATTACK;

	// Ŭ�� üũ
	if (!m_isAttack3 && m_pGameInstance->Get_DIMouseState(MOUSEKEYSTATE::DIM_RB) & 0x80)
	{
		// Ÿ�̹� üũ
		if (0.2 < m_pOwnerModel.lock()->Get_CurKeyFrameRatio() && 0.6 > m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
		{
			if (!m_isBlocked) Change_Look();

			// �޺�3 ������
			if (m_isAttack3)
			{
				m_isAttackColSet = false;

				m_isStackCombo = false;

				m_isAttack3 = false;
				m_isAttack2 = false;
				m_isAttack1 = false;
			}
			// �޺�2 ������
			else if (m_isAttack2)
			{
				CSound_Manager::GetInstance()->StopSound(CSound_Manager::CHANNELID::PLAYER_ATTACK);
				CSound_Manager::GetInstance()->PlaySound(L"Player_Attack_Voice_H.wav", CSound_Manager::CHANNELID::PLAYER_ATTACK, 1.f);
				m_isAttackColSet = false;

				m_isStackCombo = true;
				m_isAttack3 = true;

				m_isAttack2 = false;
				m_isAttack1 = false;
			}
			// �޺�1 ������
			else if (m_isAttack1)
			{
				CSound_Manager::GetInstance()->StopSound(CSound_Manager::CHANNELID::PLAYER_ATTACK);
				CSound_Manager::GetInstance()->PlaySound(L"vo_TRV_ATK_M_Tah_01.wav", CSound_Manager::CHANNELID::PLAYER_ATTACK, 1.f);
				m_isAttackColSet = false;

				m_isStackCombo = true;
				m_isAttack2 = true;
				m_isAttack1 = false;
			}
		}

		m_isBlocked = true;
		return STATE::STATE_HEAVYATTACK;
	}

	// Ŭ���� �ȵǰ�, ���� �ִϸ��̼� ���� ���
	else if (0.8 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
		return STATE::STATE_IDLE;

	// Ŭ���� �ȵǰ�, �ִϸ��̼ǵ� ���� �ȳ��� ���
	else
		return STATE::STATE_HEAVYATTACK;
}

void CState_HeavyAttack::Lock_On()
{
	if (m_isLockEnter)
	{
		m_pTarget = dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SearchTarget();
	}

	shared_ptr<CGameObject>		pCamera = m_pGameInstance->GetCamera(CCamera_Manager::CAM_ATTACH);
	_vector vDir = { 0.f, 0.f, 0.f, 0.f };

	if (m_pTarget)
	{
		vDir = m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION) - m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION);

		dynamic_pointer_cast<CCamera_Attach>(pCamera)->ChangeTargetCamLerp(XMVector3Normalize(vDir), 0.1f, m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));
	}
}

STATE CState_HeavyAttack::Combo_Check()
{
	// ���� �޺� ���� ����
	if (m_isStackCombo)
	{
		// ���� �޺� ���� ���� Ÿ�̹�
		if (0.5 < m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
		{
			m_isStackCombo = false;

			if (m_isAttack3)
			{
				m_pOwnerModel.lock()->Set_Animation((_uint)STATE_HEAVYATTACK3, false, 2.5f, 0.1f, 0);
			}

			if (m_isAttack2)
			{
				m_pOwnerModel.lock()->Set_Animation((_uint)STATE_HEAVYATTACK2, false, 3.0f, 0.1f, 0);
			}
		}
	}

	return m_eState;
}

void CState_HeavyAttack::Block_RootMotion()
{
	m_isBlocked = false;

	_vector vDir = { 0.f, 0.f, 0.f, 0.f };

	if (m_pTarget)
	{
		vDir = m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION) - m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION);

		// Ÿ���� �����Ÿ� �ȿ� ������ ������ Ÿ�� �������� look ����
		if (10.f >= XMVectorGetX(XMVector4Length(vDir)))
		{
			m_pOwnerTransform.lock()->SetLook_ForLandObject(XMVector3Normalize(vDir));

			_float fTestPlayerRadius = dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->GetColRadius();
			_float fTestMonRadius = dynamic_pointer_cast<CLandObject>(m_pTarget)->GetColRadius();
			_float fTestDistance = XMVectorGetX(XMVector4Length(vDir));


			if ((dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->GetColRadius() +
				dynamic_pointer_cast<CLandObject>(m_pTarget)->GetColRadius()) >= XMVectorGetX(XMVector4Length(vDir)))
			{
				m_isBlocked = true;
				m_pOwnerModel.lock()->SetBlockRootMotion(true);
			}
		}
	}
}


void CState_HeavyAttack::Change_Look()
{
	m_isPressed = false;

	shared_ptr<CGameObject> pCam = m_pGameInstance->GetCamera(CCamera_Manager::CAM_ATTACH);
	_vector vCamLook = pCam->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	vCamLook.m128_f32[1] = 0.f;
	_vector vCamRight = pCam->Get_TransformCom()->Get_State(CTransform::STATE_RIGHT);
	vCamRight.m128_f32[1] = 0.f;
	_vector vDir = { 0.f, 0.f, 0.f, 0.f };

	// Go Straight �ϱ� (�÷��̾� look ����)
	/*
		Go_Straight ȣ���ϸ鼭 Navigation Component �����ͼ� ���� �Ѱ��ֱ�
	*/

	if (m_pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		_matrix matY = XMMatrixRotationY(XMConvertToRadians(270.f));
		_vector vCamLeft = XMVector3TransformNormal(vCamLook, matY);
		vDir += vCamLeft;

		m_isPressed = true;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
	{
		vDir += vCamRight;

		m_isPressed = true;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
	{
		_matrix matY = XMMatrixRotationY(XMConvertToRadians(180.f));
		_vector vCamBack = XMVector3TransformNormal(vCamLook, matY);

		vDir += vCamBack;

		m_isPressed = true;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		vDir += vCamLook;

		m_isPressed = true;
	}

	if (m_isPressed)
	{
		_float fRadian = XMVectorGetX(XMVector3Dot(m_pOwnerTransform.lock()->Get_State(CTransform::STATE_LOOK), vDir));


		if (-0.6 > fRadian)
		{
			vDir = XMVectorLerp(m_pOwnerTransform.lock()->Get_State(CTransform::STATE_LOOK), vDir, 0.5f);
		}
		else if (-0.1 > fRadian)
		{
			vDir = XMVectorLerp(m_pOwnerTransform.lock()->Get_State(CTransform::STATE_LOOK), vDir, 0.4f);
		}
		else
		{
			vDir = XMVectorLerp(m_pOwnerTransform.lock()->Get_State(CTransform::STATE_LOOK), vDir, 0.3f);
		}

		m_pOwnerTransform.lock()->SetLook_ForLandObject(XMVector3Normalize(vDir));
	}
}

shared_ptr<CState> CState_HeavyAttack::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_HeavyAttack> pInstance = make_shared<CState_HeavyAttack>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Run");
		pInstance.reset();
	}

	return pInstance;
}

void CState_HeavyAttack::Free()
{
}
