#pragma once

#include "Engine_Defines.h"

/* ȭ�鿡 �׷������� ��ü���� �׸��� ������� �����Ѵ�. */
/* �����ϰ� �ִ� ������� ��ü���� Draw���� �����Ѵ�. */

BEGIN(Engine)

class CRenderer final
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONBLEND, RENDER_NONLIGHT, RENDER_GLOW, RENDER_DISTORTION, RENDER_BLEND, RENDER_UI, RENDER_DEBUG, RENDER_END };

public:
	CRenderer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, shared_ptr<class CGameObject> pGameObject);
	HRESULT Render();

#ifdef _DEBUG
public:
	HRESULT Add_DebugComponent(shared_ptr<class CComponent> pComponent);
#endif // _DEBUG

public:
	void	SetRenderDebug() { m_isRenderDebug = !m_isRenderDebug; }

private:
	ComPtr<ID3D11Device>				m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>			m_pContext = { nullptr };

private:
	list<shared_ptr<class CGameObject>>		m_RenderObjects[RENDER_END];
	shared_ptr<class CGameInstance>			m_pGameInstance = { nullptr };

private:
	shared_ptr<class CShader>				m_pShader = { nullptr };
	shared_ptr<class CShader>				m_pPostShader = { nullptr };

	shared_ptr<class CVIBuffer_Rect>		m_pVIBuffer = { nullptr };

	_float4x4								m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	_float									m_fDistortionTimer;

private:
	_bool	m_isRenderDebug = true;

#ifdef _DEBUG
private:
	list<shared_ptr<class CComponent>>		m_DebugCom;
#endif // _DEBUG

private:
	HRESULT Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_Lights();
	HRESULT Render_Final();
	HRESULT Render_NonLight();
	HRESULT Render_Glow();
	HRESULT Render_Distortion();
	HRESULT Render_Blend();
	HRESULT Render_UI();

	HRESULT Render_PostProcess();

	HRESULT Render_Sorting(RENDERGROUP eRenderGroup);

#ifdef _DEBUG
	HRESULT Render_Debug();
#endif // _DEBUG

public:
	static shared_ptr<CRenderer> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	void Free();
};

END