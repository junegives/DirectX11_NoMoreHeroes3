#include "pch.h"
#include "Level_GamePlay.h"

#include "Camera_Free.h"
#include "Camera_Attach.h"
#include "Camera_Event.h"

#include "Player.h"
#include "Bike.h"
#include "Monster_TriplePut.h"
#include "Monster_Leopardon.h"
#include "Monster_Bone.h"
#include "Sky.h"
#include "StaticObject.h"
#include "Level_Loading.h"
#include "MeshMap.h"
#include "Trigger.h"
#include "Sign.h"
#include "Coin.h"

#include "Camera_Manager.h"

#include "UI_Manager.h"
#include "UI_QuestBoard.h"
#include "UI_Teleport.h"
#include "UI_Coin.h"
#include "UI_HP_Player.h"
#include "UI_Fade.h"

#include "Particle.h"
#include "Particle_Rect.h"
#include "Particle_Point.h"

#include "Effect_Texture.h"
#include "Effect_Pillar.h"
#include "Effect_Distortion.h"

#include "Effect_Manager.h"

#include "Sound_Manager.h"

CLevel_GamePlay::CLevel_GamePlay(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Player(LAYER_PLAYER)))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(LAYER_BACKGROUND)))
		return E_FAIL;

	if (FAILED(Ready_LandObjects()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Trigger(LAYER_TELEPORT)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(LAYER_EFFECT)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(LAYER_CAMERA)))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(LAYER_UI)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Environment(LAYER_ENVIRONMENT)))
		return E_FAIL;

	m_pGameInstance->Set_CurLevel((_uint)LEVEL_VILLAGE);
	m_pPlayer->SetOnBattle(false);

	CSound_Manager::GetInstance()->StopAll();
	CSound_Manager::GetInstance()->PlayBGM(L"BGM_Village_Casual.mp3", 0.6f);
	m_pPlayer->SetCurBGM(L"BGM_Village_Casual.mp3");

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Is_KeyDown(DIK_ESCAPE))
	{
		if (m_pPlayer->IsOnBattle())
		{
			dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player())->SetWeaponAttach(false);
			m_pGameInstance->Set_CurLevel(LEVEL_VILLAGE);
			m_pPlayer->SetOnBattle(false);

			if (m_pPlayer->GetClearBattle1())
			{
				m_pPlayer->SetClearBattle2(true);
				m_pPlayer->Change_NaviCom(LEVEL_VILLAGE);
			}

			else
			{
				m_pPlayer->SetClearBattle1(true);
				m_pPlayer->Change_NaviCom(LEVEL_VILLAGE);

				dynamic_pointer_cast<CUI_QuestBoard>(m_pGameInstance->Find_GameObject(LEVEL_VILLAGE, LAYER_UI, TEXT("GameObject_UI_QuestBoard")))->SetQuest(CUI_QuestBoard::QUEST_GOLD);

				if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_VILLAGE, LAYER_TELEPORT, TEXT("GameObject_Trigger_MiniGame"),
					CTrigger::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Trigger"), CTrigger::TRIGGER_MINIGAME, true))))
					return;

				if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_VILLAGE, LAYER_ENVIRONMENT, TEXT("GameObject_Sign_MiniGame"),
					CSign::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Sign_Coin"), CSign::SIGN_MINIGAME, true))))
					return;
			}
		}

		/*if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_VILLAGE))))
			return;*/

		return;
	}

	if (m_pGameInstance->Is_KeyDown(DIK_0))
	{
		m_pGameInstance->SetMainCam(CCamera_Manager::CAM_ATTACH);

		/*CLandObject::LANDOBJ_DESC			LandObjDesc{};

		LandObjDesc.pTerrainTransform = dynamic_pointer_cast<CTransform>(m_pGameInstance->Find_Component(LEVEL_VILLAGE, LAYER_BACKGROUND, g_strTransformTag));
		LandObjDesc.pTerrainVIBuffer = dynamic_pointer_cast<CVIBuffer_HeightTerrain>(m_pGameInstance->Find_Component(LEVEL_VILLAGE, LAYER_BACKGROUND, TEXT("Com_VIBuffer")));

		if (m_isMonster1)
		{
			m_pGameInstance->Remove_Object(LEVEL_BATTLE1, LAYER_MONSTER, TEXT("GameObject_Monster_TriplePut"));
			m_isMonster1 = false;
		}

		if (m_isMonster2)
		{
			m_pGameInstance->Remove_Object(LEVEL_BATTLE2, LAYER_MONSTER, TEXT("GameObject_Monster_Leopardon"));
			m_isMonster2 = false;
		}

		if (m_isMonster3)
		{
			m_pGameInstance->Remove_Object(LEVEL_STATIC, LAYER_MONSTER, TEXT("GameObject_Monster_Bone"));
			m_isMonster3 = false;
		}

		if (m_isBike)
		{
			m_pGameInstance->Remove_Object(LEVEL_STATIC, LAYER_VEHICLE, TEXT("GameObject_Vehicle_Bike"));
			m_isBike = false;
		}

		m_pGameInstance->Add_GameObject(LEVEL_BATTLE1, LAYER_MONSTER, TEXT("GameObject_Monster_TriplePut"),
			CMonster_TriplePut::Create(m_pDevice, m_pContext, &LandObjDesc));

		m_isMonster1 = true;

		m_pPlayer->SetOnBattle(true);*/
	}

	else if (m_pGameInstance->Is_KeyDown(DIK_9))
	{
		m_pGameInstance->SetMainCam(CCamera_Manager::CAM_FREE);

		/*CLandObject::LANDOBJ_DESC			LandObjDesc{};

		LandObjDesc.pTerrainTransform = dynamic_pointer_cast<CTransform>(m_pGameInstance->Find_Component(LEVEL_VILLAGE, LAYER_BACKGROUND, g_strTransformTag));
		LandObjDesc.pTerrainVIBuffer = dynamic_pointer_cast<CVIBuffer_HeightTerrain>(m_pGameInstance->Find_Component(LEVEL_VILLAGE, LAYER_BACKGROUND, TEXT("Com_VIBuffer")));

		if (m_isMonster1)
		{
			m_pGameInstance->Remove_Object(LEVEL_BATTLE1, LAYER_MONSTER, TEXT("GameObject_Monster_TriplePut"));
			m_isMonster1 = false;
		}

		if (m_isMonster2)
		{
			m_pGameInstance->Remove_Object(LEVEL_BATTLE2, LAYER_MONSTER, TEXT("GameObject_Monster_Leopardon"));
			m_isMonster2 = false;
		}

		if (m_isMonster3)
		{
			m_pGameInstance->Remove_Object(LEVEL_STATIC, LAYER_MONSTER, TEXT("GameObject_Monster_Bone"));
			m_isMonster3 = false;
		}

		if (m_isBike)
		{
			m_pGameInstance->Remove_Object(LEVEL_STATIC, LAYER_VEHICLE, TEXT("GameObject_Vehicle_Bike"));
			m_isBike = false;
		}

		m_pGameInstance->Add_GameObject(LEVEL_BATTLE2, LAYER_MONSTER, TEXT("GameObject_Monster_Leopardon"),
			CMonster_Leopardon::Create(m_pDevice, m_pContext, &LandObjDesc));

		m_isMonster2 = true;

		m_pPlayer->SetOnBattle(true);*/
	}

	else if (m_pGameInstance->Is_KeyDown(DIK_8))
	{
		m_pGameInstance->SetMainCam(CCamera_Manager::CAM_EVENT);

		/*CLandObject::LANDOBJ_DESC			LandObjDesc{};

		LandObjDesc.pTerrainTransform = dynamic_pointer_cast<CTransform>(m_pGameInstance->Find_Component(LEVEL_VILLAGE, LAYER_BACKGROUND, g_strTransformTag));
		LandObjDesc.pTerrainVIBuffer = dynamic_pointer_cast<CVIBuffer_HeightTerrain>(m_pGameInstance->Find_Component(LEVEL_VILLAGE, LAYER_BACKGROUND, TEXT("Com_VIBuffer")));

		if (m_isMonster1)
		{
			m_pGameInstance->Remove_Object(LEVEL_BATTLE1, LAYER_MONSTER, TEXT("GameObject_Monster_TriplePut"));
			m_isMonster1 = false;
		}

		if (m_isMonster2)
		{
			m_pGameInstance->Remove_Object(LEVEL_BATTLE2, LAYER_MONSTER, TEXT("GameObject_Monster_Leopardon"));
			m_isMonster2 = false;
		}

		if (m_isMonster3)
		{
			m_pGameInstance->Remove_Object(LEVEL_STATIC, LAYER_MONSTER, TEXT("GameObject_Monster_Bone"));
			m_isMonster3 = false;
		}

		if (m_isBike)
		{
			m_pGameInstance->Remove_Object(LEVEL_STATIC, LAYER_VEHICLE, TEXT("GameObject_Vehicle_Bike"));
			m_isBike = false;
		}

		m_pGameInstance->Add_GameObject(LEVEL_STATIC, LAYER_MONSTER, TEXT("GameObject_Monster_Bone"),
			CMonster_Bone::Create(m_pDevice, m_pContext, &LandObjDesc));

		m_isMonster3 = true;

		m_pPlayer->SetOnBattle(true);*/
	}

	/*else if (m_pGameInstance->Is_KeyDown(DIK_7))
	{
		CLandObject::LANDOBJ_DESC			LandObjDesc{};

		LandObjDesc.pTerrainTransform = dynamic_pointer_cast<CTransform>(m_pGameInstance->Find_Component(LEVEL_VILLAGE, LAYER_BACKGROUND, g_strTransformTag));
		LandObjDesc.pTerrainVIBuffer = dynamic_pointer_cast<CVIBuffer_HeightTerrain>(m_pGameInstance->Find_Component(LEVEL_VILLAGE, LAYER_BACKGROUND, TEXT("Com_VIBuffer")));

		if (m_isMonster1)
		{
			m_pGameInstance->Remove_Object(LEVEL_BATTLE1, LAYER_MONSTER, TEXT("GameObject_Monster_TriplePut"));
			m_isMonster1 = false;
		}

		if (m_isMonster2)
		{
			m_pGameInstance->Remove_Object(LEVEL_BATTLE2, LAYER_MONSTER, TEXT("GameObject_Monster_Leopardon"));
			m_isMonster2 = false;
		}

		if (m_isMonster3)
		{
			m_pGameInstance->Remove_Object(LEVEL_STATIC, LAYER_MONSTER, TEXT("GameObject_Monster_Bone"));
			m_isMonster3 = false;
		}

		if (m_isBike)
		{
			m_pGameInstance->Remove_Object(LEVEL_STATIC, LAYER_VEHICLE, TEXT("GameObject_Vehicle_Bike"));
			m_isBike = false;
		}

		m_isBike = true;

		m_pPlayer->SetOnBattle(false);
	}*/
}

