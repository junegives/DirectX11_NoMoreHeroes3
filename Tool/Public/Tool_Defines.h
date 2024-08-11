#pragma once
#pragma once

#include "Engine_Defines.h"

#include <iostream>
#include <process.h>
#include <string>
#include <cstring>
#include <io.h>
#include "GameInstance.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"

//#include "ImGuizmo\ImApp.h"
#include "ImGuizmo\ImGuizmo.h"
//#include "ImGuizmo\ImSequencer.h"
//#include "ImGuizmo\ImZoomSlider.h"
//#include "ImGuizmo\ImCurveEdit.h"
//#include "ImGuizmo\GraphEditor.h"

#include <wincodec.h>
#pragma comment(lib, "windowscodecs.lib")

namespace Tool
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_MAPTOOL, LEVEL_END };

	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;
}

extern HWND				g_hWnd;
extern HINSTANCE		g_hInst;

using namespace std;
using namespace Tool;
