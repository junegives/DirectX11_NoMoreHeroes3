#ifndef InputDev_h__
#define InputDev_h__

#include "Engine_Defines.h"

BEGIN(Engine)

#pragma once
class CInput_Device
{
public:
	CInput_Device(void);
	~CInput_Device(void) = default;

public:
	_byte	Get_DIKeyState(_ubyte byKeyID) { return m_byKeyState[byKeyID]; }
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse) { return m_tMouseState.rgbButtons[eMouse]; }

	_bool	Is_KeyDown(_ubyte byKeyID);
	_bool	Is_KeyUp(_ubyte byKeyID);
	_bool	Is_KeyPressing(_ubyte byKeyID);

	_bool	Is_MouseDown(MOUSEKEYSTATE eMouse);
	_bool	Is_MouseUp(MOUSEKEYSTATE eMouse);
	_bool	Is_MousePressing(MOUSEKEYSTATE eMouse);

	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
	{
		return *(((_long*)&m_tMouseState) + eMouseState);
	}

public:
	HRESULT Ready_InputDev(HINSTANCE hInst, HWND hWnd);
	void	Tick(void);

private:
	LPDIRECTINPUT8			m_pInputSDK = nullptr;

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse = nullptr;

private:
	_byte					m_byKeyState[256];		// 키보드에 있는 모든 키값을 저장하기 위한 변수
	_byte					m_byPrevKeyState[256];	// 이전 키값을 저장하기 위한 변수

	DIMOUSESTATE			m_tMouseState;			// 마우스 값을 저장하기 위한 변수
	DIMOUSESTATE			m_tPrevMouseState;		// 이전 마우스 값을 저장하기 위한 변수

public:
	static shared_ptr<CInput_Device> Create(HINSTANCE hInst, HWND hWnd);
	void Free();
};

END
#endif