void CLevel_GamePlay::Late_Tick(_float fTimeDelta)
{
	_uint	iCurLevel = m_pGameInstance->Get_CurLevel();

	if (LEVEL_VILLAGE == iCurLevel)
	{
		m_pGameInstance->Check_Collision(LAYER_PLAYER, LAYER_TELEPORT);
	}

	else if (LEVEL_BATTLE1 == iCurLevel)
	{
		m_pGameInstance->Check_Collision(LAYER_PLAYER_ATTACK, LAYER_MONSTER);
		m_pGameInstance->Check_Collision(LAYER_PLAYER, LAYER_MONSTER_ATTACK);
		m_pGameInstance->Check_Collision(LAYER_PLAYER, LAYER_MONSTER);
	}

	else if (LEVEL_BATTLE2 == iCurLevel)
	{
		m_pGameInstance->Check_Collision(LAYER_PLAYER_ATTACK, LAYER_MONSTER);
		m_pGameInstance->Check_Collision(LAYER_PLAYER, LAYER_MONSTER_ATTACK);
		m_pGameInstance->Check_Collision(LAYER_PLAYER, LAYER_PROJECTILE);
	}

	else if (LEVEL_MINIGAME == iCurLevel)
	{
		m_pGameInstance->Check_Collision(LAYER_PLAYER, LAYER_COIN);
	}
}

