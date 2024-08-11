#include "BinaryMesh.h"

#include "BinaryBone.h"
#include "Picking.h"

#include "File_Manager.h"

CBinaryMesh::CBinaryMesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CVIBuffer(pDevice, pContext)
{
}

CBinaryMesh::CBinaryMesh(const CBinaryMesh& rhs)
    : CVIBuffer(rhs)
{
}

CBinaryMesh::~CBinaryMesh()
{
	Free();
}

HRESULT CBinaryMesh::Initialize_Prototype(CBinaryModel::TYPE eType, shared_ptr<CFile_Manager> pFile, shared_ptr<CBinaryModel> pModel, _fmatrix PivotMatrix)
{
	m_pPicking = make_shared<CPicking>(m_pDevice, m_pContext);

	pFile->Read(m_strName);
	pFile->Read(m_iMaterialIndex);
	pFile->Read(m_iNumVertices);

	_uint iNumFaces;
	pFile->Read(iNumFaces);

	m_iNumVertexBuffers = 1;					// ���� ������ ����

	m_iIndexStride = 4;							// �� �ε��� ũ�� (int������ 4byte) (�� 65535�� ������ ��� 2����Ʈ)
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT; // �ε��� ���� (2 or 4)
	m_iNumIndices = iNumFaces * 3;  // ���Ǵ� �ε����� �� ���� (�޽��� �� �� * 3(�ﰢ���� �� ����))
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;	// �׸� ���� ���� (�ﰢ�� ����Ʈ)

#pragma region VERTEX_BUFFER
	/* �������۸� �����.*/
	// ���� �޽ÿ� ���� �޽ø� �ٸ��� ����
	HRESULT		hr = eType == CBinaryModel::TYPE_NONANIM ? Ready_Vertex_Buffer_NonAnim(pFile, PivotMatrix) : Ready_Vertex_Buffer_Anim(pFile, pModel);
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

	_uint		iFaceVtxIdx_1;
	_uint		iFaceVtxIdx_2;
	_uint		iFaceVtxIdx_3;

	for (size_t i = 0; i < iNumFaces; i++)					// �ﰢ�� ������ŭ Ž��
	{
		pFile->Read(iFaceVtxIdx_1);
		m_pVerticesIndices[iNumIndices] = iFaceVtxIdx_1;
		pIndices[iNumIndices++] = iFaceVtxIdx_1;

		pFile->Read(iFaceVtxIdx_2);
		m_pVerticesIndices[iNumIndices] = iFaceVtxIdx_2;
		pIndices[iNumIndices++] = iFaceVtxIdx_2;

		pFile->Read(iFaceVtxIdx_3);
		m_pVerticesIndices[iNumIndices] = iFaceVtxIdx_3;
		pIndices[iNumIndices++] = iFaceVtxIdx_3;
	}

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CBinaryMesh::Initialize(void* pArg)
{
	return S_OK;
}

void CBinaryMesh::SetUp_BoneMatrices(_float4x4* pBoneMatrices, vector<shared_ptr<class CBinaryBone>>& Bones)
{
	for (size_t i = 0; i < m_iNumBones; i++)
	{
		_float4x4 ComBinedTransformationMatrix = Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix();

		XMStoreFloat4x4(&pBoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * XMLoadFloat4x4(&ComBinedTransformationMatrix));
	}
}

_bool CBinaryMesh::Picking(_long iMouseX, _long iMouseY, _matrix matWorld, _float3* vPickPoint, _float* fMinDist)
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

HRESULT CBinaryMesh::Ready_Vertex_Buffer_NonAnim(shared_ptr<CFile_Manager> pFile, _fmatrix PivotMatrix)
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

	_float3 vPosition;
	_float3 vNormal;
	_float2 vTexcoord;
	_float3 vTangent;

	// �� ������ ��ġ�� �ؽ�ó ��ǥ ����
	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		pFile->Read(vPosition);
		pFile->Read(vNormal);
		pFile->Read(vTexcoord);
		pFile->Read(vTangent);

		// �޽��� i��° ������ ��ġ�� pVertices �迭�� i��° �ε����� ����
		memcpy(&pVertices[i].vPosition, &vPosition, sizeof(_float3));
		// ������ ������ ��ġ�� PivotMatrix�� ��ȯ
		XMStoreFloat3(&pVertices[i].vPosition,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));

		// ������ ���� ���� ����
		memcpy(&pVertices[i].vNormal, &vNormal, sizeof(_float3));
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
		memcpy(&pVertices[i].vTexcoord, &vTexcoord, sizeof(_float2));
		// ������ ���� ���� ����
		memcpy(&pVertices[i].vTangent, &vTangent, sizeof(_float3));

		m_pVerticesPos[i] = pVertices[i].vPosition;
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

HRESULT CBinaryMesh::Ready_Vertex_Buffer_Anim(shared_ptr<CFile_Manager> pFile, shared_ptr<CBinaryModel> pModel)
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

	_float3 vPosition;
	_float3 vNormal;
	_float2 vTexcoord;
	_float3 vTangent;

	// �� ������ ��ġ�� �ؽ�ó ��ǥ ����
	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		pFile->Read(vPosition);
		pFile->Read(vNormal);
		pFile->Read(vTexcoord);
		pFile->Read(vTangent);

		// �޽��� i��° ������ ��ġ�� pVertices �迭�� i��° �ε����� ����
		memcpy(&pVertices[i].vPosition, &vPosition, sizeof(_float3));
		// ������ ���� ���� ����
		memcpy(&pVertices[i].vNormal, &vNormal, sizeof(_float3));
		// i��° ������ ù��° �ؽ�ó ��ǥ�� pVertices �迭�� i��° �ε����� ����
		memcpy(&pVertices[i].vTexcoord, &vTexcoord, sizeof(_float2));
		// ������ ���� ���� ����
		memcpy(&pVertices[i].vTangent, &vTangent, sizeof(_float3));

		m_pVerticesPos[i] = pVertices[i].vPosition;
	}

	pFile->Read(m_iNumBones);

	_float4x4	OffsetMatrix;
	_int		iBoneIndex;

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		pFile->Read(iBoneIndex);
		pFile->Read(OffsetMatrix);

		m_OffsetMatrices.push_back(OffsetMatrix);
		m_BoneIndices.push_back(iBoneIndex);
	}

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		pFile->Read(pVertices[i].vBlendIndices);
		pFile->Read(pVertices[i].vBlendWeights);
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

shared_ptr<CBinaryMesh> CBinaryMesh::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, CBinaryModel::TYPE eType, shared_ptr<CFile_Manager> pFile, shared_ptr<CBinaryModel> pModel, _fmatrix PivotMatrix)
{
	shared_ptr<CBinaryMesh> pInstance = make_shared<CBinaryMesh>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pFile, pModel, PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CMesh");
		pInstance.reset();
	}

	return pInstance;
}

shared_ptr<CComponent> CBinaryMesh::Clone(void* pArg)
{
	shared_ptr<CBinaryMesh> pInstance = make_shared<CBinaryMesh>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMesh");
		pInstance.reset();
	}

	return pInstance;
}

void CBinaryMesh::Free()
{
	Safe_Delete_Array(m_pVerticesPos);
	Safe_Delete_Array(m_pVerticesIndices);
	__super::Free();
}
