#include "pch.h"
#include "Level_Loading.h"
#include "Loader.h"

#include "GameInstance.h"
#include "BackGround.h"

#include "Level_MapTool.h"

CLevel_Loading::CLevel_Loading(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

	m_pGameInstance->Set_GameState(CGame_Manager::STATE_LOADING);
	m_pGameInstance->Set_CurLevel((_uint)LEVEL_LOADING);

    return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{
	if (true == m_pLoader->isFinished())
	{
		m_pGameInstance->Set_GameState(CGame_Manager::STATE_PROCESS);

		/* 진짜 진입하고 싶었던 레벨로 들어간다. */
		shared_ptr<CLevel> pLevel = { nullptr };

		switch (m_eNextLevel)
		{
		case LEVEL_MAPTOOL:
			pLevel = CLevel_MapTool::Create(m_pDevice, m_pContext);
			break;
		}

		if (nullptr == pLevel)
			return;

		if (FAILED(m_pGameInstance->Open_Level(m_eNextLevel, pLevel)))
			return;
	}
}

HRESULT CLevel_Loading::Render()
{
	if (nullptr == m_pLoader)
		return E_FAIL;

	if (CGame_Manager::STATE_LOADING == m_pGameInstance->Get_GameState())
		m_pLoader->Output_LoadingText();

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround()
{
	return E_NOTIMPL;
}

HRESULT CLevel_Loading::Ready_Layer_UI()
{
	return E_NOTIMPL;
}

shared_ptr<CLevel_Loading> CLevel_Loading::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eNextLevel)
{
	shared_ptr<CLevel_Loading> pInstance = make_shared<CLevel_Loading>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		pInstance = nullptr;
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();
}
