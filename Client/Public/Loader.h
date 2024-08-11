#pragma once

#include "Client_Defines.h"

/* 생성 시, 전달받은 index에 해당하는 레벨의 자원을 준비한다. */

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CLoader final
{
public:
	CLoader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CLoader() = default;

public:
	HRESULT Initialize(LEVEL eNextLevel);
	HRESULT Loading_Selector();
	_bool isFinished() const {
		return m_isFinished;
	}

#ifdef _DEBUG
public:
	void Output_LoadingText();
#endif

private:
	ComPtr<ID3D11Device>		m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };

private:
	wstring					m_strLoadingText = {};
	_bool					m_isFinished = { false };

private:
	HANDLE						m_hThread = { 0 };
	CRITICAL_SECTION			m_CriticalSection = {};
	LEVEL						m_eNextLevel = { LEVEL_END };
	shared_ptr<CGameInstance>	m_pGameInstance = { nullptr };

private:
	HRESULT Loading_For_LogoLevel();
	HRESULT Loading_For_GamePlayLevel();
	HRESULT Loading_For_VillageLevel();

public:
	static shared_ptr<CLoader> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eNextLevel);
	void Free();
};

END