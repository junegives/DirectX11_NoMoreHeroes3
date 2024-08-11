#include "pch.h"
#include "Level_Logo.h"

#include "GameInstance.h"

#include "Level_Loading.h"
#include "BackGround.h"

CLevel_Logo::CLevel_Logo(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	/*if (FAILED(Ready_Layer_BackGround(LAYER_BACKGROUND)))
		return E_FAIL;*/

    return S_OK;
}

void CLevel_Logo::Tick(_float fTimeDelta)
{
	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_VILLAGE))))
			return;

		return;
	}
}

HRESULT CLevel_Logo::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
#endif

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const LAYER_ID& eLayerID)
{
	/*CBackGround::BACKGROUND_DESC	BackGroundDesc = {};A

	BackGroundDesc.iBackData = 10;
	BackGroundDesc.iGameObjectData = 10;
	BackGroundDesc.fSpeedPerSec = 10.f;
	BackGroundDesc.fRotationPerSec = XMConvertToRadians(60.0f);

	shared_ptr<CGameObject> pGameObject = make_shared<CBackGround>(m_pDevice, m_pContext);
	pGameObject->Initialize(&BackGroundDesc);
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_LOGO, eLayerID, TEXT("GameObject_BackGround"), pGameObject)))
		return E_FAIL;*/

	return S_OK;
}

shared_ptr<CLevel_Logo> CLevel_Logo::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CLevel_Logo> pInstance = make_shared<CLevel_Logo>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		pInstance = nullptr;
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}
