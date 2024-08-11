#include "pch.h"
#include "Terrain.h"

#include "GameInstance.h"
#include "ImGui_Manager.h"

CTerrain::CTerrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CGameObject(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
    : CGameObject(rhs)
{
}

CTerrain::~CTerrain()
{
    Free();
}

HRESULT CTerrain::Initialize(void* pArg)
{
    _uint* iTerrainSize = (_uint*)pArg;

    m_iTerrainSize[0] = iTerrainSize[0];
    m_iTerrainSize[1] = iTerrainSize[1];

    SetTerrainSize(m_iTerrainSize);

    pArg = nullptr;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

void CTerrain::Priority_Tick(_float fTimeDelta)
{
}

void CTerrain::Tick(_float fTimeDelta)
{
    if (!m_bPicked && GetKeyState(VK_LBUTTON) & 0x8000)
    {
        m_bPicked = true;

        POINT ptMouse;

        GetCursorPos(&ptMouse);
        ScreenToClient(g_hWnd, &ptMouse);

        _float3 vPickPoint;

        _float  fMinDist;

        if (m_pVIBufferCom->Picking(ptMouse.x, ptMouse.y, m_pTransformCom->Get_WorldMatrix(), &vPickPoint, &fMinDist))
        {
            CImGui_Manager::GetInstance()->PickingTerrain(vPickPoint);
        }
        else
        {
            CImGui_Manager::GetInstance()->SetTerrainPickingPos({0.f, 0.f, 0.f});
        }
    }
    else if (GetKeyState(VK_LBUTTON) >= 0 && m_bPicked)
        m_bPicked = false;

}

void CTerrain::Late_Tick(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
        return;
}

HRESULT CTerrain::Render()
{
    // 셰이더 리소스 바인딩
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CTerrain::Add_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Terrain"),
        TEXT("Com_Texture"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pTextureCom[TEXTURE_DIFFUSE]))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Mask"),
        TEXT("Com_Mask"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pTextureCom[TEXTURE_MASK]))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
        TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_VIBuffer_Terrain"),
        TEXT("Com_VIBuffer"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pVIBufferCom), m_iTerrainSize)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources()
{
    _float4x4       ViewMatrix, ProjMatrix;
    _float4         CamPosition;

    ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_VIEW);
    ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_PROJ);
    CamPosition = m_pGameInstance->Get_CamPosition();

    // 월드 변환 행렬 셰이더에 연결
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    // 뷰 변환 행렬 셰이더에 연결
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
        return E_FAIL;

    // 투영 변환 행렬 셰이더에 연결
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
        return E_FAIL;

    // 텍스처 셰이더에 연결
    if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResources(m_pShaderCom, "g_DiffuseTexture")))
        return E_FAIL;

    // 마스크 텍스처 셰이더에 연결
    if (FAILED(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
        return E_FAIL;

    // 카메라 위치 셰이더에 연결
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &CamPosition, sizeof(_float4))))
        return E_FAIL;

    // 조명 관련 변수들 셰이더에 연결
    const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;

    _uint iPassIndex = { 0 };

    if (LIGHT_DESC::TYPE_DIRECTIONAL == pLightDesc->eType)
    {
        if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
            return E_FAIL;
        iPassIndex = 1;
    }

    else if (LIGHT_DESC::TYPE_POINT == pLightDesc->eType)
    {
        if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightPos", &pLightDesc->vPosition, sizeof(_float4))))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Bind_RawValue("g_fLightRange", &pLightDesc->fRange, sizeof(_float))))
            return E_FAIL;
        iPassIndex = 0;
    }

    // 셰이더의 Input Layout 설정
    if (FAILED(m_pShaderCom->Begin(iPassIndex)))
        return E_FAIL;

    return S_OK;
}

shared_ptr<CTerrain> CTerrain::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    shared_ptr<CTerrain> pInstance = make_shared<CTerrain>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CTerrain");
        pInstance.reset();
    }

    return pInstance;
}

void CTerrain::Free()
{
    if (m_pVIBufferCom)
        m_pVIBufferCom->Free();
    __super::Free();
}
