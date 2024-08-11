#include "pch.h"
#include "MeshTrail.h"

CMeshTrail::CMeshTrail(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CGameObject(pDevice, pContext)
{
}

CMeshTrail::CMeshTrail(const CMeshTrail& rhs)
    : CGameObject(rhs)
{
}

HRESULT CMeshTrail::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

void CMeshTrail::Tick(_float fTimeDelta)
{
    for (auto iter : m_MeshTrailList)
    {
        iter.fLifeTime -= fTimeDelta;
        int a = 3;
    }
}

void CMeshTrail::Late_Tick(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_GLOW, shared_from_this())))
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, shared_from_this())))
        return;
}

HRESULT CMeshTrail::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint   iNumMeshes = m_MeshTraliDesc.pModel->Get_NumMeshes();

    _uint   iIndex = 0;

    for (auto iter : m_MeshTrailList)
    {
        iIndex++;

        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &iter.WorldMatrix)))
            return E_FAIL;

        for (_uint i = 0; i < iNumMeshes; i++)
        {
            _float fAlpha = (iter.fLifeTime / m_MeshTraliDesc.fLifeTime) * ((_float)iIndex / (_float)m_MeshTraliDesc.iSize) * 0.9;
            _float4 vColor = { iter.vColor.x, iter.vColor.y, iter.vColor.z, fAlpha };

            if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fAlpha, sizeof(_float))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &iter.WorldMatrix)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(m_MeshTraliDesc.ePassType)))
                return E_FAIL;

            if (FAILED(m_MeshTraliDesc.pModel->Render(i)))
                return E_FAIL;
        }
    }

    return S_OK;
}

HRESULT CMeshTrail::Add_Component()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CMeshTrail::Bind_ShaderResources()
{
    _float4x4		 ViewMatrix, ProjMatrix;

    ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_VIEW);
    ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_PROJ);

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
        return E_FAIL;

    if (MESH_MASK_ALPHA == m_MeshTraliDesc.ePassType)
    {
        /*if (FAILED(m_pDiffuseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
            return E_FAIL;*/

        if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
            return E_FAIL;

        _float fRandomSpeedX = 0.f, fRandomSpeedY = 0.f;

        if (!m_MeshTraliDesc.fUVSpeedRangeX == 0)
        {
            _uint a = rand() % (_uint)(m_MeshTraliDesc.fUVSpeedRangeX * 100);
            _float f = _float(a) / (100.f);

            fRandomSpeedX = m_MeshTraliDesc.fUVx + _float(rand() % (_uint)(m_MeshTraliDesc.fUVSpeedRangeX * 100)) / (100.f - 50.f);
        }

        if (!m_MeshTraliDesc.fUVSpeedRangeY == 0)
        {
            fRandomSpeedY = m_MeshTraliDesc.fUVy + _float(rand() % (_uint)(m_MeshTraliDesc.fUVSpeedRangeY * 100)) / (100.f - 50.f);
        }


        if (FAILED(m_pShaderCom->Bind_RawValue("g_fUVx", &fRandomSpeedX, sizeof(_float))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_fUVy", &fRandomSpeedY, sizeof(_float))))
            return E_FAIL;
    }

    return S_OK;
}

void CMeshTrail::SettingMeshTrail(MESHTRAIL_DESC MeshTrail_Desc)
{
    m_MeshTraliDesc = MeshTrail_Desc;
    
    if (MESH_MASK_ALPHA == m_MeshTraliDesc.ePassType)
    {
        /*if (FAILED(CGameObject::Add_Component(LEVEL_BATTLE2, TEXT("Prototype_Component_Texture_Diffuse_Projectile_Leopardon_Ring"),
            TEXT("Com_Texture_Diffuse"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pDiffuseTextureCom))))
            return;*/

        if (FAILED(CGameObject::Add_Component(LEVEL_BATTLE2, m_MeshTraliDesc.strMaskTexture,
            TEXT("Com_Texture_Mask"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pMaskTextureCom))))
            return;
    }
}

void CMeshTrail::AddMeshTrail(MESHTRAIL_ELEMENT_DESC MeshTrail_Desc)
{
    MeshTrail_Desc.fUVSpeedX = m_MeshTraliDesc.fUVx;
    MeshTrail_Desc.fUVSpeedY = m_MeshTraliDesc.fUVy;
    MeshTrail_Desc.fLifeTime = m_MeshTraliDesc.fLifeTime;
    m_MeshTrailList.push_back(MeshTrail_Desc);

    if (m_MeshTraliDesc.iSize < m_MeshTrailList.size())
        m_MeshTrailList.pop_front();
}

void CMeshTrail::ClearMeshTrail()
{
    m_MeshTrailList.clear();
}

shared_ptr<CMeshTrail> CMeshTrail::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    shared_ptr<CMeshTrail> pInstance = make_shared<CMeshTrail>(pDevice, pContext);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CMeshTrail");
        pInstance.reset();
    }
    return pInstance;
}

void CMeshTrail::Free()
{
}
