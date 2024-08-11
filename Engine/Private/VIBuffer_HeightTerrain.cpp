#include "VIBuffer_HeightTerrain.h"

CVIBuffer_HeightTerrain::CVIBuffer_HeightTerrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_HeightTerrain::CVIBuffer_HeightTerrain(const CVIBuffer_HeightTerrain& rhs)
    : CVIBuffer(rhs)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
{
}

CVIBuffer_HeightTerrain::~CVIBuffer_HeightTerrain()
{
	Free();
}

HRESULT CVIBuffer_HeightTerrain::Initialize_Prototype(const wstring& strHeightMapFilePath)
{
	// 높이맵 열어서 픽셀 가로 개수, 세로 개수 가져오기
	_ulong		dwByte = { 0 };
	HANDLE		hFile = CreateFile(strHeightMapFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	BITMAPFILEHEADER		fh;
	ReadFile(hFile, &fh, sizeof(fh), &dwByte, nullptr);

	BITMAPINFOHEADER		ih;
	ReadFile(hFile, &ih, sizeof(ih), &dwByte, nullptr);

	_uint* pPixels = new _uint[ih.biWidth * ih.biHeight];
	ReadFile(hFile, pPixels, sizeof(_uint) * ih.biWidth * ih.biHeight, &dwByte, nullptr);

	CloseHandle(hFile);

	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;

	m_iNumVertexBuffers = 1;                // 정점 버퍼 개수
	m_iVertexStride = sizeof(VTXNORTEX);    // 정점 하나의 크기
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ; // 사용되는 총 정점의 개수

	m_iIndexStride = 4;             // 각 인덱스의 크기 (int형으로 4byte) (약 65535개 이하인 경우 2바이트)
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT; // 인덱스 형식 (2 or 4)
	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;  // 사용되는 인덱스의 총 개수 (가로 정점 개수 - 1) * (세로 정점 개수 - 1) * 2(한 칸 당 삼각형 2개이므로) * 3(삼각형의 세 정점)
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; // 그릴 형태 설정 (삼각형 리스트)

#pragma region VERTEX_DATA
	/* 정점 데이터 생성 */
	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];           // 정점 데이터를 담을 배열 동적 할당
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);      // 정점 데이터 배열을 0으로 초기화

	m_pVerticesPos = new _float3[m_iNumVertices];

	// 각 정점의 위치와 텍스처 좌표 설정
	for (size_t i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (size_t j = 0; j < m_iNumVerticesX; j++)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			// HeightMap의 Blue값을 높이로 가져와서 쓴다.
			// 높이맵은 회색으로, r==g==b이다. r,g,b 어떠한 값을 가져와도 되지만 b가 lsb 위치로, 10진수로 변환해도 최대 255라는 작은 값이 나오기 때문
			// & 00000000 00000000 00000000 11111111 연산을 하면 Blue값 추출 가능
			// 너무 기니까 16진수로 0x000000ff
			// / 10.f 정도 하면 최대 높이가 25.5 (알아서 비율 맞추기)
			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3(j, (pPixels[iIndex] & 0x000000ff) / 10.0f, i);

			// Normal을 채우려면 면에 대해서 수직인 벡터를 만들어야하기 때문에, 우선은(0, 0, 0)으로 채워놓음
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
		}
	}

	Safe_Delete_Array(pPixels);

#pragma endregion

#pragma region INDEX_DATA
	/* 인덱스 데이터 생성 */
	_uint* pIndices = new _uint[m_iNumIndices];                 // 인덱스 데이터를 담을 배열 동적 할당
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);        // 인덱스 데이터 배열을 0으로 초기화

	_uint       iIndicesIdx = { 0 };

	for (size_t i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			_uint iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			// 두 벡터 간의 방향 및 노멀 벡터를 계산하기 위한 벡터 변수
			_vector vSourDir, vDestDir, vNormal;

			// 각 인덱스 설정 (삼각형 두 개로 이루어진 사각형)
			pIndices[iIndicesIdx++] = iIndices[0];
			pIndices[iIndicesIdx++] = iIndices[1];
			pIndices[iIndicesIdx++] = iIndices[2];

			// 각 정점의 노말 업데이트
			// 삼각형의 두 벡터를 외적하여 노말 벡터를 구함 
			vSourDir = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDestDir = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal));


			pIndices[iIndicesIdx++] = iIndices[0];
			pIndices[iIndicesIdx++] = iIndices[2];
			pIndices[iIndicesIdx++] = iIndices[3];

			vSourDir = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDestDir = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal));
		}
	}

#pragma endregion

