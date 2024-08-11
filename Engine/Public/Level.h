#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CLevel abstract
{
public:
	CLevel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	~CLevel() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	ComPtr<ID3D11Device> m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };

protected:
	shared_ptr<class CGameInstance> m_pGameInstance = { nullptr };

public:
	virtual void Free();
};

END