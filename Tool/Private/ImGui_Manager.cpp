#include "pch.h"
#include "ImGui_Manager.h"

#include "Terrain.h"
#include "Player.h"
#include "StaticModel.h"

#include "Cell.h"

IMPLEMENT_SINGLETON_SMART(CImGui_Manager)

CImGui_Manager::CImGui_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT CImGui_Manager::Initialize(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	// 그래픽 장치 초기화하는 것처럼 ImGui도 초기화 과정이 풀요함
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// 폰트 변경

	// Setup Dear ImGui style
	// ImGui의 스타일 세팅
	//ImGui::StyleColorsDark();
	ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	// 사용하려는 플랫폼과 렌더러(장치) 세팅
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice.Get(), m_pContext.Get());

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	//ImGuizmo::CreateContext();
	
	Read_FBXList();

    return S_OK;
}

// fbx 파일 경로 읽어서 갖고있기
void CImGui_Manager::Read_FBXList()
{
	string strPath = "D:/Jusin/Project/NoMoreHeroes/Resource/Resources/Models/NMH/*.model";

	struct _finddata_t fd;

	intptr_t handle;

	if ((handle = _findfirst(strPath.c_str(), &fd)) == -1L)
	{

	}

	do
	{
		if (!strncmp("Map_", fd.name, 4))
		{
			const char* extension = strrchr(fd.name, '.');

			size_t extensionIndex = extension - fd.name;
			fd.name[extensionIndex] = '\0';

			m_strMap.push_back(fd.name);

		}

		else if (!strncmp("SM_", fd.name, 3))
		{
			const char* extension = strrchr(fd.name, '.');

			size_t extensionIndex = extension - fd.name;
			fd.name[extensionIndex] = '\0';

			m_strStaticModel.push_back(fd.name);
		}
	} while (_findnext(handle, &fd) == 0);

	_findclose(handle);
}

void CImGui_Manager::Tick(_float fTimeDelta)
{
	// 매 Tick마다 들어가야 하는 업데이트 코드
	KeyInput();
	
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	//ImGuiIO& io = ImGui::GetIO();

	//float viewManipulateRight = io.DisplaySize.x;
	//float viewManipulateTop = 0;
	//static ImGuiWindowFlags gizmoWindowFlags = 0;

	//ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_Appearing);
	//ImGui::SetNextWindowPos(ImVec2(400, 20), ImGuiCond_Appearing);
	//ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor(0.35f, 0.3f, 0.3f));
	//ImGui::Begin("Gizmo", 0, gizmoWindowFlags);
	//ImGuizmo::SetDrawlist();
	//float windowWidth = (float)ImGui::GetWindowWidth();
	//float windowHeight = (float)ImGui::GetWindowHeight();
	//ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
	//viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
	//viewManipulateTop = ImGui::GetWindowPos().y;
	//ImGuiWindow* window = ImGui::GetCurrentWindow();
	//gizmoWindowFlags = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max) ? ImGuiWindowFlags_NoMove : 0;


	Tick_Setting();
	Tick_ImGui();


	// End ImGui Frame
	ImGui::EndFrame();
}

void CImGui_Manager::Render()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	Render_Cell();
	
	/*
	if (nullptr != m_pTestModel)
		dynamic_pointer_cast<CPlayer>(m_pTestModel)->Render();*/

}

void CImGui_Manager::Tick_Setting()
{
	// 포지션 값 보정
	{
		if (m_fTerrainPickingPos[1] < 0.1f)
			m_fTerrainPickingPos[1] = 0.f;
	}
}

