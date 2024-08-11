#include "pch.h"
#include "State_Dodge.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"

#include "Player.h"
#include "Camera_Attach.h"

CState_Dodge::CState_Dodge(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_Dodge::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_Dodge::Tick(_float fTimeDelta)
{
	// 회피 중에 맞았을 때
	if (dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->IsHit())
	{
		// 애니메이션은 유지하고, 데미지만 입히자.
		// 근데 이러면 여러번 맞지않나,,???
		// 공격 주체에서 OnCol을 꺼주기 때문에 여러번 안맞을듯
		dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetDamage();
		dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetHit(false);
		m_pOwnerCollider.lock()->SetOnCollide(true);
	}

	// 할게 없을 것 같다...???

	// 그냥 애니메이션 끝나면 IDLE 리턴해주면 될듯?

	if (m_pGameInstance->Is_KeyPressing(DIK_LSHIFT))
	{
		Lock_On();
	}
	else
	{
		dynamic_pointer_cast<CCamera_Attach>(m_pGameInstance->GetCamera(CCamera_Manager::CAM_ATTACH))->SetLock(false);
	}

	//m_eState = Key_Input(fTimeDelta);
	if (0.6 <= m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
		return STATE::STATE_IDLE;

	else
		return STATE::STATE_DODGE;
}

STATE CState_Dodge::Late_Tick(_float fTimeDelta)
{

	return m_eState;
}

void CState_Dodge::Reset_State()
{
}

void CState_Dodge::Enter_State()
{
	dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_DODGE, false, 3.0f, 0.1f, 0);
	m_eState = STATE::STATE_DODGE;

	CSound_Manager::GetInstance()->PlaySound(L"foley_TRV_FT_Conc_Avoid_00.wav", CSound_Manager::CHANNELID::PLAYER_STEP, 1.3f);
}

STATE CState_Dodge::Key_Input(_float fTimeDelta)
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

void CState_Dodge::Lock_On()
{
	m_pTarget = dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->GetTarget();

	shared_ptr<CGameObject>		pCamera = m_pGameInstance->GetCamera(CCamera_Manager::CAM_ATTACH);
	_vector vDir = { 0.f, 0.f, 0.f, 0.f };

	if (m_pTarget)
	{
		vDir = m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION) - m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION);

		dynamic_pointer_cast<CCamera_Attach>(pCamera)->ChangeTargetCamLerp(XMVector3Normalize(vDir), 0.1f, m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));
	}
}

shared_ptr<CState> CState_Dodge::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_Dodge> pInstance = make_shared<CState_Dodge>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_Dodge");
		pInstance.reset();
	}

	return pInstance;
}

void CState_Dodge::Free()
{
}
