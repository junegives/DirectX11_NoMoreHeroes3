#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CRenderTarget final
{
public:
	CRenderTarget(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	~CRenderTarget();

public:
	ComPtr<ID3D11RenderTargetView> Get_RTV() const {
		return m_pRTV;
	}

public:
	HRESULT Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Clear();
	HRESULT Bind_SRV(shared_ptr<class CShader> pShader, const _char* pConstantName);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render(shared_ptr<class CShader> pShader, shared_ptr<class CVIBuffer_Rect> pVIBuffer);
#endif // _DEBUG

private:
	ComPtr<ID3D11Device>		m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>	m_pContext = { nullptr };

	ComPtr<ID3D11Texture2D>				m_pTexture2D = { nullptr };
	ComPtr<ID3D11RenderTargetView>		m_pRTV = { nullptr };
	ComPtr<ID3D11ShaderResourceView>	m_pSRV = { nullptr };

	_float4						m_vClearColor;

#ifdef _DEBUG
private:
	_float4x4					m_WorldMatrix;
#endif // _DEBUG

public:
	static shared_ptr<CRenderTarget> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	void Free();
};

END