void CImGui_Manager::Tick_ImGui()
{
	m_bCursorOnImgui = false;

	if (ImGui::Begin("Map", NULL, /*ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | */ImGuiWindowFlags_NoTitleBar))
	{
		if (ImGui::BeginTabBar("tabs"))
		{
			if (ImGui::BeginTabItem("Map"))
			{
				Tick_MapTool();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Object"))
			{
				Tick_ObjectTool();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Effect"))
			{
				Tick_EffectTool();
				ImGui::EndTabItem();
			}
		}ImGui::EndTabBar();


		if (ImGui::IsWindowHovered())
			m_bCursorOnImgui = true;

	}ImGui::End();

	if (ImGui::Begin("Info", NULL, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar))
	{
		ImGuiIO& io = ImGui::GetIO();

		ImGui::Text("Mouse Pos  : (%g, %g)", io.MousePos.x, io.MousePos.y);

		ImGui::Text("Camera Pos : (%.2f, %.2f, %.2f)", m_pGameInstance->Get_CamPosition().x, m_pGameInstance->Get_CamPosition().y, m_pGameInstance->Get_CamPosition().z);
		ImGui::Checkbox("Camera Lock", &m_bCameraLock);

	}ImGui::End();
}

void CImGui_Manager::Tick_MapTool()
{
	ImGui::SeparatorText("Create");
	
	//Tick_Terrain();
	Tick_MeshMap();	

	Tick_NaviMesh();

	ImGui::Text("");
	ImGui::Text("Current Map");
	ImGui::Text(m_strSelectedMap.c_str());

	ImGui::SeparatorText("Navigation");
	ImGui::Text("E : Attach to the Nearest Point");
	ImGui::Text("R : Delete Last Cell");
	ImGui::Text("T : Delete Last Point");

	if (ImGui::Button("LOAD NAVI")) {
		ParseInNavi();
	}

	if (ImGui::Button("RESET NAVI")) {
		m_CellVec.clear();
		m_NaviVec.clear();
		m_iPickOrder = 0;
	}

	ImGui::Text("Number of Cells : ");
	ImGui::SameLine();
	ImGui::Text("%d", m_NaviVec.size());
	ImGui::Text("Last Point : (%.2f, %.2f, %.2f)", m_vPickingPos.x, m_vPickingPos.y, m_vPickingPos.z);


	if(ImGui::Button("SAVE NAVI")) {
		ParseOutNavi();
	}


	//ImGui::Text("Picking pos  : (%g, %g, %g)", m_fTerrainPickingPos[0], m_fTerrainPickingPos[1], m_fTerrainPickingPos[2]);
}

void CImGui_Manager::Tick_Terrain()
{
	static ImU16 TerrainValueX = 10, TerrainValueZ = 10;
	static ImU16 ValueStep = 1;

	ImGui::InputScalar("Size X : ", ImGuiDataType_U8, &TerrainValueX, &ValueStep);
	if (TerrainValueX <= 0)
		TerrainValueX = 1;
	else if (TerrainValueX >= 16)
		TerrainValueX = 15;

	static ImU16 TerrainCX;
	TerrainCX = (ImU16)pow(2, TerrainValueX);

	ImGui::SameLine();
	ImGui::Text("%d", TerrainCX);

	ImGui::InputScalar("Size Z : ", ImGuiDataType_U8, &TerrainValueZ, &ValueStep);
	if (TerrainValueZ <= 0)
		TerrainValueZ = 1;
	else if (TerrainValueZ >= 16)
		TerrainValueZ = 15;

	static ImU16 TerrainCZ;
	TerrainCZ = (ImU16)pow(2, TerrainValueZ);

	ImGui::SameLine();
	ImGui::Text("%d", TerrainCZ);

	if (ImGui::Button("CREATE MAP"))
	{
		m_iTerrainSize[0] = TerrainCX;
		m_iTerrainSize[1] = TerrainCZ;

		//CreateTerrain();
		CreateMeshMap("Prototype_Component_Model_Map_Boss1");
	}
}

void CImGui_Manager::Tick_MeshMap()
{
	static int item_current_idx = 0; // Here we store our selection data as an index.

	if (ImGui::BeginListBox("##listbox 1", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (int n = 0; n < m_strMap.size(); n++)
		{
			const bool is_selected = (item_current_idx == n);

			if (ImGui::Selectable(m_strMap[n].c_str(), is_selected))
				item_current_idx = n;

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	if (ImGui::Button("LOAD MAP"))
	{
		//CreateTerrain();
		CreateMeshMap(m_strMap[item_current_idx].c_str());
	}
}

void CImGui_Manager::Tick_NaviMesh()
{
	static int item_current_idx = 0; // Here we store our selection data as an index.

	if (ImGui::BeginListBox("##listbox 1", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (int n = 0; n < m_strMap.size(); n++)
		{
			const bool is_selected = (item_current_idx == n);

			if (ImGui::Selectable(m_strMap[n].c_str(), is_selected))
				item_current_idx = n;

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	if (ImGui::Button("LOAD MAP"))
	{
		//CreateTerrain();
		CreateMeshMap(m_strMap[item_current_idx].c_str());
	}
}

void CImGui_Manager::Tick_ObjectTool()
{
	//static int item_current_idx = 0; // Here we store our selection data as an index.

	if (ImGui::BeginListBox("##listbox 1", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (int n = 0; n < m_strStaticModel.size(); n++)
		{
			const bool is_selected = (m_iCreateObjectIndex == n);
			if (ImGui::Selectable(m_strStaticModel[n].c_str(), is_selected))
				m_iCreateObjectIndex = n;

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	if (ImGui::Button("Create Object"))
	{
		//CreateTerrain();
		m_bSelectModel = true;
		CreateStaticModel(m_strStaticModel[m_iCreateObjectIndex].c_str());
		m_iSelectedModel = m_pStaticModels.size() - 1;
	}

	ImGui::SeparatorText("Loaded Object");

	if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (int n = 0; n < m_pStaticModels.size(); n++)
		{
			const wstring strSelectedW = m_pStaticModels[n]->Get_ObjectTag();
			string strSelected = string().assign(strSelectedW.begin(), strSelectedW.end());

			// 사용자 정의 ID를 만들기 위해 문자열을 기반으로 하는 고유한 ID 생성
			ImGui::PushID(n);

			const bool is_selected = (m_iSelectedModel == n);

			if (ImGui::Selectable(strSelected.c_str(), is_selected, 0, ImVec2(0, 0)))
			{
				m_iSelectedModel = n;
				// 항목이 선택되면 스크롤을 마지막으로 이동
				ImGui::SetScrollHereY();
			}

			ImGui::PopID();

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	if (ImGui::Button("Delete Object"))
	{
		m_bDeleteClicked = true;
	}

	ImGui::Text("");
	if (ImGui::RadioButton("Translate", m_CurrentGizmoOperation == ImGuizmo::TRANSLATE))
		m_CurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", m_CurrentGizmoOperation == ImGuizmo::ROTATE))
		m_CurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", m_CurrentGizmoOperation == ImGuizmo::SCALE))
		m_CurrentGizmoOperation = ImGuizmo::SCALE;

	if (m_pStaticModels.size() > 0)
		EditTransformUsingImGuizmo(m_pStaticModels[m_iSelectedModel]);

	if (m_bDeleteClicked)
		DeleteStaticModel();

	m_bDeleteClicked = false;
}

void CImGui_Manager::Tick_EffectTool()
{
//	ImGui::Begin("EffectTool Ver.0.0", nullptr, ImGuiWindowFlags_AlwaysAutoResize);                        // Create a window called "Hello, world!" and append into it.
//	if (ImGui::Button("Particle", ImVec2(80.f, 25.f)))
//		m_Tab = 0;
//	ImGui::SameLine(0, 10.f);
//	if (ImGui::Button("MeshEffect", ImVec2(80.f, 25.f)))
//		m_Tab = 1;
//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
//
//	//Particle Tool
//	if (m_Tab == 0) {
//
//		if (m_Texture != nullptr) {
//			ImGui::ImageButton(m_Texture->m_Textures[m_ParticleDesc.ImgIndex], ImVec2(50, 50));
//			_float Color[4] = { m_ParticleDesc.Color1.x, m_ParticleDesc.Color1.y, m_ParticleDesc.Color1.z, m_ParticleDesc.Color1.w };
//			_float Color2[4] = { m_ParticleDesc.Color2.x, m_ParticleDesc.Color2.y, m_ParticleDesc.Color2.z, m_ParticleDesc.Color2.w };
//
//			ImGui::ColorEdit4("ColorEdit", Color, 0);
//			ImGui::ColorEdit4("Color2Edit", Color2, 0);
//			ImGui::SliderFloat("RemoveAlpha", &m_ParticleDesc.RemoveAlpha, 0, 1.f);
//
//			m_ParticleDesc.Color1 = _float4(Color[0], Color[1], Color[2], Color[3]);
//			m_ParticleDesc.Color2 = _float4(Color2[0], Color2[1], Color2[2], Color2[3]);
//			ImGui::SliderInt("Image Index", &m_ParticleDesc.ImgIndex, 0, m_Texture->m_Textures.size() - 1);
//			ImGui::SliderInt("ShaderPass", &m_ParticleDesc.ShaderPass, 0, 10);
//			ImGui::Checkbox("Set_ParticleDir", &m_ParticleDesc.isSetParticleDir);
//		}
//		ImGui::PushStyleColor(ImGuiCol_Border, ImColor(100, 100, 100, 255).Value);
//
//		ImGui::BeginChild("State", ImVec2(300, 90), true);
//		{
//			ImGui::SliderFloat("Duration", &m_ParticleDesc.Duration, 0.f, 10.f);
//			ImGui::Checkbox("Repeat", &m_ParticleDesc.isRepeat);
//			ImGui::SameLine(0, 10.f);
//			ImGui::Checkbox("ifArriveRemove", &m_ParticleDesc.ifArriveRemove);
//			ImGui::Text("Time : %.1f", m_AccTime);
//		}
//		ImGui::EndChild();
//
//		if (ImGui::Button("Start", ImVec2(50, 20))) {
//			m_Restart = true;
//			m_isStart = true;
//			m_isEnd = false;
//			m_AccTime = 0.0;
//		}
//
//		ImGui::SameLine(0, 5.f);
//
//		if (ImGui::Button("ReSet", ImVec2(50, 20))) {
//			ZeroMemory(&m_ParticleDesc, sizeof(PARTICLEDESC));
//			m_ParticleDesc.Color1 = _float4(1.f, 1.f, 1.f, 1.f);
//			m_ParticleDesc.Color2 = _float4(0.f, 0.f, 0.f, 1.f);
//			m_ParticleDesc.NumInstance = 1;
//			m_ParticleDesc.AxisRotation = _float3(0.f, 0.f, 0.f);
//			m_ParticleDesc.Scale = _float3(1.f, 1.f, 1.f);
//			m_ParticleDesc.Translation = _float4(0.f, 0.f, 0.f, 1.f);
//			m_ParticleDesc.Duration = 10.f;
//			m_ParticleDesc.RemoveAlpha = 0.f;
//			ZeroMemory(&m_RandParicle, sizeof(PARTICLERAND));
//			m_Restart = true;
//			m_isStart = false;
//			m_isEnd = false;
//			m_AccTime = 0.0;
//			m_TotalScale = _float3(1.f, 1.f, 1.f);
//		}
//
//		_float TotalScale[3] = { m_TotalScale.x, m_TotalScale.y, m_TotalScale.z };
//		ImGui::SliderFloat3("TotalScale", TotalScale, 0.01f, 10.f);
//
//		if (TotalScale[0] <= 0.f)
//			TotalScale[0] = 0.01f;
//		if (TotalScale[1] <= 0.f)
//			TotalScale[1] = 0.01f;
//		if (TotalScale[2] <= 0.f)
//			TotalScale[2] = 0.01f;
//
//		m_TotalScale.x = TotalScale[0];
//		m_TotalScale.y = TotalScale[1];
//		m_TotalScale.z = TotalScale[2];
//
//		ImGui::BeginChild("Basic State", ImVec2(300, 210), true);
//		ImGui::Text("Basic State");
//		ImGui::SliderInt("Num Instance", &m_ParticleDesc.NumInstance, 1, 500);
//		ImGui::SliderFloat("Speed", &m_ParticleDesc.Speed, 0.f, 1.f);
//
//		_float fScale[3] = { m_ParticleDesc.Scale.x,  m_ParticleDesc.Scale.y, m_ParticleDesc.Scale.z };
//		ImGui::SliderFloat3("Scale", fScale, 0.001f, 10.f);
//		m_ParticleDesc.Scale = _float3(fScale[0], fScale[1], fScale[2]);
//		_float fTrans[3] = { m_ParticleDesc.Translation.x,  m_ParticleDesc.Translation.y, m_ParticleDesc.Translation.z };
//		ImGui::SliderFloat3("Translation", fTrans, -20.f, 20.f);
//		m_ParticleDesc.Translation = _float4(fTrans[0], fTrans[1], fTrans[2], 1.f);
//
//		ImGui::Checkbox("Set_Dir", &m_ParticleDesc.isSetDir);
//		if (m_ParticleDesc.isSetDir == true) {
//
//			_float fSetDir[3] = { m_ParticleDesc.Direction.x,  m_ParticleDesc.Direction.y, m_ParticleDesc.Direction.z };
//			ImGui::SliderFloat3("Set_Dir", fSetDir, -50.f, 50.f);
//			m_ParticleDesc.Direction = _float4(fSetDir[0], fSetDir[1], fSetDir[2], 0.f);
//
//		}
//		else {
//			_float fDir[3] = { m_ParticleDesc.Direction.x,  m_ParticleDesc.Direction.y, m_ParticleDesc.Direction.z };
//			ImGui::SliderFloat3("Direction", fDir, -1.f, 1.f);
//			m_ParticleDesc.Direction = _float4((_float)fDir[0], (_float)fDir[1], (_float)fDir[2], 0.f);
//		}
//		_float fAngle[3] = { m_ParticleDesc.AxisRotation.x,  m_ParticleDesc.AxisRotation.y, m_ParticleDesc.AxisRotation.z };
//		ImGui::SliderFloat3("RotateAxis", fAngle, -1.f, 1.f);
//		m_ParticleDesc.AxisRotation = _float3((_float)fAngle[0], (_float)fAngle[1], (_float)fAngle[2]);
//
//		ImGui::SliderFloat("RotateSpeed", &m_ParticleDesc.RotateSpeed, 0.f, 20.f);
//
//		ImGui::EndChild();
//
//		//랜덤값 주는곳
//		ImGui::BeginChild("Random_Range", ImVec2(300, 180), true);
//		ImGui::Text("Particle Range");
//		_float fRandSpeed[2] = { m_RandParicle.Speed.x, m_RandParicle.Speed.y };
//		{
//			ImGui::SliderFloat2("RandSpeed", fRandSpeed, 0.f, 1.f);
//			m_RandParicle.Speed = _float2(fRandSpeed[0], fRandSpeed[1]);
//
//			_float fRandScale[2] = { m_RandParicle.Scale.x, m_RandParicle.Scale.y };
//			ImGui::SliderFloat2("RandScale", fRandScale, 0.f, 10.f);
//			m_RandParicle.Scale = _float2(fRandScale[0], fRandScale[1]);
//
//			_float fRandDir[3] = { m_RandParicle.Direction.x,  m_RandParicle.Direction.y, m_RandParicle.Direction.z };
//			ImGui::SliderFloat3("RandDirection", fRandDir, 0.f, 1.f);
//			m_RandParicle.Direction = _float3(fRandDir[0], fRandDir[1], fRandDir[2]);
//			_float StartTrans[3] = { m_RandParicle.Translation.x,  m_RandParicle.Translation.y, m_RandParicle.Translation.z };
//			ImGui::SliderFloat3("Translation", StartTrans, 0.f, 20.f);
//			m_RandParicle.Translation = _float3(StartTrans[0], StartTrans[1], StartTrans[2]);
//			_float AxisRot[3] = { m_RandParicle.RandAxisRot.x,  m_RandParicle.RandAxisRot.y, m_RandParicle.RandAxisRot.z };
//			ImGui::SliderFloat3("Rand_Rot", AxisRot, 0.f, 1.f);
//			m_RandParicle.RandAxisRot = _float3(AxisRot[0], AxisRot[1], AxisRot[2]);
//
//			_float RotateSpeed[2] = { m_RandParicle.RotateSpeed.x, m_RandParicle.RotateSpeed.y };
//			ImGui::SliderFloat2("Rand_RotSpeed", RotateSpeed, 0.f, 5.f);
//			m_RandParicle.RotateSpeed = _float2(RotateSpeed[0], RotateSpeed[1]);
//		}
//		ImGui::EndChild();
//		ImGui::PopStyleColor();
//
//		if (ImGui::TreeNode("DummyModel")) {
//			ImGui::Checkbox("Look On/Off", &m_isDummy);
//			ImGui::SliderInt("Index", &m_DummyCnt, 0, m_DummyMaxCnt);
//
//			ImGui::TreePop();
//		}
//
//		if (ImGui::TreeNode("Save/Load")) {
//			ImGui::InputText("Tag", m_DataTag, MAX_PATH);
//			if (ImGui::Button("Save", ImVec2(50, 30)))
//				Save();
//
//			ImGui::SameLine(0, 10.f);
//			if (ImGui::Button("Load", ImVec2(50, 30)))
//				Load();
//
//			ImGui::TreePop();
//		}
//	}
//	//MeshEffect
//
//	if (m_Tab == 1) {
//		if (ImGui::TreeNode("Mesh")) {
//			DynamicListBox(".", &m_MeshTag, &m_MeshCnt);
//			ImGui::TreePop();
//		}
//
//#pragma region SettingButton
//		if (ImGui::Button("Start", ImVec2(50.f, 25.f)) || (GetAsyncKeyState(VK_END) & 0x0001)) {
//			m_MeshEffDesc.isStart = true;
//			Start_MeshEffect();
//		}
//		ImGui::SameLine(0, 10.f);
//		if (ImGui::Button("Stop", ImVec2(50.f, 25.f))) {
//			m_MeshEffDesc.isStart = m_MeshEffDesc.isStart == true ? false : true;
//		}
//		ImGui::SameLine(0, 10.f);
//		if (ImGui::Button("Reset", ImVec2(50.f, 25.f))) {
//			Reset_MeshEffect();
//		}
//#pragma endregion SettingButton
//
//		ImGui::Checkbox("isDuration", &m_MeshEffDesc.isDuration);
//		if (m_MeshEffDesc.isDuration == true) {
//			ImGui::SliderFloat("Duration", &m_MeshEffDesc.Duration, 0.f, 10.f);
//			ImGui::SliderFloat("Time", &m_MeshEffDesc.AccTime, 0.f, m_MeshEffDesc.Duration);
//		}
//
//		if (ImGui::TreeNode("Default")) {
//			ImGui::SliderInt("Shader", &m_MeshEffDesc.Shader, 0, 10);
//			ImGui::SliderFloat("RemoveAlpha", &m_MeshEffDesc.RemoveAlpha, 0.f, 1.f);
//
//			_float Color1[3] = { m_MeshEffDesc.Color1.x, m_MeshEffDesc.Color1.y, m_MeshEffDesc.Color1.z };
//			ImGui::ColorEdit3("Color1", Color1);
//			m_MeshEffDesc.Color1 = _float3(Color1[0], Color1[1], Color1[2]);
//
//			_float Color2[3] = { m_MeshEffDesc.Color2.x, m_MeshEffDesc.Color2.y, m_MeshEffDesc.Color2.z };
//			ImGui::ColorEdit3("Color2", Color2);
//			m_MeshEffDesc.Color2 = _float3(Color2[0], Color2[1], Color2[2]);
//
//			ImGui::TreePop();
//		}
//		if (ImGui::TreeNode("Alpha")) {
//			ImGui::SliderFloat("Alpha", &m_MeshEffDesc.Alpha, 0.f, 1.f);
//
//			ImGui::Checkbox("isStartAlpha", &m_MeshEffDesc.StartAlpha.isOn);
//			if (m_MeshEffDesc.StartAlpha.isOn == true) {
//				ImGui::SliderFloat("StartSpeed", &m_MeshEffDesc.StartAlpha.Speed, 0.f, 10.f);
//			}
//			ImGui::Checkbox("isEndAlpha", &m_MeshEffDesc.EndAlpha.isOn);
//			if (m_MeshEffDesc.EndAlpha.isOn == true) {
//				ImGui::SliderFloat("EndSpeed", &m_MeshEffDesc.EndAlpha.Speed, 0.f, 10.f);
//			}
//
//			ImGui::TreePop();
//		}
//		if (ImGui::TreeNode("Scale")) {
//			float Scale[3] = { m_MeshEffDesc.Scale.x, m_MeshEffDesc.Scale.y, m_MeshEffDesc.Scale.z };
//			ImGui::SliderFloat3("Scale", Scale, 0.001f, 10.f);
//			if (Scale[0] > 0.f)
//				m_MeshEffDesc.Scale.x = Scale[0];
//			if (Scale[1] > 0.f)
//				m_MeshEffDesc.Scale.y = Scale[1];
//			if (Scale[2] > 0.f)
//				m_MeshEffDesc.Scale.z = Scale[2];
//
//			float ScaleSpeed[3] = { m_MeshEffDesc.ScaleSpeed.x, m_MeshEffDesc.ScaleSpeed.y, m_MeshEffDesc.ScaleSpeed.z };
//			ImGui::SliderFloat3("ScaleSpeed", ScaleSpeed, 0.f, 10.f);
//			m_MeshEffDesc.ScaleSpeed = _float3(ScaleSpeed[0], ScaleSpeed[1], ScaleSpeed[2]);
//			ImGui::TreePop();
//		}
//		if (ImGui::TreeNode("Rotation")) {
//
//			ImGui::TreePop();
//		}
//
//	}
}

void CImGui_Manager::Render_Cell()
{
	_float4x4		 ViewMatrix, ProjMatrix, WorldMatrix;

	ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_VIEW);
	ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_PROJ);
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return;

	_float4		vColor = _float4(0.f, 1.f, 0.f, 1.f);

	if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return;

	m_pShader->Begin(0);

	for (auto& iter : m_CellVec)
		iter->Render();
}

void CImGui_Manager::KeyInput()
{
	// 카메라 잠금
	{
		if (!m_bPressed && GetKeyState('Q') < 0)
		{
			m_bPressed = true;
			m_bCameraLock = !m_bCameraLock;
		}
		else if (GetKeyState('Q') >= 0 && m_bPressed)
		{
			m_bPressed = false;
		}
	}

	// 네비 피킹
	{
		// 키 입력받은 후 피킹하면 피킹 포지션 보정
		if (m_pGameInstance->Is_KeyDown(DIK_E))
		{
			m_isAttachNearPoint = true;
		}

		// 키 입력받으면 이전 셀 삭제하기
		if (m_pGameInstance->Is_KeyDown(DIK_R))
		{
			if (0 != m_NaviVec.size())
			{
				m_NaviVec.pop_back();
				m_CellVec.pop_back();
				m_iPickOrder = 0;
			}
		}

		// 키 입력받으면 이전 포인트 삭제하기
		if (m_pGameInstance->Is_KeyDown(DIK_T))
		{
			if (0 == m_iPickOrder)
			{
				if (0 != m_NaviVec.size())
				{
					m_NaviVec.pop_back();
					m_CellVec.pop_back();
					m_iPickOrder = 2;
				}
			}

			else
			{
				m_iPickOrder--;
			}
		}

		if (m_pGameInstance->Is_KeyDown(DIK_DELETE))
		{
			m_isPickingMode = false;
		}
	}
}

HRESULT CImGui_Manager::CreateTexture(_uint& iWidth, _uint& iHeigth)
{
	// 텍스처를 생성하고 디스크에 저장하는 과정
	// Grahphic_Device에서 DepthStencilRenderTargetView를 만드는 과정과 비슷

	// 텍스처를 가리킬 변수
	ID3D11Texture2D* pTexture2D = { nullptr };

	// 텍스처를 생성할 때 필요한 설정들을 담을 구조체
	D3D11_TEXTURE2D_DESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeigth;
	// 마스크 텍스처는 그리기용이 아니기 때문에 mipmap 필요 없음
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 텍스처 만들 때 r, g, b, a 각각 8비트씩 부여(원하는대로)

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	//CPU에서 동적으로 업데이트 가능하도록 (텍스처 데이터 변경 가능)
	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	// 텍스처를 바인딩할 때 특정 용도 설정 (셰이더 리소스용)
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	// CPU에서 텍스처에 쓰기 권한 필요 (텍스처를 업데이트하려면 CPU에서 해당 텍스처의 메모리에 쓰기 작업을 수행할 수 있다.)
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

#pragma region 텍스처 초기값
	_ulong* pPixel = new _ulong[TextureDesc.Width * TextureDesc.Height];

	for (size_t i = 0; i < TextureDesc.Height; ++i)
	{
		for (size_t j = 0; j < TextureDesc.Width; ++j)
		{
			_uint		iIndex = i * TextureDesc.Width + j;

			pPixel[iIndex] = D3DCOLOR_ARGB(255, 0, 0, 0);
		}
	}

	// 텍스처의 초기 데이터를 설정하는 구조체
	D3D11_SUBRESOURCE_DATA		InitialData = {};

	// 텍스처에 할당된 픽셀 데이터 배열을 지정
	InitialData.pSysMem = pPixel;
	// 이미지에 값을 채울 때는 가로 한 줄이 몇 바이트인지 알려줘야함
	InitialData.SysMemPitch = TextureDesc.Width * 4;

	// 텍스처 생성
	// (텍스처를 생성할 때 필요한 설정들을 담은 구조체, 초기 데이터, 생성된 텍스처를 저장할 포인터)
	// 두 번째 인자 D3D11_SUBRESOURCE_DATA : 어떤 공간을 사용할 때 최초 어떤 값으로 채워 넣을지
	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &InitialData, &pTexture2D)))
		return E_FAIL;

#pragma endregion

	// 생성된 텍스처를 DDS 파일 형식으로 디스크에 저장
	if (FAILED(DirectX::SaveWICTextureToFile(m_pContext.Get(), pTexture2D, GUID_ContainerFormatBmp, TEXT("../Bin/Resources/Textures/Terrain/HeightDefault.bmp"), nullptr, nullptr, false)))
		return E_FAIL;

	Safe_Delete_Array(pPixel);
	Safe_Release(pTexture2D);

	return S_OK;
}

HRESULT CImGui_Manager::CreateMeshMap(const string& strMapTag)
{
	m_strSelectedMap = strMapTag;

	string strModelComTag = "Prototype_Component_Model_" + strMapTag;

	if (FAILED(m_pGameInstance->Remove_Layer(LEVEL_MAPTOOL, LAYER_MAP)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MAPTOOL, LAYER_MAP, wstring().assign(strMapTag.begin(), strMapTag.end()),
		CStaticModel::Create(m_pDevice, m_pContext, wstring().assign(strModelComTag.begin(), strModelComTag.end()), true))))
		return E_FAIL;

	return S_OK;
}

// Create By Picking
HRESULT CImGui_Manager::CreateStaticModel(_float3 vPickingPos)
{
	if (m_bCursorOnImgui) return S_OK;

	string strModelComName = m_strStaticModel[m_iCreateObjectIndex].c_str();
	string strModelComTag = "Prototype_Component_Model_" + strModelComName;

	_float4 vPos = { vPickingPos.x, vPickingPos.y, vPickingPos.z, 1.f };

	shared_ptr<CGameObject> pGameObject = CStaticModel::Create(m_pDevice, m_pContext, wstring().assign(strModelComTag.begin(), strModelComTag.end()));
	pGameObject->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MAPTOOL, LAYER_ENVIRONMENT, wstring().assign(strModelComName.begin(), strModelComName.end()),
		pGameObject)))
		return E_FAIL;

	m_pStaticModels.push_back(pGameObject);

	m_iSelectedModel = m_pStaticModels.size() - 1;

	return S_OK;
}

// Create By Button
HRESULT CImGui_Manager::CreateStaticModel(const string& strStaticModelTag)
{
	string strModelComTag = "Prototype_Component_Model_" + strStaticModelTag;

	shared_ptr<CGameObject> pGameObject = CStaticModel::Create(m_pDevice, m_pContext, wstring().assign(strModelComTag.begin(), strModelComTag.end()));

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MAPTOOL, LAYER_ENVIRONMENT, wstring().assign(strStaticModelTag.begin(), strStaticModelTag.end()),
		pGameObject)))
		return E_FAIL;

	m_pStaticModels.push_back(pGameObject);

	return S_OK;
}

