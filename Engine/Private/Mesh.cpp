#include "Mesh.h"

#include "Bone.h"
#include "Picking.h"

CMesh::CMesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh& rhs)
    : CVIBuffer(rhs)
{
}

CMesh::~CMesh()
{
	Free();
}

HRESULT CMesh::Initialize_Prototype(CModel::TYPE eType, const aiMesh* pAIMesh, shared_ptr<CModel> pModel, _fmatrix PivotMatrix)
{
	m_pPicking = make_shared<CPicking>(m_pDevice, m_pContext);

	strcpy_s(m_szName, pAIMesh->mName.data);

	m_iMaterialIndex = pAIMesh->mMaterialIndex;

	// pAIMesh->mNumVertices : �޽��� �������� ��Ÿ���� �迭
	// pAIMesh->mNormals : �޽��� �� ������ ���� ���� ���͵��� ��Ÿ���� �迭
	// pAIMesh->mTextureCoords : �޽��� �� ������ ���� �ؽ�ó ��ǥ�� ��Ÿ���� �迭
	// pAIMesh->mFaces : �޽��� ��(face) ������ ��Ÿ���� �迭. �� ���� ���� �ε������� ������ �ִ�.
	// pAIMesh->mNumVertices : �޽��� ���� ���� ��Ÿ���� ����
	// pAIMesh->mNumFaces : �޽��� �� ���� ��Ÿ���� ����
	// pAIMesh->mVertices : �޽��� �� ������ ��ġ ������ ��� �ִ� �迭

	m_iNumVertexBuffers = 1;					// ���� ������ ����
	m_iNumVertices = pAIMesh->mNumVertices;		// �� ������ ����

	m_iIndexStride = 4;							// �� �ε��� ũ�� (int������ 4byte) (�� 65535�� ������ ��� 2����Ʈ)
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT; // �ε��� ���� (2 or 4)
	m_iNumFaces = pAIMesh->mNumFaces;
	m_iNumIndices = pAIMesh->mNumFaces * 3;  // ���Ǵ� �ε����� �� ���� (�޽��� �� �� * 3(�ﰢ���� �� ����))
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;	// �׸� ���� ���� (�ﰢ�� ����Ʈ)

#pragma region VERTEX_BUFFER
	/* �������۸� �����.*/
	// ���� �޽ÿ� ���� �޽ø� �ٸ��� ����
	HRESULT		hr = eType == CModel::TYPE_NONANIM ? Ready_Vertex_Buffer_NonAnim(pAIMesh, PivotMatrix) : Ready_Vertex_Buffer_Anim(pAIMesh, pModel);
	if (FAILED(hr))
		return E_FAIL;
#pragma endregion

#pragma region INDEX_BUFFER
	/* �ε������۸� �����.*/
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;	// �ε��� ������ ��ü ũ�� (�� �ε����� ũ�� * �ε����� �� ����)
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;					// �ε��� ������ ��� �뵵 (DEFAULT : GPU���� �а� ���� ����)
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;			// ���ҽ��� ���ε��� �� ���Ǵ� �÷��� (�ε��� ���۷� ���)
	m_BufferDesc.CPUAccessFlags = 0;						    // CPU������ ���� ���� ���� (0 : CPU�� ���� ��� X)
	m_BufferDesc.MiscFlags = 0;									// ��Ÿ �÷��� (���� 0)
	m_BufferDesc.StructureByteStride = 0;					    // ����ü�� ũ�� (�ε��� ���۴� ����ü�� �����Ƿ� 0) (��ä���� ��� X)

	_uint* pIndices = new _uint[m_iNumIndices];			        // �ε��� �����͸� ���� �迭 ���� �Ҵ�
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);        // �ε��� ������ �迭�� 0���� �ʱ�ȭ

	m_pVerticesIndices = new _uint[m_iNumIndices];

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)					// �ﰢ�� ������ŭ Ž��
	{
		m_pVerticesIndices[iNumIndices] = pAIMesh->mFaces[i].mIndices[0];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
		m_pVerticesIndices[iNumIndices] = pAIMesh->mFaces[i].mIndices[1];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
		m_pVerticesIndices[iNumIndices] = pAIMesh->mFaces[i].mIndices[2];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];
	}

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}

