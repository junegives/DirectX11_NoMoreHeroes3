#include "pch.h"
#include "Monster_TriplePut.h"
#include "Player.h"

#include "State_TriplePut_Invisible.h"
#include "State_TriplePut_Idle.h"
#include "State_TriplePut_Walk.h"
#include "State_TriplePut_Rush.h"
#include "State_TriplePut_Spin.h"
#include "State_TriplePut_Hit.h"

#include "UI_Manager.h"
#include "UI_HP_Monster.h"

#include "GameInstance.h"

CMonster_TriplePut::CMonster_TriplePut(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CMonster(pDevice, pContext)
{
}

CMonster_TriplePut::CMonster_TriplePut(const CMonster_TriplePut& rhs)
    : CMonster(rhs)
{
}

HRESULT CMonster_TriplePut::Initialize(void* pArg)
{
    m_fColRadius = 1.3f;

    MONSTER_DESC* pMonsterDesc = (MONSTER_DESC*)pArg;

    m_iStartCell = pMonsterDesc->iStartCell;

    pMonsterDesc->fSpeedPerSec = 10.f;
    pMonsterDesc->fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(Add_States()))
        return E_FAIL;

    dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player())->SetTarget(shared_from_this());

    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ 6.05f, 3.3f, -8.4f, 1.f });
    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ -22.3f, 3.3f, -0.4f, 1.f });

    _vector vStartPos = {0.f, 0.f, 0.f, 0.f};

    m_pNavigationCom->GetCellCenterPos(&vStartPos);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vStartPos);

    CUI::UIDESC		UIDesc = {};
    UIDesc.fCX = 5; UIDesc.fCY = 5;
    UIDesc.fX = 100.f;
    UIDesc.fY = 100.f;

    m_pUI_HP = CUI_HP_Monster::Create(m_pDevice, m_pContext, &UIDesc);
    m_pUI_HP->SetMaxHP(16);

    if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_BATTLE1, LAYER_UI, m_strObjectTag, m_pUI_HP)))
        return E_FAIL;

    return S_OK;
}

void CMonster_TriplePut::Priority_Tick(_float fTimeDelta)
{
}

void CMonster_TriplePut::Tick(_float fTimeDelta)
{
    m_pStateMachineCom->Tick(fTimeDelta);
    m_eCurState = m_pStateMachineCom->Get_CurState();

    if (m_isHitAfter)
    {
        m_fHitAfter += fTimeDelta;

        if (m_pUI_HP)
        {
            if (m_fHitAfter >= 2.f)
            {
                    m_pUI_HP->SetActive(false);
                    m_isHitAfter = false;
            }

            else
            {
                m_pUI_HP->WorldToScreenPos(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 3.5f, 0.f, 0.f), 0.f);
            }
        }
    }

    __super::Tick(fTimeDelta);

    /*
    m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);

    //SetUp_OnTerrain(m_pTransformCom);
    if (STATE::STATE_JUMP != m_pStateMachineCom->Get_CurState() && !m_isYChange)
        SetUp_OnNavi(m_pTransformCom);
    else
        Jump_OnNavi(m_pTransformCom);

    m_pModelCom->Apply_RootMotion(fTimeDelta, m_pTransformCom, m_pNavigationCom);

    m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
    */
}

void CMonster_TriplePut::Late_Tick(_float fTimeDelta)
{
    if (m_pAttack1ColliderCom->IsActive())
        m_pAttack1ColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
    if (m_pAttack2ColliderCom->IsActive())
        m_pAttack2ColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

    m_pStateMachineCom->Late_Tick(fTimeDelta);

    m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

    Collision_ToPlayer();

    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
        return;

#ifdef _DEBUG
    // m_pGameInstance->Add_DebugComponent(m_pColliderCom);
    // if (m_pAttack1ColliderCom->IsActive())
        // m_pGameInstance->Add_DebugComponent(m_pAttack1ColliderCom);
    // if (m_pAttack2ColliderCom->IsActive())
        // m_pGameInstance->Add_DebugComponent(m_pAttack2ColliderCom);
#endif
}

