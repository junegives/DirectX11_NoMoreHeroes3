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

	// ���� �̸����� �ε����� ��ȯ�ϴ� �Լ�
	_int Get_BoneIndex(const _char* pBoneName) const;
	vector<shared_ptr<class CBone>> Get_Bones() { return m_Bones; }

	_uint Get_NumAnimations() { return m_iNumAnimations; }
	_uint Get_CurrentAnimation() { return m_iCurrentAnimation; }
	vector<shared_ptr<class CAnimation>> Get_Animations() { return m_Animations; }

	void Set_Animation(_uint iAnimIndex) { m_iCurrentAnimation = iAnimIndex; }


public:
	// (�� Ÿ��(����or�ȿ���), �� ���� ���, �� �ʱ� ��ȯ)
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
	// aiScene : Assimp���� �ε�� 3D ���� ������ ��� ����ü (���� �޽�, ����, �ִϸ��̼� ��)
	const aiScene* m_pAIScene = { nullptr };
	// Importer : ���� �ε��ϰ� ó���ϴ� Ŭ����
	Assimp::Importer			m_Importer;

private:
	// �� �ʱ� ��ȯ ��� (���ϴ� �̵�, ȸ��, ������)
	_float4x4				m_PivotMatrix;

private:
	// ���� �޽� ����
	_uint							m_iNumMeshes = { 0 };
	vector<shared_ptr<class CMesh>>	m_Meshes;

	// �޽ø��� ��Ƽ������ ��������� �׻� 1:1�� �ƴ�
	// ��Ƽ���� �̸� �����س��� �޽ð� � ��Ƽ������ ������ ã�Ƽ� ���� ���� ������
	_uint							m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL_DESC>		m_Materials;

	/* �� ���� ��� ���� �� �ε��Ͽ� �����̳ʿ� �����Ѵ�.  */
	vector<shared_ptr<class CBone>>		m_Bones;
	TYPE								m_eModelType;

	// �ִϸ��̼ǿ� ������
	_uint									m_iNumAnimations = { 0 };
	_uint									m_iCurrentAnimation = { 0 };
	vector<shared_ptr<class CAnimation>>	m_Animations;

private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	// aiNode�� ������� �ϴ� CBone�� �����ϰ� ���� ���븦 ����
	HRESULT Ready_Bones(aiNode* pNode, _int iParentBoneIndex);
	HRESULT Ready_Animations();
public:
	static shared_ptr<CModel> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, TYPE eModelType, const _char* pModelFilePath, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual shared_ptr<CComponent> Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END