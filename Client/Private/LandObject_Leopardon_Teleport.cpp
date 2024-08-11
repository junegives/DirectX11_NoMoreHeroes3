#include "pch.h"
#include "LandObject_Leopardon_Teleport.h"
#include "Player.h"

#include "GameInstance.h"

CLandObject_Leopardon_Teleport::CLandObject_Leopardon_Teleport(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CLandObject(pDevice, pContext)
{
}

CLandObject_Leopardon_Teleport::CLandObject_Leopardon_Teleport(const CLandObject_Leopardon_Teleport& rhs)
    : CLandObject(rhs)
{
}

HRESULT CLandObject_Leopardon_Teleport::Initialize(void* pArg)
{
    //m_isVisible = false;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

void CLandObject_Leopardon_Teleport::Priority_Tick(_float fTimeDelta)
{
}

void CLandObject_Leopardon_Teleport::Tick(_float fTimeDelta)
{
    /*if (!m_isVisible)
        return; */
    
    SetUp_OnNavi(m_pTransformCom);
}

void CLandObject_Leopardon_Teleport::Late_Tick(_float fTimeDelta)
{
    /*if (!m_isVisible)
        return;*/

    m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
        return;

    __super::Late_Tick(fTimeDelta);
}

HRESULT CLandObject_Leopardon_Teleport::Render()
{
    /*if (!m_isVisible)
        return S_OK;*/

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    // 셰이더에 던지는 루프를 클라에서 돌리려면 메시가 몇 개 있는지 알아야 한다.
    // 셰이더에 값 던지는 기능을 클라에서 선택적으로 가능
    _uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType::aiTextureType_DIFFUSE, "g_DiffuseTexture")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

#ifdef _DEBUG
    m_pColliderCom->Render();
#endif

    return S_OK;
}

HRESULT CLandObject_Leopardon_Teleport::Add_Component()
{
    /* Com_Shader */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(CGameObject::Add_Component(LEVEL_BATTLE2, TEXT("Prototype_Component_Model_LandObject_Leopardon_Teleport"),
        TEXT("Com_Model"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Collider */
    CBounding_AABB::AABB_DESC		BoundingDesc{};

    BoundingDesc.vExtents = _float3(0.3f, 0.7f, 0.3f);
    BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);

    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pColliderCom), &BoundingDesc)))
        return E_FAIL;

    /* Com_Navigation */
    CNavigation::NAVI_DESC		NaviDesc{};

    NaviDesc.iStartCellIndex = 0;

    if (FAILED(CGameObject::Add_Component(LEVEL_BATTLE2, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pNavigationCom), &NaviDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLandObject_Leopardon_Teleport::Bind_ShaderResources()
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

shared_ptr<CLandObject_Leopardon_Teleport> CLandObject_Leopardon_Teleport::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    shared_ptr<CLandObject_Leopardon_Teleport> pInstance = make_shared<CLandObject_Leopardon_Teleport>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CLandObject_Leopardon_Teleport");
        pInstance.reset();
    }

    return pInstance;
}

void CLandObject_Leopardon_Teleport::Free()
{
    __super::Free();
}