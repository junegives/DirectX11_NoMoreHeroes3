#pragma once

/* 엔진에서 제공하는 대부분의 기능들을 가지고 있는다. */
/* Object_Manager, Level_Manager, Component_Manager, Graphic_Device, Input_Device */
/* 위와 같은 여러 클래스들의 기능들을 클라이언트에 보여주고 실제 호출도 수행한다. */
#include "Renderer.h"
#include "Component_Manager.h"
#include "Pipeline.h"
#include "Camera_Manager.h"
#include "Game_Manager.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final
{
	DECLARE_SINGLETON_SMART(CGameInstance)
public:
	CGameInstance();
	~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, _Inout_ ComPtr<ID3D11Device>* ppDevice, _Inout_ ComPtr<ID3D11DeviceContext>* ppDeviceContext);
	void	Tick_Engine(_float fTimeDelta);
	HRESULT Render_Engine(const _float4& vClearColor);
	HRESULT Draw();
	void	Clear(_uint iLevelIndex);

public: /* For. Graphic_Device */
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();
	void	Draw2DText(const WCHAR* text, _float x, _float y);

public:	/* For. Input_Device */
	_bool	Is_KeyDown(_ubyte byKeyID);
	_bool	Is_KeyUp(_ubyte byKeyID);
	_bool	Is_KeyPressing(_ubyte byKeyID);

	_bool	Is_MouseDown(MOUSEKEYSTATE eMouse);
	_bool	Is_MouseUp(MOUSEKEYSTATE eMouse);
	_bool	Is_MousePressing(MOUSEKEYSTATE eMouse);

	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse);
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);

public: /* For.Timer_Manager */
	_float Compute_TimeDelta(const wstring& pTimerTag);
	HRESULT Add_Timer(const wstring& pTimerTag);

public: /* For.Renderer*/
	HRESULT Add_RenderGroup(CRenderer::RENDERGROUP eRenderGroup, shared_ptr<class CGameObject> pGameObject);
	void	SetRenderDebug();

#ifdef _DEBUG
public:
	HRESULT Add_DebugComponent(shared_ptr<class CComponent> pComponent);
#endif // _DEBUG

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelIndex, shared_ptr<class CLevel> pNewLevel);

public: /* For.Object_Manager */

	HRESULT	Set_CurLevel(_uint eCurLevel);
	_uint	Get_CurLevel();
	HRESULT Add_GameObject(_uint iLevelIndex, const LAYER_ID& eLayerID, const wstring& strObjectTag, shared_ptr<class CGameObject> pGameObject);
	shared_ptr<class CGameObject> Find_GameObject(_uint iLevelIndex, const LAYER_ID& eLayerID, const wstring& strObjectTag);
	shared_ptr<class CLayer>	Find_Layer(_uint iLevelIndex, const LAYER_ID& eLayerID);
	list<shared_ptr<class CGameObject>>	Get_Layer_Objects(_uint iLevelIndex, const LAYER_ID& eLayerID);
	//shared_ptr<class CGameObject> Clone_GameObject(_uint iLevelIndex, const LAYER_ID& eLayerID, const wstring& strObjectTag);
	HRESULT Remove_Object(_uint iLevelIndex, const LAYER_ID& eLayerID, const wstring& strObjectTag);
	HRESULT Remove_Object(_uint iLevelIndex, const LAYER_ID& eLayerID, shared_ptr<class CGameObject> pGameObject);
	HRESULT Remove_Layer(_uint iLevelIndex, const LAYER_ID& eLayerID);
	shared_ptr<class CComponent> Find_Component(_uint iLevelIndex, const LAYER_ID& eLayerID, const wstring& strComTag, _uint iIndex = 0);

public: /* For.Component_Manager */
	HRESULT Add_Component(_uint iLevelIndex, const wstring& strComponentTag, shared_ptr<class CComponent> pPrototype);
	shared_ptr<class CComponent> Find_Component(_uint iLevelIndex, const wstring& strComponentTag);
	shared_ptr<class CComponent> Find_Prototype(_uint iLevelIndex, const wstring& strComponentTag);
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, shared_ptr<class CComponent> pPrototype);
	HRESULT Remove_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);
	shared_ptr<class CComponent> Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);

