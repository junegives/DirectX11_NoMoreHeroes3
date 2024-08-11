#pragma once

#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CBinaryModel;
END

BEGIN(Tool)

class CTestStaticModel : public CGameObject
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
	CTestStaticModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag);
	CTestStaticModel(const CTestStaticModel& rhs);
	virtual ~CTestStaticModel() = default;

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
	shared_ptr<CShader>			m_pShaderCom = { nullptr };
	shared_ptr<CBinaryModel>	m_pModelCom = { nullptr };

private:
	wstring		m_strModelComTag = L"";
	_bool		m_bPicked = false;
	_bool		m_bCanPick = false;

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CTestStaticModel> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag, _bool bCanPick = false);
	virtual void Free() override;
};

END