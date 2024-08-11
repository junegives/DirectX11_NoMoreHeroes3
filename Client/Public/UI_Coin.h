#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_Coin : public CUI
{
public:
	CUI_Coin(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CUI_Coin(const CUI_Coin& rhs);
	virtual ~CUI_Coin() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_uint		GetCoin() { return m_iCoin; }
	void		SetCoin(_uint iCoin) { m_iCoin = iCoin; }
	void		GainCoin(_uint iCoin) { m_iCoin += iCoin; }
	void		LoseCoin(_uint iCoin) { m_iCoin -= iCoin; }

private:
	shared_ptr<CTexture>				m_pTextureCom = { nullptr };

	shared_ptr<CTexture>				m_pTextureCom_Num[10] = { nullptr };

	_uint								m_iCoin = 000;

protected:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CUI_Coin> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg = nullptr);
};

END