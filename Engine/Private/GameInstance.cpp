#include "GameInstance.h"
#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Component_Manager.h"
#include "Layer.h"
#include "Renderer.h"
#include "Light_Manager.h"
#include "Font_Manager.h"
#include "Collision_Manager.h"
#include "Target_Manager.h"
#include "Camera_Manager.h"

IMPLEMENT_SINGLETON_SMART(CGameInstance)

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, ComPtr<ID3D11Device>* ppDevice, ComPtr<ID3D11DeviceContext>* ppDeviceContext)
{
    /* 그래픽 초기화 */
    m_pGraphic_Device = CGraphic_Device::Create(GraphicDesc, ppDevice, ppDeviceContext, GraphicDesc.hWnd);
    if (nullptr == m_pGraphic_Device)
        return E_FAIL;

    /* 폰트 초기화 */
	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppDeviceContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

    /* 입력장치 초기화 */
    m_pInput_Device = CInput_Device::Create(hInst, GraphicDesc.hWnd);
    if (nullptr == m_pInput_Device)
        return E_FAIL;

    /* 파이프라인 초기화 */
    m_pPipeline = CPipeline::Create();
    if (nullptr == m_pPipeline)
        return E_FAIL;

    // 타임 매니저 초기화
    m_pTimer_Manager = CTimer_Manager::Create();
    if (nullptr == m_pTimer_Manager)
        return E_FAIL;

    /* 타겟 매니저 리저브 */
    m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppDeviceContext);
    if (nullptr == m_pTarget_Manager)
        return E_FAIL;

    /* 렌더러 초기화 */
    m_pRenderer = CRenderer::Create(*ppDevice, *ppDeviceContext);
    if (nullptr == m_pRenderer)
        return E_FAIL;
    
    /* 레벨 매니저 초기화 */
    m_pLevel_Manager = CLevel_Manager::Create();
    if (nullptr == m_pLevel_Manager)
        return E_FAIL;

    /* 오브젝트 매니져 리저브*/
    m_pObject_Manager = CObject_Manager::Create(iNumLevels);
    if (nullptr == m_pObject_Manager)
        return E_FAIL;

    /* 컴포넌트 매니져 리저브*/
    m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
    if (nullptr == m_pComponent_Manager)
        return E_FAIL;

    /* 조명 매니져 리저브*/
    m_pLight_Manager = CLight_Manager::Create(*ppDevice, *ppDeviceContext);
    if (nullptr == m_pLight_Manager)
        return E_FAIL;

    /* 게임 매니져 리저브*/
    m_pGame_Manager = CGame_Manager::Create(*ppDevice, *ppDeviceContext, GraphicDesc.hWnd);
    if (nullptr == m_pGame_Manager)
        return E_FAIL;

    /* 콜리전 매니져 리저브*/
    m_pCollision_Manager = CCollision_Manager::Create();
    if (nullptr == m_pCollision_Manager)
        return E_FAIL;

    /* 파이프라인 초기화 */
    m_pCamera_Manager = CCamera_Manager::Create();
    if (nullptr == m_pCamera_Manager)
        return E_FAIL;

    return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
    if (nullptr == m_pLevel_Manager ||
        nullptr == m_pObject_Manager)
        return;

	/* 다이렉트 인풋을 통해 키보드와 마우스의 상태를 저장ㅎ나다 .*/
    m_pInput_Device->Tick();

	m_pObject_Manager->Priority_Tick(fTimeDelta);

    m_pPipeline->Tick();

    // object 업데이트 용
	m_pObject_Manager->Tick(fTimeDelta);

    m_pLevel_Manager->Tick(fTimeDelta);

	m_pObject_Manager->Late_Tick(fTimeDelta);

    // 충돌 탐색 용
    m_pLevel_Manager->Late_Tick(fTimeDelta);
}

