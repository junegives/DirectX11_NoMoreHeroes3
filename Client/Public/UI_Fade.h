#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_Fade : public CUI
{ 
public:
	CUI_Fade(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CUI_Fade(const CUI_Fade& rhs);
	virtual ~CUI_Fade() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	FadeOut(_float fFadeTime);
	void	FadeIn(_float fFadeTime);

private:
	shared_ptr<CTexture>				m_pTextureCom = { nullptr };

private:
	_float								m_fMaskAlpha = 0.f;
	_float								m_fMaxFadeTime = 1.f;
	_float								m_fFadeTime = 1.f;

	_bool								m_isFadeOut = false;
	_bool								m_isFadeIn = false;

protected:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CUI_Fade> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg = nullptr);
};

END