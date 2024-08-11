#pragma once
#pragma once

#include "Engine_Defines.h"

#include <process.h>
#include <string>
#include "GameInstance.h"

namespace Tool
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_END };

	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;
}

extern HWND				g_hWnd;
extern HINSTANCE		g_hInst;

using namespace std;
using namespace Tool;
