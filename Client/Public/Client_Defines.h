#pragma once

#include "Engine_Defines.h"

#include <process.h>
#include <string>
#include "GameInstance.h"

namespace Client
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_VILLAGE, LEVEL_BATTLE1, LEVEL_BATTLE2, LEVEL_MINIGAME, LEVEL_END };

	enum HIT_TYPE { HIT_LIGHT, HIT_DEFAULT, HIT_HEAVY, HIT_KNOCKDOWN, HIT_BLOWNOFF, HIT_STUN, HIT_TRHOW, HIT_END };

	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;

	/*const _uint STATE_IDLE = 0x00000001;
	const _uint STATE_RUN = 0x00000002;*/
}

extern HWND				g_hWnd;
extern HINSTANCE		g_hInst;

using namespace std;
using namespace Client;
