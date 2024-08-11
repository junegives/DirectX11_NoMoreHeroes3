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
	
	// �ִ� 8���� ���� Ÿ���� �ٷ� �� �ִ� ���� Ÿ��  ��(������ Ÿ���� �������ϴ� �� ���Ǵ� ��ü)
	ComPtr<ID3D11RenderTargetView> pRTVs[8] = { nullptr };

	// ���� ���� Ÿ�� �� ���� ���ٽ� �並 �����´�. (������ ������ ���� Ÿ�ٰ� ���� ���ٽ� �並 �ӽ÷� �����Ѵ�.)
	// ����۸� �����´�.
	// (������ ������ Ÿ���� ��, ���� ������ ����ۿ� ���� ������ Ÿ�� ���� �����͸� �޴� ����, ���� ������ ����/���ٽ� ���ۿ� ���� ����/���ٽ� ���� �����͸� �޴� ����)
	m_pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	_uint		iNumRTVs = { 0 };

	for (auto& pRenderTarget : *pMRTList)
	{
		// �� ���� Ÿ���� �����
		// �� ����Ÿ���� ���� Ÿ�� �並 �迭�� �߰��Ѵ�.
		pRenderTarget->Clear();
		pRTVs[iNumRTVs++] = pRenderTarget->Get_RTV();
	}

	// ���� ������ ���� ���� Ÿ���� ����Ͽ� ������ ���� (���� ���� Ÿ���� ���� ������ MRT�� ����)
	// ���ο� ������ Ÿ�� �� ����/���ٽ� ���۸� �����ϴ� �Լ�
	// (������ ������ Ÿ���� ��, ������ ������ Ÿ�ٵ��� �迭, ������ ����/���ٽ� ������ ����/���ٽ� ��)
	m_pContext->OMSetRenderTargets(iNumRTVs, pRTVs->GetAddressOf(), m_pDepthStencilView.Get());


	return S_OK;
}

HRESULT CTarget_Manager::End_MRT()
{
	// ���� ������ Ÿ���� �� ���۷� �ٽ� �����Ѵ�. ������ ������ ������ Ÿ���� �����ϰ� ����۸� �ٽ� ����Ѵ�.
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
	// ����� ��������
	ID3D11RenderTargetView* pBackBufferRTV = nullptr;
	m_pContext->OMGetRenderTargets(1, &pBackBufferRTV, nullptr);

	// ����� ���� Ÿ�� Description ��������
	D3D11_RENDER_TARGET_VIEW_DESC pBackBufferDesc;
	pBackBufferRTV->GetDesc(&pBackBufferDesc);

	// ����۰� ����� �ؽ�ó �����ϱ�
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
	// ���۹� ��������
	ID3D11RenderTargetView* pBackBufferRTV = nullptr;
	m_pContext->OMGetRenderTargets(1, &pBackBufferRTV, nullptr);

	// ������� ���� Ÿ�� �ؽ�ó ��������
	ID3D11Resource*			pBackBufferResource = nullptr;
	pBackBufferRTV->GetResource(&pBackBufferResource);

	// ����� �ؽ�ó�� 'm_pBackBufferTexture'�� ����
	m_pContext->CopyResource(m_pBackBufferTexture, pBackBufferResource);

	Safe_Release(m_pBackBufferSRV);

	// ���̴��� ���� ����� �ؽ�ó�� SRV�� ����
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
