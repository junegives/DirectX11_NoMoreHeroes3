#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CTarget_Manager final
{
public:
	CTarget_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CTarget_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderTarget(const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag);
	HRESULT Begin_MRT(const wstring& strMRTTag);
	HRESULT End_MRT();
	HRESULT Bind_SRV(const wstring& strTargetTag, shared_ptr<class CShader> pShader, const _char* pConstantName);

public:
	HRESULT Ready_BackBufferCopyTexture(_uint iWinCX, _uint iWinCY);
	HRESULT Copy_BackBuffer();
	ComPtr<ID3D11ShaderResourceView>	GetBackBufferSRV() { return m_pBackBufferSRV; }

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_MRT(const wstring& strMRTTag, shared_ptr<class CShader> pShader, shared_ptr<class CVIBuffer_Rect> pVIBuffer);
#endif // _DEBUG

private:
	ComPtr<ID3D11Device>				m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>			m_pContext = { nullptr };

	ID3D11RenderTargetView*				m_pBackBufferView = { nullptr };
	ComPtr<ID3D11DepthStencilView>		m_pDepthStencilView = { nullptr };

	ID3D11Texture2D*					m_pBackBufferTexture = { nullptr };
	ID3D11ShaderResourceView*			m_pBackBufferSRV = nullptr;

private:
	map<const wstring, shared_ptr<class CRenderTarget>>				m_RenderTargets;

	/* 장치에 동시에 바인딩되어야할 렌더타겟들(최대 8개)을 미리 모아둔다. */
	map<const wstring, list<shared_ptr<class CRenderTarget>>>		m_MRTs;

private:
	shared_ptr<class CRenderTarget> Find_RenderTarget(const wstring& strTargetTag);
	list<shared_ptr<class CRenderTarget>>* Find_MRT(const wstring& strMRTTag);

public:
	static shared_ptr<CTarget_Manager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	void Free();
};

END