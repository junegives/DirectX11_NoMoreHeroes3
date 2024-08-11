#include "pch.h"
#include "State_Attack.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"

#include "Player.h"
#include "Weapon.h"
#include "Camera_Attach.h"

CState_Attack::CState_Attack(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Attack::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	m_pWeapon = dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->GetWeapon();

	return S_OK;
}

STATE CState_Attack::Tick(_float fTimeDelta)
{
	m_iCallCnt++;

	if (dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->IsHit())
	{
		dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetHit(false);
		return STATE::STATE_HIT;
	}

	m_eState = Key_Input(fTimeDelta);

	Combo_Check();

	return m_eState;
}

STATE CState_Attack::Late_Tick(_float fTimeDelta)
{
	return m_eState;
}

void CState_Attack::Reset_State()
{
	m_isAttack2 = false;
	m_isAttack3 = false;
	m_isAttack4 = false;
	m_isAttack5 = false;

	m_pOwnerAttackCollider.lock()->SetOnCollide(false);
	//m_pWeapon->StopTrail();
}

void CState_Attack::Enter_State()
{
	dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_ATTACK, false, 4.0f, 0.1f, 0);
	m_eState = STATE::STATE_ATTACK;

	m_isAttack1 = true;
	m_isAttack2 = false;
	m_isAttack3 = false;
	m_isAttack4 = false;
	m_isAttack5 = false;

	m_isStackCombo = false;
	m_isAttackColSet = false;

	m_pTarget = dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SearchTarget();
	//m_pWeapon->StartTrail(L"", L"1", {0.f, 0.f, 1.f, 1.f}, 100);
}

