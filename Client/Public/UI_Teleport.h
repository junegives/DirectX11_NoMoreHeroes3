#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_Teleport : public CUI
{
public:
	CUI_Teleport(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CUI_Teleport(const CUI_Teleport& rhs);
	virtual ~CUI_Teleport() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	shared_ptr<CTexture>				m_pTextureCom_Font = { nullptr };
	shared_ptr<CTexture>				m_pTextureCom_Button = { nullptr };

	shared_ptr<CTransform>				m_pTransformCom_Font = { nullptr };
	shared_ptr<CTransform>				m_pTransformCom_Button = { nullptr };

	_vector								m_vPosFontTop[4][2];
	_vector								m_vPosFontBottom[4][2];

private:
	_bool		m_isIn = false;
	_bool		m_isLoop = false;
	_bool		m_isOut = false;

	_float		m_fY = 0.f;

protected:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CUI_Teleport> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg = nullptr);
};

END