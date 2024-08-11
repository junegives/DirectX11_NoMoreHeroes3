#pragma once

#include "Component.h"
#include "File_Manager.h"
#include "BinaryChannel.h"

BEGIN(Engine)

class ENGINE_DLL CBinaryModel : public CComponent, public enable_shared_from_this<CBinaryModel>
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

public:
	CBinaryModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CBinaryModel(const CBinaryModel& rhs);
	virtual ~CBinaryModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	// 본의 이름으로 인덱스를 반환하는 함수
	_int Get_BoneIndex(const string& pBoneName) const;

	_float4x4* Get_CombinedBoneMatrixPtr(const string& pBoneName);

	void Set_Animation(_uint iAnimIndex) {
		m_iCurrentAnimation = iAnimIndex;
	}

	_uint Get_PrevAnimation() {
		if (m_iCurrentAnimation <= 0)
			return m_iNumAnimations - 1;
		else
			return m_iCurrentAnimation - 1;
	}

	_uint Get_NextAnimation() {
		return m_iCurrentAnimation + 1 >= m_iNumAnimations ? 0 : m_iCurrentAnimation + 1;
	}

	void Set_PrevAnimation() {
		if (m_iCurrentAnimation <= 0)
			m_iCurrentAnimation = m_iNumAnimations - 1;
		else
			m_iCurrentAnimation--;
	}

	void Set_NextAnimation() {
		m_iCurrentAnimation++;
		if (m_iCurrentAnimation >= m_iNumAnimations)
			m_iCurrentAnimation = 0;
	}

	_bool isAnimStop() { return m_isAnimStop; }

	void SetBlockRootMotion(_bool isBlockRootMotion) { m_isBlockRootMotion = isBlockRootMotion; }

public:
	// (모델 타입(움직or안움직), 모델 파일 경로, 모델 초기 변환)
	HRESULT Initialize_Prototype(TYPE eModelType, const _char* pModelFilePath, _fmatrix PivotMatrix);
	HRESULT Initialize(void* pArg);
	HRESULT Render(_uint iMeshIndex);

public:
	HRESULT Bind_Material_ShaderResource(shared_ptr<class CShader> pShader, _uint iMeshIndex, aiTextureType eMaterialType, const _char* pConstantName);
	HRESULT Bind_BoneMatrices(shared_ptr<class CShader> pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT Bind_PrevBoneMatrices(shared_ptr<class CShader> pShader, const _char* pConstantName, _float4x4* BoneMatrices);
	void	Play_Animation(_float fTimeDelta, _Out_ _float4x4* pRootTransform, _bool isLoop = false);
	void	Change_Animation(_uint iNextAnimation);
	_bool	Change_Animation_Lerp(_float fTimeDelta, _double Time, _Out_ _float4x4* pRootTransform);

public:
	// 다음 애니메이션을 설정하면서 선형 보간을 위한 세팅까지 해놓는 함수
	HRESULT Set_Animation(_uint iAnimIndex, _bool isLoop, _float fAnimSpeed, _float fChangeDuration, _uint iStartNumKeyFrames = 0);
	// 선형보간을 하면서 애니메이션 진행도에 따라 뼈를 움직이는 함수
	void	Play_Animation(_float fTimeDelta, shared_ptr<class CTransform> pTransformCom);
	// 선형보간을 하면서 다음 애니메이션으로 전환하는 함수
	HRESULT Change_Animation(_float fTimeDelta, _float fChangeDuration);
	// 루트 모션으로 위치를 보정해주는 함수
	void	Apply_RootMotion(_float fTimeDelta, shared_ptr<class CTransform> pTransformCom, shared_ptr<class CNavigation> pNavigation, _bool isYChange = false);

public:
	// 키프레임 관련 함수들
	_float	Get_KeyFrameRatio(_float fTrackPos);
	_float	Get_CurKeyFrameRatio();
	_float	Get_CurKeyFrameTime();
	HRESULT	Copy_BoneMatrix(_uint iMeshIndex, _float4x4* pBoneMatrices);

public:
	_bool	Picking(_long iMouseX, _long iMouseY, _matrix matWorld, _float3* vPickPoint, _float* fMinDist);

private:
	psMODEL									m_Model;

	_float4x4								m_PivotMatrix;
	TYPE									m_eModelType;

	_uint									m_iNumMeshes = { 0 };
	vector<shared_ptr<class CBinaryMesh>>	m_Meshes;

	_uint									m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL_DESC>				m_Materials;

	vector<shared_ptr<class CBinaryBone>>	m_Bones;

	vector<shared_ptr<class CBinaryAnimation>>	m_Animations;
	_uint									m_iNumAnimations = { 0 };
	_uint									m_iCurrentAnimation = { 0 };
	_uint									m_iNextAnimation = { 0 };

	_bool									m_isAnimChange = false;
	_bool									m_isAnimStop = false;
	_bool									m_isAnimChangeFinish = false;
	_bool									m_isAnimFirst = false;

	_float									m_fChangeDuration = 0.5;
	_float									m_fChangeTrackPosition;

	_vector									m_PrevRootPos = { 0.f, 0.f, 0.f, 1.f };
	_vector									m_CurRootPos = { 0.f, 0.f, 0.f, 1.f };

	_vector									m_PrevRootLook = { 0.f, 0.f, 1.f ,0.f };
	_vector									m_CurRootLook = { 0.f, 0.f, 1.f ,0.f };

	_vector									m_PrevDir = { 0.f, 0.f, 0.f, 0.f };
	_vector									m_PrevRootMotion = { 0.f, 0.f, 0.f, 0.f };
	_vector									m_PrevRootMotionLook = { 0.f, 0.f, 0.f, 0.f };
	_float4x4								m_RootOffsetMatrix;

	CBinaryChannel::KEYFRAME				m_BeforeChangeKeyFrames[256];
	vector<shared_ptr<class CBinaryChannel>>	m_CurrentChannels;
	vector<shared_ptr<class CBinaryChannel>>	m_NextChannels;
	_uint										m_iNextStartKeyFrame;

	_bool									m_isBlockRootMotion = false;

private:
	HRESULT Read_Parsed_File(const wstring& strFilePath);
	HRESULT Read_Static_Model(shared_ptr<CFile_Manager> pFile);
	HRESULT Read_Dynamic_Model(shared_ptr<CFile_Manager> pFile);
	HRESULT Read_Static_Meshes(shared_ptr<CFile_Manager> pFile);
	HRESULT Read_Materials(shared_ptr<CFile_Manager> pFile);
	HRESULT Read_Dynamic_Meshes(shared_ptr<CFile_Manager> pFile);
	HRESULT Read_Bones(shared_ptr<CFile_Manager> pFile);
	HRESULT Read_Animations(shared_ptr<CFile_Manager> pFile);

public:
	static shared_ptr<CBinaryModel> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, TYPE eModelType, const _char* pModelFilePath, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual shared_ptr<CComponent> Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END