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
	// ȸ�� �߿� �¾��� ��
	if (dynamic_pointer_cast<CPlayer>(m_pOwnerObject.lock())->IsHit())
	{
		// �ִϸ��̼��� �����ϰ�, �������� ������.
		// �ٵ� �̷��� ������ �����ʳ�,,???
		// ���� ��ü���� OnCol�� ���ֱ� ������ ������ �ȸ�����
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

	
	// ���� �������� look ����
	//m_pOwnerObject.lock()->Get_TransformCom()->SetLook_ForLandObject(vTargetCenter);

	//// Y�� �������� ȸ�� ��ȯ ����
	//_matrix matRotation = XMMatrixRotationY(fRadian);
	//// �������� ���� �̵� ��ȯ
	//_matrix matTranslation = XMMatrixTranslation(dynamic_pointer_cast<CBounding_Sphere>(dynamic_pointer_cast<CCollider>(m_pTarget->Find_Component(TEXT("Com_Collider")))->GetBounding())->GetRadius(), 0.0f, 0.0f);
	//// ȸ�� ��ȯ�� �̵� ��ȯ ����
	//_matrix matTransformation = matRotation * matTranslation;
	//// ��ȯ�� �����Ͽ� ���ο� ��ġ ���� ���
	//_vector vNewPos = XMVector3TransformCoord(m_vOriginPos - vTargetCenter, matTransformation);
	//vNewPos.m128_f32[1] = m_vOriginPos.m128_f32[1];
	//// ���ο� ��ġ ���͸� �������� ���Ͽ� Set
	//m_pOwnerTransform.lock()->Set_State(CTransform::STATE_POSITION, vNewPos + vTargetCenter);

	//// ���� �ð��� ���� ȸ�� ���� ���
	//float angle = angularSpeed * time;

	//// ȸ�� ��� ���
	//D3DXMatrixRotationAxis(&rotationMatrix, &axis, angle);

	//// ���� �� ��ġ ���
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






	// ���� XMConvertToRadians(180 * m_pOwnerModel.lock()->Get_CurKeyFrameRatio();
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
	
	// ���ο� ����
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

	// ������ ����
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
