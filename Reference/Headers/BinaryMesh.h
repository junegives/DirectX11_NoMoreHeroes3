#pragma once
#include "VIBuffer.h"
#include "BinaryModel.h"

/* CMesh : ���� �����ϴ� �ϳ��� �޽ø� ���� ������ �ε����� �����Ѵ�. */
/* �� �ϳ��� �޽��� ������ ���� �� ���� (�޽����� �ϳ��� ���� ����, �ε��� ���� ����) */
/* ������ �ε����� �����ؾ� �ϱ� ������ VIBuffer�� �ڽ����� �����Ѵ�. */

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
	// �޽��� �� ���� ���� ��ȯ ����� ����Ͽ� �迭�� pBoneMatrices�� �����ϴ� �Լ�
	void	SetUp_BoneMatrices(_float4x4* pBoneMatrices, vector<shared_ptr<class CBinaryBone>>& Bones);

public:
	virtual _bool	Picking(_long iMouseX, _long iMouseY, _matrix matWorld, _float3* vPickPoint, _float* fMinDist) override;

private:
	_uint*		m_pVerticesIndices = { nullptr };

private:
	// �޽� �̸� ����
	string				m_strName;
	// � �޽ð� � ��Ƽ������ ������ �� �� �ֵ��� �޽� ��� ������ ��Ƽ���� �ε��� ����
	_uint				m_iMaterialIndex = { 0 };

	/* �� �޽ð� �̿��ϰ� �ִ� ���� */
	_uint				m_iNumBones = { 0 };
	vector<_uint>		m_BoneIndices;
	vector<_float4x4>	m_OffsetMatrices;	// �ش� ���� ���� ��ȯ ��� (<-> TransformationMatrix : �θ� ���� ������� ��ȯ�� ��Ÿ���� ���)

public:
	// Non Animation �޽ø� ���� ���� ���۸� �غ��ϴ� �Լ�
	HRESULT Ready_Vertex_Buffer_NonAnim(shared_ptr<CFile_Manager> pFile, _fmatrix PivotMatrix);

	// Animation �޽ø� ���� ���� ���۸� �غ��ϴ� �Լ�
	HRESULT Ready_Vertex_Buffer_Anim(shared_ptr<CFile_Manager> pFile, shared_ptr<CBinaryModel> pModel);

public:
	// �޽ð� ���� �޾ƿ;� �Ѵ�. (���� Ȱ���ϱ� ���ؼ�)
	static shared_ptr<CBinaryMesh> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, CBinaryModel::TYPE eType, shared_ptr<CFile_Manager> pFile, shared_ptr<CBinaryModel> pModel, _fmatrix PivotMatrix);
	virtual shared_ptr<CComponent> Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END