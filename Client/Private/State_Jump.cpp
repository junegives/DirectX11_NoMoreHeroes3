#include "pch.h"
#include "State_Jump.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"

#include "Player.h"
#include "Camera_Attach.h"

#include "Sound_Manager.h"

CState_Jump::CState_Jump(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Jump::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Jump::Tick(_float fTimeDelta)
{
	if (dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->IsHit())
	{
		dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetHit(false);
		return STATE::STATE_HIT;
	}

	m_eState = Key_Input(fTimeDelta);

	m_eState = Jump_Check();

	return m_eState;
}

STATE CState_Jump::Late_Tick(_float fTimeDelta)
{
	return m_eState;
}

void CState_Jump::Reset_State()
{
}

void CState_Jump::Enter_State()
{
	dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_JUMP, false, 5.0f, 0, 0);
	m_eState = STATE::STATE_JUMP;

	m_isIn = true;
	m_isLoop = false;
	m_isLanding = false;

	m_fJumpTime = 0.f;

	CSound_Manager::GetInstance()->StopSound(CSound_Manager::CHANNELID::PLAYER_STEP);
	CSound_Manager::GetInstance()->PlaySound(L"Travis_Jump.wav", CSound_Manager::CHANNELID::PLAYER_STEP, 1.f);
}

STATE CState_Jump::Key_Input(_float fTimeDelta)
{
	// 떨어지기 전 까지 입력받을 수 있는 건
	// 상하좌우 이동


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

	if (m_pGameInstance->Get_DIKeyState(DIK_1) & 0x80)
	{
		return STATE::STATE_KILLERSLASH;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_2) & 0x80)
	{
		return STATE::STATE_DEATHKICK;
	}

	m_isPressed = false;

	shared_ptr<CGameObject> pCam = m_pGameInstance->GetCamera(CCamera_Manager::CAM_ATTACH);
	_vector vCamLook = pCam->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	vCamLook.m128_f32[1] = 0.f;
	_vector vCamRight = pCam->Get_TransformCom()->Get_State(CTransform::STATE_RIGHT);
	vCamRight.m128_f32[1] = 0.f;
	_vector vDir = { 0.f, 0.f, 0.f, 0.f };

	if (GetKeyState('A') & 0x8000)
	{
		_matrix matY = XMMatrixRotationY(XMConvertToRadians(270.f));
		_vector vCamLeft = XMVector3TransformNormal(vCamLook, matY);
		vDir += vCamLeft;
		m_isPressed = true;
	}

	if (GetKeyState('D') & 0x8000)
	{
		vDir += vCamRight;
		m_isPressed = true;
	}

	if (GetKeyState('S') & 0x8000)
	{
		_matrix matY = XMMatrixRotationY(XMConvertToRadians(180.f));
		_vector vCamBack = XMVector3TransformNormal(vCamLook, matY);

		vDir += vCamBack;
		m_isPressed = true;
	}

	if (GetKeyState('W') & 0x8000)
	{
		vDir += vCamLook;
		m_isPressed = true;
	}

	if (m_isPressed)
	{
		vDir = XMVectorLerp(m_pOwnerTransform.lock()->Get_State(CTransform::STATE_LOOK), vDir, 0.1f);
		m_pOwnerTransform.lock()->SetLook_ForLandObject(XMVector3Normalize(vDir));
		m_pOwnerTransform.lock()->Go_Straight(fTimeDelta, m_pNavigation);

		return STATE::STATE_JUMP;
	}

	return STATE::STATE_JUMP;
}

void CState_Jump::Lock_On()
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

STATE CState_Jump::Jump_Check()
{
	if (m_isIn)
	{

		Jump();

		// 모션 끝나는지 체크하고 Loop 모션으로 넘어가
		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_isLoop = true;
			m_isIn = false; 
			dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_JUMP_LOOP, true, 5.0f, 0.0f, 0);
			return STATE::STATE_JUMP;
		}
	}

	else if (m_isLoop)
	{
		// 점프 코드 넣고, 바닥과 거리 계산해서 Landing 모션으로 넘어가
		if (0.3f > Jump() && m_isFalling)
		{
			m_isLanding = true;
			m_isLoop = false;
			if (m_isPressed)
				dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_JUMP_LANDING_RUN, false, 3.0f, 0.0f, 0);
			else
				dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_JUMP_LANDING, false, 5.0f, 0.0f, 0);

			CSound_Manager::GetInstance()->StopSound(CSound_Manager::CHANNELID::PLAYER_STEP);
			CSound_Manager::GetInstance()->PlaySound(L"foley_TRV_FT_Conc_Land_00.wav", CSound_Manager::CHANNELID::PLAYER_STEP, 1.f);
			return STATE::STATE_JUMP;
		}
	}

	else if (m_isLanding)
	{
		// 땅에 닿았는지 판단하고 IDLE로 변경
		if (0.5 < m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
		{
			return STATE::STATE_IDLE;
		}
	}

	return STATE::STATE_JUMP;
}

// 바닥과 남은 거리 리턴
_float CState_Jump::Jump()
{
	_vector	vCurPos = m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION);
	_float	fCurHeight = m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION).m128_f32[1];

	_float fChange = m_fMaxSpeed - 0.1f * m_fJumpTime * m_fJumpTime;

	if (fChange <= -0.65f)
		fChange = -0.65f;

	m_fJumpTime += 0.1f;

	if (fChange <= 0.f)
		m_isFalling = true;

	fCurHeight += fChange;

	vCurPos.m128_f32[1] = fCurHeight;

	m_pOwnerTransform.lock()->Set_State(CTransform::STATE_POSITION, vCurPos);

	_float fHeight = m_pNavigation->Compute_Height(vCurPos);

	return fCurHeight - fHeight;
}

shared_ptr<CState> CState_Jump::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Jump> pInstance = make_shared<CState_Jump>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Jump");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Jump::Free()
{
}