STATE CState_Attack::Key_Input(_float fTimeDelta)
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

	// 타격 타이밍 조절
	if (!m_isAttackColSet && !m_isStackCombo)
	{
		// 콤보4 시전중
		if (m_isAttack5)
		{
			// 타이밍
			if (0.25 < m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
			{
				CSound_Manager::GetInstance()->StopSound(CSound_Manager::CHANNELID::PLAYER_ATTACK);
				CSound_Manager::GetInstance()->PlaySound(L"vo_TRV_ATK_M_Tah_02.wav", CSound_Manager::CHANNELID::PLAYER_ATTACK, 1.f);
				m_isAttackColSet = true;
				m_pOwnerAttackCollider.lock()->SetOnCollide(true);
				m_pOwnerAttackCollider.lock()->SetHitType(HIT_HEAVY);
				m_pOwnerAttackCollider.lock()->SetAP(2);
				m_pWeapon->Set_AttackType(3);
			}
		}
		// 콤보4 시전중
		if (m_isAttack4)
		{
			// 타이밍
			if (0.1 < m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
			{
				CSound_Manager::GetInstance()->StopSound(CSound_Manager::CHANNELID::PLAYER_ATTACK);
				CSound_Manager::GetInstance()->PlaySound(L"vo_TRV_ATK_M_Yah_00.wav", CSound_Manager::CHANNELID::PLAYER_ATTACK, 1.f);
				m_isAttackColSet = true;
				m_pOwnerAttackCollider.lock()->SetOnCollide(true);
				m_pOwnerAttackCollider.lock()->SetHitType(HIT_DEFAULT);
				m_pOwnerAttackCollider.lock()->SetAP(2);
				m_pWeapon->Set_AttackType(2);
			}
		}
		// 콤보3 시전중
		else if (m_isAttack3)
		{
			// 타이밍
			if (0.1 < m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
			{
				CSound_Manager::GetInstance()->StopSound(CSound_Manager::CHANNELID::PLAYER_ATTACK);
				CSound_Manager::GetInstance()->PlaySound(L"vo_TRV_ATK_S_Hu_02.wav", CSound_Manager::CHANNELID::PLAYER_ATTACK, 1.f);
				m_isAttackColSet = true;
				m_pOwnerAttackCollider.lock()->SetOnCollide(true);
				m_pOwnerAttackCollider.lock()->SetHitType(HIT_DEFAULT);
				m_pOwnerAttackCollider.lock()->SetAP(1);
				m_pWeapon->Set_AttackType(0);
			}
		}
		// 콤보2 시전중
		else if (m_isAttack2)
		{
			// 타이밍
			if (0.1 < m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
			{
				CSound_Manager::GetInstance()->StopSound(CSound_Manager::CHANNELID::PLAYER_ATTACK);
				CSound_Manager::GetInstance()->PlaySound(L"vo_TRV_ATK_S_Ha_02.wav", CSound_Manager::CHANNELID::PLAYER_ATTACK, 1.f);
				m_isAttackColSet = true;
				m_pOwnerAttackCollider.lock()->SetOnCollide(true);
				m_pOwnerAttackCollider.lock()->SetHitType(HIT_DEFAULT);
				m_pOwnerAttackCollider.lock()->SetAP(1);
				m_pWeapon->Set_AttackType(1);
			}
		}
		// 콤보1 시전중
		else if (m_isAttack1)
		{
			// 타이밍
			if (0.1 < m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
			{
				CSound_Manager::GetInstance()->StopSound(CSound_Manager::CHANNELID::PLAYER_ATTACK);
				CSound_Manager::GetInstance()->PlaySound(L"vo_TRV_ATK_S_Cha_10.wav", CSound_Manager::CHANNELID::PLAYER_ATTACK, 1.f);
				m_isAttackColSet = true;
				m_pOwnerAttackCollider.lock()->SetOnCollide(true);
				m_pOwnerAttackCollider.lock()->SetHitType(HIT_DEFAULT);
				m_pOwnerAttackCollider.lock()->SetAP(1);
				m_pWeapon->Set_AttackType(0);
			}
		}
	}

	Block_RootMotion();

	// 이미 다음 콤보 쌓인 경우
	if (m_isStackCombo)
		return STATE::STATE_ATTACK;

	// 클릭 체크
	if (!m_isAttack5 && m_pGameInstance->Get_DIMouseState(MOUSEKEYSTATE::DIM_LB) & 0x80)
	{
		// 타이밍 체크
		if (0.2 < m_pOwnerModel.lock()->Get_CurKeyFrameRatio() && 0.6 > m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
		{
			if (!m_isBlocked) Change_Look();

			// 콤보4 시전중
			if (m_isAttack4)
			{
				m_isAttackColSet = false;
				m_isStackCombo = true;
				m_isAttack5 = true;

				m_isAttack4 = false;
				m_isAttack3 = false;
				m_isAttack2 = false;
				m_isAttack1 = false;
			}
			// 콤보3 시전중
			else if (m_isAttack3)
			{
				m_isAttackColSet = false;
				m_isStackCombo = true;
				m_isAttack4 = true;

				m_isAttack3 = false;
				m_isAttack2 = false;
				m_isAttack1 = false;
			}
			// 콤보2 시전중
			else if (m_isAttack2)
			{
				m_isAttackColSet = false;
				m_isStackCombo = true;
				m_isAttack3 = true;

				m_isAttack2 = false;
				m_isAttack1 = false;
			}
			// 콤보1 시전중
			else if (m_isAttack1)
			{
				m_isAttackColSet = false;
				m_isStackCombo = true;
				m_isAttack2 = true;
				m_isAttack1 = false;
			}
		}
		return STATE::STATE_ATTACK;
	}

	// 클릭도 안되고, 현재 애니메이션 끝난 경우
	else if (0.8 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
		return STATE::STATE_IDLE;

	// 클릭도 안되고, 애니메이션도 아직 안끝난 경우
	else
		return STATE::STATE_ATTACK;
}

void CState_Attack::Lock_On()
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

//void CState_Attack::Block_Check()
//{
//	if (m_pOwnerModel.lock()->isAnimStop())
//	{
//		std::wstring outputString = L"Value: " + std::to_wstring(m_iCallCnt) + L" : ";
//		OutputDebugStringW(outputString.c_str());
//
//		OutputDebugString(TEXT("isAnimStop() : true\n"));
//		m_isBlocked = false;
//	}
//
//	else
//	{
//		m_isBlocked = true;
//	}
//}

STATE CState_Attack::Combo_Check()
{
	// 다음 콤보 실행 예정
	if (m_isStackCombo)
	{
		// 다음 콤보 실행 가능 타이밍
		if (0.5 < m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
		{
			m_isStackCombo = false;

			if (m_isAttack5)
			{
				m_pOwnerModel.lock()->Set_Animation((_uint)STATE_ATTACK5, false, 2.0f, 0.1f, 0);
			}

			if (m_isAttack4)
			{
				m_pOwnerModel.lock()->Set_Animation((_uint)STATE_ATTACK4, false, 2.5f, 0.1f, 0);
			}

			if (m_isAttack3)
			{
				m_pOwnerModel.lock()->Set_Animation((_uint)STATE_ATTACK3, false, 3.0f, 0.1f, 0);
			}

			if (m_isAttack2)
			{
				m_pOwnerModel.lock()->Set_Animation((_uint)STATE_ATTACK2, false, 4.0f, 0.1f, 0);
			}
		}
	}

	return m_eState;
}

void CState_Attack::Block_RootMotion()
{
	m_isBlocked = false;

	_vector vDir = { 0.f, 0.f, 0.f, 0.f };

	if (m_pTarget)
	{
		vDir = m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION) - m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION);

		// 타겟이 사정거리 안에 있으면 무조건 타겟 방향으로 look 설정
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

void CState_Attack::Change_Look()
{
	m_isPressed = false;

	_vector vDir = { 0.f, 0.f, 0.f, 0.f };

	shared_ptr<CGameObject> pCam = m_pGameInstance->GetCamera(CCamera_Manager::CAM_ATTACH);
	_vector vCamLook = pCam->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	vCamLook.m128_f32[1] = 0.f;
	_vector vCamRight = pCam->Get_TransformCom()->Get_State(CTransform::STATE_RIGHT);
	vCamRight.m128_f32[1] = 0.f;

	// Go Straight 하기 (플레이어 look 기준)
	/*
		Go_Straight 호출하면서 Navigation Component 가져와서 같이 넘겨주기
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
		//vDir = XMVectorLerp(m_pOwnerTransform.lock()->Get_State(CTransform::STATE_LOOK), vDir, 0.3);

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

shared_ptr<CState> CState_Attack::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Attack> pInstance = make_shared<CState_Attack>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Run");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Attack::Free()
{
}