HRESULT CImGui_Manager::DeleteStaticModel()
{
	if (m_pStaticModels.empty() || m_iSelectedModel < 0 || m_iSelectedModel >= m_pStaticModels.size()) return E_FAIL;

	if (FAILED(m_pGameInstance->Remove_Object(LEVEL_MAPTOOL, LAYER_ENVIRONMENT, m_pStaticModels[m_iSelectedModel])))
		return E_FAIL;

	m_pStaticModels.erase(m_pStaticModels.begin() + m_iSelectedModel);
	if (m_iSelectedModel != 0)
		m_iSelectedModel--;

	return S_OK;
}

HRESULT CImGui_Manager::CreateTerrain()
{
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MAPTOOL, LAYER_BACKGROUND, TEXT("GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext, m_iTerrainSize))))
		return E_FAIL;

	return S_OK;
}

void CImGui_Manager::PickingTerrain(_float3 vPickingPos)
{
	SetTerrainPickingPos(vPickingPos);

	m_bSelectModel = true;

	if (false == m_bSelectModel)
	{
		return;
	}

	//m_strSelectModelTag = TEXT("Prototype_Component_Model_NMH");

	//// 모델 생성
	//_float4 vPos = { m_fTerrainPickingPos[0], 0.f, m_fTerrainPickingPos[2], 1.f };

	//shared_ptr<CStaticModel> pStaticModel = CStaticModel::Create(m_pDevice, m_pContext, L"Prototype_Component_Model_NMH");
	//pStaticModel->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));

	//if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MAPTOOL, LAYER_ENVIRONMENT, m_strSelectModelTag,
	//	CStaticModel::Create(m_pDevice, m_pContext, L"Prototype_Component_Model_NMH"))))
	//	return;
}

void CImGui_Manager::PickingModel(const _char* strSelectModelTag)
{

}

void CImGui_Manager::EditTransformUsingImGuizmo(shared_ptr<CGameObject> pGameObject)
{
	_float4x4	CameraView = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_VIEW);
	_float4x4	CameraProj = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_PROJ);

	_float		ViewMatrix[16];
	_float		ProjMatrix[16];

	_float4x4	ObjectWorld;
	XMStoreFloat4x4(&ObjectWorld, pGameObject->Get_TransformCom()->Get_WorldMatrix());
	_float		WorldMatrix[16];

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			ViewMatrix[j + i * 4] = CameraView.m[i][j];
			ProjMatrix[j + i * 4] = CameraProj.m[i][j];
			WorldMatrix[j + i * 4] = ObjectWorld.m[i][j];
		}
	}

	/*ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();
	_float windowWidth = (float)ImGui::GetWindowWidth();
	_float windowHeight = (float)ImGui::GetWindowHeight();
	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);*/

	auto p = ImGui::GetItemRectMin();
	auto s = ImGui::GetItemRectSize();
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
	ImGuizmo::BeginFrame();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	ImGuizmo::Manipulate(
		ViewMatrix,
		ProjMatrix,
		m_CurrentGizmoOperation,
		ImGuizmo::WORLD,
		WorldMatrix);
	
	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(WorldMatrix, matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Tr", matrixTranslation);
	ImGui::InputFloat3("Rt", matrixRotation);
	ImGui::InputFloat3("Sc", matrixScale);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, WorldMatrix);

	XMMATRIX matrix = XMMATRIX(
		WorldMatrix[0], WorldMatrix[1], WorldMatrix[2], WorldMatrix[3],
		WorldMatrix[4], WorldMatrix[5], WorldMatrix[6], WorldMatrix[7],
		WorldMatrix[8], WorldMatrix[9], WorldMatrix[10], WorldMatrix[11],
		WorldMatrix[12], WorldMatrix[13], WorldMatrix[14], WorldMatrix[15]
	);

	_float4x4 worldMat;
	XMStoreFloat4x4(&worldMat, matrix);

	pGameObject->Get_TransformCom()->Set_WorldMatrix(worldMat);
}

