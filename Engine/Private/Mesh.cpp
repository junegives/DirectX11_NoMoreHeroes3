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

	// pAIMesh->mNumVertices : 메시의 정점들을 나타내는 배열
	// pAIMesh->mNormals : 메시의 각 정점에 대한 정규 벡터들을 나타내는 배열
	// pAIMesh->mTextureCoords : 메시의 각 정점에 대한 텍스처 좌표를 나타내는 배열
	// pAIMesh->mFaces : 메시의 면(face) 정보를 나타내는 배열. 각 면은 정점 인덱스들을 가지고 있다.
	// pAIMesh->mNumVertices : 메시의 정점 수를 나타내는 변수
	// pAIMesh->mNumFaces : 메시의 면 수를 나타내는 변수
	// pAIMesh->mVertices : 메시의 각 정점의 위치 정보를 담고 있는 배열

	m_iNumVertexBuffers = 1;					// 정점 버퍼의 개수
	m_iNumVertices = pAIMesh->mNumVertices;		// 총 정점의 개수

	m_iIndexStride = 4;							// 각 인덱스 크기 (int형으로 4byte) (약 65535개 이하인 경우 2바이트)
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT; // 인덱스 형식 (2 or 4)
	m_iNumFaces = pAIMesh->mNumFaces;
	m_iNumIndices = pAIMesh->mNumFaces * 3;  // 사용되는 인덱스의 총 개수 (메시의 면 수 * 3(삼각형의 세 정점))
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;	// 그릴 형태 설정 (삼각형 리스트)

#pragma region VERTEX_BUFFER
	/* 정점버퍼를 만든다.*/
	// 정적 메시와 동적 메시를 다르게 생성
	HRESULT		hr = eType == CModel::TYPE_NONANIM ? Ready_Vertex_Buffer_NonAnim(pAIMesh, PivotMatrix) : Ready_Vertex_Buffer_Anim(pAIMesh, pModel);
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

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)					// 삼각형 개수만큼 탐색
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
	m_pVerticesNormal = new _float3[m_iNumVertices];
	m_pVerticesTexcoord = new _float2[m_iNumVertices];
	m_pVerticesTangent = new _float3[m_iNumVertices];

	// 각 정점의 위치와 텍스처 좌표 설정
	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		// 메시의 i번째 정점의 위치를 pVertices 배열의 i번째 인덱스에 저장
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		// 저장한 정점의 위치를 PivotMatrix로 변환
		XMStoreFloat3(&pVertices[i].vPosition,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));

		// 정점의 법선 벡터 저장
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
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
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		// 정점의 접선 벡터 저장
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));

		m_pVerticesPos[i] = pVertices[i].vPosition;
		m_pVerticesNormal[i] = pVertices[i].vNormal;
		m_pVerticesTexcoord[i] = pVertices[i].vTexcoord;
		m_pVerticesTangent[i] = pVertices[i].vTangent;
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

