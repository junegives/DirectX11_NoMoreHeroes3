#include "pch.h"
#include "State_PerfectDodge.h"

#include "GameObject.h"
#include "Transform.h"
#include "Body_Player.h"
#include "Camera_Attach.h"

#include "Player.h"

CState_PerfectDodge::CState_PerfectDodge(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CState(pDevice, pContext)
{
}

HRESULT CState_PerfectDodge::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	__super::Initialize(eState, pOwnerMachine);

	return S_OK;
}

STATE CState_PerfectDodge::Tick(_float fTimeDelta)
{
	// 회피 중에 맞았을 때
	if (dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->IsHit())
	{
		// 애니메이션은 유지하고, 데미지만 입히자.
		// 근데 이러면 여러번 맞지않나,,???
		// 공격 주체에서 OnCol을 꺼주기 때문에 여러번 안맞을듯
		//dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetDamage();
		dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SetHit(false);
		m_pOwnerCollider.lock()->SetOnCollide(true);
	}
	

	if (0.9 > m_pOwnerModel.lock()->Get_CurKeyFrameRatio())
	{
		m_pOwnerTransform.lock()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 1.f);

		m_pOwnerTransform.lock()->Set_State(CTransform::STATE_POSITION, m_vTargetCenter);

		m_pOwnerTransform.lock()->Set_State(CTransform::STATE_POSITION, m_vTargetCenter
			- XMVector4Normalize(m_pOwnerTransform.lock()->Get_State(CTransform::STATE_LOOK))
			* (dynamic_pointer_cast<CBounding_Sphere>(dynamic_pointer_cast<CCollider>(m_pTarget->Find_Component(TEXT("Com_Collider")))->GetBounding())->GetRadius()
				+ (dynamic_pointer_cast<CBounding_Sphere>(dynamic_pointer_cast<CCollider>(m_pOwnerObject.lock()->Find_Component(TEXT("Com_Collider")))->GetBounding())->GetRadius())));

		


		/*m_pOwnerTransform.lock()->Go_BackwardXZ(fTimeDelta);*/
	}


	shared_ptr<CGameObject>		pCamera = m_pGameInstance->GetCamera(CCamera_Manager::CAM_ATTACH);
	_vector vDir = { 0.f, 0.f, 0.f, 0.f };

	if (m_pTarget)
	{
		vDir = m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION) - m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION);

		dynamic_pointer_cast<CCamera_Attach>(pCamera)->ChangeTargetCamLerp(XMVector3Normalize(vDir), 0.1f, m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));
	}

	
	// 몬스터 원점으로 look 설정
	//m_pOwnerObject.lock()->Get_TransformCom()->SetLook_ForLandObject(vTargetCenter);

	//// Y축 기준으로 회전 변환 생성
	//_matrix matRotation = XMMatrixRotationY(fRadian);
	//// 반지름에 따른 이동 변환
	//_matrix matTranslation = XMMatrixTranslation(dynamic_pointer_cast<CBounding_Sphere>(dynamic_pointer_cast<CCollider>(m_pTarget->Find_Component(TEXT("Com_Collider")))->GetBounding())->GetRadius(), 0.0f, 0.0f);
	//// 회전 변환과 이동 변환 결합
	//_matrix matTransformation = matRotation * matTranslation;
	//// 변환을 적용하여 새로운 위치 벡터 계산
	//_vector vNewPos = XMVector3TransformCoord(m_vOriginPos - vTargetCenter, matTransformation);
	//vNewPos.m128_f32[1] = m_vOriginPos.m128_f32[1];
	//// 새로운 위치 벡터를 기준점에 더하여 Set
	//m_pOwnerTransform.lock()->Set_State(CTransform::STATE_POSITION, vNewPos + vTargetCenter);

	//// 현재 시간에 따라 회전 각도 계산
	//float angle = angularSpeed * time;

	//// 회전 행렬 계산
	//D3DXMatrixRotationAxis(&rotationMatrix, &axis, angle);

	//// 점의 새 위치 계산
	//D3DXVECTOR4 transformedPoint;
	//D3DXVec3Transform(&transformedPoint, &point, &rotationMatrix);

	//_matrix RotationMatrix = XMMatrixRotationAxis({ 0, 1, 0, 0 }, fRadian);

	//void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
	//{
	//	_matrix     RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	//	for (size_t i = STATE_RIGHT; i < STATE_POSITION; i++)
	//	{
	//		_vector     vStateDir = Get_State(STATE(i));

	//		Set_State(STATE(i), XMVector4Transform(vStateDir, RotationMatrix));
	//	}
	//}

	//m_pOwnerTransform.lock()->Turn({ 0, 1, 0, 0 }, fTimeDelta);






	// 각도 XMConvertToRadians(180 * m_pOwnerModel.lock()->Get_CurKeyFrameRatio();
	/*_matrix PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180 * m_pOwnerModel.lock()->Get_CurKeyFrameRatio()));
	_vector PivotVector = XMVectorRotateRight()

	m_pOwnerTransform.lock()->Set_State(CTransform::STATE_POSITION, m_pOwnerTransform.lock()->Get_State(CTransform::STATE_POSITION) * PivotMatrix);*/







	/*if (0.5 > m_pOwnerModel.lock()->Get_CurKeyFrameRatio()) {
		m_pOwnerTransform.lock()->Go_StraightXZ(fTimeDelta * 0.05f);
		m_pOwnerTransform.lock()->Go_RightXZ(fTimeDelta * 0.05f);
	}
	else {
		m_pOwnerTransform.lock()->Go_StraightXZ(fTimeDelta * 0.05f);
		m_pOwnerTransform.lock()->Go_LeftXZ(fTimeDelta * 0.05f);
	}*/
	
	// 슬로우 유지
	//m_pGameInstance->Set_Slow();

	m_eState = Key_Input(fTimeDelta);

	return m_eState;
}