HRESULT CMonster_TriplePut::Render()
{
    _vector vTest = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

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

void CMonster_TriplePut::OnCollision(LAYER_ID eColLayer, shared_ptr<class CCollider> pCollider)
{
    _vector vHitDir, vCurPos, vColObjPos, vCurLook;
    _float fRadian;

    wstring outputString;

    _uint	iLoopExit = 100;

    switch (eColLayer)
    {
    case Engine::LAYER_PLAYER_ATTACK:

        if (!m_pColliderCom->IsOnCollide())
            return;

        if (!pCollider->IsOnCollide())
            return;

        //m_pColliderCom->SetOnCollide(false);
        pCollider->SetOnCollide(false);
        m_isHit = true;

        vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        vColObjPos = m_pGameInstance->Get_Player()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

        vCurLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
        vHitDir = XMVector4Normalize(vCurPos - vColObjPos);

        fRadian = XMVectorGetX(XMVector3Dot(vCurLook, vHitDir));

        // 정면 충돌 (플레이어 기준)
        if (0 > fRadian)
            m_eHitDir = DIR_F;

        // 후면 충돌 (플레이어 기준)
        else
            m_eHitDir = DIR_B;

        m_isHit = true;
        m_eHitType = (HIT_TYPE)pCollider->GetHitType();
        m_iSaveDamage = pCollider->GetAP();

        break;
    default:
        break;
    }
}

HRESULT CMonster_TriplePut::Add_Component()
{
    /* Com_Model */
    if (FAILED(CGameObject::Add_Component(LEVEL_BATTLE1, TEXT("Prototype_Component_Model_Monster_TriplePut"),
        TEXT("Com_Model"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pModelCom))))
        return E_FAIL;

    /* Com_StateMachine */
    CStateMachine::STATEMACHINE_DESC	StateMachineDesc{};

    StateMachineDesc.pOwner = shared_from_this();
    StateMachineDesc.pOwnerBody = shared_from_this();

    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"),
        TEXT("Com_StateMachine"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pStateMachineCom), &StateMachineDesc)))
        return E_FAIL;

    /* Com_Collider */
    CBounding_Sphere::SPHERE_DESC		BoundingDesc{};

    BoundingDesc.vCenter = _float3(0.f, 1.5f, 0.f);
    BoundingDesc.fRadius = m_fColRadius;


    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pColliderCom), &BoundingDesc)))
        return E_FAIL;

    m_pGameInstance->Add_Collider(LAYER_MONSTER, m_pColliderCom);
    m_pColliderCom->SetOwner(shared_from_this());

    /* Com_Attack1Collider */

    BoundingDesc.vCenter = _float3(0.f, 1.5f, 0.f);
    BoundingDesc.fRadius = 1.8f;

    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Attack1Collider"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pAttack1ColliderCom), &BoundingDesc)))
        return E_FAIL;

    m_pGameInstance->Add_Collider(LAYER_MONSTER_ATTACK, m_pAttack1ColliderCom);
    m_pAttack1ColliderCom->SetOwner(shared_from_this());
    m_pAttack1ColliderCom->SetActive(false);

    m_pAttack1ColliderCom->SetHitType((_uint)HIT_KNOCKDOWN);
    m_pAttack1ColliderCom->SetAP(3);

    /* Com_Attack2Collider */

    BoundingDesc.vCenter = _float3(0.f, 1.5f, 0.f);
    BoundingDesc.fRadius = 3.4f;

    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Attack2Collider"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pAttack2ColliderCom), &BoundingDesc)))
        return E_FAIL;

    m_pGameInstance->Add_Collider(LAYER_MONSTER_ATTACK, m_pAttack2ColliderCom);
    m_pAttack2ColliderCom->SetOwner(shared_from_this());
    m_pAttack2ColliderCom->SetActive(false);

    m_pAttack2ColliderCom->SetHitType((_uint)HIT_KNOCKDOWN);
    m_pAttack2ColliderCom->SetAP(3);

    /* Com_Navigation */
    CNavigation::NAVI_DESC		NaviDesc{};

    NaviDesc.iStartCellIndex = m_iStartCell;

    if (FAILED(CGameObject::Add_Component(LEVEL_BATTLE1, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pNavigationCom), &NaviDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster_TriplePut::Bind_ShaderResources()
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

HRESULT CMonster_TriplePut::Add_States()
{
    shared_ptr<CState> pState = CState_TriplePut_Idle::Create(m_pDevice, m_pContext, STATE::STATE_TRIPLEPUT_IDLE, m_pStateMachineCom);
    if (nullptr == pState)
        return E_FAIL;
    m_pStateMachineCom->Add_State(pState->Get_State(), pState);
    pState->Set_OwnerModel(m_pModelCom);
    pState->Set_OwnerCollider(m_pColliderCom);

    pState = CState_TriplePut_Hit::Create(m_pDevice, m_pContext, STATE::STATE_TRIPLEPUT_HIT, m_pStateMachineCom);
    if (nullptr == pState)
        return E_FAIL;
    m_pStateMachineCom->Add_State(pState->Get_State(), pState);
    pState->Set_OwnerModel(m_pModelCom);
    pState->Set_OwnerCollider(m_pColliderCom);

    pState = CState_TriplePut_Invisible::Create(m_pDevice, m_pContext, STATE::STATE_TRIPLEPUT_INVISIBLE, m_pStateMachineCom);
    if (nullptr == pState)
        return E_FAIL;
    m_pStateMachineCom->Add_State(pState->Get_State(), pState);
    pState->Set_OwnerModel(m_pModelCom);
    pState->Set_OwnerCollider(m_pColliderCom);

    pState = CState_TriplePut_Walk::Create(m_pDevice, m_pContext, STATE::STATE_TRIPLEPUT_WALK, m_pStateMachineCom);
    if (nullptr == pState)
        return E_FAIL;
    m_pStateMachineCom->Add_State(pState->Get_State(), pState);
    pState->Set_OwnerModel(m_pModelCom);
    pState->Set_OwnerCollider(m_pColliderCom);

    pState = CState_TriplePut_Rush::Create(m_pDevice, m_pContext, STATE::STATE_TRIPLEPUT_RUSH, m_pStateMachineCom);
    if (nullptr == pState)
        return E_FAIL;
    m_pStateMachineCom->Add_State(pState->Get_State(), pState);
    pState->Set_OwnerModel(m_pModelCom);
    pState->Set_OwnerCollider(m_pColliderCom);
    shared_ptr<CState_TriplePut_Rush> pRushState = dynamic_pointer_cast<CState_TriplePut_Rush>(pState);
    pRushState->SetOwnerAttackCollider(m_pAttack1ColliderCom);
    pRushState->SetTrail();
    
    pState = CState_TriplePut_Spin::Create(m_pDevice, m_pContext, STATE::STATE_TRIPLEPUT_SPIN, m_pStateMachineCom);
    if (nullptr == pState)
        return E_FAIL;
    m_pStateMachineCom->Add_State(pState->Get_State(), pState);
    pState->Set_OwnerModel(m_pModelCom);
    pState->Set_OwnerCollider(m_pColliderCom);
    dynamic_pointer_cast<CState_TriplePut_Spin>(pState)->SetOwnerAttackCollider(m_pAttack2ColliderCom);

    m_pStateMachineCom->Set_State(STATE::STATE_TRIPLEPUT_IDLE);

    return S_OK;
}

void CMonster_TriplePut::Collision_ToPlayer()
{
    shared_ptr<CPlayer> pPlayer = dynamic_pointer_cast<CPlayer>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, LAYER_PLAYER, TEXT("GameObject_Player")));
    shared_ptr<CCollider> pPlayerCollider = dynamic_pointer_cast<CCollider>(pPlayer->Get_PartObjectComponent(TEXT("Part_Weapon"), TEXT("Com_Collider")));

    /*CCollider*		pPlayerCollider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(LEVEL_STATIC, LAYER_PLAYER, TEXT("Com_Collider")));
    if (nullptr == pPlayerCollider)
        return;
    */

    m_pColliderCom->Intersect(pPlayerCollider);
}

