#include "pch.h"
#include "Level_Village.h"

#include "Camera_Free.h"
#include "Camera_Attach.h"
#include "Player.h"
#include "Monster_TriplePut.h"
#include "Monster_Leopardon.h"
#include "Sky.h"
#include "StaticObject.h"
#include "MeshMap.h"

CLevel_Village::CLevel_Village(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Village::Initialize()
{
	//if (FAILED(Ready_Lights()))
	//	return E_FAIL;

	//if (FAILED(Ready_Layer_Player(LAYER_PLAYER)))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(LAYER_BACKGROUND)))
		return E_FAIL;

	if (FAILED(Ready_LandObjects()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(LAYER_CAMERA)))
		return E_FAIL;

	return S_OK;
}

void CLevel_Village::Tick(_float fTimeDelta)
{
	//if (m_pGameInstance->Is_KeyDown(DIK_0))
	//{
	//	CLandObject::LANDOBJ_DESC			LandObjDesc{};

	//	LandObjDesc.pTerrainTransform = dynamic_pointer_cast<CTransform>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, LAYER_BACKGROUND, g_strTransformTag));
	//	LandObjDesc.pTerrainVIBuffer = dynamic_pointer_cast<CVIBuffer_HeightTerrain>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, LAYER_BACKGROUND, TEXT("Com_VIBuffer")));

	//	if (m_isMonster1)
	//	{
	//		m_pGameInstance->Remove_Object(LEVEL_GAMEPLAY, LAYER_MONSTER, TEXT("GameObject_Monster_TriplePut"));
	//		m_isMonster1 = false;
	//	}

	//	if (m_isMonster2)
	//	{
	//		m_pGameInstance->Remove_Object(LEVEL_GAMEPLAY, LAYER_MONSTER, TEXT("GameObject_Monster_Leopardon"));
	//		m_isMonster2 = false;
	//	}

	//	m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, LAYER_MONSTER, TEXT("GameObject_Monster_TriplePut"),
	//		CMonster_TriplePut::Create(m_pDevice, m_pContext, &LandObjDesc));

	//	m_isMonster1 = true;
	//}

	//else if (m_pGameInstance->Is_KeyDown(DIK_9))
	//{
	//	CLandObject::LANDOBJ_DESC			LandObjDesc{};

	//	LandObjDesc.pTerrainTransform = dynamic_pointer_cast<CTransform>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, LAYER_BACKGROUND, g_strTransformTag));
	//	LandObjDesc.pTerrainVIBuffer = dynamic_pointer_cast<CVIBuffer_HeightTerrain>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, LAYER_BACKGROUND, TEXT("Com_VIBuffer")));

	//	if (m_isMonster1)
	//	{
	//		m_pGameInstance->Remove_Object(LEVEL_GAMEPLAY, LAYER_MONSTER, TEXT("GameObject_Monster_TriplePut"));
	//		m_isMonster1 = false;
	//	}

	//	if (m_isMonster2)
	//	{
	//		m_pGameInstance->Remove_Object(LEVEL_GAMEPLAY, LAYER_MONSTER, TEXT("GameObject_Monster_Leopardon"));
	//		m_isMonster2 = false;
	//	}

	//	m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, LAYER_MONSTER, TEXT("GameObject_Monster_Leopardon"),
	//		CMonster_Leopardon::Create(m_pDevice, m_pContext, &LandObjDesc));

	//	m_isMonster2 = true;
	//}
}

void CLevel_Village::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Check_Collision(LAYER_MONSTER, LAYER_PLAYER_ATTACK);
	m_pGameInstance->Check_Collision(LAYER_PLAYER, LAYER_MONSTER_ATTACK);
	m_pGameInstance->Check_Collision(LAYER_PLAYER, LAYER_PROJECTILE);
	m_pGameInstance->Check_Collision(LAYER_PLAYER, LAYER_MONSTER);
}

HRESULT CLevel_Village::Render()
{
	//#ifdef _DEBUG
	//	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
	//#endif

	return S_OK;
}