HRESULT CMesh::Ready_Vertex_Buffer_Anim(const aiMesh* pAIMesh, shared_ptr<CModel> pModel)
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

	// 각 정점의 위치와 텍스처 좌표 설정
	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		// 메시의 i번째 정점의 위치를 pVertices 배열의 i번째 인덱스에 저장
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		// 정점의 법선 벡터 저장
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		// i번째 정점의 첫번째 텍스처 좌표를 pVertices 배열의 i번째 인덱스에 저장
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		// 정점의 접선 벡터 저장
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));

		m_pVerticesPos[i] = pVertices[i].vPosition;
	}

	/* 이 메시의 정점들이 상태를 받아와야하는 메시에 영향을 주는 뼈들의 전체 갯수*/
	m_iNumBones = pAIMesh->mNumBones;

	// 메시의 본 하나마다 반복
	for (size_t i = 0; i < m_iNumBones; i++)
	{
		aiBone* pAIBone = pAIMesh->mBones[i];

		/*
		aiBone->mName		: 뼈의 이름을 나타내는 aiString
		aiBone->mNumWeights	: 해당 뼈가 영향을 미치는 정점의 수
		aiBone->mWeights	: 정점에 대한 뼈의 영향 정보를 담고 있는 aiVertexWeight 구조체(mVertexId, mWeight) 배열
		aiBone->mVertexId	: 뼈가 영향을 미치는 정점의 인덱스
		aiBone->mWeight		: 해당 정점에 대한 뼈의 가중치
		*/

		/*
		aiBone->mTransformation : 해당 본의 로컬 변환을 나타내는 4x4 행렬 (부모 본에 상대적으로 어떻게 변환되는지 나타내는 행렬)
		aiBone->mOffsetMatrix : 해당 본이 스켈레톤에 속할 때, 해당 뼈가 모델에 적용되는 로컬 변환 (스키닝 시에 각 본이 버텍스에 적용되는 변환)
		-> 스키닝 : 모델의 피부를 뼈의 움직임에 따라 자연스럽게 변형시키는 기술
		*/

		// OffsetMatrix : 뼈들이 어떤 메시에 사용되어야 할 때, 얼마나 더 보정을 거쳐서 사용되야 하느냐
		_float4x4	OffsetMatrix;

		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));

		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.push_back(OffsetMatrix);

		// 본 이름으로 본의 인덱스 가져와서 m_BoneIndices에 저장
		m_BoneIndices.push_back(pModel->Get_BoneIndex(pAIBone->mName.data));

		// 본이 영향을 미치는 정점마다 반복
		/* 이 메시에게 영향을 주는 i번째 뼈는 몇 개의 정점들에게 영향을 준다. */
		for (size_t j = 0; j < pAIBone->mNumWeights; j++)
		{
			/* pAIBone->mWeights[j].mVertexId : 이 뼈가 영향을 주는 j번째 정점의 인덱스 */
			// pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x : 현재 뼈가 영향을 주는 j번째 정점의 인덱스의 정점에 대한 첫 번째 뼈의 가중치
			// 해당 정점에 대한 가중치가 아직 설정되지 않았다면(0으로 초기화되어 있으면) 해당 뼈에 대한 영향 설정
			
			// j vs. mVertexId
			// j : 현재 뼈가 영향을 미치는 정점들 순회 -> 현재 뼈가 영향을 미치는 정점 리턴 (상대 인덱스 같은 느낌)
			// mVertexId : 현재 뼈가 영향을 미치는 정점의 인덱스 -> 현재 뼈가 영향을 미치는 정점의 인덱스 리턴 (절대 인덱스 같은 느낌)

			// mWeight vs. vBlendWeights
			// mWeight : 하나의 정점에 대한 하나의 뼈의 영향 (0~1), 해당 정점이 여러 뼈에 영향을 받을 경우, 각 뼈에 대한 mWeight값이 합쳐져서 1이 되어야 한다.
			// vBlendWeights : 하나의 정점에 대한 여러 뼈에 대한 가중치를 나타내는 벡터, xyzw(합쳐서 1)의 성분으로 나뉘어, 해당 정점이 받는 4가지 뼈의 영향에 대한 가중치를 나타낸다.
			if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x)
			{
				// x 성분에 현재 뼈의 인덱스 설정
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = i;
				// x 성분에 현재 뼈의 가중치 설정
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight;
			}
			// else if 문으로 처리하여 해당 정점(j)에 해당 뼈(i)의 가중치가 이미 들어온 경우에는 다음 정점으로 넘어감
			else if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y)
			{
				// y 성분에 현재 뼈의 인덱스 설정
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = i;
				// y 성분에 현재 뼈의 가중치 설정
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight;
			}
			else if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z)
			{
				// z 성분에 현재 뼈의 인덱스 설정
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = i;
				// z 성분에 현재 뼈의 가중치 설정
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;
			}
			else if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w)
			{
				// w 성분에 현재 뼈의 인덱스 설정
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = i;
				// w 성분에 현재 뼈의 가중치 설정
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;
			}
		}
	}

	// 메쉬에 본이 없는 경우 본 1개로 설정
	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		m_BoneIndices.push_back(pModel->Get_BoneIndex(m_szName));

		_float4x4		OffsetMatrix;
		
		// 로컬 변환은 필요하지 않기 때문에 항등행렬로 설정
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_OffsetMatrices.push_back(OffsetMatrix);
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