public:	/* For. Pipeline */
	void		Set_Transform(CPipeline::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix);
	_float4x4	Get_Transform_Float4x4(CPipeline::D3DTRANSFORMSTATE eState);
	_matrix		Get_Transform_Matrix(CPipeline::D3DTRANSFORMSTATE eState);
	_float4x4	Get_Transform_Float4x4_Inverse(CPipeline::D3DTRANSFORMSTATE eState);
	_matrix		Get_Transform_Matrix_Inverse(CPipeline::D3DTRANSFORMSTATE eState);
	_float4		Get_CamPosition();
	_float4		Get_CamLook();

public:	/* For. Camera_Manager */
	HRESULT							AddCamera(CCamera_Manager::CAMERA_ID eCamType, shared_ptr<class CGameObject> pCamera);
	shared_ptr<class CGameObject>	GetCamera(CCamera_Manager::CAMERA_ID eCamType);
	void							SetMainCam(CCamera_Manager::CAMERA_ID eCamType);

public:	/* For. Light_Manager */
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;
	HRESULT Render_Light(shared_ptr<class CShader> pShader, shared_ptr<class CVIBuffer_Rect> pVIBuffer);

public: /* For. Game_Manager */
	void							Change_ClipCursor();
	HRESULT							Set_GameState(CGame_Manager::EGameState eState);
	CGame_Manager::EGameState		Get_GameState();
	HRESULT							Set_Player(shared_ptr<class CGameObject> pPlayer);
	shared_ptr<class CGameObject>	Get_Player();

public: /* For.Font_Manager */
	HRESULT Add_Font(const wstring& strFontTag, const wstring& strFontFilePath);
	HRESULT Render_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor = Colors::White, _float fRotation = 0.f, const _float2& vOrigin = _float2(0.f, 0.f), _float fScale = 1.f);

public:	/* For. Collision_Manager */
	void	Add_Collider(const LAYER_ID& eLayerID, shared_ptr<class CCollider> pCollider);
	void	Check_Collision(const LAYER_ID& eLayerSour, const LAYER_ID& eLayerDest);

public: /* For.Target_Manager */
	HRESULT Add_RenderTarget(const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag);
	HRESULT Begin_MRT(const wstring& strMRTTag);
	HRESULT End_MRT();
	HRESULT Bind_SRV(const wstring& strTargetTag, shared_ptr<class CShader> pShader, const _char* pConstantName);
	HRESULT Copy_BackBuffer();
	ComPtr<ID3D11ShaderResourceView> GetBackBufferSRV();
	HRESULT Ready_BackBufferCopyTexture(_uint iWinCX, _uint iWinCY);

#ifdef _DEBUG
	HRESULT Ready_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_MRT(const wstring& strMRTTag, shared_ptr<CShader> pShader, shared_ptr<CVIBuffer_Rect> pVIBuffer);
#endif // _DEBUG

private:
	shared_ptr<class CGraphic_Device>		m_pGraphic_Device = { nullptr };
	shared_ptr<class CInput_Device>			m_pInput_Device = { nullptr };
	shared_ptr<class CTimer_Manager>		m_pTimer_Manager = { nullptr };
	shared_ptr<class CLevel_Manager>		m_pLevel_Manager = { nullptr };
	shared_ptr<class CObject_Manager>		m_pObject_Manager = { nullptr };
	shared_ptr<class CComponent_Manager>	m_pComponent_Manager = { nullptr };
	shared_ptr<class CRenderer>				m_pRenderer = { nullptr };
	shared_ptr<class CPipeline>				m_pPipeline = { nullptr };
	shared_ptr<class CCamera_Manager>		m_pCamera_Manager = { nullptr };
	shared_ptr<class CLight_Manager>		m_pLight_Manager = { nullptr };
	shared_ptr<class CGame_Manager>			m_pGame_Manager = { nullptr };
	shared_ptr<class CFont_Manager>			m_pFont_Manager = { nullptr };
	shared_ptr<class CCollision_Manager>	m_pCollision_Manager = { nullptr };
	shared_ptr<class CTarget_Manager>		m_pTarget_Manager = { nullptr };

public:
	void Free();
};

END