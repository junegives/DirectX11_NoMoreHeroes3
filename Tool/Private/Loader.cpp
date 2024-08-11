#include "pch.h"
#include "Loader.h"
#include "GameInstance.h"

#include "Terrain.h"
#include "Camera_Free.h"
#include "Player.h"
#include "StaticModel.h"

#include "ModelParser.h"

CLoader::CLoader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
}

_uint APIENTRY wThreadMain(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CLoader* pLoader = static_cast<CLoader*>(pArg);

	pLoader->Loading_Selector();

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, wThreadMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_Selector()
{
	EnterCriticalSection(&m_CriticalSection);

	HRESULT		hr = {};

	switch (m_eNextLevel)
	{
	case LEVEL_LOGO:
		hr = Loading_For_LogoLevel();
		break;
	case LEVEL_MAPTOOL:
		hr = Loading_For_MapTool();
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	LeaveCriticalSection(&m_CriticalSection);

	return S_OK;
}

#ifdef _DEBUG
void CLoader::Output_LoadingText()
{
	SetWindowText(g_hWnd, m_strLoadingText.c_str());
}
#endif

HRESULT CLoader::Loading_For_LogoLevel()
{
	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;

	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");
	for (size_t i = 0; i < 99999999; i++)
		int a = 10;

	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");
	for (size_t i = 0; i < 99999999; i++)
		int a = 10;

	m_strLoadingText = TEXT("객체원형를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;



	return S_OK;
}

HRESULT CLoader::Loading_For_MapTool()
{
	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");
	// For. Prototye_GameObject_Texture_Terrain
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	// Prototye_GameObject_Texture_Mask용
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Textures/Terrain/Mask.bmp"), 1))))
		return E_FAIL;

	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");

	/*shared_ptr<CModelParser> pModelParser = make_shared<CModelParser>();
	pModelParser->Read_Dir(TEXT("../../Resource/Resources/Models/NMH/"), true);*/

	// 모델이 애초에 앞쪽(z+)을 향해있는게 편함 (Go Straight시 백스텝 방지)
	_matrix			PivotMatrix = XMMatrixIdentity();

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Map_VCGBoss"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Map_VCGBoss.model", PivotMatrix))))
		return E_FAIL;*/

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Map_FuBoss"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Map_FuBoss.model", PivotMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Map_KHBoss"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Map_KHBoss.model", PivotMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Map_Underground"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Map_Underground.model", PivotMatrix))))
		return E_FAIL;*/

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Map_Village"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Map_Village.model", PivotMatrix))))
		return E_FAIL;*/


	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Map_VillageSmall"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Map_VillageSmall.model", PivotMatrix))))
		return E_FAIL;*/

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Map_VillageFinal"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Map_VillageFinal.model", PivotMatrix))))
		return E_FAIL;

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Map_Parking"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Map_Parking.model", PivotMatrix))))
		return E_FAIL;*/

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Map_Battle"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Map_Battle.model", PivotMatrix))))
		return E_FAIL;*/

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Map_Hangar"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Map_Hangar.model", PivotMatrix))))
		return E_FAIL;*/

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Map_MiniGame"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Map_MiniGame.model", PivotMatrix))))
		return E_FAIL;*/

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Map_KHBoss"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Map_KHBoss.model", PivotMatrix))))
		return E_FAIL;*/

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Navigations/Map_KHBoss.Navi")))))
		return E_FAIL;*/

	/* For.Prototype_Component_Model_ForkLift */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_SM_SushiShop"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/SM_SushiShop.model", PivotMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_SM_Bulldozer"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/SM_Bulldozer.model", PivotMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_SM_Shrine"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/SM_Shrine.model", PivotMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_SM_SignA"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/SM_SignA.model", PivotMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_SM_StreetLamp"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/SM_StreetLamp.model", PivotMatrix))))
		return E_FAIL;
	/*
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Player"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_ANIM, "../../Resource/Resources/Models/NMH/Player_ArmorTravis.model", PivotMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_Boss_Midori"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_ANIM, "../../Resource/Resources/Models/NMH/Boss/Boss_Midori.model", PivotMatrix))))
		return E_FAIL;

	*/

	PivotMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_Travis"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_ANIM, "../../Resource/Resources/Models/NMH/Player_Travis.model", PivotMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Navigations/Map_VillageFinal.Navi")))))
		return E_FAIL;

	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");

	/* For.Light */
	{
		LIGHT_DESC		LightDesc = {};

		LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
		LightDesc.vPosition = _float4(30.f, 10.f, 30.f, 1.f);
		LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
		LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
		LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
		LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
		LightDesc.fRange = 30.f;

		if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
			return E_FAIL;
	}

	m_strLoadingText = TEXT("객체원형를(을) 로딩 중 입니다.");

	// For. GameObject_Camera_Free
	{
		CCamera_Free::CAMERA_FREE_DESC		CameraDesc = {};

		CameraDesc.fMouseSensor = 0.1f;
		CameraDesc.vEye = _float4(0.0f, 30.0f, -25.0f, 1.0f);
		CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
		CameraDesc.fFovy = XMConvertToRadians(60.0f);
		CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
		CameraDesc.fNear = 0.2f;
		CameraDesc.fFar = 1000.f;
		CameraDesc.fSpeedPerSec = 70.f;
		CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MAPTOOL, LAYER_CAMERA, TEXT("GameObject_Camera_Free"),
			CCamera_Free::Create(m_pDevice, m_pContext, &CameraDesc))))
			return E_FAIL;
	}

	{
		//if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MAPTOOL, LAYER_PLAYER, TEXT("GameObject_Player"),
		//	CPlayer::Create(m_pDevice, m_pContext))))
		//	return E_FAIL;
	}

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

shared_ptr<CLoader> CLoader::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eNextLevel)
{
	shared_ptr<CLoader> pInstance = make_shared<CLoader>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed to Created : CLoader");
		pInstance = nullptr;
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CriticalSection);
	DeleteObject(m_hThread);
	CloseHandle(m_hThread);
}
