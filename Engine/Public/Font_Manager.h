#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CFont_Manager final
{
public:
	CFont_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CFont_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_Font(const wstring& strFontTag, const wstring& strFontFilePath);
	HRESULT Render(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor = Colors::White, _float fRotation = 0.f, const _float2& vOrigin = _float2(0.f, 0.f), _float fScale = 1.f);

private:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };

private:
	map<const wstring, shared_ptr<class CCustomFont>>			m_Fonts;

private:
	shared_ptr<class CCustomFont> Find_Font(const wstring& strFontTag);

public:
	static shared_ptr<CFont_Manager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void Free();
};

END