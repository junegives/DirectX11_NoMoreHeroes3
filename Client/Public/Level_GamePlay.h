#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "LandObject.h"
#include "Player.h"

class CLevel_GamePlay final : public CLevel
{
public:
	CLevel_GamePlay(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const LAYER_ID& eLayerID);
	HRESULT Ready_Layer_BackGround(const LAYER_ID& eLayerID);
	HRESULT Ready_Layer_Effect(const LAYER_ID& eLayerID);
	HRESULT Ready_LandObjects();
	HRESULT Ready_Layer_Player(const LAYER_ID& eLayerID, CLandObject::LANDOBJ_DESC& LandObjDesc);
	HRESULT Ready_Layer_Monster(const LAYER_ID& eLayerID, CLandObject::LANDOBJ_DESC& LandObjDesc);
	HRESULT Ready_Layer_Static_Object(const LAYER_ID& eLayerID);
	HRESULT Ready_Layer_Trigger(const LAYER_ID& eLayerID);
	HRESULT Ready_Layer_UI(const LAYER_ID& eLayerID);
	HRESULT Ready_Layer_Environment(const LAYER_ID& eLayerID);

public:
	static shared_ptr<CLevel_GamePlay> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void Free() override;

private:
	_bool				m_isMonster1	= false;
	_bool				m_isMonster2	= false;
	_bool				m_isMonster3	= false;
	_bool				m_isBike		= false;

	shared_ptr<CPlayer> m_pPlayer = nullptr;
};

