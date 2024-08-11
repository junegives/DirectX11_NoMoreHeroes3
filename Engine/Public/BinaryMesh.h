#pragma once
#include "VIBuffer.h"
#include "BinaryModel.h"

/* CMesh : 모델을 구성하는 하나의 메시를 위한 정점과 인덱스를 보관한다. */
/* 모델 하나에 메쉬가 여러개 있을 수 있음 (메쉬마다 하나의 정점 버퍼, 인덱스 버퍼 생성) */
/* 정점과 인덱스를 생성해야 하기 때문에 VIBuffer의 자식으로 생성한다. */

BEGIN(Engine)

class CBinaryMesh : public CVIBuffer
{
public:
	CBinaryMesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CBinaryMesh(const CBinaryMesh& rhs);
	virtual ~CBinaryMesh();

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

	_uint Get_NumBones() const {
		return m_iNumBones;
	}

public:
	virtual HRESULT Initialize_Prototype(CBinaryModel::TYPE eType, shared_ptr<CFile_Manager> pFile, shared_ptr<CBinaryModel> pModel, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg = nullptr) override;
public:
	// 메시의 각 뼈에 대한 변환 행렬을 계산하여 배열인 pBoneMatrices에 저장하는 함수
	void	SetUp_BoneMatrices(_float4x4* pBoneMatrices, vector<shared_ptr<class CBinaryBone>>& Bones);

public:
	virtual _bool	Picking(_long iMouseX, _long iMouseY, _matrix matWorld, _float3* vPickPoint, _float* fMinDist) override;

private:
	_uint*		m_pVerticesIndices = { nullptr };

private:
	// 메시 이름 저장
	string				m_strName;
	// 어떤 메시가 어떤 머티리얼을 쓰는지 알 수 있도록 메시 멤버 변수에 머티리얼 인덱스 저장
	_uint				m_iMaterialIndex = { 0 };

	/* 한 메시가 이용하고 있는 뼈들 */
	_uint				m_iNumBones = { 0 };
	vector<_uint>		m_BoneIndices;
	vector<_float4x4>	m_OffsetMatrices;	// 해당 뼈의 로컬 변환 행렬 (<-> TransformationMatrix : 부모 뼈에 상대적인 변환을 나타내는 행렬)

public:
	// Non Animation 메시를 위한 정점 버퍼를 준비하는 함수
	HRESULT Ready_Vertex_Buffer_NonAnim(shared_ptr<CFile_Manager> pFile, _fmatrix PivotMatrix);

	// Animation 메시를 위한 정점 버퍼를 준비하는 함수
	HRESULT Ready_Vertex_Buffer_Anim(shared_ptr<CFile_Manager> pFile, shared_ptr<CBinaryModel> pModel);

public:
	// 메시가 모델을 받아와야 한다. (본을 활용하기 위해서)
	static shared_ptr<CBinaryMesh> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, CBinaryModel::TYPE eType, shared_ptr<CFile_Manager> pFile, shared_ptr<CBinaryModel> pModel, _fmatrix PivotMatrix);
	virtual shared_ptr<CComponent> Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END