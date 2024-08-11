#include "pch.h"
#include "Bike.h"

#include "StateMachine.h"

#include "State_Bike_Idle.h"
#include "State_Bike_Run.h"
#include "State_Bike_Nitro.h"
#include "State_Bike_Sit.h"
#include "State_Bike_MiniGame_Idle.h"
#include "State_Bike_MiniGame_Run.h"

#include "GameInstance.h"

CBike::CBike(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLandObject(pDevice, pContext)
{
}

CBike::CBike(const CBike& rhs)
	: CLandObject(rhs)
{
}

HRESULT CBike::Initialize(void* pArg)
{
	BIKE_DESC* pBikeDesc = (BIKE_DESC*)pArg;

	m_pSocketMatrix = pBikeDesc->pSocketMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->SetSpeedPerSec(20.f);
	m_pTransformCom->SetRotationPerSec(XMConvertToRadians(90.f));

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_States()))
		return E_FAIL;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ -22.3f, 3.3f, -0.4f, 1.f });

	_vector vStartPos = { 0.f, 0.f, 0.f, 0.f };

	m_pNavigationCom->GetCellCenterPos(&vStartPos);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vStartPos);

	//m_pTransformCom->TurnRadian({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(10));

	return S_OK;
}

void CBike::Priority_Tick(_float fTimeDelta)
{
}

void CBike::Tick(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	m_pStateMachineCom->Tick(fTimeDelta);
	m_eCurState = m_pStateMachineCom->Get_CurState();

	switch (m_eCurState)
	{
	case Engine::STATE_BIKE_IDLE:
		if (m_fSpeed > 0.f)
			m_fSpeed -= m_fSpeedChange;
		break;
	case Engine::STATE_BIKE_END:
		break;
	default:
		break;
	}

	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);

	SetUp_OnNavi(m_pTransformCom);

	m_pModelCom->Apply_RootMotion(fTimeDelta, m_pTransformCom, m_pNavigationCom);
}

