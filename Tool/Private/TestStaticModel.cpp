#include "pch.h"
//#include "TestStaticModel.h"
//
//#include "GameInstance.h"
//#include "ImGui_Manager.h"
//
//CTestStaticModel::CTestStaticModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag)
//    : CGameObject(pDevice, pContext)
//    , m_strModelComTag(strModelComTag)
//{
//}
//
//CTestStaticModel::CTestStaticModel(const CTestStaticModel& rhs)
//    : CGameObject(rhs)
//{
//}
//
//HRESULT CTestStaticModel::Initialize(void* pArg)
//{
//
//    if (FAILED(__super::Initialize(pArg)))
//        return E_FAIL;
//
//    if (FAILED(Add_Component()))
//        return E_FAIL;
//   
//    return S_OK;
//}
//
//void CTestStaticModel::Priority_Tick(_float fTimeDelta)
//{
//}
//
//void CTestStaticModel::Tick(_float fTimeDelta)
//{
//    if (m_bCanPick && !m_bPicked && GetKeyState(VK_LBUTTON) & 0x8000)
//    {
//        m_bPicked = true;
//
//        POINT ptMouse;
//
//        GetCursorPos(&ptMouse);
//        ScreenToClient(g_hWnd, &ptMouse);
//
//        _float3 vPickPoint;
//
//        if (m_pModelCom->Picking(ptMouse.x, ptMouse.y, m_pTransformCom->Get_WorldMatrix(), &vPickPoint))
//        {
//            CImGui_Manager::GetInstance()->CreateStaticModel(vPickPoint);
//        }
//        else
//        {
//            CImGui_Manager::GetInstance()->SetTerrainPickingPos({ 0.f, 0.f, 0.f });
//        }
//    }
//    else if (m_bCanPick && GetKeyState(VK_LBUTTON) >= 0 && m_bPicked)
//        m_bPicked = false;
//}
//
//void CTestStaticModel::Late_Tick(_float fTimeDelta)
//{
//    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
//        return;
//}
//
//HRESULT CTestStaticModel::Render()
//{
//    _vector vTest = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
//
//    if (FAILED(Bind_ShaderResources()))
//        return E_FAIL;
//
//    // 셰이더에 던지는 루프를 클라에서 돌리려면 메시가 몇 개 있는지 알아야 한다.
//    // 셰이더에 값 던지는 기능을 클라에서 선택적으로 가능
//    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
//
//    for (size_t i = 0; i < iNumMeshes; i++)
//    {
//        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType::aiTextureType_DIFFUSE, "g_DiffuseTexture")))
//            return E_FAIL;
//
//        if (FAILED(m_pShaderCom->Begin(0)))
//            return E_FAIL;
//
//        if (FAILED(m_pModelCom->Render(i)))
//            return E_FAIL;
//    }
//
//    return S_OK;
//}
//
//HRESULT CTestStaticModel::Add_Component()
//{
//    /* Com_Shader */
//    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
//        TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
//        return E_FAIL;
//
//    /* Com_Model */
//    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, m_strModelComTag,
//        TEXT("Com_Model"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pModelCom))))
//        return E_FAIL;
//
//    return S_OK;
//}
//
//HRESULT CTestStaticModel::Bind_ShaderResources()
//{
//    _float4x4		 ViewMatrix, ProjMatrix;
//
//    ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_VIEW);
//    ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_PROJ);
//
//    _float4         CamPos;
//    CamPos = m_pGameInstance->Get_CamPosition();
//
//    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
//        return E_FAIL;
//
//    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
//        return E_FAIL;
//
//    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
//        return E_FAIL;
//
//    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &CamPos, sizeof(_float4))))
//        return E_FAIL;
//
//    const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
//    if (nullptr == pLightDesc)
//        return E_FAIL;
//
//    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
//        return E_FAIL;
//    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
//        return E_FAIL;
//    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
//        return E_FAIL;
//    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
//        return E_FAIL;
//
//    return S_OK;
//}
//
//shared_ptr<CTestStaticModel> CTestStaticModel::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag, _bool bCanPick)
//{
//    shared_ptr<CTestStaticModel> pInstance = make_shared<CTestStaticModel>(pDevice, pContext, strModelComTag);
//
//    if (FAILED(pInstance->Initialize()))
//    {
//        MSG_BOX("Failed to Created : CStaticModel");
//        pInstance.reset();
//    }
//
//    pInstance->Set_CanPick(bCanPick);
//
//    return pInstance;
//}
//
//void CTestStaticModel::Free()
//{
//    __super::Free();
//}
