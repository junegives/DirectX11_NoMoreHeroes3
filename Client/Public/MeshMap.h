#pragma once

#include "Client_Defines.h"
#include "StaticObject.h"

BEGIN(Engine)
class CShader;
class CBinaryModel;
class CNavigation;
END

BEGIN(Client)

class CMeshMap : public CStaticObject
{
public:
	CMeshMap(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag, const wstring& strNaviTag, const LEVEL& eLevel);
	CMeshMap(const CMeshMap& rhs);
	virtual ~CMeshMap() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	shared_ptr<CNavigation> m_pNavigationCom = { nullptr };

private:
	wstring		m_strNaviTag = L"";
	_bool		m_bPicked = false;
	_bool		m_bCanPick = false;

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CMeshMap> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag, const wstring& strNaviTag, const LEVEL& eLevel, _bool bCanPick = false);
	virtual void Free() override;
};

END