void CBike::Late_Tick(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	m_pStateMachineCom->Late_Tick(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
		return;

#ifdef _DEBUG
	// m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CBike::Render()
{
	if (!m_isActive)
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	// 셰이더에 던지는 루프를 클라에서 돌리려면 메시가 몇 개 있는지 알아야 한다.
	// 셰이더에 값 던지는 기능을 클라에서 선택적으로 가능
	_uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType::aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void CBike::Set_Attached(_bool isAttached)
{
	m_isAttached = isAttached;

	//if (m_isAttached)
	//{
	//	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.5f, 0.f, 0.f, 1.f));
	//	m_isRotated = false;

	//	_matrix	BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	//	for (size_t i = 0; i < 3; i++)
	//	{
	//		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
	//	}
	//	
	//	XMStoreFloat4x4(&m_WorldMatrix, BoneMatrix * m_pParentTransform->Get_WorldMatrix());
	//}
	////m_pTransformCom->Set_Scale(0.1f, 0.1f, 0.1f);
	////m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.5f, 0.f, 0.f, 1.f));

	//else
	//{
	//	//m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, -1.f, 0.f), XMConvertToRadians(180.0f));
	//}
}

void CBike::SetOnMiniGame(_bool isOnMiniGame)
{
	m_isOnMiniGame = isOnMiniGame;

	if (m_isOnMiniGame)
		m_isOnMiniGameNow = true;
}

void CBike::SetOnMiniGameNow(_bool isOnMiniGameNow)
{
	m_isOnMiniGameNow = isOnMiniGameNow;
}

HRESULT CBike::Add_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Vehicle_Cat"),
		TEXT("Com_Model"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pModelCom))))
		return E_FAIL;

	m_pSocketMatrix = m_pModelCom->Get_CombinedBoneMatrixPtr("Head");
	m_pRootMatrix = m_pModelCom->Get_CombinedBoneMatrixPtr("root");

	/* Com_Collider */
	CBounding_Sphere::SPHERE_DESC		BoundingDesc{};

	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);
	BoundingDesc.fRadius = 2.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider(LAYER_PLAYER, m_pColliderCom);
	m_pColliderCom->SetOwner(shared_from_this());

	/* Com_StateMachine */
	CStateMachine::STATEMACHINE_DESC	StateMachineDesc{};

	StateMachineDesc.pOwner = shared_from_this();
	StateMachineDesc.pOwnerBody = shared_from_this();

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"),
		TEXT("Com_StateMachine"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pStateMachineCom), &StateMachineDesc)))
		return E_FAIL;

	/* Com_Navigation */
	CNavigation::NAVI_DESC		NaviDesc{};

	NaviDesc.iStartCellIndex = 5;

	if (FAILED(CGameObject::Add_Component(LEVEL_VILLAGE, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBike::Add_States()
{
	shared_ptr<CState> pState = CState_Bike_Idle::Create(m_pDevice, m_pContext, STATE::STATE_BIKE_IDLE, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_OwnerModel(m_pModelCom);
	pState->Set_NavigationCom(m_pNavigationCom);
	pState->Set_OwnerCollider(m_pColliderCom);

	pState = CState_Bike_Run::Create(m_pDevice, m_pContext, STATE::STATE_BIKE_RUN, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_OwnerModel(m_pModelCom);
	pState->Set_NavigationCom(m_pNavigationCom);
	pState->Set_OwnerCollider(m_pColliderCom);

	pState = CState_Bike_Nitro::Create(m_pDevice, m_pContext, STATE::STATE_BIKE_NITRO, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_OwnerModel(m_pModelCom);
	pState->Set_NavigationCom(m_pNavigationCom);
	pState->Set_OwnerCollider(m_pColliderCom);

	pState = CState_Bike_Sit::Create(m_pDevice, m_pContext, STATE::STATE_BIKE_IDLE_SIT, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_OwnerModel(m_pModelCom);

	pState = CState_Bike_MiniGame_Idle::Create(m_pDevice, m_pContext, STATE::STATE_BIKE_MINIGAME_IDLE, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_OwnerModel(m_pModelCom);
	pState->Set_NavigationCom(m_pNavigationCom);

	pState = CState_Bike_MiniGame_Run::Create(m_pDevice, m_pContext, STATE::STATE_BIKE_MINIGAME_RUN, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_OwnerModel(m_pModelCom);
	pState->Set_NavigationCom(m_pNavigationCom);

	//pState = CState_Attack::Create(m_pDevice, m_pContext, STATE::STATE_ATTACK, m_pStateMachineCom);
	//if (nullptr == pState)
	//	return E_FAIL;
	//m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	//pState->Set_OwnerModel(m_pBodyModel);
	//pState->Set_OwnerCollider(m_pColliderCom);
	//dynamic_pointer_cast<CState_Attack>(pState)->SetOwnerAttackCollider(m_pWeaponCollider);

	//pState = CState_HeavyAttack::Create(m_pDevice, m_pContext, STATE::STATE_HEAVYATTACK, m_pStateMachineCom);
	//if (nullptr == pState)
	//	return E_FAIL;
	//m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	//pState->Set_OwnerModel(m_pBodyModel);
	//pState->Set_OwnerCollider(m_pColliderCom);
	//dynamic_pointer_cast<CState_HeavyAttack>(pState)->SetOwnerAttackCollider(m_pWeaponCollider);

	//pState = CState_Dodge::Create(m_pDevice, m_pContext, STATE::STATE_DODGE, m_pStateMachineCom);
	//if (nullptr == pState)
	//	return E_FAIL;
	//m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	//pState->Set_OwnerModel(m_pBodyModel);
	//pState->Set_OwnerCollider(m_pColliderCom);

	//pState = CState_Jump::Create(m_pDevice, m_pContext, STATE::STATE_JUMP, m_pStateMachineCom);
	//if (nullptr == pState)
	//	return E_FAIL;
	//m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	//pState->Set_OwnerModel(m_pBodyModel);
	//pState->Set_NavigationCom(m_pNavigationCom);
	//pState->Set_OwnerCollider(m_pColliderCom);

	//pState = CState_KillerSlash::Create(m_pDevice, m_pContext, STATE::STATE_KILLERSLASH, m_pStateMachineCom);
	//if (nullptr == pState)
	//	return E_FAIL;
	//m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	//pState->Set_OwnerModel(m_pBodyModel);
	//pState->Set_OwnerCollider(m_pColliderCom);
	//dynamic_pointer_cast<CState_KillerSlash>(pState)->SetOwnerAttackCollider(m_pWeaponCollider);

	//pState = CState_DeathKick::Create(m_pDevice, m_pContext, STATE::STATE_DEATHKICK, m_pStateMachineCom);
	//if (nullptr == pState)
	//	return E_FAIL;
	//m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	//pState->Set_OwnerModel(m_pBodyModel);
	//pState->Set_OwnerCollider(m_pColliderCom);
	//dynamic_pointer_cast<CState_DeathKick>(pState)->SetOwnerAttackCollider(m_pWeaponCollider);

	//pState = CState_DashAttack::Create(m_pDevice, m_pContext, STATE::STATE_DASHATTACK, m_pStateMachineCom);
	//if (nullptr == pState)
	//	return E_FAIL;
	//m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	//pState->Set_OwnerModel(m_pBodyModel);
	//pState->Set_OwnerCollider(m_pColliderCom);
	//dynamic_pointer_cast<CState_DashAttack>(pState)->SetOwnerAttackCollider(m_pWeaponCollider);

	//pState = CState_Guard::Create(m_pDevice, m_pContext, STATE::STATE_GUARD, m_pStateMachineCom);
	//if (nullptr == pState)
	//	return E_FAIL;
	//m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	//pState->Set_OwnerModel(m_pBodyModel);
	//pState->Set_OwnerCollider(m_pColliderCom);
	//pState->Set_NavigationCom(m_pNavigationCom);

	//pState = CState_PerfectDodge::Create(m_pDevice, m_pContext, STATE::STATE_PERFECTDODGE, m_pStateMachineCom);
	//if (nullptr == pState)
	//	return E_FAIL;
	//m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	//pState->Set_OwnerModel(m_pBodyModel);
	//pState->Set_OwnerCollider(m_pColliderCom);
	//pState->Set_NavigationCom(m_pNavigationCom);

	//pState = CState_Hit::Create(m_pDevice, m_pContext, STATE::STATE_HIT, m_pStateMachineCom);
	//if (nullptr == pState)
	//	return E_FAIL;
	//m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	//pState->Set_OwnerModel(m_pBodyModel);
	//pState->Set_OwnerCollider(m_pColliderCom);
	//pState->Set_NavigationCom(m_pNavigationCom);

	//pState = CState_Transform::Create(m_pDevice, m_pContext, STATE::STATE_TRANSFORM, m_pStateMachineCom);
	//if (nullptr == pState)
	//	return E_FAIL;
	//m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	//pState->Set_OwnerModel(m_pBodyModel);
	//pState->Set_OwnerCollider(m_pColliderCom);
	//pState->Set_NavigationCom(m_pNavigationCom);

	//pState = CState_Armor_Missile::Create(m_pDevice, m_pContext, STATE::STATE_ARMOR_MISSILE_IN, m_pStateMachineCom);
	//if (nullptr == pState)
	//	return E_FAIL;
	//m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	//pState->Set_OwnerModel(m_pBodyModel);
	//pState->Set_OwnerCollider(m_pColliderCom);
	//pState = CState_Armor_Missile::Create(m_pDevice, m_pContext, STATE::STATE_ARMOR_MISSILE_IN, m_pStateMachineCom);
	//if (nullptr == pState)
	//	return E_FAIL;

	//pState = CState_Throw::Create(m_pDevice, m_pContext, STATE::STATE_THROW_LEOPARDON, m_pStateMachineCom);
	//if (nullptr == pState)
	//	return E_FAIL;
	//m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	//pState->Set_OwnerModel(m_pBodyModel);
	//pState->Set_OwnerCollider(m_pColliderCom);

	m_pStateMachineCom->Set_State(STATE::STATE_BIKE_IDLE);

	return S_OK;
}

HRESULT CBike::Bind_ShaderResources()
{
	_float4x4		 ViewMatrix, ProjMatrix;

	ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_VIEW);
	ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_PROJ);

	_float4         CamPos;
	CamPos = m_pGameInstance->Get_CamPosition();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	/*if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &CamPos, sizeof(_float4))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;*/

	return S_OK;
}

void CBike::Key_Input(_float fTimeDelta)
{
}

void CBike::Change_NaviCom(_uint iStartCellIdx, shared_ptr<CNavigation> pNavigation)
{
	m_pNavigationCom = pNavigation;

	_vector vStartPos = { 0.f, 0.f, 0.f, 0.f };

	m_pNavigationCom->GetCellCenterPos(iStartCellIdx, &vStartPos);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vStartPos);
}

shared_ptr<CBike> CBike::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
	shared_ptr<CBike> pInstance = make_shared<CBike>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBike");
		pInstance.reset();
	}

	return pInstance;
}

void CBike::Free()
{
	__super::Free();
}
