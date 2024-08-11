#include "pch.h"
#include "State_Vehicle_Jump.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"

#include "Player.h"
#include "Camera_Attach.h"

CState_Vehicle_Jump::CState_Vehicle_Jump(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Vehicle_Jump::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Vehicle_Jump::Tick(_float fTimeDelta)
{
	m_eState = Jump_Check();

	return m_eState;
}

STATE CState_Vehicle_Jump::Late_Tick(_float fTimeDelta)
{
	return m_eState;
}

void CState_Vehicle_Jump::Reset_State()
{
}

void CState_Vehicle_Jump::Enter_State()
{
	dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_JUMP, false, 5.0f, 0, 0);
	dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetJump(false);
	m_eState = STATE::STATE_VEHICLE_FALL;

	m_isIn = true;
	m_isLoop = false;
	m_isLanding = false;

	m_vStartHeight = m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION).m128_f32[1];

	m_fJumpTime = 0.f;
}

STATE CState_Vehicle_Jump::Jump_Check()
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
			return STATE::STATE_VEHICLE_FALL;
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
			return STATE::STATE_VEHICLE_FALL;
		}
	}

	else if (m_isLanding)
	{
		// 땅에 닿았는지 판단하고 IDLE로 변경
		if (0.15 < m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
		{
			return STATE::STATE_VEHICLE_RUN;
		}
	}

	return STATE::STATE_VEHICLE_FALL;
}

// 바닥과 남은 거리 리턴
_float CState_Vehicle_Jump::Jump()
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

	return fCurHeight - m_vStartHeight;
}

shared_ptr<CState> CState_Vehicle_Jump::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Vehicle_Jump> pInstance = make_shared<CState_Vehicle_Jump>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Vehicle_Jump");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Vehicle_Jump::Free()
{
}
