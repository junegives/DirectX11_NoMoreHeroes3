#include "Renderer.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"

CRenderer::CRenderer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice(pDevice)
    , m_pContext(pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT CRenderer::Initialize()
{
    // 렌더링 뷰포트를 정의하는 구조체
    D3D11_VIEWPORT         ViewPortDesc{};

    // 렌더링 뷰포트의 수를 나타내는 변수
    _uint      iNumViewports = 1;

    // 현재 렌더링 파이프라인에서 설정된 뷰포트를 가져옴
    m_pContext->RSGetViewports(&iNumViewports, &ViewPortDesc);

    /* 내 게임에 필요한 렌더타겟들을 추가한다.*/

    /* For.Target_Diffuse */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
        return E_FAIL;

    /* For.Target_Normal */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
        return E_FAIL;

    /* For.Target_Depth */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    /* For.Target_Shade*/
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
        return E_FAIL;

    /* For.Target_Specular */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* For.MRT_GameObjects */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
        return E_FAIL;

    /* For.MRT_LightAcc */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
        return E_FAIL;

    /* Copy Back Buffer */
    if (FAILED(m_pGameInstance->Ready_BackBufferCopyTexture((_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height)))
        return E_FAIL;

    /* For.Target_Effect */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    /* For.Target_Glow */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Glow"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    /* For.Target_Blur_X */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_X"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    /* For.Target_Blur_Y */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_Y"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    /* For.Target_Distortion */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Distortion"), (_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* For.MRT_LightAcc */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Effect"))))
        return E_FAIL;
    /* For.MRT_Glow */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Glow"), TEXT("Target_Glow"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_X"), TEXT("Target_Blur_X"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_Y"), TEXT("Target_Blur_Y"))))
        return E_FAIL;
    /* For. MRT_Distortion*/
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
        return E_FAIL;

    // 사각형 버퍼 생성 (화면 그리는데 사용)
    m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
    if (nullptr == m_pVIBuffer)
        return E_FAIL;

    // Deferred 렌더링을 위한 셰이더 생성
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;

    // Post Processing 렌더링을 위한 셰이더 생성
    m_pPostShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_PostProcess.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;

    // 세계 행렬을 단위 행렬로 초기화
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
    m_WorldMatrix._11 = ViewPortDesc.Width;
    m_WorldMatrix._22 = ViewPortDesc.Height;

    // 뷰 행렬을 단위 행렬로 초기화
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    // 직교 투영 행렬을 생성하여 초기화. 2D 그래픽을 그리기 위해 사용된다.
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewPortDesc.Width, ViewPortDesc.Height, 0.f, 1.f));

#ifdef _DEBUG
    if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Diffuse"), 70.0f, 72.0f, 240.f, 144.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Normal"), 70.0f, 216.0f, 240.f, 144.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Shade"), 70.0f, 504.0f, 240.f, 144.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Depth"), 70.0f, 360.0f, 240.0f, 144.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Specular"), 70.0f, 648.0f, 240.0f, 144.f)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Glow"), 310.0f, 72.0f, 240.f, 144.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Blur_X"), 310.0f, 216.0f, 240.f, 144.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Blur_Y"), 310.0f, 360.0f, 240.f, 144.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_Debug(TEXT("Target_Distortion"), 310.0f, 504.0f, 240.f, 144.f)))
        return E_FAIL;
#endif // _DEBUG

    return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, shared_ptr<CGameObject> pGameObject)
{
    if (eRenderGroup >= RENDER_END)
        return E_FAIL;

    m_RenderObjects[eRenderGroup].push_back(pGameObject);

    //Safe_AddRef(pGameObject);

    return S_OK;
}

HRESULT CRenderer::Render()
{
    if (FAILED(Render_Priority()))
        return E_FAIL;
    /* 이 그룹에서 그려지는 객체들은 빛연산이 필요하다. */
    /* -> 이 객체들의 픽셀당 노멀정보와 디퓨즈정보를 받아와야할 필요가 생겼다. */
    if (FAILED(Render_NonBlend()))
        return E_FAIL;
    if (FAILED(Render_Lights()))
        return E_FAIL;
    if (FAILED(Render_NonLight()))
        return E_FAIL;
    if (FAILED(Render_Final()))
        return E_FAIL;
    if (FAILED(Render_Blend()))
        return E_FAIL;
    if (FAILED(Render_Glow()))
        return E_FAIL;
    if (FAILED(Render_Distortion()))
        return E_FAIL;
    if (FAILED(Render_UI()))
        return E_FAIL;

#ifdef _DEBUG
    if (m_isRenderDebug)
    {
        if (FAILED(Render_Debug()))
            return E_FAIL;
    }
#endif // _DEBUG

    return S_OK;
}

