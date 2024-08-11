#include "Font_Manager.h"
#include "CustomFont.h"

CFont_Manager::CFont_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
}

HRESULT CFont_Manager::Initialize()
{

	return S_OK;
}

HRESULT CFont_Manager::Add_Font(const wstring & strFontTag, const wstring & strFontFilePath)
{
	if (nullptr != Find_Font(strFontTag))
		return E_FAIL;

	shared_ptr<CCustomFont>		pFont = CCustomFont::Create(m_pDevice, m_pContext, strFontFilePath);
	if (nullptr == pFont)
		return E_FAIL;

	m_Fonts.emplace(strFontTag, pFont);

	return S_OK;
}

HRESULT CFont_Manager::Render(const wstring & strFontTag, const wstring & strText, const _float2 & vPosition, _fvector vColor, _float fRotation, const _float2 & vOrigin, _float fScale)
{
	shared_ptr<CCustomFont>		pFont = Find_Font(strFontTag);
	if (nullptr == pFont)
		return E_FAIL;

	return pFont->Render(strText, vPosition, vColor, fRotation, vOrigin, fScale);	
}

shared_ptr<CCustomFont> CFont_Manager::Find_Font(const wstring & strFontTag)
{
	auto	iter = m_Fonts.find(strFontTag);

	if (iter == m_Fonts.end())
		return nullptr;

	return iter->second;	
}

shared_ptr<CFont_Manager> CFont_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CFont_Manager>		pInstance = make_shared<CFont_Manager>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CFont_Manager");
		pInstance.reset();
	}

	return pInstance;
}

void CFont_Manager::Free()
{
	for (auto& Pair : m_Fonts)
		Pair.second.reset();
	m_Fonts.clear();

	/*Safe_Release(m_pDevice);
	Safe_Release(m_pContext);*/
}
