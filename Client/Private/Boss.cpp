#include "pch.h"
//#include "Player.h"
//
//#include "GameInstance.h"
//
//CPlayer::CPlayer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
//    : CGameObject(pDevice, pContext)
//{
//}
//
//CPlayer::CPlayer(const CPlayer& rhs)
//    : CGameObject(rhs)
//{
//}
//
//HRESULT CPlayer::Initialize(void* pArg)
//{
//    if (FAILED(__super::Initialize(pArg)))
//        return E_FAIL;
//
//    if (FAILED(Add_Component()))
//        return E_FAIL;
//
//    m_pModelCom->Set_Animation(0);
//
//    XMStoreFloat4(&m_PrevTransPos, { 0.f, 0.f, 0.f, 0.f });
//
//    _float4 InitPos = { 0, 33, 280, 1 };
//
//    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&InitPos));
//
//    return S_OK;
//}
//
//void CPlayer::Priority_Tick(_float fTimeDelta)
//{
//}
//
//void CPlayer::Tick(_float fTimeDelta)
//{
//    _float4x4 pRootTransform;
//
//    XMStoreFloat4x4(&pRootTransform, XMMatrixIdentity());
//
//    Key_Input();
//    if (m_bAnimChanging)
//        m_bAnimChanging = !m_pModelCom->Change_Animation_Lerp(fTimeDelta, 3.0f, &pRootTransform);
//    else
//        m_pModelCom->Play_Animation(fTimeDelta, &pRootTransform, true);
//
//    //XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_PrevTransformMatrix));
//
//    XMStoreFloat4x4(&pRootTransform, XMLoadFloat4x4(&pRootTransform));
//
//    _vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
//
//    //XMLoadFloat4(&pRootTransform) - XMLoadFloat4(&m_PrevTransform);
//
//    _float4 RootTransPos;
//
//    RootTransPos.x = pRootTransform.m[3][0];
//    RootTransPos.y = pRootTransform.m[3][2];
//    RootTransPos.z = -pRootTransform.m[3][1];
//    RootTransPos.w = 1.f;
//
//    if (RootTransPos.x != 0.f || RootTransPos.y != 0.f || RootTransPos.z != 0.f)
//        vPos += XMLoadFloat4(&RootTransPos) - XMLoadFloat4(&m_PrevTransPos);
//
//    else
//        int a = 3;
//
//    //vPos += XMLoadFloat4(&pRootTransform);
//    vPos.m128_f32[3] = 1.f;
//
//    //m_PrevTransPos = pRootTransform;
//
//    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
//
//    XMStoreFloat4(&m_PrevTransPos, XMLoadFloat4(&RootTransPos));
//}
//
//void CPlayer::Late_Tick(_float fTimeDelta)
//{
//    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
//        return;
//}
//
//HRESULT CPlayer::Render()
//{
//    //m_pGameInstance->Draw2DText(L"Hello, World!", g_iWinSizeX / 2.f, 30.f);
//
//    _vector vTest = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
//
//    if (FAILED(Bind_ShaderResources()))
//        return E_FAIL;
//
//    // 셰이더에 던지는 루프를 클라에서 돌리려면 메시가 몇 개 있는지 알아야 한다.
//    // 셰이더에 값 던지는 기능을 클라에서 선택적으로 가능
//    _uint   iNumMeshes = m_pModelCom->Get_NumMeshes();
//
//    for (size_t i = 0; i < iNumMeshes; i++)
//    {
//        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType::aiTextureType_DIFFUSE, "g_DiffuseTexture")))
//            return E_FAIL;
//
//        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
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
//void CPlayer::Key_Input()
//{
//    // 애니메이션 변경
//    {
//        if (!m_bPressed[0] && GetKeyState(VK_RIGHT) < 0)
//        {
//            m_bPressed[0] = true;
//            m_pModelCom->Change_Animation(m_pModelCom->Get_NextAnimation());
//            m_bAnimChanging = true;
//        }
//        else if (GetKeyState(VK_RIGHT) >= 0 && m_bPressed)
//        {
//            m_bPressed[0] = false;
//        }
//
//        if (!m_bPressed[1] && GetKeyState(VK_LEFT) < 0)
//        {
//            m_bPressed[1] = true;
//            m_pModelCom->Change_Animation(m_pModelCom->Get_PrevAnimation());
//            m_bAnimChanging = true;
//        }
//        else if (GetKeyState(VK_LEFT) >= 0 && m_bPressed)
//        {
//            m_bPressed[1] = false;
//        }
//    }
//}
//
//HRESULT CPlayer::Add_Component()
//{
//    /* Com_Shader */
//    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
//        TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
//        return E_FAIL;
//
//    /* Com_Model */
//    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player_Travis"),
//        TEXT("Com_Model"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pModelCom))))
//        return E_FAIL;
//
//    return S_OK;
//}
//
//HRESULT CPlayer::Bind_ShaderResources()
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
//shared_ptr<CPlayer> CPlayer::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
//{
//    shared_ptr<CPlayer> pInstance = make_shared<CPlayer>(pDevice, pContext);
//
//    if (FAILED(pInstance->Initialize()))
//    {
//        MSG_BOX("Failed to Created : CPlayer");
//        pInstance.reset();
//    }
//
//    return pInstance;
//}
//
//void CPlayer::Free()
//{
//    __super::Free();
//}