_bool CMonster_TriplePut::SetDamage()
{
    m_fHitAfter = 0.0f;
    m_isHitAfter = true;

    if (m_pUI_HP)
    {
        m_pUI_HP->SetActive(true);
        m_pUI_HP->WorldToScreenPos(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 3.5f, 0.f, 0.f), 0.f);
    }

    if (m_isDead)
    {
        m_pColliderCom->SetOnCollide(false);
        return false;
    }

    CSound_Manager::GetInstance()->StopSound(CSound_Manager::CHANNELID::MONSTER3_ATTACK2);

    _int iRand = rand() % 2;

    if (m_iSaveDamage >= 5)
    {
        CSound_Manager::GetInstance()->PlaySound(L"Weapon_Attack_Heavy.wav", CSound_Manager::CHANNELID::MONSTER3_ATTACK2, 1.f);
    }
    else if (m_iSaveDamage >= 3)
    {
        if (iRand)
            CSound_Manager::GetInstance()->PlaySound(L"Weapon_Attack_Heavy.wav", CSound_Manager::CHANNELID::MONSTER3_ATTACK2, 1.f);
        else
            CSound_Manager::GetInstance()->PlaySound(L"Weapon_Attack_M.wav", CSound_Manager::CHANNELID::MONSTER3_ATTACK2, 1.f);
    }
    else
    {
        if (iRand)
            CSound_Manager::GetInstance()->PlaySound(L"Player_Hit.wav", CSound_Manager::CHANNELID::MONSTER3_ATTACK2, 1.f);
        else
            CSound_Manager::GetInstance()->PlaySound(L"Weapon_Attack_M.wav", CSound_Manager::CHANNELID::MONSTER3_ATTACK2, 1.f);
    }

    if (!m_pUI_HP->LoseHP(m_iSaveDamage))
    {
        m_isDead = true;
        m_pUI_HP->SetActive(false);
        m_pColliderCom->SetOnCollide(false);
        m_pGameInstance->Remove_Object(m_pGameInstance->Get_CurLevel(), LAYER_UI, m_strObjectTag);
        CUI_Manager::GetInstance()->RemoveUI(m_strObjectTag);
        m_pUI_HP.reset();

        return false;
    }

    return true;
}

shared_ptr<CMonster_TriplePut> CMonster_TriplePut::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    shared_ptr<CMonster_TriplePut> pInstance = make_shared<CMonster_TriplePut>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CMonster_TriplePut");
        pInstance.reset();
    }

    return pInstance;
}

void CMonster_TriplePut::Free()
{
    __super::Free();
}