#include "pch.h"
#include "Level_MapTool.h"
#include "LandObject.h"
#include "Player.h"

#include "Camera_Free.h"

CLevel_MapTool::CLevel_MapTool(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_MapTool::Initialize()
{
	/*if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(LAYER_CAMERA)))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(LAYER_BACKGROUND)))
		return E_FAIL;*/

	/*if (FAILED(Ready_Layer_Player(LAYER_PLAYER)))
		return E_FAIL;*/
	m_pGameInstance->Set_CurLevel((_uint)LEVEL_MAPTOOL);

	return S_OK;
}

void CLevel_MapTool::Tick(_float fTimeDelta)
{
}

HRESULT CLevel_MapTool::Render()
{
	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Lights()
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

HRESULT CLevel_MapTool::Ready_Layer_Camera(const LAYER_ID& eLayerID)
{

	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_Player(const LAYER_ID& eLayerID)
{
	CLandObject::LANDOBJ_DESC			LandObjDesc{};

	LandObjDesc.pTerrainTransform = dynamic_pointer_cast<CTransform>(m_pGameInstance->Find_Component(LEVEL_MAPTOOL, LAYER_BACKGROUND, g_strTransformTag));
	LandObjDesc.pTerrainVIBuffer = dynamic_pointer_cast<CVIBuffer_HeightTerrain>(m_pGameInstance->Find_Component(LEVEL_MAPTOOL, LAYER_BACKGROUND, TEXT("Com_VIBuffer")));

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, LAYER_PLAYER, TEXT("GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext, &LandObjDesc))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_BackGround(const LAYER_ID& eLayerID)
{

	return S_OK;
}

shared_ptr<CLevel_MapTool> CLevel_MapTool::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CLevel_MapTool> pInstance = make_shared<CLevel_MapTool>(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_MapTool");
		pInstance = nullptr;
	}

	return pInstance;
}

void CLevel_MapTool::Free()
{
	__super::Free();
}
