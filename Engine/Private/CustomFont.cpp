#include "..\Public\CustomFont.h"

CCustomFont::CCustomFont(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
}

HRESULT CCustomFont::Initialize(const wstring & strFontFilePath)
{
	m_pBatch = make_shared<SpriteBatch>(m_pContext.Get());
	m_pFont = make_shared<SpriteFont>(m_pDevice.Get(), strFontFilePath.c_str());

	return S_OK;
}

HRESULT CCustomFont::Render(const wstring& strText, const _float2& vPosition, _fvector vColor, _float fRotation, const _float2& vOrigin, _float fScale)
{
	// 자체 셰이더를 쓰는 애들은 render에서 GSSetShader(지오메트리 셰이더) null처리 해줘야함
	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pBatch->Begin();

	m_pFont->DrawString(m_pBatch.get(), strText.c_str(), vPosition, vColor, fRotation, vOrigin, fScale);

	m_pBatch->End();

	return S_OK;
}

shared_ptr<CCustomFont> CCustomFont::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring & strFontFilePath)
{
	shared_ptr<CCustomFont>		pInstance = make_shared<CCustomFont>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(strFontFilePath)))
	{
		MSG_BOX("Failed to Created : CCustomFont");
		pInstance.reset();
	}

	return pInstance;
}

void CCustomFont::Free()
{
	m_pBatch.reset();
	m_pFont.reset();

	/*Safe_Release(m_pContext);
	Safe_Release(m_pDevice);*/
}
