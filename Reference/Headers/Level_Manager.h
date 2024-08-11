#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CLevel_Manager final
{
public:
	CLevel_Manager();
	~CLevel_Manager() = default;

public:
	HRESULT Initialize();
	void	Tick(_float fTimeDelta);
	void	Late_Tick(_float fTimeDelta);
	HRESULT Render();
	HRESULT Open_Level(_uint iLevelIndex, shared_ptr<class CLevel> pNewLevel);

private:
	_uint								m_iLevelIndex = { 0 };
	shared_ptr<class CLevel>			m_pCurrentLevel = { nullptr };
	shared_ptr<class CGameInstance>		m_pGameInstance = { nullptr };

public:
	static shared_ptr<CLevel_Manager> Create();
	void Free();
};

END