#include "pch.h"
#include "State_Transform.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"

#include "Player.h"

CState_Transform::CState_Transform(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Transform::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Transform::Tick(_float fTimeDelta)
{
	if (!m_isChanged && 0.4 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
	{
		dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->ChangeModel(true);
		m_isChanged = true;
	}

	//m_eState = Key_Input(fTimeDelta);
	if (m_pOwnerModel.lock()->isAnimStop())
	{
		m_pOwnerCollider.lock()->SetOnCollide(true);

		dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->PlayAllModel(false);
		dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_IDLE, true, 2.0f, 0.1f, 0);
		return STATE::STATE_ARMOR_MISSILE_IN;
	}

	else
		return STATE::STATE_TRANSFORM;
}

STATE CState_Transform::Late_Tick(_float fTimeDelta)
{

	return m_eState;
}

void CState_Transform::Reset_State()
{
}

void CState_Transform::Enter_State()
{
	dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->PlayAllModel(true);

	dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_TRANSFORM, false, 3.0f, 0.2f,  0);
	dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation_TransModel((_uint)STATE_ARMOR_TRANSFORM - ((_uint)STATE_PLAYER_END + 1), false, 3.0f, 0.2f,  0);
	m_eState = STATE::STATE_TRANSFORM;
	m_isChanged = false;

	m_pOwnerCollider.lock()->SetOnCollide(false);
}

STATE CState_Transform::Key_Input(_float fTimeDelta)
{
	if (m_pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		return STATE::STATE_JUMP;
	}

	if (m_pGameInstance->Get_DIMouseState(MOUSEKEYSTATE::DIM_LB) & 0x80)
	{
		return STATE::STATE_ATTACK;
	}

	// vLook을 바꿔준 다음에 (카메라의 look 기준)
	/*
		1. vDir에 카메라의 vLook 가져오기
		2. vDir에 대해 누른 키의 방향으로 최종 방향 설정하기
		3. 플레이어의 vLook에서 vDir로 방향 보간 (아마 RotationSpeed를 쓰면 될듯)
		4. 플레이어의 vLook을 보간된 벡터로 설정
	*/

	m_isPressed = false;

	shared_ptr<CGameObject> pCam = m_pGameInstance->GetCamera(CCamera_Manager::CAM_ATTACH);
	_vector vCamLook = pCam->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	vCamLook.m128_f32[1] = 0.f;
	_vector vCamRight = pCam->Get_TransformCom()->Get_State(CTransform::STATE_RIGHT);
	vCamRight.m128_f32[1] = 0.f;
	_vector vDir = { 0.f, 0.f, 0.f, 0.f };

	//_vector vDir = m_pOwnerTransform.lock()->Get_State(CTransform::STATE_LOOK);

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
		m_pOwnerTransform.lock()->Go_Straight(fTimeDelta);

		return STATE::STATE_RUN;
	}

	return STATE::STATE_IDLE;
}

shared_ptr<CState> CState_Transform::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Transform> pInstance = make_shared<CState_Transform>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Transform");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Transform::Free()
{
}
