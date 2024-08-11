#include "pch.h"
#include "Loader.h"
#include "GameInstance.h"

#include "BackGround.h"
#include "Terrain.h"
#include "Camera_Free.h"
#include "Camera_Attach.h"
#include "Player.h"
#include "Monster.h"
#include "StaticObject.h"

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
	case LEVEL_GAMEPLAY:
		hr = Loading_For_GamePlayLevel();
		break;
	case LEVEL_VILLAGE:
		hr = Loading_For_VillageLevel();
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
	//m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Textures/Default%d.jpg"), 2))))
	//	return E_FAIL;

	//m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");
	//for (size_t i = 0; i < 99999999; i++)
	//	int a = 10;

	//m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");
	//for (size_t i = 0; i < 99999999; i++)
	//	int a = 10;

	//m_strLoadingText = TEXT("객체원형를(을) 로딩 중 입니다.");
	///* For.Prototype_GameObject_BackGround */
	//{
	//	CBackGround::BACKGROUND_DESC	BackGroundDesc = {};

	//	BackGroundDesc.iBackData = 10;
	//	BackGroundDesc.iGameObjectData = 10;
	//	BackGroundDesc.fSpeedPerSec = 10.f;
	//	BackGroundDesc.fRotationPerSec = XMConvertToRadians(60.0f);

	//	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_LOGO, LAYER_BACKGROUND, TEXT("GameObject_BackGround"),
	//		CBackGround::Create(m_pDevice, m_pContext, &BackGroundDesc))))
	//		return E_FAIL;
	//}

	//m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	//m_isFinished = true;



	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlayLevel()
{
	_matrix PivotMatrix = XMMatrixIdentity();

	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");

	/*shared_ptr<CModelParser> pModelParser = make_shared<CModelParser>();
	pModelParser->Read_Dir(TEXT("../../Resource/Resources/Models/NMH/"), false);*/

	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");
	// For. Prototye_GameObject_Texture_Terrain
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	// Prototye_GameObject_Texture_Mask용
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Texture_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Textures/Terrain/Mask.bmp"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 5))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
		return E_FAIL;

	/* UI Texture */
	{
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QuestBoard"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/TX_UI_HUD_QuestBoard.png"), 1))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_JeaneTab"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/TX_UI_HUD_JeaneTab_Top.png"), 1))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_TeleportFont"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/TX_UI_HUD_AccessFont.png"), 1))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_TeleportButton"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/TX_UI_HUD_SPSheet_AccessButton.png"), 1))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Coin"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/TX_UI_HUD_Coin.png"), 1))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Coin_Num0"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/TX_UI_HUD_Coin_Num0.png"), 1))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Coin_Num1"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/TX_UI_HUD_Coin_Num1.png"), 1))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Coin_Num2"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/TX_UI_HUD_Coin_Num2.png"), 1))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Coin_Num3"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/TX_UI_HUD_Coin_Num3.png"), 1))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Coin_Num4"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/TX_UI_HUD_Coin_Num4.png"), 1))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Coin_Num5"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/TX_UI_HUD_Coin_Num5.png"), 1))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Coin_Num6"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/TX_UI_HUD_Coin_Num6.png"), 1))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Coin_Num7"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/TX_UI_HUD_Coin_Num7.png"), 1))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Coin_Num8"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/TX_UI_HUD_Coin_Num8.png"), 1))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Coin_Num9"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/TX_UI_HUD_Coin_Num9.png"), 1))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Fade"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/TX_UI_HUD_fade_06.png"), 1))))
			return E_FAIL;


		/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QuestJeane"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/TX_UI_HUD_JeaneTab_Jeane.png"), 1))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QuestJeane"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/TX_UI_HUD_JeaneTab_Jeane.png"), 1))))
			return E_FAIL;*/

		// Effect
		{
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BATTLE2, TEXT("Prototype_Component_Texture_Mask_Vertical"),
				CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/T_FxNoiseA02msk_KK_Verticla.png"), 1))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BATTLE2, TEXT("Prototype_Component_Texture_Mask_Mosaic"),
				CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/T_FxNoiseA17_KK_MM2.png"), 1))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BATTLE2, TEXT("Prototype_Component_Texture_Mask_CubeOutline"),
				CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/Mask_Cube.png"), 1))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mask_Rush"),
				CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/Trail_Rush2.png"), 1))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Trail"),
				CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/Trail.png"), 1))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Attack_Line"),
				CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/TX_FX_Line1.png"), 1))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Attack_Particle_Blue"),
				CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/SpriteParticle.png"), 1))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Attack_Line_Mask"),
				CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/gradient_D.png"), 1))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Pillar_Wave"),
				CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/NoiseCaustic03.png"), 1))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effect_Sword"),
				CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Effect_Sword.model", PivotMatrix))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effect_Sword_In"),
				CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Effect_Sword_In.model", PivotMatrix))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effect_Sword_Out"),
				CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Effect_Sword_Out.model", PivotMatrix))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effect_Trigger_Pillar"),
				CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Effect_TriggerPillar.model", PivotMatrix))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effect_Sphere"),
				CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Effect_Sphere.model", PivotMatrix))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Distortion_01"),
				CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/Distortion_01.png"), 1))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Distortion_02"),
				CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/Distortion_02.png"), 1))))
				return E_FAIL;
		}
	}
	
	// 모델이 애초에 앞쪽(z+)을 향해있는게 편함 (Go Straight시 백스텝 방지)

	// Maps
	{
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Map_Village"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Map_VillageFinal.model", PivotMatrix))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BATTLE1, TEXT("Prototype_Component_Model_Map_Battle1"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Map_Battle.model", PivotMatrix))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MINIGAME, TEXT("Prototype_Component_Model_Map_MiniGame"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Map_MiniGame.model", PivotMatrix))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BATTLE2, TEXT("Prototype_Component_Model_Map_Battle2"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Map_Parking.model", PivotMatrix))))
			return E_FAIL;

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Map_VCGBoss"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Map_VCGBoss.model", PivotMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Map_FuBoss"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Map_FuBoss.model", PivotMatrix))))
		return E_FAIL;*/

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Map_KHBoss"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Map_KHBoss.model", PivotMatrix))))
		return E_FAIL;*/



	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Map"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Map_VillageCut.model", PivotMatrix))))
		return E_FAIL;*/

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Map_Parking"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Map_Parking.model", PivotMatrix))))
		return E_FAIL;*/

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Map_Village"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Map_Village.model", PivotMatrix))))
		return E_FAIL;*/
	}

	// SM_Objects
	{
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Sign_Plant"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/SM_Plant.model", PivotMatrix))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Sign_SpaceShip"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/SM_SpaceShip.model", PivotMatrix))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Sign_Coin"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/SM_Coin.model", PivotMatrix))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MINIGAME, TEXT("Prototype_Component_Model_Coin_Default"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/SM_Coin_Default.model", PivotMatrix))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MINIGAME, TEXT("Prototype_Component_Model_Coin_Copper"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/SM_Coin_Copper.model", PivotMatrix))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MINIGAME, TEXT("Prototype_Component_Model_Coin_Silver"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/SM_Coin_Silver.model", PivotMatrix))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MINIGAME, TEXT("Prototype_Component_Model_Coin_Gold"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/SM_Coin_Gold.model", PivotMatrix))))
			return E_FAIL;
	}

	// Characters
	{
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_Travis"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_ANIM, "../../Resource/Resources/Models/NMH/Player_Travis.model", PivotMatrix))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_Travis_Armor"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_ANIM, "../../Resource/Resources/Models/NMH/Player_Travis_Armor.model", PivotMatrix))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BATTLE1, TEXT("Prototype_Component_Model_Monster_TriplePut"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_ANIM, "../../Resource/Resources/Models/NMH/Monster_TriplePut.model", PivotMatrix))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BATTLE2, TEXT("Prototype_Component_Model_Monster_Leopardon"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_ANIM, "../../Resource/Resources/Models/NMH/Monster_Leopardon.model", PivotMatrix))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Monster_Bone"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_ANIM, "../../Resource/Resources/Models/NMH/Monster_Bone.model", PivotMatrix))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_NPC_Male"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_ANIM, "../../Resource/Resources/Models/NMH/NPC_Male.model", PivotMatrix))))
			return E_FAIL;
	}

	// NonAnim Model
	{
	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_SM_SushiShop"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/SM_SushiShop.model", PivotMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_SM_Bulldozer"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/SM_Bulldozer.model", PivotMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_SM_Shrine"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/SM_Shrine.model", PivotMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_SM_SignA"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/SM_SignA.model", PivotMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_SM_StreetLamp"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/SM_StreetLamp.model", PivotMatrix))))
		return E_FAIL;*/

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Boss_Midori"),
		CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_ANIM, "../../Resource/Resources/Models/NMH/Boss/Boss_Midori.model", PivotMatrix))))
		return E_FAIL;*/

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Trigger"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Obj_Trigger.model", PivotMatrix))))
			return E_FAIL;
	}

	// Anim Model
	{
		/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Model_Vehicle_Bike"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_ANIM, "../../Resource/Resources/Models/NMH/Vehicle_Bike.model", PivotMatrix))))
			return E_FAIL;*/

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Vehicle_Cat"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_ANIM, "../../Resource/Resources/Models/NMH/Vehicle_Cat.model", PivotMatrix))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BATTLE2, TEXT("Prototype_Component_Model_Projectile_Leopardon_SquareWall"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_ANIM, "../../Resource/Resources/Models/NMH/Projectile_Leopardon_SquareWall.model", PivotMatrix))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BATTLE2, TEXT("Prototype_Component_Model_LandObject_Leopardon_Teleport"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/LandObject_Leopardon_Teleport.model", PivotMatrix))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BATTLE2, TEXT("Prototype_Component_Model_Projectile_Leopardon_Ring"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Projectile_Leopardon_Ring.model", PivotMatrix))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Weapon_BeamKatana"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_ANIM, "../../Resource/Resources/Models/NMH/Weapon_BeamKatana.model", PivotMatrix))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BATTLE2, TEXT("Prototype_Component_Model_Weapon_CosmicRifle"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_NONANIM, "../../Resource/Resources/Models/NMH/Weapon_CosmicRifle.model", PivotMatrix))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Weapon_Bone"),
			CBinaryModel::Create(m_pDevice, m_pContext, CBinaryModel::TYPE_ANIM, "../../Resource/Resources/Models/NMH/Weapon_Bone.model", PivotMatrix))))
			return E_FAIL;
	}

	m_strLoadingText = TEXT("네비게이션를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_Navigation */

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Navigations/Map_VillageFinal.Navi")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BATTLE1, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Navigations/Map_Battle.Navi")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BATTLE2, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Navigations/Map_Parking.Navi")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MINIGAME, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Navigations/Map_MiniGame.Navi")))))
		return E_FAIL;

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VILLAGE, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Navigations/Map_Village.Navi")))))
		return E_FAIL;*/

	m_strLoadingText = TEXT("충돌체를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	m_strLoadingText = TEXT("FSM를(을) 로딩 중입니다.");
	/* For. Prototype_Component_StateMachine */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"),
		CStateMachine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strLoadingText = TEXT("객체원형를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_VillageLevel()
{
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
