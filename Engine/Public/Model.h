#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel : public CComponent, public enable_shared_from_this<CModel>
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

public:
	CModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	TYPE Get_Type() const { return m_eModelType; }

	_uint Get_NumMeshes() const { return m_iNumMeshes; }
	vector<shared_ptr<class CMesh>> Get_Meshes() { return m_Meshes; }

	_uint	Get_NumMaterials() { return m_iNumMaterials; }
	vector<MESH_MATERIAL_DESC> Get_Materials() { return m_Materials; }

	// 본의 이름으로 인덱스를 반환하는 함수
	_int Get_BoneIndex(const _char* pBoneName) const;
	vector<shared_ptr<class CBone>> Get_Bones() { return m_Bones; }

	_uint Get_NumAnimations() { return m_iNumAnimations; }
	_uint Get_CurrentAnimation() { return m_iCurrentAnimation; }
	vector<shared_ptr<class CAnimation>> Get_Animations() { return m_Animations; }

	void Set_Animation(_uint iAnimIndex) { m_iCurrentAnimation = iAnimIndex; }


public:
	// (모델 타입(움직or안움직), 모델 파일 경로, 모델 초기 변환)
	HRESULT Initialize_Prototype(TYPE eModelType, const _char* pModelFilePath, _fmatrix PivotMatrix);
	HRESULT Initialize(void* pArg);
	HRESULT Render(_uint iMeshIndex);

public:
	HRESULT Bind_Material_ShaderResource(shared_ptr<class CShader> pShader, _uint iMeshIndex, aiTextureType eMaterialType, const _char* pConstantName);
	HRESULT Bind_BoneMatrices(shared_ptr<class CShader> pShader, const _char* pConstantName, _uint iMeshIndex);
	void	Play_Animation(_float fTimeDelta, _bool isLoop = false);

public:
	_bool	Picking(_long iMouseX, _long iMouseY, _matrix matWorld, _float3* vPickPoint, _float* fMinDist);

private:
	// aiScene : Assimp에서 로드된 3D 모델의 정보를 담는 구조체 (모델의 메시, 재질, 애니메이션 등)
	const aiScene* m_pAIScene = { nullptr };
	// Importer : 모델을 로드하고 처리하는 클래스
	Assimp::Importer			m_Importer;

private:
	// 모델 초기 변환 행렬 (원하는 이동, 회전, 스케일)
	_float4x4				m_PivotMatrix;

private:
	// 모델의 메시 정보
	_uint							m_iNumMeshes = { 0 };
	vector<shared_ptr<class CMesh>>	m_Meshes;

	// 메시마다 머티리얼을 사용하지만 항상 1:1은 아님
	// 머티리얼 미리 저장해놓고 메시가 어떤 머티리얼을 쓰는지 찾아서 갖다 쓰는 식으로
	_uint							m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL_DESC>		m_Materials;

	/* 내 모델의 모든 뼈를 다 로드하여 컨테이너에 저장한다.  */
	vector<shared_ptr<class CBone>>		m_Bones;
	TYPE								m_eModelType;

	// 애니메이션용 변수들
	_uint									m_iNumAnimations = { 0 };
	_uint									m_iCurrentAnimation = { 0 };
	vector<shared_ptr<class CAnimation>>	m_Animations;

private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	// aiNode를 기반으로 하는 CBone을 생성하고 모델의 뼈대를 구성
	HRESULT Ready_Bones(aiNode* pNode, _int iParentBoneIndex);
	HRESULT Ready_Animations();
public:
	static shared_ptr<CModel> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, TYPE eModelType, const _char* pModelFilePath, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual shared_ptr<CComponent> Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END