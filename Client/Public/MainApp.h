#pragma once

#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CMainApp final
{
public:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	HRESULT Render();

private:
	shared_ptr<CGameInstance>		m_pGameInstance = { nullptr };
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };

#ifdef _DEBUG
private:
	_tchar						m_szFPS[MAX_PATH] = TEXT("");
	_uint						m_iRenderCount = { 0 };
	_float						m_fTimeAcc = { 0 };
#endif


private:
	HRESULT Open_Level(LEVEL eStartLevel);
	HRESULT Ready_Gara();
	HRESULT Ready_Prototype_Component_Static();

public:
	static shared_ptr<CMainApp> Create();
	void Free();
};

END