STATE CState_PerfectDodge::Late_Tick(_float fTimeDelta)
{

	return m_eState;
}

void CState_PerfectDodge::Reset_State()
{
	_bool isOnPerfectDodge = dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->IsOnPerfectDodge();
	m_pOwnerCollider.lock()->SetOnCollide(true);

	CSound_Manager::GetInstance()->PlaySound(L"Player_SuperDodge.wav", CSound_Manager::CHANNELID::PLAYER_STEP, 1.3f);

	int a = 3;
}

void CState_PerfectDodge::Enter_State()
{
	_bool isOnPerfectDodge = dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->IsOnPerfectDodge();

	m_pTarget = dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->SearchTarget();

	dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_PERFECTDODGE, false, 1.0f, 0.1f, 0);
	m_eState = STATE::STATE_PERFECTDODGE;
	m_isIn = true;

	m_vOriginPos = m_pOwnerObject.lock()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

	// 몬스터의 원점
	m_vTargetCenter = m_pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

	m_pOwnerCollider.lock()->SetOnCollide(false);
}

STATE CState_PerfectDodge::Key_Input(_float fTimeDelta)
{
	if (m_isIn)
	{
		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_isIn = false;
			m_isLoop = true;
			dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_PERFECTDODGE_LOOP, false, 0.5f, 0.1f, 0);
		}
	}

	else if (m_isLoop)
	{
		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_isIn = false;
			m_isLoop = false;
			m_isOut = true;
			dynamic_pointer_cast<CBody_Player>(m_pOwnerBody.lock())->Set_Animation((_uint)STATE_PERFECTDODGE_OUT, false, 2.f, 0.1f, 0);
		}
	}

	else if (m_isOut)
	{
		if (m_pOwnerModel.lock()->isAnimStop())
		{
			m_pOwnerCollider.lock()->SetOnCollide(true);

			m_isIn = false;
			m_isLoop = false;
			m_isOut = false;

			return STATE::STATE_IDLE;
		}
	}

	return STATE::STATE_PERFECTDODGE;
}

shared_ptr<CState> CState_PerfectDodge::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg)
{
	shared_ptr<CState_PerfectDodge> pInstance = make_shared<CState_PerfectDodge>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eState, pOwnerMachine, pArg)))
	{
		MSG_BOX("Failed to Created : CState_PerfectDodge");
		pInstance.reset();
	}

	return pInstance;
}

void CState_PerfectDodge::Free()
{
}