HRESULT CLevel_Village::Ready_Lights()
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

	return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_Camera(const LAYER_ID& eLayerID)
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
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, LAYER_CAMERA, TEXT("Prototype_GameObject_Camera_Attach"), pGameObject)))
		return E_FAIL;

	m_pGameInstance->AddCamera(CCamera_Manager::CAM_ATTACH, pGameObject);

	//CCamera_Free::CAMERA_FREE_DESC		CameraDesc = {};

	CameraDesc.fMouseSensor = 0.1f;
	CameraDesc.vEye = _float4(0.0f, 30.0f, -25.0f, 1.0f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 1000.f;
	CameraDesc.fSpeedPerSec = 50.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	//shared_ptr<CGameObject> pGameObject = CCamera_Free::Create(m_pDevice, m_pContext, &CameraDesc);
	pGameObject = CCamera_Free::Create(m_pDevice, m_pContext, &CameraDesc);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, LAYER_CAMERA, TEXT("GameObject_Camera_Free"), pGameObject)))
		return E_FAIL;

	m_pGameInstance->AddCamera(CCamera_Manager::CAM_FREE, pGameObject);

	return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_BackGround(const LAYER_ID& eLayerID)
{
	/*shared_ptr<CGameObject> pGameObject = make_shared<CPlayer>(m_pDevice, m_pContext);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, LAYER_BACKGROUND, TEXT("Prototype_GameObject_Terrain"), pGameObject)))
		return E_FAIL;*/

	/* For.Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, LAYER_BACKGROUND, TEXT("GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_VILLAGE, LAYER_MAP, TEXT("Map_Village"),
		CMeshMap::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Map_Village"), true))))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_Village::Ready_LandObjects()
{
	CLandObject::LANDOBJ_DESC			LandObjDesc{};

	LandObjDesc.pTerrainTransform = dynamic_pointer_cast<CTransform>(m_pGameInstance->Find_Component(LEVEL_VILLAGE, LAYER_BACKGROUND, g_strTransformTag));
	LandObjDesc.pTerrainVIBuffer = dynamic_pointer_cast<CVIBuffer_HeightTerrain>(m_pGameInstance->Find_Component(LEVEL_VILLAGE, LAYER_BACKGROUND, TEXT("Com_VIBuffer")));

	m_pGameInstance->Find_GameObject(LEVEL_STATIC, LAYER_PLAYER, TEXT("GameObject_Player"));

	dynamic_pointer_cast<CTransform>(m_pGameInstance->Find_Component(LEVEL_STATIC, LAYER_PLAYER, g_strTransformTag, 0))->Set_State(CTransform::STATE_POSITION, {0.f, 0.f, 0.f, 1.f});

	/*if (FAILED(Ready_Layer_Player(LAYER_PLAYER, LandObjDesc)))
		return E_FAIL;*/

	/*if (FAILED(Ready_Layer_Monster(LAYER_MONSTER, LandObjDesc)))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_Player(const LAYER_ID& eLayerID, CLandObject::LANDOBJ_DESC& LandObjDesc)
{
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, LAYER_PLAYER, TEXT("GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext, &LandObjDesc))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Village::Ready_Layer_Monster(const LAYER_ID& eLayerID, CLandObject::LANDOBJ_DESC& LandObjDesc)
{
	/*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, LAYER_MONSTER, TEXT("GameObject_Monster_TriplePut"),
		CMonster_TriplePut::Create(m_pDevice, m_pContext, &LandObjDesc))))
		return E_FAIL;*/
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, LAYER_MONSTER, TEXT("GameObject_Monster_TriplePut"),
		CMonster_TriplePut::Create(m_pDevice, m_pContext, &LandObjDesc))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, LAYER_MONSTER, TEXT("GameObject_Monster_Leopardon"),
		CMonster_Leopardon::Create(m_pDevice, m_pContext, &LandObjDesc))))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CLevel_Village> CLevel_Village::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CLevel_Village> pInstance = make_shared<CLevel_Village>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Village");
		pInstance = nullptr;
	}

	return pInstance;
}

void CLevel_Village::Free()
{
	__super::Free();
}
