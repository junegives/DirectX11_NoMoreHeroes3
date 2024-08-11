#pragma once
#include "VIBuffer.h"
#include "Model.h"

/* CMesh : ���� �����ϴ� �ϳ��� �޽ø� ���� ������ �ε����� �����Ѵ�. */
/* �� �ϳ��� �޽��� ������ ���� �� ���� (�޽����� �ϳ��� ���� ����, �ε��� ���� ����) */
/* ������ �ε����� �����ؾ� �ϱ� ������ VIBuffer�� �ڽ����� �����Ѵ�. */

BEGIN(Engine)

class CMesh : public CVIBuffer
{
public:
	CMesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh();

public:
	string				Get_MeshName() { return m_szName; }
	_uint				Get_MaterialIndex() const { return m_iMaterialIndex; }
	_uint				Get_NumBones() const { return m_iNumBones; }
	vector<_uint>		Get_BoneIndices() { return m_BoneIndices; }
	vector<_float4x4>	Get_OffsetMatrices() { return m_OffsetMatrices; }

	_uint				Get_NumVertices() { return m_iNumVertices; }
	_uint				Get_NumFaces() { return m_iNumFaces; }

	_float3* GetVerticesPos() { return m_pVerticesPos; }
	_float3* GetVerticesNormal() { return m_pVerticesNormal; }
	_float2* GetVerticesTexcoord() { return m_pVerticesTexcoord; }
	_float3* GetVerticesTangent() { return m_pVerticesTangent; }

	_uint* GetIndices() { return m_pVerticesIndices; }

public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eType, const aiMesh* pAIMesh, shared_ptr<CModel> pModel, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg = nullptr) override;
public:
	// �޽��� �� ���� ���� ��ȯ ����� ����Ͽ� �迭�� pBoneMatrices�� �����ϴ� �Լ�
	void	SetUp_BoneMatrices(_float4x4* pBoneMatrices, vector<shared_ptr<class CBone>>& Bones);

public:
	virtual _bool	Picking(_long iMouseX, _long iMouseY, _matrix matWorld, _float3* vPickPoint, _float* fMinDist) override;

private:
	_float3*	m_pVerticesNormal = { nullptr };
	_float2*	m_pVerticesTexcoord = { nullptr };
	_float3*	m_pVerticesTangent = { nullptr };

	_uint*		m_pVerticesIndices = { nullptr };

private:
	// �޽� �̸� ����
	_char				m_szName[MAX_PATH] = "";
	// � �޽ð� � ��Ƽ������ ������ �� �� �ֵ��� �޽� ��� ������ ��Ƽ���� �ε��� ����
	_uint				m_iMaterialIndex = { 0 };

	_uint				m_iNumFaces;

	/* �� �޽ð� �̿��ϰ� �ִ� ���� */
	_uint				m_iNumBones = { 0 };
	vector<_uint>		m_BoneIndices;
	vector<_float4x4>	m_OffsetMatrices;	// �ش� ���� ���� ��ȯ ��� (<-> TransformationMatrix : �θ� ���� ������� ��ȯ�� ��Ÿ���� ���)

public:
	// Non Animation �޽ø� ���� ���� ���۸� �غ��ϴ� �Լ�
	HRESULT Ready_Vertex_Buffer_NonAnim(const aiMesh* pAIMesh, _fmatrix PivotMatrix);

	// Animation �޽ø� ���� ���� ���۸� �غ��ϴ� �Լ�
	HRESULT Ready_Vertex_Buffer_Anim(const aiMesh* pAIMesh, shared_ptr<CModel> pModel);

public:
	// �޽ð� ���� �޾ƿ;� �Ѵ�. (���� Ȱ���ϱ� ���ؼ�)
	static shared_ptr<CMesh> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, CModel::TYPE eType, const aiMesh* pAIMesh, shared_ptr<CModel> pModel, _fmatrix PivotMatrix);
	virtual shared_ptr<CComponent> Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END