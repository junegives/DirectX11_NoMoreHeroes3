#include "RenderTarget.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

CRenderTarget::CRenderTarget(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
}

CRenderTarget::~CRenderTarget()
{
	Free();
}

HRESULT CRenderTarget::Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	m_vClearColor = vClearColor;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iSizeX;
	TextureDesc.Height = iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = ePixelFormat;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D.Get(), nullptr, &m_pRTV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D.Get(), nullptr, &m_pSRV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderTarget::Clear()
{
	m_pContext->ClearRenderTargetView(m_pRTV.Get(), (_float*)&m_vClearColor);

	return S_OK;
}

HRESULT CRenderTarget::Bind_SRV(shared_ptr<class CShader> pShader, const _char* pConstantName)
{
	return pShader->Bind_SRV(pConstantName, m_pSRV);
}

#ifdef _DEBUG
// 디버그 렌더링을 준비하는 함수
// 디버그 렌더링을 위해 렌더 타겟의 월드 행렬을 설정하는 함수
HRESULT CRenderTarget::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	D3D11_VIEWPORT			ViewPortDesc{};

	_uint		iNumViewports = 1;

	m_pContext->RSGetViewports(&iNumViewports, &ViewPortDesc);

	// 월드 행렬을 단위 행렬로 초기화
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	// 월드 행렬의 스케일 요소 창크기로, 월드 행렬을 뷰포트의 가운데로 이동하여 렌더링 위치 조정
	m_WorldMatrix._11 = fSizeX;
	m_WorldMatrix._22 = fSizeY;
	m_WorldMatrix._41 = fX - ViewPortDesc.Width * 0.5f;
	m_WorldMatrix._42 = -fY + ViewPortDesc.Height * 0.5f;

	return S_OK;
}

HRESULT CRenderTarget::Render(shared_ptr<class CShader> pShader, shared_ptr<class CVIBuffer_Rect> pVIBuffer)
{
	if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(pShader->Bind_SRV("g_Texture", m_pSRV)))
		return E_FAIL;

	pShader->Begin(0);

	pVIBuffer->Bind_Buffers();

	pVIBuffer->Render();

	return S_OK;
}
#endif // _DEBUG

shared_ptr<CRenderTarget> CRenderTarget::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	shared_ptr<CRenderTarget> pInstance = make_shared<CRenderTarget>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iSizeX, iSizeY, ePixelFormat, vClearColor)))
	{
		MSG_BOX("Failed to Created : CRenderTarget");
		pInstance.reset();
	}

	return pInstance;
}

void CRenderTarget::Free()
{
	//Safe_Release(m_pSRV);
	//Safe_Release(m_pRTV);
	//Safe_Release(m_pTexture2D);

	//m_pDevice.Reset();
	//m_pContext.Reset();
}
