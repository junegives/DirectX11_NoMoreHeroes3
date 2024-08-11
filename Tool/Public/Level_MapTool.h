#pragma once

#include "Tool_Defines.h"
#include "Level.h"

class CLevel_MapTool final : public CLevel
{
public:
	CLevel_MapTool(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CLevel_MapTool() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const LAYER_ID& eLayerID);
	HRESULT Ready_Layer_Player(const LAYER_ID& eLayerID);
	HRESULT Ready_Layer_BackGround(const LAYER_ID& eLayerID);

public:
	static shared_ptr<CLevel_MapTool> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void Free() override;
};