#pragma region VERTEX_BUFFER
	/* 정점 버퍼 생성 */
	// 인덱스 생성 단계에서 정점의 노말을 업데이트하기 때문에 정점 버퍼 생성 단계가 뒤로 미뤄짐

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;      // 정점 버퍼의 전체 크기 (각 정점의 크기 * 정점의 총 개수)
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;                       // 정점 버퍼의 사용 용도 (DEFAULT : GPU에서 읽고 쓰기 가능 (정적))
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;              // 리소스와 바인딩 할 때 사용되는 플래그 (정점 버퍼로 사용)
	m_BufferDesc.CPUAccessFlags = 0;                                // CPU 접근 권한 지정 (0 : CPU의 접근 허용 X)
	m_BufferDesc.MiscFlags = 0;                                     // 기타 플래그 (보통 0)
	m_BufferDesc.StructureByteStride = m_iVertexStride;             // 구조체의 크기 (현재는 각 정점의 크기와 동일)

	m_InitialData.pSysMem = pVertices;  // 정점 데이터를 초기 데이터로 설정

	// 정점 버퍼 생성 및 초기화
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	// 동적으로 할당한 배열 해제
	// 내가 직접 할당한 공간이 CreateBuffer시에 사용하는 공간이 아니다.
	// 따라서 pSystem으로 초기 데이터 공간을 따로 잡아준다.
	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	/* 인덱스 버퍼 생성*/
	// 정점 버퍼 생성 단계 이후에 진행되어야 한다.
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;	// 인덱스 버퍼의 전체 크기 (각 인덱스의 크기 * 인덱스의 총 개수)
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;					// 인덱스 버퍼의 사용 용도 (DEFAULT : GPU에서 읽고 쓰기 가능)
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;			// 리소스와 바인딩할 때 사용되는 플래그 (인덱스 버퍼로 사용)
	m_BufferDesc.CPUAccessFlags = 0;                            // CPU에서의 접근 권한 지정 (0 : CPU의 접근 허용 X)
	m_BufferDesc.MiscFlags = 0;									// 기타 플래그 (보통 0)
	m_BufferDesc.StructureByteStride = 0;                       // 구조체의 크기 (인덱스 버퍼는 구조체가 없으므로 0) (안채워도 상관 X)

	// 인덱스 데이터를 초기 데이터로 설정
	m_InitialData.pSysMem = pIndices;

	// 인덱스 버퍼 생성 및 초기화
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	// 동적으로 할당한 배열 해제
	Safe_Delete_Array(pIndices);

#pragma endregion

    return S_OK;
}

HRESULT CVIBuffer_HeightTerrain::Initialize(void* pArg)
{
    return S_OK;
}

_float CVIBuffer_HeightTerrain::Compute_Height(_fvector vLocalPos)
{
	_float3		vTargetPos;
	XMStoreFloat3(&vTargetPos, vLocalPos);

	// 인덱스 = 로컬 좌표의 z값 * 지형의 가로 정점 개수 + 로컬 좌표의 x값
	_uint		iIndex = _uint(vTargetPos.z) * m_iNumVerticesX + _uint(vTargetPos.x);

	_uint		iIndices[4] = {
		iIndex + m_iNumVerticesX,
		iIndex + m_iNumVerticesX + 1,
		iIndex + 1,
		iIndex
	};

	// 타겟의 x좌표와 삼각형의 첫 번째 꼭지점의 x 좌표 간의 차이
	_float		fWidth = vTargetPos.x - m_pVerticesPos[iIndices[0]].x;
	// 첫 번째 꼭지점의 z좌표와 타겟의 z 좌표 간의 차이
	_float		fDepth = m_pVerticesPos[iIndices[0]].z - vTargetPos.z;

	_vector		vPlane;

	// 삼각형을 구성하는 세 점으로 평면 생성
	/*
	XMPlaneFromPointNormal(); -> 평면의 점 1개와 노말벡터
	XMPlaneFromPoints(); -> 평면을 만족하는 점 3개 (점 3개를 넣는 순서가 중요함 (앞면으로 정의하고 싶은 쪽에서 봤을 때 시계방향으로 점 3개))
	*/
	/* 오른쪽 위 삼각형 */
	if (fWidth > fDepth)
	{
		vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]), XMLoadFloat3(&m_pVerticesPos[iIndices[1]]), XMLoadFloat3(&m_pVerticesPos[iIndices[2]]));
	}
	/* 왼쪽 하단 삼각형 */
	else
	{
		vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]), XMLoadFloat3(&m_pVerticesPos[iIndices[2]]), XMLoadFloat3(&m_pVerticesPos[iIndices[3]]));
	}

	// ax + by + cz + d = 0

	// y = (-ax - cz - d) / b

	return (-vPlane.m128_f32[0] * vTargetPos.x - vPlane.m128_f32[2] * vTargetPos.z - vPlane.m128_f32[3]) / vPlane.m128_f32[1];
}

_bool CVIBuffer_HeightTerrain::Picking(_long iMouseX, _long iMouseY, _matrix matWorld, _float3* vPickPoint, _float* fMinDist)
{
	return _bool();
}

shared_ptr<CVIBuffer_HeightTerrain> CVIBuffer_HeightTerrain::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strHeightMapFilePath)
{
    shared_ptr<CVIBuffer_HeightTerrain> pInstance = make_shared<CVIBuffer_HeightTerrain>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(strHeightMapFilePath)))
    {
        MSG_BOX("Failed to Created : CVIBuffer_HeightTerrain");
        pInstance.reset();
    }

    return pInstance;
}

shared_ptr<CComponent> CVIBuffer_HeightTerrain::Clone(void* pArg)
{
    shared_ptr<CVIBuffer_HeightTerrain> pInstance = make_shared<CVIBuffer_HeightTerrain>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CVIBuffer_HeightTerrain");
        pInstance.reset();
    }

    return pInstance;
}

void CVIBuffer_HeightTerrain::Free()
{
	__super::Free();
}