void CMesh::SetUp_BoneMatrices(_float4x4* pBoneMatrices, vector<shared_ptr<class CBone>>& Bones)
{
	for (size_t i = 0; i < m_iNumBones; i++)
	{
		_float4x4 ComBinedTransformationMatrix = Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix();

		XMStoreFloat4x4(&pBoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * XMLoadFloat4x4(&ComBinedTransformationMatrix));
	}
}

_bool CMesh::Picking(_long iMouseX, _long iMouseY, _matrix matWorld, _float3* vPickPoint, _float* fMinDist)
{
	m_pPicking->RayToWorld(iMouseX, iMouseY);


	for (size_t i = 0; i < m_iNumIndices; i+=3)
	{
		if (m_pPicking->IntersectWorld(m_pVerticesPos[m_pVerticesIndices[i]], m_pVerticesPos[m_pVerticesIndices[i + 1]], m_pVerticesPos[m_pVerticesIndices[i + 2]], vPickPoint, fMinDist))
		{
			return true;
		}
	}

	return false;
}

HRESULT CMesh::Ready_Vertex_Buffer_NonAnim(const aiMesh* pAIMesh, _fmatrix PivotMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);			// ���� �ϳ��� ũ��

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;	// ���� ������ ��ü ũ�� (�� ������ ũ�� * ������ �� ����)
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;					// ���� ������ ��� �뵵 (DEFAULT : GPU���� �а� ���� ���� (����))
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;			// ���ҽ��� ���ε� �� �� ���Ǵ� �÷��� (���� ���۷� ���)
	m_BufferDesc.CPUAccessFlags = 0;							// CPU ���� ���� ���� (0 : CPU�� ���� ��� X)
	m_BufferDesc.MiscFlags = 0;									// ��Ÿ �÷��� (���� 0)
	m_BufferDesc.StructureByteStride = m_iVertexStride;			// ����ü�� ũ�� (����� �� ������ ũ��� ����)

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];			// ���� �����͸� ���� �迭 ���� �Ҵ�
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);	// ���� ������ �迭�� 0���� �ʱ�ȭ

	m_pVerticesPos = new _float3[m_iNumVertices];
	m_pVerticesNormal = new _float3[m_iNumVertices];
	m_pVerticesTexcoord = new _float2[m_iNumVertices];
	m_pVerticesTangent = new _float3[m_iNumVertices];

	// �� ������ ��ġ�� �ؽ�ó ��ǥ ����
	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		// �޽��� i��° ������ ��ġ�� pVertices �迭�� i��° �ε����� ����
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		// ������ ������ ��ġ�� PivotMatrix�� ��ȯ
		XMStoreFloat3(&pVertices[i].vPosition,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));

		// ������ ���� ���� ����
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		// ������ ���� ���͸� PivotMatrix�� ��ȯ
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix));

		// i��° ������ ù��° �ؽ�ó ��ǥ�� pVertices �迭�� i��° �ε����� ����
		// -> �޽ô� ���� ���� �ؽ�ó ��ǥ�� ���� �� �ִ�.
		// ���� �ȿ��� texcoord�� �ִ� 8�� ���� �� �ִ�
		// mTextureCoords[0]�� �޾ƿͼ� uv ��ǥ �ϳ��� ���ø��ؼ� ���
		// -> diffuse, normal, spec �� ���� ����� ������ uv ��ǥ �ϳ��� �ᵵ �ȴ�.
		// -> ��Ȥ, �ٸ��ٸ� ��� �ؽ�ó���� uv texcoord�� �޾ƿ;� �Ѵ�.
		// -> �ؽ�ó�� �̻��ϰ� �ȴٸ� [0]�� ���� �ٸ� �ε����� uv ��ǥ�� ������ �ϴ� ��쵵 ����
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		// ������ ���� ���� ����
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));

		m_pVerticesPos[i] = pVertices[i].vPosition;
		m_pVerticesNormal[i] = pVertices[i].vNormal;
		m_pVerticesTexcoord[i] = pVertices[i].vTexcoord;
		m_pVerticesTangent[i] = pVertices[i].vTangent;
	}

	// ���� �����͸� �ʱ� �����ͷ� ����
	// ���� ���� �Ҵ��� ������ CreateBuffer�ÿ� ����ϴ� ������ �ƴϴ�.
	// ���� pSystem���� �ʱ� ������ ������ ���� ����ش�.
	m_InitialData.pSysMem = pVertices;

	// ���� ���� ���� �� �ʱ�ȭ
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	// �������� �Ҵ��� �迭 ����
	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_Vertex_Buffer_Anim(const aiMesh* pAIMesh, shared_ptr<CModel> pModel)
{
	m_iVertexStride = sizeof(VTXANIMMESH);			// ���� �ϳ��� ũ��

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;	// ���� ������ ��ü ũ�� (�� ������ ũ�� * ������ �� ����)
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;					// ���� ������ ��� �뵵 (DEFAULT : GPU���� �а� ���� ���� (����))
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;			// ���ҽ��� ���ε� �� �� ���Ǵ� �÷��� (���� ���۷� ���)
	m_BufferDesc.CPUAccessFlags = 0;							// CPU ���� ���� ���� (0 : CPU�� ���� ��� X)
	m_BufferDesc.MiscFlags = 0;									// ��Ÿ �÷��� (���� 0)
	m_BufferDesc.StructureByteStride = m_iVertexStride;			// ����ü�� ũ�� (����� �� ������ ũ��� ����)

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];			// ���� �����͸� ���� �迭 ���� �Ҵ�
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);	// ���� ������ �迭�� 0���� �ʱ�ȭ

	m_pVerticesPos = new _float3[m_iNumVertices];

	// �� ������ ��ġ�� �ؽ�ó ��ǥ ����
	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		// �޽��� i��° ������ ��ġ�� pVertices �迭�� i��° �ε����� ����
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		// ������ ���� ���� ����
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		// i��° ������ ù��° �ؽ�ó ��ǥ�� pVertices �迭�� i��° �ε����� ����
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		// ������ ���� ���� ����
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));

		m_pVerticesPos[i] = pVertices[i].vPosition;
	}

	/* �� �޽��� �������� ���¸� �޾ƿ;��ϴ� �޽ÿ� ������ �ִ� ������ ��ü ����*/
	m_iNumBones = pAIMesh->mNumBones;

	// �޽��� �� �ϳ����� �ݺ�
	for (size_t i = 0; i < m_iNumBones; i++)
	{
		aiBone* pAIBone = pAIMesh->mBones[i];

		/*
		aiBone->mName		: ���� �̸��� ��Ÿ���� aiString
		aiBone->mNumWeights	: �ش� ���� ������ ��ġ�� ������ ��
		aiBone->mWeights	: ������ ���� ���� ���� ������ ��� �ִ� aiVertexWeight ����ü(mVertexId, mWeight) �迭
		aiBone->mVertexId	: ���� ������ ��ġ�� ������ �ε���
		aiBone->mWeight		: �ش� ������ ���� ���� ����ġ
		*/

		/*
		aiBone->mTransformation : �ش� ���� ���� ��ȯ�� ��Ÿ���� 4x4 ��� (�θ� ���� ��������� ��� ��ȯ�Ǵ��� ��Ÿ���� ���)
		aiBone->mOffsetMatrix : �ش� ���� ���̷��濡 ���� ��, �ش� ���� �𵨿� ����Ǵ� ���� ��ȯ (��Ű�� �ÿ� �� ���� ���ؽ��� ����Ǵ� ��ȯ)
		-> ��Ű�� : ���� �Ǻθ� ���� �����ӿ� ���� �ڿ������� ������Ű�� ���
		*/

		// OffsetMatrix : ������ � �޽ÿ� ���Ǿ�� �� ��, �󸶳� �� ������ ���ļ� ���Ǿ� �ϴ���
		_float4x4	OffsetMatrix;

		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));

		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.push_back(OffsetMatrix);

		// �� �̸����� ���� �ε��� �����ͼ� m_BoneIndices�� ����
		m_BoneIndices.push_back(pModel->Get_BoneIndex(pAIBone->mName.data));

		// ���� ������ ��ġ�� �������� �ݺ�
		/* �� �޽ÿ��� ������ �ִ� i��° ���� �� ���� �����鿡�� ������ �ش�. */
		for (size_t j = 0; j < pAIBone->mNumWeights; j++)
		{
			/* pAIBone->mWeights[j].mVertexId : �� ���� ������ �ִ� j��° ������ �ε��� */
			// pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x : ���� ���� ������ �ִ� j��° ������ �ε����� ������ ���� ù ��° ���� ����ġ
			// �ش� ������ ���� ����ġ�� ���� �������� �ʾҴٸ�(0���� �ʱ�ȭ�Ǿ� ������) �ش� ���� ���� ���� ����
			
			// j vs. mVertexId
			// j : ���� ���� ������ ��ġ�� ������ ��ȸ -> ���� ���� ������ ��ġ�� ���� ���� (��� �ε��� ���� ����)
			// mVertexId : ���� ���� ������ ��ġ�� ������ �ε��� -> ���� ���� ������ ��ġ�� ������ �ε��� ���� (���� �ε��� ���� ����)

			// mWeight vs. vBlendWeights
			// mWeight : �ϳ��� ������ ���� �ϳ��� ���� ���� (0~1), �ش� ������ ���� ���� ������ ���� ���, �� ���� ���� mWeight���� �������� 1�� �Ǿ�� �Ѵ�.
			// vBlendWeights : �ϳ��� ������ ���� ���� ���� ���� ����ġ�� ��Ÿ���� ����, xyzw(���ļ� 1)�� �������� ������, �ش� ������ �޴� 4���� ���� ���⿡ ���� ����ġ�� ��Ÿ����.
			if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x)
			{
				// x ���п� ���� ���� �ε��� ����
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = i;
				// x ���п� ���� ���� ����ġ ����
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight;
			}
			// else if ������ ó���Ͽ� �ش� ����(j)�� �ش� ��(i)�� ����ġ�� �̹� ���� ��쿡�� ���� �������� �Ѿ
			else if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y)
			{
				// y ���п� ���� ���� �ε��� ����
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = i;
				// y ���п� ���� ���� ����ġ ����
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight;
			}
			else if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z)
			{
				// z ���п� ���� ���� �ε��� ����
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = i;
				// z ���п� ���� ���� ����ġ ����
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;
			}
			else if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w)
			{
				// w ���п� ���� ���� �ε��� ����
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = i;
				// w ���п� ���� ���� ����ġ ����
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;
			}
		}
	}

	// �޽��� ���� ���� ��� �� 1���� ����
	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		m_BoneIndices.push_back(pModel->Get_BoneIndex(m_szName));

		_float4x4		OffsetMatrix;
		
		// ���� ��ȯ�� �ʿ����� �ʱ� ������ �׵���ķ� ����
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_OffsetMatrices.push_back(OffsetMatrix);
	}

	// ���� �����͸� �ʱ� �����ͷ� ����
	// ���� ���� �Ҵ��� ������ CreateBuffer�ÿ� ����ϴ� ������ �ƴϴ�.
	// ���� pSystem���� �ʱ� ������ ������ ���� ����ش�.
	m_InitialData.pSysMem = pVertices;

	// ���� ���� ���� �� �ʱ�ȭ
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	// �������� �Ҵ��� �迭 ����
	Safe_Delete_Array(pVertices);

	return S_OK;
}

shared_ptr<CMesh> CMesh::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, CModel::TYPE eType, const aiMesh* pAIMesh, shared_ptr<CModel> pModel, _fmatrix PivotMatrix)
{
	shared_ptr<CMesh> pInstance = make_shared<CMesh>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pAIMesh, pModel, PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CMesh");
		pInstance.reset();
	}

	return pInstance;
}

shared_ptr<CComponent> CMesh::Clone(void* pArg)
{
	shared_ptr<CMesh> pInstance = make_shared<CMesh>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMesh");
		pInstance.reset();
	}

	return pInstance;
}

void CMesh::Free()
{
	Safe_Delete_Array(m_pVerticesPos);
	Safe_Delete_Array(m_pVerticesNormal);
	Safe_Delete_Array(m_pVerticesTexcoord);
	Safe_Delete_Array(m_pVerticesTangent);
	Safe_Delete_Array(m_pVerticesIndices);
	__super::Free();
}
