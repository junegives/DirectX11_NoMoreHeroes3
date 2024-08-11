#include "pch.h"
#include "Projectile_Leopardon_Cube.h"
#include "Player.h"

#include "GameInstance.h"

CProjectile_Leopardon_Cube::CProjectile_Leopardon_Cube(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CProjectile(pDevice, pContext)
{
}

CProjectile_Leopardon_Cube::CProjectile_Leopardon_Cube(const CProjectile_Leopardon_Cube& rhs)
    : CProjectile(rhs)
{
}

HRESULT CProjectile_Leopardon_Cube::Initialize(void* pArg)
{
    m_isVisible = false;

    CTransform::TRANSFORM_DESC pTransformDesc;

    pTransformDesc.fSpeedPerSec = 30.f;
    pTransformDesc.fRotationPerSec = XMConvertToRadians(60.0f);


    if (FAILED(__super::Initialize(&pTransformDesc)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, {0.f, -1000.f, 0.f, 1.f });

    m_pModelCom->Set_Animation(1, true, 1.f, 0.0f, 0);

    m_pColliderCom->SetHitType((_uint)HIT_KNOCKDOWN);
    m_pColliderCom->SetAP(5);

    m_vColor0 = { 0.9, 0.3, 0.3 };
    m_vColor1 = { 0.5, 0.87, 0.96 };


    /*m_eHitType = HIT_KNOCKDOWN;
    m_iAP = 15;*/

    return S_OK;
}

void CProjectile_Leopardon_Cube::Priority_Tick(_float fTimeDelta)
{
}

void CProjectile_Leopardon_Cube::Tick(_float fTimeDelta)
{
    if (!m_isVisible)
        return;

    if (!m_isThrow)
        m_fLifeTime += fTimeDelta;

    m_fSoundDelay += fTimeDelta;

    if (m_isThrow && m_isLaunched && 0.2f <= m_fSoundDelay)
    {
        m_fSoundDelay = 0.f;

        CSound_Manager::GetInstance()->StopSound(CSound_Manager::CHANNELID::MONSTER1_ATTACK2);
        CSound_Manager::GetInstance()->PlaySound(L"Monster_Cube.wav", CSound_Manager::CHANNELID::MONSTER1_ATTACK2, 1.0f);
    }

    else
        int a = 3;

    m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);
}

void CProjectile_Leopardon_Cube::Late_Tick(_float fTimeDelta)
{
    m_isVisible = true;
    if (!m_isVisible)
        return;

    m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

    if(m_isLaunched && !Projectile_Go_Straight(fTimeDelta))
    {
        HideCube();
    }

    __super::Late_Tick(fTimeDelta);
}

HRESULT CProjectile_Leopardon_Cube::Render()
{
    m_isVisible = true;
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

        if (m_isThrow)
        {
            if (FAILED(m_pMaskTextureThrow->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
                return E_FAIL;
        }

        else
        {
            if (FAILED(m_pMaskTextureGenerate->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
                return E_FAIL;
            if (FAILED(m_pMaskTextureCube->Bind_ShaderResource(m_pShaderCom, "g_OutlineMaskTexture", 0)))
                return E_FAIL;
        }

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        if (m_isThrow)
        {
            if (FAILED(m_pShaderCom->Begin(3)))
                return E_FAIL;
        }
        else
        {
            if (FAILED(m_pShaderCom->Begin(2)))
                return E_FAIL;
        }

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

void CProjectile_Leopardon_Cube::OnCollision(LAYER_ID eColLayer, shared_ptr<class CCollider> pCollider)
{
    switch (eColLayer)
    {
    case Engine::LAYER_PLAYER:
        m_pColliderCom->SetOnCollide(false);
        break;
    case Engine::LAYER_ENVIRONMENT:
        break;
    default:
        break;
    }

}

HRESULT CProjectile_Leopardon_Cube::Add_Component()
{
    /* Com_Shader */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(CGameObject::Add_Component(LEVEL_BATTLE2, TEXT("Prototype_Component_Model_Projectile_Leopardon_SquareWall"),
        TEXT("Com_Model"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(CGameObject::Add_Component(LEVEL_BATTLE2, TEXT("Prototype_Component_Texture_Mask_Vertical"),
        TEXT("Com_Texture_Mask_Generate"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pMaskTextureGenerate))))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(CGameObject::Add_Component(LEVEL_BATTLE2, TEXT("Prototype_Component_Texture_Mask_Mosaic"),
        TEXT("Com_Texture_Mask_Throw"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pMaskTextureThrow))))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(CGameObject::Add_Component(LEVEL_BATTLE2, TEXT("Prototype_Component_Texture_Mask_CubeOutline"),
        TEXT("Com_Texture_Mask_Outline"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pMaskTextureCube))))
        return E_FAIL;

    /* Com_Navigation */
    CNavigation::NAVI_DESC		NaviDesc{};

    NaviDesc.iStartCellIndex = 0;

    if (FAILED(CGameObject::Add_Component(LEVEL_BATTLE2, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pNavigationCom), &NaviDesc)))
        return E_FAIL; 

    /* Com_Collider */
    CBounding_Sphere::SPHERE_DESC		BoundingDesc{};

    BoundingDesc.fRadius = 3.f;
    BoundingDesc.vCenter = _float3(0.f, BoundingDesc.fRadius / 2.f, 0.f);

    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pColliderCom), &BoundingDesc)))
        return E_FAIL;

    m_pColliderCom->SetOwner(shared_from_this());
    m_pGameInstance->Add_Collider(LAYER_PROJECTILE, m_pColliderCom);
    m_pColliderCom->SetOnCollide(false);

    return S_OK;
}

HRESULT CProjectile_Leopardon_Cube::Bind_ShaderResources()
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

    /*_float fRandMaskAlpha0 = m_fMaskAlpha0 + _float(rand() % (_uint)(0.01f * 1000)) / 1000.f;
    _float fRandMaskAlpha1 = m_fMaskAlpha1 + _float(rand() % (_uint)(0.01f * 1000)) / 1000.f;*/


    _float fRandMaskAlpha0, fRandMaskAlpha1;
    _float3 vColor0, vColor1;
    _float fLifeTimePoint = m_fLifeTime - floor(m_fLifeTime);

    if (m_isThrow)
    {
        vColor0 = { 1.f, 1.f, 1.f };
        vColor1 = { 1.f, 0.988f, 0.529f };

        m_fMaskAlpha0 = 0.05f;
        m_fMaskAlpha1 = 0.05f;
    }

    else
    {
        if (m_fLifeTime < 0.8f)
        {
            /*fRandMaskAlpha0 = m_fMaskAlpha0 + (m_fLifeTime / 10.f) - _float(rand() % (_uint)(0.05f * 1000)) / 1000;
            fRandMaskAlpha1 = m_fMaskAlpha1 - (m_fLifeTime / 10.f) + _float(rand() % (_uint)(0.05f * 1000)) / 1000;*/
            m_fMaskAlpha0 += (_float(rand() % (_uint)(0.001f * 10000)) / 10000) + 0.0003f;
            m_fMaskAlpha1 -= (_float(rand() % (_uint)(0.001f * 10000)) / 10000) - 0.0003f;
        }

        else
        {
            /*fRandMaskAlpha0 = m_fMaskAlpha0 + (0.8f / 10.f) + _float(rand() % (_uint)(0.05f * 1000)) / 1000 - 0.025f;
            fRandMaskAlpha1 = m_fMaskAlpha1 - (0.8f / 10.f) + _float(rand() % (_uint)(0.05f * 1000)) / 1000 - 0.025f;*/
            m_fMaskAlpha0 += _float(rand() % (_uint)(0.02f * 1000)) / 1000 - 0.01f;
            m_fMaskAlpha1 += _float(rand() % (_uint)(0.02f * 1000)) / 1000 - 0.01f;
        }

        // if (m_fLifeTime의 소수점 > 0.4f && m_fLifeTime의 소수점 < 0.6f)
        if (fLifeTimePoint >= 0.4f && fLifeTimePoint < 0.5f)
        {
            vColor0.x = m_vColor0.x - (fLifeTimePoint - 0.4f) * (0.4f / 0.1f);
            vColor0.y = m_vColor0.y + (fLifeTimePoint - 0.4f) * (0.57f / 0.1f);
            vColor0.z = m_vColor0.z + (fLifeTimePoint - 0.4f) * (0.66f / 0.1f);

            vColor1.x = m_vColor1.x + (fLifeTimePoint - 0.4f) * (0.4f / 0.1f);
            vColor1.y = m_vColor1.y - (fLifeTimePoint - 0.4f) * (0.57f / 0.1f);
            vColor1.z = m_vColor1.z - (fLifeTimePoint - 0.4f) * (0.66f / 0.1f);
        }

        else if (fLifeTimePoint >= 0.5f && fLifeTimePoint < 0.9f)
        {
            vColor0 = m_vColor1;
            vColor1 = m_vColor0;
        }

        else if (fLifeTimePoint >= 0.9f)
        {
            vColor0.x = m_vColor0.x + (fLifeTimePoint - 0.9f) * (0.4f / 0.1f);
            vColor0.y = m_vColor0.y - (fLifeTimePoint - 0.9f) * (0.57f / 0.1f);
            vColor0.z = m_vColor0.z - (fLifeTimePoint - 0.9f) * (0.66f / 0.1f);

            vColor1.x = m_vColor1.x - (fLifeTimePoint - 0.9f) * (0.4f / 0.1f);
            vColor1.y = m_vColor1.y + (fLifeTimePoint - 0.9f) * (0.57f / 0.1f);
            vColor1.z = m_vColor1.z + (fLifeTimePoint - 0.9f) * (0.66f / 0.1f);
        }

        else
        {
            vColor0 = m_vColor0;
            vColor1 = m_vColor1;
        }
    }

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaskAlpha0", &m_fMaskAlpha0, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaskAlpha1", &m_fMaskAlpha1, sizeof(_float))))
        return E_FAIL;
    

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor0", &vColor0, sizeof(_float3))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor1", &vColor1, sizeof(_float3))))
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

