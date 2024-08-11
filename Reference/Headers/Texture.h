#pragma once

#include "Component.h"

/* 텍스쳐들을 로드하고 저장한다. */

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
public:
	CTexture(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg = nullptr) override;

public:
	HRESULT Bind_ShaderResource(shared_ptr<class CShader> pShader, const _char* pConstantName, _uint iIndex);
	HRESULT Bind_ShaderResources(shared_ptr<class CShader> pShader, const _char* pConstantName);

public:
	vector<ComPtr<ID3D11ShaderResourceView>>			GetSRVs() { return m_SRVs; }

private:
	_uint												m_iNumTextures = { 0 };
	vector<ComPtr<ID3D11ShaderResourceView>>			m_SRVs;


public:
	static shared_ptr<CTexture> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strTextureFilePath, _uint iNumTextures);
	virtual shared_ptr<CComponent> Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END