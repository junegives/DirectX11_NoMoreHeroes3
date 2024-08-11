#include "Target_Manager.h"
#include "RenderTarget.h"

CTarget_Manager::CTarget_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice(pDevice), m_pContext(pContext)
{
}

HRESULT CTarget_Manager::Initialize()
{
    return S_OK;
}

HRESULT CTarget_Manager::Add_RenderTarget(const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
    if (nullptr != Find_RenderTarget(strTargetTag))
        return E_FAIL;

    shared_ptr<CRenderTarget> pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iSizeX, iSizeY, ePixelFormat, vClearColor);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    m_RenderTargets.emplace(strTargetTag, pRenderTarget);

    return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag)
{
	shared_ptr<CRenderTarget> pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	list<shared_ptr<CRenderTarget>>* pMRTList = Find_MRT(strMRTTag);

	if (nullptr == pMRTList)
	{
		list<shared_ptr<CRenderTarget>>		MRTList;
		MRTList.push_back(pRenderTarget);

		m_MRTs.emplace(strMRTTag, MRTList);
	}
	else
		pMRTList->push_back(pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(const wstring& strMRTTag)
{
	list<shared_ptr<CRenderTarget>>* pMRTList = Find_MRT(strMRTTag);
	
	// 최대 8개의 렌더 타겟을 다룰 수 있는 렌더 타겟  뷰(렌더링 타겟을 렌더링하는 데 사용되는 객체)
	ComPtr<ID3D11RenderTargetView> pRTVs[8] = { nullptr };

	// 현재 렌더 타겟 및 깊이 스텐실 뷰를 가져온다. (이전에 설정된 렌더 타겟과 깊이 스텐실 뷰를 임시로 보관한다.)
	// 백버퍼를 가져온다.
	// (가져올 렌더링 타겟의 수, 현재 설정된 백버퍼에 대한 렌더링 타겟 뷰의 포인터를 받는 변수, 현재 설정된 깊이/스텐실 버퍼에 대한 깊이/스텐실 뷰의 포인터를 받는 변수)
	m_pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	_uint		iNumRTVs = { 0 };

	for (auto& pRenderTarget : *pMRTList)
	{
		// 각 렌더 타겟을 지우고
		// 각 렌더타겟의 렌더 타겟 뷰를 배열에 추가한다.
		pRenderTarget->Clear();
		pRTVs[iNumRTVs++] = pRenderTarget->Get_RTV();
	}

	// 새로 설정된 다중 렌더 타겟을 사용하여 렌더링 시작 (현재 렌더 타겟을 새로 설정한 MRT로 변경)
	// 새로운 렌더링 타겟 및 깊이/스텐실 버퍼를 설정하는 함수
	// (설정할 렌더링 타겟의 수, 설정할 렌더링 타겟들의 배열, 설정할 깊이/스텐실 버퍼의 깊이/스텐실 뷰)
	m_pContext->OMSetRenderTargets(iNumRTVs, pRTVs->GetAddressOf(), m_pDepthStencilView.Get());


	return S_OK;
}

HRESULT CTarget_Manager::End_MRT()
{
	// 현재 렌더링 타겟을 백 버퍼로 다시 설정한다. 이전에 설정된 렌더링 타겟을 해제하고 백버퍼를 다시 사용한다.
	m_pContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView.Get());

	Safe_Release(m_pBackBufferView);
	//Safe_Release(m_pDepthStencilView);

	return S_OK;
}

HRESULT CTarget_Manager::Bind_SRV(const wstring& strTargetTag, shared_ptr<class CShader> pShader, const _char* pConstantName)
{
	shared_ptr<CRenderTarget> pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_SRV(pShader, pConstantName);
}

HRESULT CTarget_Manager::Ready_BackBufferCopyTexture(_uint iWinCX, _uint iWinCY)
{
	// 백버퍼 가져오기
	ID3D11RenderTargetView* pBackBufferRTV = nullptr;
	m_pContext->OMGetRenderTargets(1, &pBackBufferRTV, nullptr);

	// 백버퍼 렌더 타겟 Description 가져오기
	D3D11_RENDER_TARGET_VIEW_DESC pBackBufferDesc;
	pBackBufferRTV->GetDesc(&pBackBufferDesc);

	// 백버퍼가 복사될 텍스처 생성하기
	D3D11_TEXTURE2D_DESC tBackBufferCopyDesc;
	ZeroMemory(&tBackBufferCopyDesc, sizeof(D3D11_TEXTURE2D_DESC));

	tBackBufferCopyDesc.Width = iWinCX;
	tBackBufferCopyDesc.Height = iWinCY;
	tBackBufferCopyDesc.MipLevels = 1;
	tBackBufferCopyDesc.ArraySize = 1;
	tBackBufferCopyDesc.Format = pBackBufferDesc.Format;
	tBackBufferCopyDesc.SampleDesc.Quality = 0;
	tBackBufferCopyDesc.SampleDesc.Count = 1;
	tBackBufferCopyDesc.Usage = D3D11_USAGE_DEFAULT;
	tBackBufferCopyDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	tBackBufferCopyDesc.CPUAccessFlags = 0;
	tBackBufferCopyDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&tBackBufferCopyDesc, nullptr, &m_pBackBufferTexture)))
		return E_FAIL;

	Safe_Release(pBackBufferRTV);

	return S_OK;
}