void CImGui_Manager::PickingMap(_float3 vPickingPos)
{
	if (m_bCursorOnImgui)
		return;

	// 어떠한 키 하나를 입력받으면 점 위치 보정
	// vNaviVec의 크기가 0이 아닐 때,
	// 피킹한 점과 가장 가까운 vNaviVec의 한 점으로 점 위치 변경하기
	_float3		vNearest = { -100000.f, -100000.f, -100000.f };
	_vector		vPicking = XMLoadFloat3(&vPickingPos);
	_vector		vCalcPoint;

	m_vPickingPos = vPickingPos;

	_float2 vPos = { m_vPickingPos.x, m_vPickingPos.z };

	_float2 v0 = {};
	_float2 v1 = {};
	_float2 v2 = {};

	_float fDist;

	if (!m_isPickingMode)
	{
		// 셀 지우기
		for (auto iter = m_NaviVec.begin(); iter != m_NaviVec.end(); )
		{
			v0 = { iter->pointA.x, iter->pointA.z };
			v1 = { iter->pointB.x, iter->pointB.z };
			v2 = { iter->pointC.x, iter->pointC.z };

			float areaABC = abs((v0.x * (v1.y - v2.y) + v1.x * (v2.y - v0.y) + v2.x * (v0.y - v1.y)) / 2.0f);

			// 점과 세 꼭지점을 이용하여 세 삼각형의 면적 계산
			float areaPBC = abs((vPos.x * (v1.y - v2.y) + v1.x * (v2.y - vPos.y) + v2.x * (vPos.y - v1.y)) / 2.0f);
			float areaPCA = abs((v0.x * (vPos.y - v2.y) + vPos.x * (v2.y - v0.y) + v2.x * (v0.y - vPos.y)) / 2.0f);
			float areaPAB = abs((v0.x * (v1.y - vPos.y) + v1.x * (vPos.y - v0.y) + vPos.x * (v0.y - v1.y)) / 2.0f);

			_float fArea = areaABC - (areaPBC + areaPCA + areaPAB);

			std::wstring outputString = L"면적 : " + std::to_wstring(fArea) + L"\n";
			OutputDebugStringW(outputString.c_str());

			// 세 삼각형의 면적의 합이 삼각형의 면적과 같으면 점이 삼각형 내부에 있는 것
			if (0.01 >= fArea && -0.01 <= fArea)
			{
				m_NaviVec.erase(iter);

				m_isPickingMode = true;
				return;
			}

			iter++;
		}

		m_isPickingMode = true;
		return;
	}

	if (m_isAttachNearPoint && m_NaviVec.size() != 0)
	{
		for (auto& iter : m_NaviVec)
		{
			vCalcPoint = XMLoadFloat3(&iter.pointA);

			if (XMVectorGetX(XMVector3Length(XMVectorSubtract(XMLoadFloat3(&vNearest), vPicking)))
				> XMVectorGetX(XMVector3Length(XMVectorSubtract(vCalcPoint, vPicking))))
				vNearest = iter.pointA;

			vCalcPoint = XMLoadFloat3(&iter.pointB);

			if (XMVectorGetX(XMVector3Length(XMVectorSubtract(XMLoadFloat3(&vNearest), vPicking)))
				> XMVectorGetX(XMVector3Length(XMVectorSubtract(vCalcPoint, vPicking))))
				vNearest = iter.pointB;

			vCalcPoint = XMLoadFloat3(&iter.pointC);

			if (XMVectorGetX(XMVector3Length(XMVectorSubtract(XMLoadFloat3(&vNearest), vPicking)))
				> XMVectorGetX(XMVector3Length(XMVectorSubtract(vCalcPoint, vPicking))))
				vNearest = iter.pointC;
		}

		m_vCellPos[m_iPickOrder++] = vNearest;
	}

	else
		m_vCellPos[m_iPickOrder++] = vPickingPos;


	m_isAttachNearPoint = false;

	// 삼각형 채우면 셀 저장
	if (3 <= m_iPickOrder)
	{
		// 점 저장
		SaveCell(m_vCellPos);
		m_iPickOrder = 0;
	}
}

