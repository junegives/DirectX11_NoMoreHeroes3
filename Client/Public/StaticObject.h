#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CBinaryModel;
class CNavigation;
END

BEGIN(Client)

class CStaticObject abstract : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		wstring		strModelComTag;

		_float4		vPos;
		_float4		vRotation;
		_float4		vScale;

	} STATICOBJECT_DESC;

public:
	CStaticObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag, const LEVEL& eLevel);
	CStaticObject(const CStaticObject& rhs);
	virtual ~CStaticObject() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Set_ModelComTag(const wstring& strModelComTag) { m_strModelComTag = strModelComTag.c_str(); }
	void	Set_CanPick(_bool bCanPick) { m_bCanPick = bCanPick; }

protected:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CBinaryModel> m_pModelCom = { nullptr };

protected:
	LEVEL		m_eLevel = LEVEL_END;
	wstring		m_strModelComTag = L"";
	_bool		m_bPicked = false;
	_bool		m_bCanPick = false;

protected:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	virtual void Free() override;
};

END