HRESULT CTarget_Manager::Copy_BackBuffer()
{
	// 백퍼버 가져오기
	ID3D11RenderTargetView* pBackBufferRTV = nullptr;
	m_pContext->OMGetRenderTargets(1, &pBackBufferRTV, nullptr);

	// 백버퍼의 렌더 타겟 텍스처 가져오기
	ID3D11Resource*			pBackBufferResource = nullptr;
	pBackBufferRTV->GetResource(&pBackBufferResource);

	// 백버퍼 텍스처를 'm_pBackBufferTexture'에 저장
	m_pContext->CopyResource(m_pBackBufferTexture, pBackBufferResource);

	Safe_Release(m_pBackBufferSRV);

	// 쉐이더에 던질 백버퍼 텍스처로 SRV를 생성
	if (FAILED(m_pDevice->CreateShaderResourceView(m_pBackBufferTexture, nullptr, &m_pBackBufferSRV)))
		return E_FAIL;

	Safe_Release(pBackBufferResource);
	Safe_Release(pBackBufferRTV);

	return S_OK;
}

HRESULT CTarget_Manager::Ready_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	shared_ptr<CRenderTarget> pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);
}

HRESULT CTarget_Manager::Render_MRT(const wstring& strMRTTag, shared_ptr<class CShader> pShader, shared_ptr<class CVIBuffer_Rect> pVIBuffer)
{
	list<shared_ptr<CRenderTarget>>* pMRTList = Find_MRT(strMRTTag);

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Render(pShader, pVIBuffer);
	}

	return S_OK;
}

shared_ptr<class CRenderTarget> CTarget_Manager::Find_RenderTarget(const wstring& strTargetTag)
{
	auto	iter = m_RenderTargets.find(strTargetTag);

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;
}

list<shared_ptr<class CRenderTarget>>* CTarget_Manager::Find_MRT(const wstring& strMRTTag)
{
	auto	iter = m_MRTs.find(strMRTTag);
	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

shared_ptr<CTarget_Manager> CTarget_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CTarget_Manager> pInstance = make_shared<CTarget_Manager>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CTarget_Manager");
		pInstance.reset();
	}

	return pInstance;
}

void CTarget_Manager::Free()
{
	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
		{
			pRenderTarget->Free();
			pRenderTarget.reset();
		}

		Pair.second.clear();
	}

	m_MRTs.clear();

	for (auto& Pair : m_RenderTargets)
	{
		Pair.second->Free();
	}

	m_RenderTargets.clear();

	m_pContext.Reset();
	m_pDevice.Reset();
	Safe_Release(m_pBackBufferTexture);
	Safe_Release(m_pBackBufferSRV);
}
