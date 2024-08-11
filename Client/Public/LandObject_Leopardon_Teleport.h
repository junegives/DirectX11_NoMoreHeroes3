#pragma once

#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Engine)
class CShader;
class CBinaryModel;
class CCollider;
END

BEGIN(Client)

class CLandObject_Leopardon_Teleport : public CLandObject
{
public:
	CLandObject_Leopardon_Teleport(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CLandObject_Leopardon_Teleport(const CLandObject_Leopardon_Teleport& rhs);
	virtual ~CLandObject_Leopardon_Teleport() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

private:
	shared_ptr<CShader>			m_pShaderCom = { nullptr };
	shared_ptr<CBinaryModel>	m_pModelCom = { nullptr };
	shared_ptr<CCollider>		m_pColliderCom = { nullptr };

public:
	_bool						IsVisible() { return m_isVisible; }
	void						SetVisible(_bool isVisible) { m_isVisible = isVisible; }

private:
	_bool						m_isVisible = true;

public:
	static shared_ptr<CLandObject_Leopardon_Teleport> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END