HRESULT CRenderer::Add_DebugComponent(shared_ptr<class CComponent> pComponent)
{
    m_DebugCom.push_back(pComponent);

    return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
    if (m_RenderObjects[RENDER_PRIORITY].empty())
        return S_OK;

    for (auto& pGameObject : m_RenderObjects[RENDER_PRIORITY])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();

        //Safe_Release(pGameObject);
    }

    m_RenderObjects[RENDER_PRIORITY].clear();

    return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
    if (m_RenderObjects[RENDER_NONBLEND].empty())
        return S_OK;

    /* Diffuse + Normal */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
        return E_FAIL;

    for (auto& pGameObject : m_RenderObjects[RENDER_NONBLEND])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();

        //Safe_Release(pGameObject);
    }

    m_RenderObjects[RENDER_NONBLEND].clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
    _float4x4   matProjInv = m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeline::D3DTS_PROJ);
    _float4x4   matViewInv = m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeline::D3DTS_VIEW);
    _float4      vCamPos = m_pGameInstance->Get_CamPosition();


    /* Shade */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &matProjInv)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &matViewInv)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &vCamPos, sizeof(_float4))))
        return E_FAIL;

    /* 노말 렌더타겟을 쉐이더에 던진다. */
    if (FAILED(m_pGameInstance->Bind_SRV(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_SRV(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
        return E_FAIL;

    m_pGameInstance->Render_Light(m_pShader, m_pVIBuffer);

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Final()
{
    /* 사각형을 직교투영으로 화면에 꽉 채워서 그린다. */
    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    /* 디퓨즈 렌더타겟을 쉐이더에 던진다. */
    if (FAILED(m_pGameInstance->Bind_SRV(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
        return E_FAIL;
    /* 셰이더 렌더타겟을 쉐이더에 던진다. */
    if (FAILED(m_pGameInstance->Bind_SRV(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_SRV(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
        return E_FAIL;

    m_pShader->Begin(3);

    m_pVIBuffer->Bind_Buffers();
    m_pVIBuffer->Render();

    return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
    if (m_RenderObjects[RENDER_NONLIGHT].empty())
        return S_OK;

    for (auto& pGameObject : m_RenderObjects[RENDER_NONLIGHT])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();
    }

    m_RenderObjects[RENDER_NONLIGHT].clear();

    return S_OK;
}

HRESULT CRenderer::Render_Glow()
{
    if (m_RenderObjects[RENDER_GLOW].empty())
        return S_OK;

    // 알파 소팅 (카메라 기준)
    Render_Sorting(RENDER_GLOW);

    // 일단 MRT_Glow에 먼저 그리기
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Glow"))))
        return E_FAIL;

    for (auto& pGameObject : m_RenderObjects[RENDER_GLOW])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();
    }

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    m_RenderObjects[RENDER_GLOW].clear();

    if (FAILED(m_pPostShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pPostShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pPostShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Copy_BackBuffer()))
        return E_FAIL;

    if (FAILED(m_pPostShader->Bind_SRV("g_BackBufferTexture", m_pGameInstance->GetBackBufferSRV())))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_X"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_SRV(TEXT("Target_Glow"), m_pPostShader, "g_BlurTexture")))
        return E_FAIL;

    m_pPostShader->Begin(1);

    m_pVIBuffer->Bind_Buffers();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_Y"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_SRV(TEXT("Target_Blur_X"), m_pPostShader, "g_BlurTexture")))
        return E_FAIL;

    m_pPostShader->Begin(2);

    m_pVIBuffer->Bind_Buffers();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_SRV(TEXT("Target_Blur_Y"), m_pPostShader, "g_BlendTexture")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_SRV(TEXT("Target_Glow"), m_pPostShader, "g_BlurTexture")))
        return E_FAIL;

    m_pPostShader->Begin(0);

    m_pVIBuffer->Bind_Buffers();
    m_pVIBuffer->Render();

    return S_OK;
}

HRESULT CRenderer::Render_Distortion()
{
    if (m_RenderObjects[RENDER_DISTORTION].empty())
        return S_OK;

    // 일단 MRT_Distortion에 먼저 그리기
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Distortion"))))
        return E_FAIL;

    for (auto& pGameObject : m_RenderObjects[RENDER_DISTORTION])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();
    }

    m_RenderObjects[RENDER_DISTORTION].clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    if (FAILED(m_pPostShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pPostShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pPostShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Copy_BackBuffer()))
        return E_FAIL;

    if (FAILED(m_pPostShader->Bind_SRV("g_BackBufferTexture", m_pGameInstance->GetBackBufferSRV())))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_SRV(TEXT("Target_Distortion"), m_pPostShader, "g_DistortionTexture")))
        return E_FAIL;

    m_pPostShader->Begin(3);

    m_pVIBuffer->Bind_Buffers();
    m_pVIBuffer->Render();

    return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
    if (m_RenderObjects[RENDER_BLEND].empty())
        return S_OK;

    // 알파소팅
    Render_Sorting(RENDER_BLEND);

    for (auto& pGameObject : m_RenderObjects[RENDER_BLEND])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();

        //Safe_Release(pGameObject);
    }

    m_RenderObjects[RENDER_BLEND].clear();

    return S_OK;
}

HRESULT CRenderer::Render_UI()
{
    if (m_RenderObjects[RENDER_UI].empty())
        return S_OK;

    for (auto& pGameObject : m_RenderObjects[RENDER_UI])
    {
        if (nullptr != pGameObject)
            pGameObject->Render();

        //Safe_Release(pGameObject);
    }

    m_RenderObjects[RENDER_UI].clear();

    return S_OK;
}

HRESULT CRenderer::Render_PostProcess()
{
    return E_NOTIMPL;
}

HRESULT CRenderer::Render_Sorting(RENDERGROUP eRenderGroup)
{
    _float4 vCamPos = m_pGameInstance->Get_CamPosition();

    if (isnan(vCamPos.x))
        return S_OK;

    m_RenderObjects[eRenderGroup].sort([vCamPos](const shared_ptr<CGameObject>& pSrc, const shared_ptr<CGameObject>& pDst) {
        _vector vSrcPos = pSrc->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
        _vector vDstPos = pDst->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

        _float fDistSrc = XMVectorGetX(XMVector4Length(vSrcPos - XMLoadFloat4(&vCamPos)));
        _float fDistDst = XMVectorGetX(XMVector4Length(vDstPos - XMLoadFloat4(&vCamPos)));

        if (fDistSrc > fDistDst)
            return true;

        return false;
        });

    return S_OK;
}

HRESULT CRenderer::Render_Debug()
{
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    m_pGameInstance->Render_MRT(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer);

    m_pGameInstance->Render_MRT(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer);

    m_pGameInstance->Render_MRT(TEXT("MRT_Glow"), m_pShader, m_pVIBuffer);

    m_pGameInstance->Render_MRT(TEXT("MRT_Blur_X"), m_pShader, m_pVIBuffer);

    m_pGameInstance->Render_MRT(TEXT("MRT_Blur_Y"), m_pShader, m_pVIBuffer);

    m_pGameInstance->Render_MRT(TEXT("MRT_Glow"), m_pShader, m_pVIBuffer);

    m_pGameInstance->Render_MRT(TEXT("MRT_Distortion"), m_pShader, m_pVIBuffer);

    for (auto& pComponent : m_DebugCom)
    {
        pComponent->Render();
    }

    m_DebugCom.clear();

    return S_OK;
}

shared_ptr<CRenderer> CRenderer::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{

    shared_ptr<CRenderer>      pInstance = make_shared<CRenderer>(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CRenderer");
        //Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}


void CRenderer::Free()
{
    for (size_t i = 0; i < RENDER_END; i++)
        m_RenderObjects[i].clear();

    m_pContext.Reset();
    m_pDevice.Reset();
}