#include "pch.h"
#include "UI_Manager.h"
#include "UI.h"
#include "UI_Coin.h"
#include "UI_HP_Player.h"

IMPLEMENT_SINGLETON_SMART(CUI_Manager)

CUI_Manager::CUI_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
}

void CUI_Manager::AddUI(const wstring& strUITag, shared_ptr<class CUI> pUI)
{
	m_UIs.emplace(strUITag, pUI);
}

void CUI_Manager::RemoveUI(const wstring& strUITag)
{
	auto iter = m_UIs.find(strUITag);

	if (iter != m_UIs.end())
		m_UIs.erase(iter);
}

shared_ptr<class CUI> CUI_Manager::GetUI(const wstring& strUITag)
{
	return m_UIs.find(strUITag)->second;
}

void CUI_Manager::AllVisibleOn()
{
	for (auto& iter : m_UIs)
		(iter.second)->SetVisible(true);
}

void CUI_Manager::AllVisibleOff()
{
	for (auto& iter : m_UIs)
		(iter.second)->SetVisible(false);
}

void CUI_Manager::AllActiveOn()
{
	for (auto& iter : m_UIs)
		(iter.second)->SetActive(true);
}

void CUI_Manager::AllActiveOff()
{
	for (auto& iter : m_UIs)
		(iter.second)->SetActive(false);
}

void CUI_Manager::SetCoin(_uint iCoin)
{
	m_pCoinUI->SetCoin(iCoin);
}

void CUI_Manager::GainCoin(_uint iCoin)
{
	m_pCoinUI->GainCoin(iCoin);
}

void CUI_Manager::LoseCoin(_uint iCoin)
{
	m_pCoinUI->LoseCoin(iCoin);
}

_uint CUI_Manager::GetCoin()
{
	return m_pCoinUI->GetCoin();
}

void CUI_Manager::SetMaxHP(_uint iHP)
{
	m_pPlayerHP->SetMaxHP(iHP);
}

void CUI_Manager::FullHP()
{
	m_pPlayerHP->FullHP();
}

void CUI_Manager::LoseHP(_uint iHP)
{
	m_pPlayerHP->LoseHP(iHP);
}
