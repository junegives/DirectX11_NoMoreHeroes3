#include "pch.h"
#include "Monster_Leopardon.h"
#include "Player.h"
#include "PartObject.h"
#include "Weapon_Leopardon.h"

#include "State_Leopardon_Idle.h"
#include "State_Leopardon_SquareWall.h"
#include "State_Leopardon_Occurrence.h"
#include "State_Leopardon_Warp.h"
#include "State_Leopardon_LaserBeam.h"
#include "State_Leopardon_BeamSword.h"
#include "State_Leopardon_Ring.h"
#include "State_Leopardon_Hit.h"

#include "LandObject_Leopardon_Teleport.h"
#include "Projectile_Leopardon_Ring.h"

#include "UI_HP_Boss.h"
#include "UI_Manager.h"

#include "GameInstance.h"

CMonster_Leopardon::CMonster_Leopardon(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CMonster(pDevice, pContext)
{
}

CMonster_Leopardon::CMonster_Leopardon(const CMonster_Leopardon& rhs)
    : CMonster(rhs)
{
}

shared_ptr<class CComponent> CMonster_Leopardon::Get_PartObjectComponent(const wstring& strPartObjTag, const wstring& strComTag)
{
    auto	iter = m_MonsterParts.find(strPartObjTag);
    if (iter == m_MonsterParts.end())
        return nullptr;

    return iter->second->Find_Component(strComTag);
}

HRESULT CMonster_Leopardon::Initialize(void* pArg)
{
    m_fColRadius = 0.7f;

    MONSTER_DESC* pMonsterDesc = (MONSTER_DESC*)pArg;

    m_iStartCell = pMonsterDesc->iStartCell;

    pMonsterDesc->fSpeedPerSec = 10.f;
    pMonsterDesc->fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(Add_PartObjects()))
        return E_FAIL;

    if (FAILED(Add_States()))
        return E_FAIL;
    
    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ 6.05f, 3.3f, -8.4f, 1.f });

    dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player())->SetTarget(shared_from_this());

    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ -22.3f, 3.3f, -0.4f, 1.f });

    _vector vStartPos = {0.f, 0.f, 0.f, 0.f};

    m_pNavigationCom->GetCellCenterPos(&vStartPos);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vStartPos);

    m_iHP = 150;

    CUI::UIDESC		UIDesc = {};
    UIDesc.fCX = 5; UIDesc.fCY = 5;
    UIDesc.fX = 100.f;
    UIDesc.fY = 100.f;

    m_pUI_HP = CUI_HP_Boss::Create(m_pDevice, m_pContext, &UIDesc);
    m_pUI_HP->SetMaxHP(150);

    if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_BATTLE2, LAYER_UI, m_strObjectTag, m_pUI_HP)))
        return E_FAIL;

    return S_OK;
}

void CMonster_Leopardon::Priority_Tick(_float fTimeDelta)
{
    for (auto& Pair : m_MonsterParts)
        (Pair.second)->Priority_Tick(fTimeDelta);
}

void CMonster_Leopardon::Tick(_float fTimeDelta)
{
    for (auto& pTeleport : m_TeleportVec)
    {
        pTeleport->Tick(fTimeDelta);
    }

    for (auto& pRing : m_RingVec)
    {
        pRing->Tick(fTimeDelta);
    }

    m_pStateMachineCom->Tick(fTimeDelta);
    m_eCurState = m_pStateMachineCom->Get_CurState();

    for (auto& Pair : m_MonsterParts)
        (Pair.second)->Tick(fTimeDelta);

    __super::Tick(fTimeDelta);

    if (m_pAttackColliderCom->IsActive())
        m_pAttackColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

    if (m_isDead)
    {
        for (auto iter : m_TeleportVec)
        {
            iter.reset();

            
        }
    }
}

void CMonster_Leopardon::Late_Tick(_float fTimeDelta)
{
    for (auto& pTeleport : m_TeleportVec)
    {
        pTeleport->Late_Tick(fTimeDelta);
    }

    for (auto iter = m_RingVec.begin(); iter != m_RingVec.end(); )
    {
        (*iter)->Late_Tick(fTimeDelta);
        if (!(*iter)->IsAlive())
        {
            iter = m_RingVec.erase(iter);
        }
        else
            ++iter;
    }

    m_pStateMachineCom->Late_Tick(fTimeDelta);

    for (auto& Pair : m_MonsterParts)
        (Pair.second)->Late_Tick(fTimeDelta);

    m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

    Collision_ToPlayer();

    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
        return;

#ifdef _DEBUG
    // m_pGameInstance->Add_DebugComponent(m_pColliderCom);
    // if (m_pAttackColliderCom->IsActive())
        // m_pGameInstance->Add_DebugComponent(m_pAttackColliderCom);
#endif
}

