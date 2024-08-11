#pragma once

#include "Client_Defines.h"

BEGIN(Engine)


END

BEGIN(Client)

class CUI_Manager final
{
	DECLARE_SINGLETON_SMART(CUI_Manager)
public:
	CUI_Manager();
	~CUI_Manager() = default;

public:
	void					AddUI(const wstring& strUITag, shared_ptr<class CUI> pUI);
	void					RemoveUI(const wstring& strUITag);
	shared_ptr<class CUI>	GetUI(const wstring& strUITag);

	void					AllVisibleOn();
	void					AllVisibleOff();

	void					AllActiveOn();
	void					AllActiveOff();

public:
	void						SetCoinUI(shared_ptr<class CUI_Coin> pCoinUI) { m_pCoinUI = pCoinUI; }
	shared_ptr<class CUI_Coin>	GetCoinUI() { return m_pCoinUI; }

	void					SetCoin(_uint iCoin);
	void					GainCoin(_uint iCoin);
	void					LoseCoin(_uint iCoin);
	_uint					GetCoin();

	void							SetHPUI(shared_ptr<class CUI_HP_Player> pHPUI) { m_pPlayerHP = pHPUI; }
	shared_ptr<class CUI_HP_Player>	GetHPUI() { return m_pPlayerHP; }

	void					SetMaxHP(_uint iHP);
	void					FullHP();
	void					LoseHP(_uint iHP);

private:
	shared_ptr<class CGameInstance>						m_pGameInstance = { nullptr };
	map<const wstring, shared_ptr<class CUI>>			m_UIs;

	shared_ptr<class CUI_Coin>							m_pCoinUI;
	shared_ptr<class CUI_HP_Player>						m_pPlayerHP;
};

END