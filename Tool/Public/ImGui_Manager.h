#pragma once

#include "Tool_Defines.h"
#include "Player.h"

BEGIN(Engine)
class Cell;
END

BEGIN(Tool)

class CImGui_Manager final
{
	DECLARE_SINGLETON_SMART(CImGui_Manager)

public:
	CImGui_Manager();
	~CImGui_Manager() = default;

public:
	HRESULT Initialize(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	void	Tick(_float fTimeDelta);
	void	Render();

private:
	_bool	m_bCursorOnImgui = false;

	int		m_iTerrainSize[2] = { 0, 0 };
	_float	m_fTerrainPickingPos[3] = { 0.f, 0.f, 0.f };

	_bool	m_bPressed = false;
	_bool	m_bCameraLock = true;

	_vector	m_vCameraAt = { 0.f, 0.f, 0.f, 0.f };
	_vector	m_vCameraEye = { 0.f, 0.f, 0.f, 0.f };

	_vector	m_vCameraInfo[2] = {};

private:
	string			m_strSelectedMap = "";
	vector<string>	m_strMap;
	vector<string>	m_strStaticModel;

private:
	_bool	m_bSelectModel = false;
	const	_tchar* m_strSelectModelTag = { nullptr };

	_bool	m_isPickingMode = true;

public:
	void	Read_FBXList();

	void	Tick_Setting();
	void	Tick_ImGui();

	/* MapTool */
	void	Tick_MapTool();
	void	Tick_Terrain();
	void	Tick_MeshMap();
	void	Tick_NaviMesh();

	void	Tick_ObjectTool();

	void	Tick_EffectTool();

	void	Render_Cell();

	void	KeyInput();

public:
	HRESULT		CreateTexture(_uint& iWidth, _uint& iHeigth);
	HRESULT		CreateMeshMap(const string& strMapTag);

	HRESULT		CreateStaticModel(_float3 vPickingPos);
	HRESULT		CreateStaticModel(const string& strStaticModelTag);
	HRESULT		DeleteStaticModel();

	HRESULT		CreateTerrain();
	_bool		IsCameraLock() { return m_bCameraLock; }
	void		PickingTerrain(_float3 vPickingPos);
	void		PickingModel(const _char* strSelectModelTag);
	void		SetTerrainPickingPos(_float3 vPickingPos)
	{
		m_fTerrainPickingPos[0] = vPickingPos.x;
		m_fTerrainPickingPos[1] = vPickingPos.y;
		m_fTerrainPickingPos[2] = vPickingPos.z;
	}

	void		EditTransformUsingImGuizmo(shared_ptr<CGameObject> pGameObject);


	// 네비 생성용
	void		PickingMap(_float3 vPickingPos);

	typedef struct {
		_float3 pointA;
		_float3 pointB;
		_float3 pointC;
	} CELLPOS;

	_bool		m_isAttachNearPoint = false;
	// 셀의 총 개수
	_uint		m_iCellCnt = 0;
	// 클릭한 셀의 순서
	_uint		m_iPickOrder = 0;
	// 피킹한 포지션
	_float3		m_vPickingPos = { 0.f, 0.f, 0.f };
	// 한 셀을 구성하는 포지션 3개
	_float3		m_vCellPos[3];
	// 피킹 포지션을 기록하는 벡터
	vector<CELLPOS>	m_NaviVec;
	// 셀 출력을 위해 Cell 타입으로 생성 후 render
	vector<shared_ptr<class CCell>> m_CellVec;

	void		SaveCell(_float3* saveCellPos);

	HRESULT		ParseInNavi();
	HRESULT		ParseOutNavi();

private:
	shared_ptr<class CShader>			m_pShader = { nullptr };

public:

private:
	shared_ptr<CGameObject> m_pTestModel = { nullptr };

private:
	_uint							m_iSelectedModel = { 0 };
	vector<shared_ptr<CGameObject>> m_pStaticModels = {};
	ImGuizmo::OPERATION m_CurrentGizmoOperation = ImGuizmo::TRANSLATE;
	_bool							m_bDeleteClicked = false;

	_uint							m_iCreateObjectIndex = { 0 };

	_bool							m_isPlayerCreated = false;


private:
	ComPtr<ID3D11Device>				m_pDevice = nullptr;
	ComPtr<ID3D11DeviceContext>			m_pContext = nullptr;
	shared_ptr<class CGameInstance>		m_pGameInstance = nullptr;


	// Effect Tool
public:


private:
	_uint m_Tab = 0;

//	//파티클
//	PARTICLEDESC			m_ParticleDesc;
//	PARTICLERAND			m_RandParicle;
//	_bool					m_Restart = false;
//	CTexture*				m_Texture = nullptr;
//	_double*				m_TotalTime = nullptr;
//	char m_DataTag[MAX_PATH] = ""; //파티클 Save/Load
//	vector<VTXMATRIX>* m_SaveData = nullptr;
//	_bool m_isStart = false;
//	_bool m_isEnd = false;
//	_double m_AccTime = 0.0;
//
//public: //메쉬 이펙트
//	vector<Item> m_MeshTag;
//	_int m_MeshCnt = 0;
//	MESHEFFECTDESC m_MeshEffDesc;

















public:
	void Free();
};

END