HRESULT CGameInstance::Render_Engine(const _float4& vClearColor)
{
    if (nullptr == m_pGraphic_Device)
        return E_FAIL;

    m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
    m_pGraphic_Device->Clear_DepthStencil_View();

    /* 그리기용 객체를 통해 무언가를 그린다. */
    if (FAILED(Draw()))
        return E_FAIL;

    return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::Draw()
{
    /* 그리기용 객체를 통해 무언가를 그린다. */
    m_pRenderer->Render();
    //m_pLevel_Manager->Render();

    return S_OK;
}

void CGameInstance::Clear(_uint iLevelIndex)
{
    if (nullptr == m_pObject_Manager ||
        nullptr == m_pComponent_Manager)
        return;

    m_pObject_Manager->Clear(iLevelIndex);
    m_pComponent_Manager->Clear(iLevelIndex);
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
    if (nullptr == m_pGraphic_Device)
        return E_FAIL;

    m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
    return S_OK;
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
    if (nullptr == m_pGraphic_Device)
        return E_FAIL;

    m_pGraphic_Device->Clear_DepthStencil_View();
    return S_OK;
}

HRESULT CGameInstance::Present()
{
    if (nullptr == m_pGraphic_Device)
        return E_FAIL;

    m_pGraphic_Device->Present();
    return S_OK;
}

void CGameInstance::Draw2DText(const WCHAR* text, _float x, _float y)
{
    if (nullptr == m_pGraphic_Device)
        return;

    //m_pGraphic_Device->Draw2DText(text, x, y);
}

_bool CGameInstance::Is_KeyDown(_ubyte byKeyID)
{
    if (nullptr == m_pInput_Device)
        return 0;

    return m_pInput_Device->Is_KeyDown(byKeyID);
}

_bool CGameInstance::Is_KeyUp(_ubyte byKeyID)
{
    if (nullptr == m_pInput_Device)
        return 0;

    return m_pInput_Device->Is_KeyUp(byKeyID);
}

_bool CGameInstance::Is_KeyPressing(_ubyte byKeyID)
{
    if (nullptr == m_pInput_Device)
        return 0;

    return m_pInput_Device->Is_KeyPressing(byKeyID);
}

_bool CGameInstance::Is_MouseDown(MOUSEKEYSTATE eMouse)
{
    if (nullptr == m_pInput_Device)
        return 0;

    return m_pInput_Device->Is_MouseDown(eMouse);
}

_bool CGameInstance::Is_MouseUp(MOUSEKEYSTATE eMouse)
{
    if (nullptr == m_pInput_Device)
        return 0;

    return m_pInput_Device->Is_MouseUp(eMouse);
}

_bool CGameInstance::Is_MousePressing(MOUSEKEYSTATE eMouse)
{
    if (nullptr == m_pInput_Device)
        return 0;

    return m_pInput_Device->Is_MousePressing(eMouse);
}

_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
    if (nullptr == m_pInput_Device)
        return 0;

    return m_pInput_Device->Get_DIKeyState(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
    if (nullptr == m_pInput_Device)
        return 0;

    return m_pInput_Device->Get_DIMouseState(eMouse);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
    if (nullptr == m_pInput_Device)
        return 0;

    return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

_float CGameInstance::Compute_TimeDelta(const wstring& pTimerTag)
{
    if (nullptr == m_pTimer_Manager)
        return 0.f;

    return m_pTimer_Manager->Compute_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Add_Timer(const wstring& pTimerTag)
{
    if (nullptr == m_pTimer_Manager)
        return E_FAIL;

    return m_pTimer_Manager->Add_Timer(pTimerTag);
}

HRESULT CGameInstance::Add_RenderGroup(CRenderer::RENDERGROUP eRenderGroup, shared_ptr<class CGameObject> pGameObject)
{
    if (nullptr == m_pRenderer)
        return E_FAIL;

    return m_pRenderer->Add_RenderGroup(eRenderGroup, pGameObject);
}

void CGameInstance::SetRenderDebug()
{
    if (nullptr == m_pRenderer)
        return;

    return m_pRenderer->SetRenderDebug();
}

HRESULT CGameInstance::Add_DebugComponent(shared_ptr<class CComponent> pComponent)
{
    return m_pRenderer->Add_DebugComponent(pComponent);
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, shared_ptr<class CLevel> pNewLevel)
{
    if (nullptr == m_pLevel_Manager)
        return E_FAIL;

    return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);
}

HRESULT CGameInstance::Set_CurLevel(_uint eCurLevel)
{
    if (nullptr == m_pGame_Manager)
        return E_FAIL;

    m_pObject_Manager->Set_CurLevel(eCurLevel);

    return S_OK;
}

_uint CGameInstance::Get_CurLevel()
{
    if (nullptr == m_pGame_Manager)
        return 0;

    return m_pObject_Manager->Get_CurLevel();
}

HRESULT CGameInstance::Add_GameObject(_uint iLevelIndex, const LAYER_ID& eLayerID, const wstring& strObjectTag, shared_ptr<class CGameObject> pGameObject)
{
    if (nullptr == m_pObject_Manager)
        return E_FAIL;

    return m_pObject_Manager->Add_GameObject(iLevelIndex, eLayerID, strObjectTag, pGameObject);
}

shared_ptr<class CGameObject> CGameInstance::Find_GameObject(_uint iLevelIndex, const LAYER_ID& eLayerID, const wstring& strObjectTag)
{
    if (nullptr == m_pObject_Manager)
        return nullptr;

    return m_pObject_Manager->Find_GameObject(iLevelIndex, eLayerID, strObjectTag);
}

shared_ptr<class CLayer> CGameInstance::Find_Layer(_uint iLevelIndex, const LAYER_ID& eLayerID)
{
    if (nullptr == m_pObject_Manager)
        return nullptr;

    return m_pObject_Manager->Find_Layer(iLevelIndex, eLayerID);
}

list<shared_ptr<class CGameObject>> CGameInstance::Get_Layer_Objects(_uint iLevelIndex, const LAYER_ID& eLayerID)
{
    return m_pObject_Manager->Get_Layer_Objects(iLevelIndex, eLayerID);
}

//shared_ptr<class CGameObject> CGameInstance::Clone_GameObject(_uint iLevelIndex, const LAYER_ID& eLayerID, const wstring& strObjectTag)
//{
//    if (nullptr == m_pObject_Manager)
//        return nullptr;
//
//    return m_pObject_Manager->Clone_GameObject(iLevelIndex, eLayerID, strObjectTag);
//}

HRESULT CGameInstance::Remove_Object(_uint iLevelIndex, const LAYER_ID& eLayerID, const wstring& strObjectTag)
{
    if (nullptr == m_pObject_Manager)
        return E_FAIL;

    return m_pObject_Manager->Remove_Object(iLevelIndex, eLayerID, strObjectTag);
}

HRESULT CGameInstance::Remove_Object(_uint iLevelIndex, const LAYER_ID& eLayerID, shared_ptr<class CGameObject> pGameObject)
{
    if (nullptr == m_pObject_Manager)
        return E_FAIL;

    return m_pObject_Manager->Remove_Object(iLevelIndex, eLayerID, pGameObject);
}

HRESULT CGameInstance::Remove_Layer(_uint iLevelIndex, const LAYER_ID& eLayerID)
{
    if (nullptr == m_pObject_Manager)
        return E_FAIL;

    return m_pObject_Manager->Remove_Layer(iLevelIndex, eLayerID);
}

shared_ptr<class CComponent> CGameInstance::Find_Component(_uint iLevelIndex, const LAYER_ID& eLayerID, const wstring& strComTag, _uint iIndex)
{
    return m_pObject_Manager->Find_Component(iLevelIndex, eLayerID, strComTag, iIndex);
}

HRESULT CGameInstance::Add_Component(_uint iLevelIndex, const wstring& strComponentTag, shared_ptr<class CComponent> pPrototype)
{
    if (nullptr == m_pComponent_Manager)
        return E_FAIL;

    return m_pComponent_Manager->Add_Component(iLevelIndex, strComponentTag, pPrototype);
}

shared_ptr<class CComponent> CGameInstance::Find_Component(_uint iLevelIndex, const wstring& strComponentTag)
{
    if (nullptr == m_pComponent_Manager)
        return nullptr;

    return m_pComponent_Manager->Find_Component(iLevelIndex, strComponentTag);
}

shared_ptr<class CComponent> CGameInstance::Find_Prototype(_uint iLevelIndex, const wstring& strComponentTag)
{
    if (nullptr == m_pComponent_Manager)
        return nullptr;

    return m_pComponent_Manager->Find_Prototype(iLevelIndex, strComponentTag);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, shared_ptr<CComponent> pPrototype)
{
    if (nullptr == m_pComponent_Manager)
        return E_FAIL;

    return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Remove_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag)
{
    if (nullptr == m_pComponent_Manager)
        return E_FAIL;

    return m_pComponent_Manager->Remove_Prototype(iLevelIndex, strPrototypeTag);
}

shared_ptr<CComponent> CGameInstance::Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg)
{
    if (nullptr == m_pComponent_Manager)
        return nullptr;

    return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

void CGameInstance::Set_Transform(CPipeline::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
    if (nullptr == m_pPipeline)
        return;

    return m_pPipeline->Set_Transform(eState, TransformMatrix);
}

_float4x4 CGameInstance::Get_Transform_Float4x4(CPipeline::D3DTRANSFORMSTATE eState)
{
    if (nullptr == m_pPipeline)
        return _float4x4();

    return m_pPipeline->Get_Transform_Float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Matrix(CPipeline::D3DTRANSFORMSTATE eState)
{
    if (nullptr == m_pPipeline)
        return _matrix();

    return m_pPipeline->Get_Transform_Matrix(eState);
}

_float4x4 CGameInstance::Get_Transform_Float4x4_Inverse(CPipeline::D3DTRANSFORMSTATE eState)
{
    if (nullptr == m_pPipeline)
        return _float4x4();

    return m_pPipeline->Get_Transform_Float4x4_Inverse(eState);
}

_matrix CGameInstance::Get_Transform_Matrix_Inverse(CPipeline::D3DTRANSFORMSTATE eState)
{
    if (nullptr == m_pPipeline)
        return _matrix();

    return m_pPipeline->Get_Transform_Matrix_Inverse(eState);
}

_float4 CGameInstance::Get_CamPosition()
{
    if (nullptr == m_pPipeline)
        return _float4();

    return m_pPipeline->Get_CamPosition();
}

_float4 CGameInstance::Get_CamLook()
{
    if (nullptr == m_pPipeline)
        return _float4();

    return m_pPipeline->Get_CamLook();
}

HRESULT CGameInstance::AddCamera(CCamera_Manager::CAMERA_ID eCamType, shared_ptr<class CGameObject> pCamera)
{
    if (nullptr == m_pCamera_Manager)
        return E_FAIL;

    m_pCamera_Manager->AddCamera(eCamType, pCamera);

    return S_OK;
}

shared_ptr<class CGameObject> CGameInstance::GetCamera(CCamera_Manager::CAMERA_ID eCamType)
{
    if (nullptr == m_pCamera_Manager)
        return nullptr;

    return m_pCamera_Manager->GetCamera(eCamType);
}

void CGameInstance::SetMainCam(CCamera_Manager::CAMERA_ID eCamType)
{
    if (nullptr == m_pCamera_Manager)
        return;

    return m_pCamera_Manager->SetMainCam(eCamType);
}

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
    if (nullptr == m_pLight_Manager)
        return E_FAIL;

    return m_pLight_Manager->Add_Light(LightDesc);
}

const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iIndex) const
{
    if (nullptr == m_pLight_Manager)
        return nullptr;

    return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Render_Light(shared_ptr<class CShader> pShader, shared_ptr<class CVIBuffer_Rect> pVIBuffer)
{
    return m_pLight_Manager->Render(pShader, pVIBuffer);
}

void CGameInstance::Change_ClipCursor()
{
    if (nullptr == m_pGame_Manager)
        return;

    m_pGame_Manager->Change_ClipCursor();
}

HRESULT CGameInstance::Set_GameState(CGame_Manager::EGameState eState)
{
    if (nullptr == m_pGame_Manager)
        return E_FAIL;

    m_pGame_Manager->Set_GameState(eState);

    return S_OK;
}

CGame_Manager::EGameState CGameInstance::Get_GameState()
{
    if (nullptr == m_pGame_Manager)
        return CGame_Manager::STATE_END;

    return m_pGame_Manager->Get_GameState();
}

HRESULT CGameInstance::Set_Player(shared_ptr<class CGameObject> pPlayer)
{
    if (nullptr == m_pGame_Manager)
        return E_FAIL;

    m_pGame_Manager->Set_Player(pPlayer);

    return S_OK;
}

shared_ptr<class CGameObject> CGameInstance::Get_Player()
{
    if (nullptr == m_pGame_Manager)
        return nullptr;

    return m_pGame_Manager->Get_Player();
}

HRESULT CGameInstance::Add_Font(const wstring& strFontTag, const wstring& strFontFilePath)
{
    return m_pFont_Manager->Add_Font(strFontTag, strFontFilePath);
}

HRESULT CGameInstance::Render_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor, _float fRotation, const _float2& vOrigin, _float fScale)
{
    return m_pFont_Manager->Render(strFontTag, strText, vPosition, vColor, fRotation, vOrigin, fScale);
}

void CGameInstance::Add_Collider(const LAYER_ID& eLayerID, shared_ptr<class CCollider> pCollider)
{
    if (nullptr == m_pCollision_Manager)
        return;

    return m_pCollision_Manager->Add_Collider(eLayerID, pCollider);
}

void CGameInstance::Check_Collision(const LAYER_ID& eLayerSour, const LAYER_ID& eLayerDest)
{
    if (nullptr == m_pCollision_Manager)
        return;

    return m_pCollision_Manager->Check_Collision(eLayerSour, eLayerDest);
}

HRESULT CGameInstance::Add_RenderTarget(const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
    return m_pTarget_Manager->Add_RenderTarget(strTargetTag, iSizeX, iSizeY, ePixelFormat, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag)
{
    return m_pTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const wstring& strMRTTag)
{
    return m_pTarget_Manager->Begin_MRT(strMRTTag);
}

HRESULT CGameInstance::End_MRT()
{
    return m_pTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_SRV(const wstring& strTargetTag, shared_ptr<class CShader> pShader, const _char* pConstantName)
{
    return m_pTarget_Manager->Bind_SRV(strTargetTag, pShader, pConstantName);
}

HRESULT CGameInstance::Copy_BackBuffer()
{
    return m_pTarget_Manager->Copy_BackBuffer();
}

ComPtr<ID3D11ShaderResourceView> CGameInstance::GetBackBufferSRV()
{
    return m_pTarget_Manager->GetBackBufferSRV();
}

HRESULT CGameInstance::Ready_BackBufferCopyTexture(_uint iWinCX, _uint iWinCY)
{
    return m_pTarget_Manager->Ready_BackBufferCopyTexture(iWinCX, iWinCY);
}

HRESULT CGameInstance::Ready_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
    return m_pTarget_Manager->Ready_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);
}

HRESULT CGameInstance::Render_MRT(const wstring& strMRTTag, shared_ptr<CShader> pShader, shared_ptr<CVIBuffer_Rect> pVIBuffer)
{
    return m_pTarget_Manager->Render_MRT(strMRTTag, pShader, pVIBuffer);
}

void CGameInstance::Free()
{
    m_pTarget_Manager->Free();
    m_pTarget_Manager.reset();

    m_pRenderer->Free();
    m_pRenderer.reset();

    m_pLight_Manager->Free();
    m_pLight_Manager.reset();

    m_pFont_Manager->Free();
    m_pObject_Manager->Free();
    m_pLevel_Manager->Free();
    m_pComponent_Manager->Free();
    m_pInput_Device->Free();
    //m_pGraphic_Device->Free();

    m_pCollision_Manager->Free();
    m_pObject_Manager.reset();
    m_pLevel_Manager.reset();
    m_pGraphic_Device.reset();
    CGameInstance::DestroyInstance();
}
