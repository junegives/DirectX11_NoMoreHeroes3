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
	// ���̸� ��� �ȼ� ���� ����, ���� ���� ��������
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

	m_iNumVertexBuffers = 1;                // ���� ���� ����
	m_iVertexStride = sizeof(VTXNORTEX);    // ���� �ϳ��� ũ��
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ; // ���Ǵ� �� ������ ����

	m_iIndexStride = 4;             // �� �ε����� ũ�� (int������ 4byte) (�� 65535�� ������ ��� 2����Ʈ)
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT; // �ε��� ���� (2 or 4)
	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;  // ���Ǵ� �ε����� �� ���� (���� ���� ���� - 1) * (���� ���� ���� - 1) * 2(�� ĭ �� �ﰢ�� 2���̹Ƿ�) * 3(�ﰢ���� �� ����)
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; // �׸� ���� ���� (�ﰢ�� ����Ʈ)

#pragma region VERTEX_DATA
	/* ���� ������ ���� */
	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];           // ���� �����͸� ���� �迭 ���� �Ҵ�
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);      // ���� ������ �迭�� 0���� �ʱ�ȭ

	m_pVerticesPos = new _float3[m_iNumVertices];

	// �� ������ ��ġ�� �ؽ�ó ��ǥ ����
	for (size_t i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (size_t j = 0; j < m_iNumVerticesX; j++)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			// HeightMap�� Blue���� ���̷� �����ͼ� ����.
			// ���̸��� ȸ������, r==g==b�̴�. r,g,b ��� ���� �����͵� ������ b�� lsb ��ġ��, 10������ ��ȯ�ص� �ִ� 255��� ���� ���� ������ ����
			// & 00000000 00000000 00000000 11111111 ������ �ϸ� Blue�� ���� ����
			// �ʹ� ��ϱ� 16������ 0x000000ff
			// / 10.f ���� �ϸ� �ִ� ���̰� 25.5 (�˾Ƽ� ���� ���߱�)
			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3(j, (pPixels[iIndex] & 0x000000ff) / 10.0f, i);

			// Normal�� ä����� �鿡 ���ؼ� ������ ���͸� �������ϱ� ������, �켱��(0, 0, 0)���� ä������
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
		}
	}

	Safe_Delete_Array(pPixels);

#pragma endregion

#pragma region INDEX_DATA
	/* �ε��� ������ ���� */
	_uint* pIndices = new _uint[m_iNumIndices];                 // �ε��� �����͸� ���� �迭 ���� �Ҵ�
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);        // �ε��� ������ �迭�� 0���� �ʱ�ȭ

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

			// �� ���� ���� ���� �� ��� ���͸� ����ϱ� ���� ���� ����
			_vector vSourDir, vDestDir, vNormal;

			// �� �ε��� ���� (�ﰢ�� �� ���� �̷���� �簢��)
			pIndices[iIndicesIdx++] = iIndices[0];
			pIndices[iIndicesIdx++] = iIndices[1];
			pIndices[iIndicesIdx++] = iIndices[2];

			// �� ������ �븻 ������Ʈ
			// �ﰢ���� �� ���͸� �����Ͽ� �븻 ���͸� ���� 
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
	/* ���� ���� ���� */
	// �ε��� ���� �ܰ迡�� ������ �븻�� ������Ʈ�ϱ� ������ ���� ���� ���� �ܰ谡 �ڷ� �̷���

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;      // ���� ������ ��ü ũ�� (�� ������ ũ�� * ������ �� ����)
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;                       // ���� ������ ��� �뵵 (DEFAULT : GPU���� �а� ���� ���� (����))
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;              // ���ҽ��� ���ε� �� �� ���Ǵ� �÷��� (���� ���۷� ���)
	m_BufferDesc.CPUAccessFlags = 0;                                // CPU ���� ���� ���� (0 : CPU�� ���� ��� X)
	m_BufferDesc.MiscFlags = 0;                                     // ��Ÿ �÷��� (���� 0)
	m_BufferDesc.StructureByteStride = m_iVertexStride;             // ����ü�� ũ�� (����� �� ������ ũ��� ����)

	m_InitialData.pSysMem = pVertices;  // ���� �����͸� �ʱ� �����ͷ� ����

	// ���� ���� ���� �� �ʱ�ȭ
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	// �������� �Ҵ��� �迭 ����
	// ���� ���� �Ҵ��� ������ CreateBuffer�ÿ� ����ϴ� ������ �ƴϴ�.
	// ���� pSystem���� �ʱ� ������ ������ ���� ����ش�.
	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	/* �ε��� ���� ����*/
	// ���� ���� ���� �ܰ� ���Ŀ� ����Ǿ�� �Ѵ�.
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;	// �ε��� ������ ��ü ũ�� (�� �ε����� ũ�� * �ε����� �� ����)
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;					// �ε��� ������ ��� �뵵 (DEFAULT : GPU���� �а� ���� ����)
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;			// ���ҽ��� ���ε��� �� ���Ǵ� �÷��� (�ε��� ���۷� ���)
	m_BufferDesc.CPUAccessFlags = 0;                            // CPU������ ���� ���� ���� (0 : CPU�� ���� ��� X)
	m_BufferDesc.MiscFlags = 0;									// ��Ÿ �÷��� (���� 0)
	m_BufferDesc.StructureByteStride = 0;                       // ����ü�� ũ�� (�ε��� ���۴� ����ü�� �����Ƿ� 0) (��ä���� ��� X)

	// �ε��� �����͸� �ʱ� �����ͷ� ����
	m_InitialData.pSysMem = pIndices;

	// �ε��� ���� ���� �� �ʱ�ȭ
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	// �������� �Ҵ��� �迭 ����
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

	// �ε��� = ���� ��ǥ�� z�� * ������ ���� ���� ���� + ���� ��ǥ�� x��
	_uint		iIndex = _uint(vTargetPos.z) * m_iNumVerticesX + _uint(vTargetPos.x);

	_uint		iIndices[4] = {
		iIndex + m_iNumVerticesX,
		iIndex + m_iNumVerticesX + 1,
		iIndex + 1,
		iIndex
	};

	// Ÿ���� x��ǥ�� �ﰢ���� ù ��° �������� x ��ǥ ���� ����
	_float		fWidth = vTargetPos.x - m_pVerticesPos[iIndices[0]].x;
	// ù ��° �������� z��ǥ�� Ÿ���� z ��ǥ ���� ����
	_float		fDepth = m_pVerticesPos[iIndices[0]].z - vTargetPos.z;

	_vector		vPlane;

	// �ﰢ���� �����ϴ� �� ������ ��� ����
	/*
	XMPlaneFromPointNormal(); -> ����� �� 1���� �븻����
	XMPlaneFromPoints(); -> ����� �����ϴ� �� 3�� (�� 3���� �ִ� ������ �߿��� (�ո����� �����ϰ� ���� �ʿ��� ���� �� �ð�������� �� 3��))
	*/
	/* ������ �� �ﰢ�� */
	if (fWidth > fDepth)
	{
		vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]), XMLoadFloat3(&m_pVerticesPos[iIndices[1]]), XMLoadFloat3(&m_pVerticesPos[iIndices[2]]));
	}
	/* ���� �ϴ� �ﰢ�� */
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