void CImGui_Manager::SaveCell(_float3* saveCellPos)
{
	// 점 세개를 시계방향으로 정렬
	CELLPOS alignedCellPos;

	alignedCellPos.pointA = saveCellPos[0];
	alignedCellPos.pointB = saveCellPos[1];
	alignedCellPos.pointC = saveCellPos[2];

	// 여기까지 pointA가 가장 아래 왼쪽에 있는 점
	XMVECTOR vectorAB = XMVectorSet(alignedCellPos.pointB.x - alignedCellPos.pointA.x, 0.0f, alignedCellPos.pointB.z - alignedCellPos.pointA.z, 0.0f);
	XMVECTOR vectorAC = XMVectorSet(alignedCellPos.pointC.x - alignedCellPos.pointA.x, 0.0f, alignedCellPos.pointC.z - alignedCellPos.pointA.z, 0.0f);

	XMVECTOR cross = XMVector3Cross(vectorAB, vectorAC);

	if (XMVectorGetY(cross) < 0) {
		// 시계 방향으로 정렬되어 있지 않으면 순서를 바꿔줌
		std::swap(alignedCellPos.pointB, alignedCellPos.pointC);
	}

	saveCellPos[0] = alignedCellPos.pointA;
	saveCellPos[1] = alignedCellPos.pointB;
	saveCellPos[2] = alignedCellPos.pointC;

	m_NaviVec.push_back(alignedCellPos);

	saveCellPos[0].y += 0.1f;
	saveCellPos[1].y += 0.1f;
	saveCellPos[2].y += 0.1f;

	shared_ptr<CCell> pCell = CCell::Create(m_pDevice, m_pContext, saveCellPos, 0);

	m_CellVec.push_back(pCell);
}

