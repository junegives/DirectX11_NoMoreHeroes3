#include "Level_Manager.h"
#include "Level.h"

#include "GameInstance.h"

CLevel_Manager::CLevel_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT CLevel_Manager::Initialize()
{
    return S_OK;
}

void CLevel_Manager::Tick(_float fTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->Tick(fTimeDelta);
}

void CLevel_Manager::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->Late_Tick(fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	if (nullptr == m_pCurrentLevel)
		return E_FAIL;

	return m_pCurrentLevel->Render();
}

HRESULT CLevel_Manager::Open_Level(_uint iLevelIndex, shared_ptr<class CLevel> pNewLevel)
{
	if (nullptr != m_pCurrentLevel)
		m_pGameInstance->Clear(m_iLevelIndex);

    m_pCurrentLevel = pNewLevel;

    m_iLevelIndex = iLevelIndex;

    return S_OK;
}

shared_ptr<CLevel_Manager> CLevel_Manager::Create()
{
	shared_ptr<CLevel_Manager> pInstance = make_shared<CLevel_Manager>();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Manager");
		pInstance = nullptr;
	}

	return pInstance;
}

void CLevel_Manager::Free()
{
}
