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

	m_iNumVertexBuffers = 1;					// 정점 버퍼의 개수

	m_iIndexStride = 4;							// 각 인덱스 크기 (int형으로 4byte) (약 65535개 이하인 경우 2바이트)
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT; // 인덱스 형식 (2 or 4)
	m_iNumIndices = iNumFaces * 3;  // 사용되는 인덱스의 총 개수 (메시의 면 수 * 3(삼각형의 세 정점))
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;	// 그릴 형태 설정 (삼각형 리스트)

#pragma region VERTEX_BUFFER
	/* 정점버퍼를 만든다.*/
	// 정적 메시와 동적 메시를 다르게 생성
	HRESULT		hr = eType == CBinaryModel::TYPE_NONANIM ? Ready_Vertex_Buffer_NonAnim(pFile, PivotMatrix) : Ready_Vertex_Buffer_Anim(pFile, pModel);
	if (FAILED(hr))
		return E_FAIL;
#pragma endregion

#pragma region INDEX_BUFFER
	/* 인덱스버퍼를 만든다.*/
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;	// 인덱스 버퍼의 전체 크기 (각 인덱스의 크기 * 인덱스의 총 개수)
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;					// 인덱스 버퍼의 사용 용도 (DEFAULT : GPU에서 읽고 쓰기 가능)
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;			// 리소스와 바인딩할 때 사용되는 플래그 (인덱스 버퍼로 사용)
	m_BufferDesc.CPUAccessFlags = 0;						    // CPU에서의 접근 권한 지정 (0 : CPU의 접근 허용 X)
	m_BufferDesc.MiscFlags = 0;									// 기타 플래그 (보통 0)
	m_BufferDesc.StructureByteStride = 0;					    // 구조체의 크기 (인덱스 버퍼는 구조체가 없으므로 0) (안채워도 상관 X)

	_uint* pIndices = new _uint[m_iNumIndices];			        // 인덱스 데이터를 담을 배열 동적 할당
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);        // 인덱스 데이터 배열을 0으로 초기화

	m_pVerticesIndices = new _uint[m_iNumIndices];

	_uint		iNumIndices = { 0 };

	_uint		iFaceVtxIdx_1;
	_uint		iFaceVtxIdx_2;
	_uint		iFaceVtxIdx_3;

	for (size_t i = 0; i < iNumFaces; i++)					// 삼각형 개수만큼 탐색
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
	m_iVertexStride = sizeof(VTXMESH);			// 정점 하나의 크기

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;	// 정점 버퍼의 전체 크기 (각 정점의 크기 * 정점의 총 개수)
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;					// 정점 버퍼의 사용 용도 (DEFAULT : GPU에서 읽고 쓰기 가능 (정적))
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;			// 리소스와 바인딩 할 때 사용되는 플래그 (정점 버퍼로 사용)
	m_BufferDesc.CPUAccessFlags = 0;							// CPU 접근 권한 지정 (0 : CPU의 접근 허용 X)
	m_BufferDesc.MiscFlags = 0;									// 기타 플래그 (보통 0)
	m_BufferDesc.StructureByteStride = m_iVertexStride;			// 구조체의 크기 (현재는 각 정점의 크기와 동일)

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];			// 정점 데이터를 담을 배열 동적 할당
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);	// 정점 데이터 배열을 0으로 초기화

	m_pVerticesPos = new _float3[m_iNumVertices];

	_float3 vPosition;
	_float3 vNormal;
	_float2 vTexcoord;
	_float3 vTangent;

	// 각 정점의 위치와 텍스처 좌표 설정
	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		pFile->Read(vPosition);
		pFile->Read(vNormal);
		pFile->Read(vTexcoord);
		pFile->Read(vTangent);

		// 메시의 i번째 정점의 위치를 pVertices 배열의 i번째 인덱스에 저장
		memcpy(&pVertices[i].vPosition, &vPosition, sizeof(_float3));
		// 저장한 정점의 위치를 PivotMatrix로 변환
		XMStoreFloat3(&pVertices[i].vPosition,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));

		// 정점의 법선 벡터 저장
		memcpy(&pVertices[i].vNormal, &vNormal, sizeof(_float3));
		// 정점의 법선 벡터를 PivotMatrix로 변환
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix));

		// i번째 정점의 첫번째 텍스처 좌표를 pVertices 배열의 i번째 인덱스에 저장
		// -> 메시는 여러 개의 텍스처 좌표를 가질 수 있다.
		// 정점 안에는 texcoord를 최대 8개 넣을 수 있다
		// mTextureCoords[0]을 받아와서 uv 좌표 하나로 샘플링해서 사용
		// -> diffuse, normal, spec 등 생긴 모양이 같으면 uv 좌표 하나로 써도 된다.
		// -> 간혹, 다르다면 모든 텍스처마다 uv texcoord를 받아와야 한다.
		// -> 텍스처가 이상하게 된다면 [0]번 말고 다른 인덱스의 uv 좌표로 입혀야 하는 경우도 있음
		memcpy(&pVertices[i].vTexcoord, &vTexcoord, sizeof(_float2));
		// 정점의 접선 벡터 저장
		memcpy(&pVertices[i].vTangent, &vTangent, sizeof(_float3));

		m_pVerticesPos[i] = pVertices[i].vPosition;
	}

	// 정점 데이터를 초기 데이터로 설정
	// 내가 직접 할당한 공간이 CreateBuffer시에 사용하는 공간이 아니다.
	// 따라서 pSystem으로 초기 데이터 공간을 따로 잡아준다.
	m_InitialData.pSysMem = pVertices;

	// 정점 버퍼 생성 및 초기화
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	// 동적으로 할당한 배열 해제
	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CBinaryMesh::Ready_Vertex_Buffer_Anim(shared_ptr<CFile_Manager> pFile, shared_ptr<CBinaryModel> pModel)
{
	m_iVertexStride = sizeof(VTXANIMMESH);			// 정점 하나의 크기

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;	// 정점 버퍼의 전체 크기 (각 정점의 크기 * 정점의 총 개수)
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;					// 정점 버퍼의 사용 용도 (DEFAULT : GPU에서 읽고 쓰기 가능 (정적))
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;			// 리소스와 바인딩 할 때 사용되는 플래그 (정점 버퍼로 사용)
	m_BufferDesc.CPUAccessFlags = 0;							// CPU 접근 권한 지정 (0 : CPU의 접근 허용 X)
	m_BufferDesc.MiscFlags = 0;									// 기타 플래그 (보통 0)
	m_BufferDesc.StructureByteStride = m_iVertexStride;			// 구조체의 크기 (현재는 각 정점의 크기와 동일)

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];			// 정점 데이터를 담을 배열 동적 할당
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);	// 정점 데이터 배열을 0으로 초기화

	m_pVerticesPos = new _float3[m_iNumVertices];

	_float3 vPosition;
	_float3 vNormal;
	_float2 vTexcoord;
	_float3 vTangent;

	// 각 정점의 위치와 텍스처 좌표 설정
	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		pFile->Read(vPosition);
		pFile->Read(vNormal);
		pFile->Read(vTexcoord);
		pFile->Read(vTangent);

		// 메시의 i번째 정점의 위치를 pVertices 배열의 i번째 인덱스에 저장
		memcpy(&pVertices[i].vPosition, &vPosition, sizeof(_float3));
		// 정점의 법선 벡터 저장
		memcpy(&pVertices[i].vNormal, &vNormal, sizeof(_float3));
		// i번째 정점의 첫번째 텍스처 좌표를 pVertices 배열의 i번째 인덱스에 저장
		memcpy(&pVertices[i].vTexcoord, &vTexcoord, sizeof(_float2));
		// 정점의 접선 벡터 저장
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

	// 정점 데이터를 초기 데이터로 설정
	// 내가 직접 할당한 공간이 CreateBuffer시에 사용하는 공간이 아니다.
	// 따라서 pSystem으로 초기 데이터 공간을 따로 잡아준다.
	m_InitialData.pSysMem = pVertices;

	// 정점 버퍼 생성 및 초기화
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	// 동적으로 할당한 배열 해제
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