HRESULT CImGui_Manager::ParseInNavi()
{

	string		strFileName = "../../Resource/Resources/Navigations/";

	strFileName.append(m_strSelectedMap);

	strFileName.append(".navi");

	_ulong		dwByte = { 0 };
	HANDLE		hFile = CreateFile(wstring().assign(strFileName.begin(), strFileName.end()).c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	while (true)
	{
		_float3		vPoints[CCell::POINT_END];

		// _float3 형식의 'POINT_END' 갯수만큼의 데이터를 읽어와서 vPoints 배열에 저장
		ReadFile(hFile, vPoints, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);

		if (0 == dwByte)
			break;

		m_vCellPos[0] = vPoints[0];
		m_vCellPos[1] = vPoints[1];
		m_vCellPos[2] = vPoints[2];

		CELLPOS vCellPos;
		vCellPos.pointA = m_vCellPos[0];
		vCellPos.pointB = m_vCellPos[1];
		vCellPos.pointC = m_vCellPos[2];

		m_NaviVec.push_back(vCellPos);

		vPoints[0].y += 0.1f;
		vPoints[1].y += 0.1f;
		vPoints[2].y += 0.1f;

		shared_ptr<CCell> pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_CellVec.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_CellVec.push_back(pCell);
	}

	CloseHandle(hFile);

	if (!m_isPlayerCreated)
	{
		CLandObject::LANDOBJ_DESC			LandObjDesc{};

		LandObjDesc.pTerrainTransform = dynamic_pointer_cast<CTransform>(m_pGameInstance->Find_Component(LEVEL_MAPTOOL, LAYER_BACKGROUND, g_strTransformTag));
		LandObjDesc.pTerrainVIBuffer = dynamic_pointer_cast<CVIBuffer_HeightTerrain>(m_pGameInstance->Find_Component(LEVEL_MAPTOOL, LAYER_BACKGROUND, TEXT("Com_VIBuffer")));

		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, LAYER_PLAYER, TEXT("GameObject_Player"),
			CPlayer::Create(m_pDevice, m_pContext, &LandObjDesc))))
			return E_FAIL;

	}
	/* Com_Navigation */
	CNavigation::NAVI_DESC		NaviDesc{};

	NaviDesc.iStartCellIndex = 0;

	dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player())->Change_NaviCom();

	return S_OK;
}

HRESULT CImGui_Manager::ParseOutNavi()
{
	_ulong		dwByte = { 0 };

	string		strFileName = "../../Resource/Resources/Navigations/";

	strFileName.append(m_strSelectedMap);

	strFileName.append(".navi");

	HANDLE		hFile = CreateFile(wstring().assign(strFileName.begin(), strFileName.end()).c_str(),
									GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	for (auto& iter : m_NaviVec)
	{
		WriteFile(hFile, &iter.pointA, sizeof(_float3), &dwByte, nullptr);
		WriteFile(hFile, &iter.pointB, sizeof(_float3), &dwByte, nullptr);
		WriteFile(hFile, &iter.pointC, sizeof(_float3), &dwByte, nullptr);
	}

	CloseHandle(hFile);

	if (FAILED(m_pGameInstance->Remove_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Navigation"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Navigations/Map_VillageCut.Navi")))))
		return E_FAIL;

	return S_OK;
}

void CImGui_Manager::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
