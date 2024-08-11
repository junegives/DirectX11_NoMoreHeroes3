#include "pch.h"
#include "Projectile_Leopardon_Ring.h"
#include "Player.h"
#include "MeshTrail.h"

#include "Sound_Manager.h"

#include "GameInstance.h"

CProjectile_Leopardon_Ring::CProjectile_Leopardon_Ring(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CProjectile(pDevice, pContext)
{
}

CProjectile_Leopardon_Ring::CProjectile_Leopardon_Ring(const CProjectile_Leopardon_Ring& rhs)
    : CProjectile(rhs)
{
}

HRESULT CProjectile_Leopardon_Ring::Initialize(void* pArg)
{
    m_isVisible = true;

    CTransform::TRANSFORM_DESC pTransformDesc;

    pTransformDesc.fSpeedPerSec = 10.f;
    pTransformDesc.fRotationPerSec = XMConvertToRadians(60.0f);

    if (FAILED(__super::Initialize(&pTransformDesc)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pColliderCom->SetHitType((_uint)HIT_HEAVY);
    m_pColliderCom->SetAP(5);

    /*m_eHitType = HIT_HEAVY;
    m_iAP = 5;*/

    CMeshTrail::MESHTRAIL_DESC MeshTrail_Desc = {};
    MeshTrail_Desc.pModel = m_pModelCom;
    MeshTrail_Desc.iSize = 3;
    MeshTrail_Desc.fLifeTime = 1.f;
    MeshTrail_Desc.vColor = { 1.f, 1.f, 1.f };
    MeshTrail_Desc.ePassType = CMeshTrail::MESH_MASK_ALPHA;
    MeshTrail_Desc.fUVx = 0.1f;
    MeshTrail_Desc.fUVy = 0.0f;
    MeshTrail_Desc.fUVSpeedRangeX = 0.03f;
    MeshTrail_Desc.fUVSpeedRangeY = 0.0f;
    MeshTrail_Desc.strDiffuseTexture = L"";
    MeshTrail_Desc.strMaskTexture = TEXT("Prototype_Component_Texture_Mask_Vertical");

    m_pMeshTrail = CMeshTrail::Create(m_pDevice, m_pContext);
    m_pMeshTrail->SettingMeshTrail(MeshTrail_Desc);

    m_fLifeTime = 5.f;


    return S_OK;
}

void CProjectile_Leopardon_Ring::Priority_Tick(_float fTimeDelta)
{
}

void CProjectile_Leopardon_Ring::Tick(_float fTimeDelta)
{
    if (!m_isVisible)
        return;

    if (!m_isSoundOn && 7.f >= XMVectorGetX(XMVector4Length(m_pGameInstance->Get_Player()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
    {
        CSound_Manager::GetInstance()->PlaySound(L"Monster_Ring_Throw.wav", CSound_Manager::CHANNELID::MONSTER1_ATTACK, 1.0f);
        m_isSoundOn = true;
    }

    else if (m_isSoundOn && 12.f <= XMVectorGetX(XMVector4Length(m_pGameInstance->Get_Player()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
    {
        CSound_Manager::GetInstance()->StopSound(CSound_Manager::CHANNELID::MONSTER1_ATTACK);
        m_isSoundOn = false;
    }

    if (m_isSpreadFirst && m_fSpreadTime <= m_fSpreadFirstMaxTime)
    {
        m_fSpreadTime += fTimeDelta;

        // 왼쪽으로 퍼지는 애들
        if (3 > m_iIdx)
        {
            /*_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
            _vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos - vRight * fTimeDelta);*/
            m_pTransformCom->Go_LeftXZ(fTimeDelta * 2.f);
        }

        // 오른쪽으로 퍼지는 애들
        else if (6 <= m_iIdx)
        {
            /*_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
            _vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + vRight * fTimeDelta);*/
            m_pTransformCom->Go_RightXZ(fTimeDelta * 2.f);
        }

        if (m_fSpreadTime >= m_fSpreadFirstMaxTime)
        {
            m_fSpreadTime = 0.f;
            m_isSpreadFirst = false;
        }
    }

    if (m_isSpreadSecond && m_fSpreadTime <= m_fSpreadSecondMaxTime)
    {
        m_fSpreadTime += fTimeDelta;

        switch (m_iIdx)
        {
            // 왼쪽으로 퍼지는 애들
        case 0:
        case 3:
        case 6:
            m_pTransformCom->Go_LeftXZ(fTimeDelta * 0.65f);
            break;
            // 오른쪽으로 퍼지는 애들
        case 2:
        case 5:
        case 8:
            m_pTransformCom->Go_RightXZ(fTimeDelta * 0.65f);
            break;

        default:
            break;
        }

        if (m_fSpreadTime >= m_fSpreadSecondMaxTime)
        {
            m_fSpreadTime = 0.f;
            m_isSpreadSecond = false;
        }

    }

    m_pMeshTrail->Tick(fTimeDelta);
    m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CProjectile_Leopardon_Ring::Late_Tick(_float fTimeDelta)
{
    if (!m_isVisible)
        return;

    m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

   /* m_fLifeTime -= fTimeDelta;

    if (0.f >= m_fLifeTime)
        m_isAlive = false;*/

    if (!Projectile_Go_Straight(fTimeDelta))
    {
        m_isAlive = false;
    }

    __super::Late_Tick(fTimeDelta);

    m_pMeshTrail->Late_Tick(fTimeDelta);
}

HRESULT CProjectile_Leopardon_Ring::Render()
{
    if (!m_isVisible)
        return S_OK;

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

        if (FAILED(m_pShaderCom->Begin(3)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    if (0 == m_iGenerateAfterImage % 3)
    {
        m_iGenerateAfterImage = 0;
        CMeshTrail::MESHTRAIL_ELEMENT_DESC MeshTrail_Element_Desc = {};
        MeshTrail_Element_Desc.fLifeTime = 1.f;
        MeshTrail_Element_Desc.vColor = { 1.f, 1.f, 1.f };
        XMStoreFloat4x4(&MeshTrail_Element_Desc.WorldMatrix, m_pTransformCom->Get_WorldMatrix());

        m_pMeshTrail->AddMeshTrail(MeshTrail_Element_Desc);
    }

    m_iGenerateAfterImage++;

    return S_OK;
}

void CProjectile_Leopardon_Ring::OnCollision(LAYER_ID eColLayer, shared_ptr<class CCollider> pCollider)
{
    switch (eColLayer)
    {
    case Engine::LAYER_PLAYER:
        m_pColliderCom->SetOnCollide(true);
        m_isAlive = false;
        break;
    case Engine::LAYER_ENVIRONMENT:
        break;
    default:
        break;
    }
}

HRESULT CProjectile_Leopardon_Ring::Add_Component()
{
    /* Com_Shader */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(CGameObject::Add_Component(LEVEL_BATTLE2, TEXT("Prototype_Component_Model_Projectile_Leopardon_Ring"),
        TEXT("Com_Model"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pModelCom))))
        return E_FAIL;

    /*if (FAILED(CGameObject::Add_Component(LEVEL_BATTLE2, TEXT("Prototype_Component_Texture_Diffuse_Projectile_Leopardon_Ring"),
        TEXT("Com_Texture_Diffuse"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pDiffuseTexture))))
        return E_FAIL;*/

    if (FAILED(CGameObject::Add_Component(LEVEL_BATTLE2, TEXT("Prototype_Component_Texture_Mask_Vertical"),
        TEXT("Com_Texture_Mask"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pMaskTextureCom))))
        return E_FAIL;

    /* Com_Navigation */
    CNavigation::NAVI_DESC		NaviDesc{};

    NaviDesc.iStartCellIndex = 0;

    if (FAILED(CGameObject::Add_Component(LEVEL_BATTLE2, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pNavigationCom), &NaviDesc)))
        return E_FAIL;

    /* Com_Collider */
    CBounding_Sphere::SPHERE_DESC		BoundingDesc{};

    BoundingDesc.fRadius = 0.7f;
    BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pColliderCom), &BoundingDesc)))
        return E_FAIL;

    m_pColliderCom->SetOwner(shared_from_this());
    m_pGameInstance->Add_Collider(LAYER_PROJECTILE, m_pColliderCom);
    m_pColliderCom->SetOnCollide(true);

    return S_OK;
}

HRESULT CProjectile_Leopardon_Ring::Bind_ShaderResources()
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

    if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
        return E_FAIL;

    /*if (FAILED(m_pDiffuseTexture->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
        return E_FAIL;*/

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

void CProjectile_Leopardon_Ring::Throw()
{
    m_isVisible = true;
    m_pNavigationCom->Compute_CurrentIdx(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

void CProjectile_Leopardon_Ring::Hide()
{
    m_isVisible = false;
}

shared_ptr<CProjectile_Leopardon_Ring> CProjectile_Leopardon_Ring::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    shared_ptr<CProjectile_Leopardon_Ring> pInstance = make_shared<CProjectile_Leopardon_Ring>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CProjectile_Leopardon_Ring");
        pInstance.reset();
    }

    return pInstance;
}

void CProjectile_Leopardon_Ring::Free()
{
    __super::Free();
}