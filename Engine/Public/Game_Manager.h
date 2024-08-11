#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CGame_Manager final
{
public:
	enum EGameState { STATE_LOADING, STATE_PROCESS, STATE_PAUSE, STATE_CUTSCENE, STATE_LOCK, STATE_SLOW, STATE_END };

public:
	CGame_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	~CGame_Manager() = default;

public:
	HRESULT Initialize(HWND hWnd);

public:
	void		Set_GameState(EGameState eGameState) { m_eCurrentState = eGameState; }
	EGameState	Get_GameState() { return m_eCurrentState; }

	void							Set_Player(shared_ptr<class CGameObject> pPlayer) { m_pPlayer = pPlayer; }
	shared_ptr<class CGameObject>	Get_Player() { if (m_pPlayer.lock()) return m_pPlayer.lock(); else return nullptr; }

	void							Change_ClipCursor();


private:
	ComPtr<ID3D11Device>				m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>			m_pContext = { nullptr };

private:
	EGameState	m_eCurrentState = STATE_END;
	weak_ptr<class CGameObject> m_pPlayer;

	HWND			m_hWnd;
	RECT			m_ClientRect;
	POINT			m_p1, m_p2;
	_bool			m_isClipCursor = true;


public:
	static shared_ptr<CGame_Manager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, HWND hWnd);
};

END