void CProjectile_Leopardon_Cube::ThrowCube()
{
    m_pColliderCom->SetOnCollide(true);

    m_isThrow = true;
    m_isLaunched = true;
    m_pModelCom->Set_Animation(0, true, 1.5f, 0.0f, 0);
    m_pNavigationCom->Compute_CurrentIdx(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

void CProjectile_Leopardon_Cube::HideCube()
{
    m_isVisible = false;
    m_isLaunched = false;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 0.f, -1000.f, 0.f, 1.f });
    m_pModelCom->Set_Animation(1, false, 1.f, 0.0f, 0);
}

void CProjectile_Leopardon_Cube::GenerateCube()
{
    m_isThrow = false;
    m_fUVx = 0.f, m_fUVy = 0.f;
    m_fMaskAlpha0 = 0.1f, m_fMaskAlpha1 = 0.29f;
    m_fLifeTime = 0.0f;
}

shared_ptr<CProjectile_Leopardon_Cube> CProjectile_Leopardon_Cube::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    shared_ptr<CProjectile_Leopardon_Cube> pInstance = make_shared<CProjectile_Leopardon_Cube>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CProjectile_Leopardon_Cube");
        pInstance.reset();
    }

    return pInstance;
}

void CProjectile_Leopardon_Cube::Free()
{
    __super::Free();
}