HRESULT CLevel_GamePlay::Render()
{
	//#ifdef _DEBUG
	//	SetWindowText(g_hWnd, TEXT("°ÔÀÓÇÃ·¹ÀÌ·¹º§ÀÔ´Ï´Ù."));
	//#endif

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC		LightDesc = {};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vPosition = _float4(30.f, 10.f, 30.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.fRange = 30.f;

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const LAYER_ID& eLayerID)
{
	CCamera_Attach::CAMERA_ATTACH_DESC		CameraDesc = {};

	CameraDesc.fMouseSensor = 0.1f;
	CameraDesc.vEye = _float4(0.0f, 30.0f, -25.0f, 1.0f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 1000.f;
	CameraDesc.fSpeedPerSec = 100.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	shared_ptr<CGameObject> pGameObject = make_shared<CCamera_Attach>(m_pDevice, m_pContext);
	pGameObject->Initialize(&CameraDesc);
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, LAYER_CAMERA, TEXT("GameObject_Camera_Attach"), pGameObject)))
		return E_FAIL;

	m_pGameInstance->AddCamera(CCamera_Manager::CAM_ATTACH, pGameObject);
	//CCamera_Manager::GetInstance()->AddCamera(CCamera_Manager::CAMERA_ATTACH, pGameObject);

	CameraDesc.fMouseSensor = 0.1f;
	CameraDesc.vEye = _float4(0.0f, 30.0f, -25.0f, 1.0f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 1000.f;
	CameraDesc.fSpeedPerSec = 50.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	pGameObject = CCamera_Free::Create(m_pDevice, m_pContext, &CameraDesc);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, LAYER_CAMERA, TEXT("GameObject_Camera_Free"), pGameObject)))
		return E_FAIL;

	m_pGameInstance->AddCamera(CCamera_Manager::CAM_FREE, pGameObject);

	CameraDesc.vEye = _float4(0.0f, 30.0f, -25.0f, 1.0f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 1000.f;
	CameraDesc.fSpeedPerSec = 50.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	pGameObject = CCamera_Event::Create(m_pDevice, m_pContext, &CameraDesc);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, LAYER_CAMERA, TEXT("GameObject_Camera_Event"), pGameObject)))
		return E_FAIL;

	m_pGameInstance->AddCamera(CCamera_Manager::CAM_EVENT, pGameObject);

	m_pGameInstance->SetMainCam(CCamera_Manager::CAM_ATTACH);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const LAYER_ID& eLayerID)
{
	/*shared_ptr<CGameObject> pGameObject = make_shared<CPlayer>(m_pDevice, m_pContext);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_VILLAGE, LAYER_BACKGROUND, TEXT("Prototype_GameObject_Terrain"), pGameObject)))
		return E_FAIL;*/

	/* For.Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, LAYER_BACKGROUND, TEXT("GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_BATTLE2, LAYER_MAP, TEXT("Map_Battle2"),
		CMeshMap::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Map_Battle2"), TEXT("Prototype_Component_Navigation"), LEVEL_BATTLE2, true))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_VILLAGE, LAYER_MAP, TEXT("Map_Village"),
		CMeshMap::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Map_Village"), TEXT("Prototype_Component_Navigation"), LEVEL_VILLAGE, true))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_BATTLE1, LAYER_MAP, TEXT("Map_Battle1"),
		CMeshMap::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Map_Battle1"), TEXT("Prototype_Component_Navigation"), LEVEL_BATTLE1, true))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, LAYER_MAP, TEXT("Map_MiniGame"),
		CMeshMap::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Map_MiniGame"), TEXT("Prototype_Component_Navigation"), LEVEL_MINIGAME, true))))
		return E_FAIL;

	/*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_VILLAGE, LAYER_MAP, TEXT("Map_Parking"),
		CMeshMap::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Map_Parking"), TEXT("Prototype_Component_Navigation"), true))))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const LAYER_ID& eLayerID)
{
	/*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, eLayerID, TEXT("Prototype_GameObject_Particle_Rect"),
		CParticle_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, eLayerID, TEXT("Prototype_GameObject_Particle_Point"),
		CParticle_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;*/

	CParticle::PARTICLE_DESC particleDesc = {};

	// ¹«±â Å¸°Ý ½Ã ÆÄÆ¼Å¬
	particleDesc.eParticleType = CParticle::PARTICLE_SLASH;
	particleDesc.iNumInstance = 50;
	particleDesc.isTexture = false;
	particleDesc.iShaderPass = 1;

	{
		particleDesc.vPivot = _float3(1.f, 0.5f, 0.2f);
		particleDesc.vCenter = _float3(0.0f, 0.0f, 0.0f);
		particleDesc.vRange = _float3(0.0f, 0.0f, 0.0f);
		particleDesc.vSize = _float2(1.f, 1.f);
		particleDesc.vSpeed = _float2(2.0f, 13.f);
		particleDesc.vLifeTime = _float2(0.2f, 0.24f);
		particleDesc.fDuration = 0.3f;
		particleDesc.isLoop = false;
		particleDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
		particleDesc.isSetDir = true;
		particleDesc.vDirRange = _float3(1.f, 1.f, 1.f);
		particleDesc.vDirX = _float2(0.3f, 0.8f);
		particleDesc.vDirY = _float2(-0.5f, 0.5f);
		particleDesc.vDirZ = _float2(0.3f, 0.8f);

		shared_ptr<CParticle> pParticle = CParticle::Create(m_pDevice, m_pContext, &particleDesc);

		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, eLayerID, TEXT("GameObject_Particle_Attack"), pParticle)))
			return E_FAIL;

		pParticle->SetActive(false);

		CEffect_Manager::GetInstance()->AddParticle(TEXT("GameObject_Particle_Attack"), pParticle);
	}

	particleDesc.eParticleType = CParticle::PARTICLE_SLASH_LONG;
	particleDesc.iNumInstance = 30;
	particleDesc.isTexture = false;
	particleDesc.iShaderPass = 2;

	{
		particleDesc.vPivot = _float3(1.f, 0.5f, 0.2f);
		particleDesc.vCenter = _float3(0.0f, 0.0f, 0.0f);
		particleDesc.vRange = _float3(0.0f, 0.0f, 0.0f);
		particleDesc.vSize = _float2(0.05f, 0.2f);
		particleDesc.vSpeed = _float2(2.0f, 17.f);
		particleDesc.vLifeTime = _float2(0.1f, 0.2f);
		particleDesc.fDuration = 0.5f;
		particleDesc.isLoop = false;
		particleDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
		particleDesc.isSetDir = true;
		particleDesc.vDirRange = _float3(1.f, 0.f, 0.f);
		particleDesc.vDirX = _float2(0.0f, 0.0f);
		particleDesc.vDirY = _float2(0.0f, 0.0f);
		particleDesc.vDirZ = _float2(0.0f, 0.0f);

		shared_ptr<CParticle> pParticle = CParticle::Create(m_pDevice, m_pContext, &particleDesc);

		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, eLayerID, TEXT("GameObject_Particle_Attack_Long"), pParticle)))
			return E_FAIL;

		pParticle->SetActive(false);

		CEffect_Manager::GetInstance()->AddParticle(TEXT("GameObject_Particle_Attack_Long"), pParticle);
	}

	CEffect::EFFECT_DESC effectDesc = {};
	{
		effectDesc.eEffectType = CEffect::EFFECT_TEXTURE;
		effectDesc.vCenter = { 0.f, 0.f, 0.f};
		effectDesc.vSize = { 1.3f, 20.f, 1.f };
		effectDesc.isMove = false;
		effectDesc.vDir = { 0.f, 0.f, 0.f };
		effectDesc.fMoveSpeed = 10.f;
		effectDesc.isRotate = false;
		effectDesc.vColor = { 1.f, 1.f, 1.f, 1.f };
		effectDesc.fLifeTime = 0.3f;
		effectDesc.fDuration = 0.3f;
		effectDesc.isUseMask = true;
		effectDesc.isLoop = false;
		effectDesc.iShaderPass = 0;

		shared_ptr<CEffect_Texture> pEffectAttackSlash = CEffect_Texture::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Texture_Attack_Line"), TEXT("Prototype_Component_Texture_Attack_Line_Mask"), &effectDesc);

		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, eLayerID, TEXT("GameObject_Effect_Attack_Line"), pEffectAttackSlash)))
			return E_FAIL;
		pEffectAttackSlash->SetActive(false);

		CEffect_Manager::GetInstance()->AddEffect(TEXT("GameObject_Effect_Attack_Line"), pEffectAttackSlash);
	}

	effectDesc = {};
	{
		effectDesc.eEffectType = CEffect::EFFECT_TEXTURE;
		effectDesc.vCenter = { 0.f, 0.f, 0.f };
		effectDesc.vSize = { 4.f, 4.f, 1.f };
		effectDesc.isMove = false;
		effectDesc.vDir = { 0.f, 0.f, 0.f };
		effectDesc.fMoveSpeed = 0.3f;
		effectDesc.isRotate = false;
		effectDesc.isThorwColor = true;
		effectDesc.vColor = { 1.f, 0.878f, 0.231f, 1.f };
		effectDesc.fLifeTime = 0.3f;
		effectDesc.fDuration = 0.3f;
		effectDesc.isLoop = false;
		effectDesc.iShaderPass = 2;
		effectDesc.isSprite = true;
		effectDesc.isUseMask = false;
		effectDesc.iUVx = 9;
		effectDesc.iUVy = 7;

		shared_ptr<CEffect_Texture> pEffectAttackParticle = CEffect_Texture::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Texture_Attack_Particle_Blue"), TEXT(""), &effectDesc);

		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, eLayerID, TEXT("GameObject_Effect_Attack_Particle_Blue"), pEffectAttackParticle)))
			return E_FAIL;
		pEffectAttackParticle->SetActive(false);

		CEffect_Manager::GetInstance()->AddEffect(TEXT("GameObject_Effect_Attack_Particle_Blue"), pEffectAttackParticle);
	}

	effectDesc = {};
	{
		effectDesc.eEffectType = CEffect::EFFECT_MESH;
		effectDesc.vCenter = { 0.f, 0.f, 0.f };
		effectDesc.vSize = { 4.f, 4.f, 1.f };
		effectDesc.isMove = false;
		effectDesc.vDir = { 0.f, 0.f, 0.f };
		effectDesc.fMoveSpeed = 0.01f;
		effectDesc.isRotate = false;
		effectDesc.isThorwColor = true;
		effectDesc.vColor = { 0.435f, 0.812f, 0.812f, 1.f };
		effectDesc.isLoop = true;
		effectDesc.iShaderPass = 4;
		effectDesc.isSprite = false;
		effectDesc.isUseMask = true;

		shared_ptr<CEffect_Pillar> pEffectTriggerPillar = CEffect_Pillar::Create(m_pDevice, m_pContext, &effectDesc);

		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, eLayerID, TEXT("GameObject_Effect_Trigger_Pillar"), pEffectTriggerPillar)))
			return E_FAIL;
		pEffectTriggerPillar->SetActive(false);

		CEffect_Manager::GetInstance()->AddEffect(TEXT("GameObject_Effect_Trigger_Pillar"), pEffectTriggerPillar);
	}

	effectDesc = {};
	{
		effectDesc.eEffectType = CEffect::EFFECT_MESH;
		effectDesc.vCenter = { 0.f, 0.f, 0.f };
		effectDesc.vSize = { 1.5f, 2.f, 1.5f };
		effectDesc.isMove = false;
		effectDesc.vDir = { 0.f, 0.f, 0.f };
		effectDesc.fMoveSpeed = 0.01f;
		effectDesc.isRotate = false;
		effectDesc.isThorwColor = false;
		effectDesc.vColor = { 1.f, 1.f, 1.f, 1.f };
		effectDesc.isLoop = false;
		effectDesc.iShaderPass = 5;
		effectDesc.isSprite = false;
		effectDesc.isUseMask = true;
		effectDesc.fDuration = 1.f;

		wstring strModelComTag = L"Prototype_Component_Model_Effect_Sphere";
		wstring strMaskTextureComTag = L"Prototype_Component_Texture_Distortion_02";

		shared_ptr<CEffect_Distortion> pEffectDistortion = CEffect_Distortion::Create(m_pDevice, m_pContext, strModelComTag, strMaskTextureComTag, &effectDesc);

		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, eLayerID, TEXT("GameObject_Effect_Distortion_Test"), pEffectDistortion)))
			return E_FAIL;
		pEffectDistortion->SetActive(false);

		CEffect_Manager::GetInstance()->AddEffect(TEXT("GameObject_Effect_Distortion_Test"), pEffectDistortion);
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LandObjects()
{
	CLandObject::LANDOBJ_DESC			LandObjDesc{};

	LandObjDesc.pTerrainTransform = dynamic_pointer_cast<CTransform>(m_pGameInstance->Find_Component(LEVEL_VILLAGE, LAYER_BACKGROUND, g_strTransformTag));
	LandObjDesc.pTerrainVIBuffer = dynamic_pointer_cast<CVIBuffer_HeightTerrain>(m_pGameInstance->Find_Component(LEVEL_VILLAGE, LAYER_BACKGROUND, TEXT("Com_VIBuffer")));

	if (FAILED(Ready_Layer_Player(LAYER_PLAYER, LandObjDesc)))
		return E_FAIL;

	/*if (FAILED(Ready_Layer_Monster(LAYER_MONSTER, LandObjDesc)))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const LAYER_ID& eLayerID, CLandObject::LANDOBJ_DESC& LandObjDesc)
{
	m_pPlayer = CPlayer::Create(m_pDevice, m_pContext, &LandObjDesc);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, LAYER_PLAYER, TEXT("GameObject_Player"),
		dynamic_pointer_cast<CGameObject>(m_pPlayer))))
		return E_FAIL;

	CBike::BIKE_DESC			BikeObjDesc{};
	BikeObjDesc.pParentTransform = m_pPlayer->Get_TransformCom();
	BikeObjDesc.pSocketMatrix = m_pPlayer->GetBodyModel()->Get_CombinedBoneMatrixPtr("weapon_r");

	shared_ptr<CBike> pBike = CBike::Create(m_pDevice, m_pContext, &BikeObjDesc);

	m_pGameInstance->Add_GameObject(LEVEL_STATIC, LAYER_VEHICLE, TEXT("GameObject_Vehicle_Bike"), pBike);

	m_pPlayer->SetBike(pBike);

	/*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, LAYER_PLAYER, TEXT("GameObject_Vehicle_Bike"),
		CBike::Create(m_pDevice, m_pContext, &LandObjDesc))))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const LAYER_ID& eLayerID, CLandObject::LANDOBJ_DESC& LandObjDesc)
{
	/*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_VILLAGE, LAYER_MONSTER, TEXT("GameObject_Monster_TriplePut"),
		CMonster_TriplePut::Create(m_pDevice, m_pContext, &LandObjDesc))))
		return E_FAIL;*/
	/*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_BATTLE1, LAYER_MONSTER, TEXT("GameObject_Monster_TriplePut"),
		CMonster_TriplePut::Create(m_pDevice, m_pContext, &LandObjDesc))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_BATTLE2, LAYER_MONSTER, TEXT("GameObject_Monster_Leopardon"),
		CMonster_Leopardon::Create(m_pDevice, m_pContext, &LandObjDesc))))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Static_Object(const LAYER_ID& eLayerID)
{
	return E_NOTIMPL;
}

HRESULT CLevel_GamePlay::Ready_Layer_Trigger(const LAYER_ID& eLayerID)
{
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_VILLAGE, eLayerID, TEXT("GameObject_Trigger_Battle1"),
		CTrigger::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Trigger"), CTrigger::TRIGGER_BATTLE1, true))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_VILLAGE, eLayerID, TEXT("GameObject_Trigger_Battle2"),
		CTrigger::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Trigger"), CTrigger::TRIGGER_BATTLE2, true))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_VILLAGE, eLayerID, TEXT("GameObject_Trigger_MiniGame"),
		CTrigger::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Trigger"), CTrigger::TRIGGER_MINIGAME, true))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const LAYER_ID& eLayerID)
{
	CUI::UIDESC		UIDesc = {};

	UIDesc.fCX = 500; UIDesc.fCY = 71;
	UIDesc.fX = UIDesc.fCX * 0.5f;
	UIDesc.fY = UIDesc.fCY * 1.2f;

	shared_ptr<CUI>	pUI = CUI_QuestBoard::Create(m_pDevice, m_pContext, &UIDesc);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_VILLAGE, eLayerID, TEXT("GameObject_UI_QuestBoard"), pUI)))
		return E_FAIL;

	CUI_Manager::GetInstance()->AddUI(TEXT("GameObject_UI_QuestBoard"), pUI);

	// Fade UI
	UIDesc.fCX = 1280; UIDesc.fCY = 720;
	UIDesc.fX = g_iWinSizeX * 0.5f;
	UIDesc.fY = g_iWinSizeY * 0.5f;

	pUI = CUI_Fade::Create(m_pDevice, m_pContext, &UIDesc);
	pUI->SetActive(false);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, eLayerID, TEXT("GameObject_UI_Fade"), pUI)))
		return E_FAIL;

	CUI_Manager::GetInstance()->AddUI(TEXT("GameObject_UI_Fade"), pUI);

	// UI ÅÚ·¹Æ÷Æ®
	UIDesc.fCX = g_iWinSizeX; UIDesc.fCY = g_iWinSizeY;
	UIDesc.fX = UIDesc.fCX * 0.5f;
	UIDesc.fY = UIDesc.fCY * 0.5f;

	pUI = CUI_Teleport::Create(m_pDevice, m_pContext, &UIDesc);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_VILLAGE, eLayerID, TEXT("GameObject_UI_Teleport"), pUI)))
		return E_FAIL;

	CUI_Manager::GetInstance()->AddUI(TEXT("GameObject_UI_Teleport"), pUI);

	UIDesc.fCX = 40; UIDesc.fCY = 80;
	UIDesc.fX = g_iWinSizeX - 150.f;
	UIDesc.fY = 70.f;

	pUI = CUI_Coin::Create(m_pDevice, m_pContext, &UIDesc);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, eLayerID, TEXT("GameObject_UI_Coin"), pUI)))
		return E_FAIL;

	CUI_Manager::GetInstance()->AddUI(TEXT("GameObject_UI_Coin"), pUI);
	CUI_Manager::GetInstance()->SetCoinUI(dynamic_pointer_cast<CUI_Coin>(pUI));

	UIDesc.fCX = 5; UIDesc.fCY = 5;
	UIDesc.fX = 100.f;
	UIDesc.fY = 100.f;

	pUI = CUI_HP_Player::Create(m_pDevice, m_pContext, &UIDesc);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_BATTLE1, eLayerID, TEXT("GameObject_UI_HP_Player"), pUI)))
		return E_FAIL;

	CUI_Manager::GetInstance()->AddUI(TEXT("GameObject_UI_HP_Player_Battle1"), pUI);

	UIDesc.fCX = 5; UIDesc.fCY = 5;
	UIDesc.fX = 100.f;
	UIDesc.fY = 100.f;

	pUI = CUI_HP_Player::Create(m_pDevice, m_pContext, &UIDesc);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_BATTLE2, eLayerID, TEXT("GameObject_UI_HP_Player"), pUI)))
		return E_FAIL;

	CUI_Manager::GetInstance()->AddUI(TEXT("GameObject_UI_HP_Player_Battle2"), pUI);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Environment(const LAYER_ID& eLayerID)
{
	/*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_VILLAGE, eLayerID, TEXT("GameObject_Sign_Battle1"),
		CSign::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Sign_SpaceShip"), CSign::SIGN_BATTLE1, true))))
		return E_FAIL;*/

	/*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_VILLAGE, eLayerID, TEXT("GameObject_Sign_Battle2"),
		CSign::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Sign_SpaceShip"), CSign::SIGN_BATTLE2, true))))
		return E_FAIL;*/

	/*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_VILLAGE, LAYER_ENVIRONMENT, TEXT("GameObject_Sign_MiniGame"),
		CSign::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Sign_Coin"), CSign::SIGN_MINIGAME, true))))
		return E_FAIL;*/

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_VILLAGE, eLayerID, TEXT("GameObject_Coin_Default"),
		CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Default"), CCoin::COIN_DEFAULT, XMVectorSet(-144.f, 7.f, 220.f, 1.f), false))))
		return E_FAIL;
	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 5.f, 7.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 9.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 11.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 13.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 15.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 17.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 5.f, 19.f, 1.f), true))))
			return E_FAIL;
	}


	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 50.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 52.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 54.f, 1.f), true))))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(-5.3f, 5.f, 65.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(-5.3f, 5.f, 67.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(-5.3f, 5.f, 69.f, 1.f), true))))
			return E_FAIL;
	}

	// Â«Çª¿ë
	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 7.f, 90.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 7.0f, 92.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 7.f, 94.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 7.f, 96.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 7.f, 98.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 7.f, 100.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 7.f, 102.f, 1.f), true))))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(0.f, 5.f, 120.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(0.f, 5.f, 122.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(0.f, 5.f, 124.f, 1.f), true))))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(5.3f, 5.f, 130.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(5.3f, 5.f, 132.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(5.3f, 5.f, 134.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(5.3f, 5.f, 136.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(5.3f, 5.f, 138.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(5.3f, 5.f, 140.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(5.3f, 5.f, 142.f, 1.f), true))))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_GOLD, XMVectorSet(-5.3f, 5.f, 160.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_GOLD, XMVectorSet(-5.3f, 5.f, 162.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_GOLD, XMVectorSet(-5.3f, 5.f, 164.f, 1.f), true))))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(-5.3f, 5.f, 170.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(-5.3f, 5.f, 172.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(-5.3f, 5.f, 174.f, 1.f), true))))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 5.f, 234.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 5.f, 236.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 5.f, 238.f, 1.f), true))))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 5.f, 7.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 9.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 11.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 13.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 15.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 17.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 5.f, 19.f + 200.f, 1.f), true))))
			return E_FAIL;
	}


	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 50.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 52.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 54.f + 200.f, 1.f), true))))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(-5.3f, 5.f, 65.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(-5.3f, 5.f, 67.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(-5.3f, 5.f, 69.f + 200.f, 1.f), true))))
			return E_FAIL;
	}

	// Â«Çª¿ë
	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 6.3f, 90.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 6.5f, 92.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 6.7f, 94.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 6.9f, 96.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 7.f, 98.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 7.f, 100.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 7.f, 102.f + 200.f, 1.f), true))))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(-5.3f, 5.f, 120.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(-5.3f, 5.f, 122.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(-5.3f, 5.f, 124.f + 200.f, 1.f), true))))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(0.3f, 5.f, 130.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(0.3f, 5.f, 132.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.3f, 5.f, 134.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(0.3f, 5.f, 136.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.3f, 5.f, 138.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(0.3f, 5.f, 140.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.3f, 5.f, 142.f + 200.f, 1.f), true))))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(-5.3f, 5.f, 150.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(-5.3f, 5.f, 152.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(-5.3f, 5.f, 154.f + 200.f, 1.f), true))))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(-5.3f, 5.f, 170.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(-5.3f, 5.f, 172.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(-5.3f, 5.f, 174.f + 200.f, 1.f), true))))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 5.f, 234.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 5.f, 236.f + 200.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 5.f, 238.f + 200.f, 1.f), true))))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 5.f, 7.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 9.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 11.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 13.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 15.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 17.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 5.f, 19.f+ 400.f, 1.f), true))))
			return E_FAIL;
	}


	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 50.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 52.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 54.f+ 400.f, 1.f), true))))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(-5.3f, 5.f, 65.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(-5.3f, 5.f, 67.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(-5.3f, 5.f, 69.f+ 400.f, 1.f), true))))
			return E_FAIL;
	}

	// Â«Çª¿ë
	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(5.3f, 5.f, 90.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(5.3f, 5.f, 92.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(5.3f, 5.f, 94.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(5.3f, 5.f, 96.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(5.3f, 5.f, 98.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(5.3f, 5.f, 100.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(5.3f, 5.f, 102.f+ 400.f, 1.f), true))))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(-0.f, 5.f, 120.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(0.f, 5.f, 122.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(0.f, 5.f, 124.f+ 400.f, 1.f), true))))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(-5.3f, 5.f, 130.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(-5.3f, 5.f, 132.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(-5.3f, 5.f, 134.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(-5.3f, 5.f, 136.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(-5.3f, 5.f, 138.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(-5.3f, 5.f, 140.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(-5.3f, 5.f, 142.f+ 400.f, 1.f), true))))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 5.f, 150.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 5.f, 152.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 5.f, 154.f+ 400.f, 1.f), true))))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(-5.3f, 5.f, 170.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(-5.3f, 5.f, 172.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(-5.3f, 5.f, 174.f+ 400.f, 1.f), true))))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 5.f, 234.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 5.f, 236.f+ 400.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 5.f, 238.f+ 400.f, 1.f), true))))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(-5.3f, 5.f, 660.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(-5.3f, 5.f, 662.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(-5.3f, 5.f, 664.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(-5.3f, 5.f, 666.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(-5.3f, 5.f, 668.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(-5.3f, 5.f, 670.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(-5.3f, 5.f, 672.f, 1.f), true))))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(5.3f, 7.f, 700.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(5.3f, 7.f, 702.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(5.3f, 7.f, 704.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(5.3f, 7.f, 706.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(5.3f, 7.f, 708.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Silver"), CCoin::COIN_SILVER, XMVectorSet(5.3f, 7.f, 710.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(5.3f, 7.f, 712.f, 1.f), true))))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 750.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 752.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Copper"), CCoin::COIN_COPPER, XMVectorSet(0.f, 5.f, 754.f, 1.f), true))))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(-5.3f, 5.f, 780.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(-5.3f, 5.f, 782.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(-5.3f, 5.f, 784.f, 1.f), true))))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 7.f, 800.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 7.f, 802.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 7.f, 804.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 7.f, 806.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 7.f, 808.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 7.f, 810.f, 1.f), true))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MINIGAME, eLayerID, TEXT("GameObject_Coin_Copper"),
			CCoin::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Coin_Gold"), CCoin::COIN_GOLD, XMVectorSet(0.f, 7.f, 812.f, 1.f), true))))
			return E_FAIL;
	}

	return S_OK;
}

shared_ptr<CLevel_GamePlay> CLevel_GamePlay::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CLevel_GamePlay> pInstance = make_shared<CLevel_GamePlay>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		pInstance = nullptr;
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();
}