HRESULT CMonster_Leopardon::Render()
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

void    CMonster_Leopardon::OnCollision(LAYER_ID eColLayer, shared_ptr<CCollider> pCollider)
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

        // 내 포지션과
        vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        // 충돌체 오너 포지션 검사
        vColObjPos = m_pGameInstance->Get_Player()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

        // 내가 현재 보고있는 방향 체크
        vCurLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
        // 때린 방향 = 현재 몬스터 포지션 - 플레이어 포지션 (플레이어가 몬스터를 때린 방향 검사)
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

HRESULT CMonster_Leopardon::Add_Component()
{

    /* Com_Model */
    if (FAILED(CGameObject::Add_Component(LEVEL_BATTLE2, TEXT("Prototype_Component_Model_Monster_Leopardon"),
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

    BoundingDesc.vCenter = _float3(0.f, 1.3f, 0.f);
    BoundingDesc.fRadius = m_fColRadius;


    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pColliderCom), &BoundingDesc)))
        return E_FAIL;

    m_pGameInstance->Add_Collider(LAYER_MONSTER, m_pColliderCom);
    m_pColliderCom->SetOwner(shared_from_this());
    
    /* Com_Navigation */
    CNavigation::NAVI_DESC		NaviDesc{};

    NaviDesc.iStartCellIndex = m_iStartCell;

    if (FAILED(CGameObject::Add_Component(LEVEL_BATTLE2, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pNavigationCom), &NaviDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster_Leopardon::Bind_ShaderResources()
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

HRESULT CMonster_Leopardon::Add_PartObjects()
{
    CWeapon_Leopardon::WEAPON_DESC			WeaponObjDesc{};
    WeaponObjDesc.pParentTransform = m_pTransformCom;
    WeaponObjDesc.pSocketMatrix = m_pModelCom->Get_CombinedBoneMatrixPtr("gun_pos");

    /* For.Prototype_GameObject_Weapon */
    shared_ptr<CPartObject> pWeapon = CWeapon_Leopardon::Create(m_pDevice, m_pContext, &WeaponObjDesc);
    /*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, LAYER_PLAYER, TEXT("GameObject_Weapon"),
        pWeapon)))
        return E_FAIL;*/
    m_MonsterParts.emplace(TEXT("Part_Weapon"), pWeapon);

    /* Com_AttackCollider */
    m_pAttackColliderCom = dynamic_pointer_cast<CCollider>(m_MonsterParts.find(TEXT("Part_Weapon"))->second->Find_Component(TEXT("Com_Collider")));
    m_pGameInstance->Add_Collider(LAYER_MONSTER_ATTACK, m_pAttackColliderCom);
    m_pAttackColliderCom->SetOwner(shared_from_this());
    m_pAttackColliderCom->SetActive(false);

    m_pAttackColliderCom->SetHitType((_uint)HIT_HEAVY);
    m_pAttackColliderCom->SetAP(5);

    return S_OK;
}

HRESULT CMonster_Leopardon::Add_States()
{
    shared_ptr<CState> pState = CState_Leopardon_Idle::Create(m_pDevice, m_pContext, STATE::STATE_LEOPARDON_IDLE, m_pStateMachineCom);
    if (nullptr == pState)
        return E_FAIL;
    m_pStateMachineCom->Add_State(pState->Get_State(), pState);
    pState->Set_OwnerCollider(m_pColliderCom);
    pState->Set_OwnerModel(m_pModelCom);

    pState = CState_Leopardon_SquareWall::Create(m_pDevice, m_pContext, STATE::STATE_LEOPARDON_SQUAREWALL, m_pStateMachineCom);
    if (nullptr == pState)
        return E_FAIL;
    m_pStateMachineCom->Add_State(pState->Get_State(), pState);
    pState->Set_OwnerCollider(m_pColliderCom);
    pState->Set_OwnerModel(m_pModelCom);

    pState = CState_Leopardon_Ring::Create(m_pDevice, m_pContext, STATE::STATE_LEOPARDON_SPREADRING, m_pStateMachineCom);
    if (nullptr == pState)
        return E_FAIL;
    m_pStateMachineCom->Add_State(pState->Get_State(), pState);
    pState->Set_OwnerCollider(m_pColliderCom);
    pState->Set_OwnerModel(m_pModelCom);

    pState = CState_Leopardon_Occurrence::Create(m_pDevice, m_pContext, STATE::STATE_LEOPARDON_OCCURRENCE, m_pStateMachineCom);
    if (nullptr == pState)
        return E_FAIL;
    m_pStateMachineCom->Add_State(pState->Get_State(), pState);
    pState->Set_OwnerCollider(m_pColliderCom);
    pState->Set_OwnerModel(m_pModelCom);

    pState = CState_Leopardon_Warp::Create(m_pDevice, m_pContext, STATE::STATE_LEOPARDON_WARP, m_pStateMachineCom);
    if (nullptr == pState)
        return E_FAIL;
    m_pStateMachineCom->Add_State(pState->Get_State(), pState);
    pState->Set_OwnerCollider(m_pColliderCom);
    pState->Set_OwnerModel(m_pModelCom);

    pState = CState_Leopardon_LaserBeam::Create(m_pDevice, m_pContext, STATE::STATE_LEOPARDON_LASERBEAM, m_pStateMachineCom);
    if (nullptr == pState)
        return E_FAIL;
    m_pStateMachineCom->Add_State(pState->Get_State(), pState);
    pState->Set_OwnerCollider(m_pColliderCom);
    pState->Set_OwnerModel(m_pModelCom);

    pState = CState_Leopardon_BeamSword::Create(m_pDevice, m_pContext, STATE::STATE_LEOPARDON_BEAMSWORD, m_pStateMachineCom);
    if (nullptr == pState)
        return E_FAIL;
    m_pStateMachineCom->Add_State(pState->Get_State(), pState);
    pState->Set_OwnerCollider(m_pColliderCom);
    pState->Set_OwnerModel(m_pModelCom);
    dynamic_pointer_cast<CState_Leopardon_BeamSword>(pState)->SetOwnerAttackCollider(m_pAttackColliderCom);

    pState = CState_Leopardon_Hit::Create(m_pDevice, m_pContext, STATE::STATE_LEOPARDON_HIT, m_pStateMachineCom);
    if (nullptr == pState)
        return E_FAIL;
    m_pStateMachineCom->Add_State(pState->Get_State(), pState);
    pState->Set_OwnerCollider(m_pColliderCom);
    pState->Set_OwnerModel(m_pModelCom);

    m_pStateMachineCom->Set_State(STATE::STATE_LEOPARDON_IDLE);

    return S_OK;
}

void CMonster_Leopardon::Collision_ToPlayer()
{
    shared_ptr<CPlayer> pPlayer = dynamic_pointer_cast<CPlayer>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, LAYER_PLAYER, TEXT("GameObject_Player")));
    shared_ptr<CCollider> pPlayerCollider = dynamic_pointer_cast<CCollider>(pPlayer->Get_PartObjectComponent(TEXT("Part_Weapon"), TEXT("Com_Collider")));

    /*CCollider*		pPlayerCollider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(LEVEL_STATIC, LAYER_PLAYER, TEXT("Com_Collider")));
    if (nullptr == pPlayerCollider)
        return;
    */

    m_pColliderCom->Intersect(pPlayerCollider);
}

_bool CMonster_Leopardon::SetDamage()
{
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

    if (!m_pUI_HP->LoseHP(m_iSaveDamage * 6))
    {
        m_isDead = true;
        m_pColliderCom->SetOnCollide(false);
        m_pGameInstance->Remove_Object(m_pGameInstance->Get_CurLevel(), LAYER_UI, m_strObjectTag);
        CUI_Manager::GetInstance()->RemoveUI(m_strObjectTag);
        m_pUI_HP.reset();

        return false;
    }

    return true;
}

shared_ptr<CMonster_Leopardon> CMonster_Leopardon::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    shared_ptr<CMonster_Leopardon> pInstance = make_shared<CMonster_Leopardon>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CMonster_Leopardon");
        pInstance.reset();
    }

    return pInstance;
}

void CMonster_Leopardon::Free()
{
    __super::Free();
}