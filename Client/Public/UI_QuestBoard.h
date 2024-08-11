#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_QuestBoard : public CUI
{
public:
	enum QUEST_ID { QUEST_BATTLE, QUEST_GOLD, QUEST_BOSS, QUEST_END };

public:
	CUI_QuestBoard(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CUI_QuestBoard(const CUI_QuestBoard& rhs);
	virtual ~CUI_QuestBoard() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void		SetQuest(QUEST_ID eQuestID) { m_strCurrentQuest = m_QuestMap[eQuestID]; }
	wstring		GetQuest(QUEST_ID eQuestID) { return m_QuestMap[eQuestID]; }

private:
	shared_ptr<CTexture>				m_pTextureCom = { nullptr };

private:
	_bool								m_isFold = false;
	unordered_map<QUEST_ID, wstring>	m_QuestMap;
	wstring								m_strCurrentQuest = L"";

protected:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CUI_QuestBoard> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg = nullptr);
};

END