#pragma once

#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CNavigation;
END

BEGIN(Tool)

class CStaticModel : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		wstring		strModelComTag;

		_float4		vPos;
		_float4		vRotation;
		_float4		vScale;

	} STATICMODEL_DESC;

public:
	CStaticModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag);
	CStaticModel(const CStaticModel& rhs);
	virtual ~CStaticModel() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Set_ModelComTag(const wstring& strModelComTag) { m_strModelComTag = strModelComTag.c_str(); }
	void	Set_CanPick(_bool bCanPick) { m_bCanPick = bCanPick; }

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CBinaryModel> m_pModelCom = { nullptr };
	shared_ptr<CNavigation> m_pNavigationCom = { nullptr };

private:
	wstring		m_strModelComTag = L"";
	_bool		m_bPicked = false;
	_bool		m_bCanPick = false;

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CStaticModel> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag, _bool bCanPick = false);
	virtual void Free() override;
};

END