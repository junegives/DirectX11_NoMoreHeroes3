#include "pch.h"
#include "MeshMap.h"

#include "GameInstance.h"

CMeshMap::CMeshMap(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag, const wstring& strNaviTag, const LEVEL& eLevel)
    : CStaticObject(pDevice, pContext, strModelComTag, eLevel)
    , m_strNaviTag(strNaviTag)
{
}

CMeshMap::CMeshMap(const CMeshMap& rhs)
    : CStaticObject(rhs)
{
}

HRESULT CMeshMap::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

void CMeshMap::Priority_Tick(_float fTimeDelta)
{
}

void CMeshMap::Tick(_float fTimeDelta)
{
    m_pNavigationCom->Tick(m_pTransformCom->Get_WorldMatrix());

    if (m_bCanPick && !m_bPicked && GetKeyState(VK_LBUTTON) & 0x8000)
    {
        m_bPicked = true;

        POINT ptMouse;

        GetCursorPos(&ptMouse);
        ScreenToClient(g_hWnd, &ptMouse);

        //if (m_pModelCom->Picking(ptMouse.x, ptMouse.y, m_pTransformCom->Get_WorldMatrix(), &vPickPoint))
        //{
        //    int a = 3;
        //    //CImGui_Manager::GetInstance()->CreateStaticModel(vPickPoint);
        //}

    }
    else if (m_bCanPick && GetKeyState(VK_LBUTTON) >= 0 && m_bPicked)
        m_bPicked = false;
}

void CMeshMap::Late_Tick(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
        return;

#ifdef _DEBUG
    // m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
#endif
}

HRESULT CMeshMap::Render()
{
    _vector vTest = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    // 셰이더에 던지는 루프를 클라에서 돌리려면 메시가 몇 개 있는지 알아야 한다.
    // 셰이더에 값 던지는 기능을 클라에서 선택적으로 가능
    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType::aiTextureType_DIFFUSE, "g_DiffuseTexture")))
            return E_FAIL;

        if (LEVEL_BATTLE2 == m_eLevel)
        {
            if (FAILED(m_pShaderCom->Begin(5)))
                return E_FAIL;
        }

        else
        {
            if (FAILED(m_pShaderCom->Begin(0)))
                return E_FAIL;
        }

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CMeshMap::Add_Component()
{
    /* Com_Navigation */
    if (FAILED(CGameObject::Add_Component(m_eLevel, m_strNaviTag,
        TEXT("Com_Navigation"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pNavigationCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CMeshMap::Bind_ShaderResources()
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

    return S_OK;
}

shared_ptr<CMeshMap> CMeshMap::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag, const wstring& strNaviTag, const LEVEL& eLevel, _bool bCanPick)
{
    shared_ptr<CMeshMap> pInstance = make_shared<CMeshMap>(pDevice, pContext, strModelComTag, strNaviTag, eLevel);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CMeshMap");
        pInstance.reset();
    }

    pInstance->Set_CanPick(bCanPick);

    return pInstance;
}

void CMeshMap::Free()
{
    __super::Free();
}
