#include "VIBuffer_SwordTrail.h"

CVIBuffer_SwordTrail::CVIBuffer_SwordTrail(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_SwordTrail::CVIBuffer_SwordTrail(const CVIBuffer_SwordTrail& rhs)
    : CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_SwordTrail::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CVIBuffer_SwordTrail::Initialize(void* pArg)
{
	m_iLimitPointCount = 24;
	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iNumVertices = m_iLimitPointCount;
	m_iNumCatmullRomVertices = (m_iNumVertices - 4) * 10 + 4;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumCatmullRomVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;		//버텍스 버퍼 바인드용
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumCatmullRomVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumCatmullRomVertices);

	for (_uint i = 0; i < m_iNumCatmullRomVertices; ++i)
	{
		pVertices[i].vPosition = _float3(0.f, 0.f, 0.f);
	}

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	m_iIndexStride = 2;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_iNumIndices = (m_iNumCatmullRomVertices - 2 ) * 3;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC; /* 정적버퍼를 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;				//인덱스 버퍼용
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	/*for (_uint iIndex = 0; iIndex < m_iNumIndices - 6; iIndex += 6)
	{
		pIndices[iIndex] = iIndex + 3;
		pIndices[iIndex + 1] = iIndex + 1;
		pIndices[iIndex + 2] = iIndex;

		pIndices[iIndex + 3] = iIndex + 2;
		pIndices[iIndex + 4] = iIndex + 3;
		pIndices[iIndex + 5] = iIndex;
	}*/
	for (_uint iIndex = 0; iIndex < m_iNumCatmullRomVertices - 2; iIndex += 2)
	{
		pIndices[iNumIndices++] = iIndex + 3;
		pIndices[iNumIndices++] = iIndex + 1;
		pIndices[iNumIndices++] = iIndex;

		pIndices[iNumIndices++] = iIndex + 2;
		pIndices[iNumIndices++] = iIndex + 3;
		pIndices[iNumIndices++] = iIndex;
	}

	ZeroMemory(&m_InitialData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);


	return S_OK;
}

HRESULT CVIBuffer_SwordTrail::Update(TRAIL_DESC TrailPoint)
{
	_float3 UpperPos;
	_float3 LowerPos;

	XMStoreFloat3(&UpperPos, XMVector3TransformCoord(XMLoadFloat3(&TrailPoint.vUpperPos), XMLoadFloat4x4(&TrailPoint.vTrailMatrix)));
	XMStoreFloat3(&LowerPos, XMVector3TransformCoord(XMLoadFloat3(&TrailPoint.vLowerPos), XMLoadFloat4x4(&TrailPoint.vTrailMatrix)));

	if (!isnan(LowerPos.x))
	{
		m_TrailPoint.push_back(LowerPos);
		m_TrailPoint.push_back(UpperPos);
	}

	while (m_TrailPoint.size() < m_iLimitPointCount)
	{
		m_TrailPoint.push_back(LowerPos);
		m_TrailPoint.push_back(UpperPos);
	}
	if (m_TrailPoint.size() > m_iLimitPointCount)
	{
		m_TrailPoint.pop_front();
		m_TrailPoint.pop_front();
	}

	//VTXPOSTEX* pVertices = (VTXPOSTEX*)SubResource.pData;
	
	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	auto TrailPointIter = m_TrailPoint.begin();

	_vector		vPos[60];

	for (_uint iIndex = 0; iIndex < m_TrailPoint.size(); iIndex += 2)
	{
		vPos[iIndex] = XMVectorSetW(XMLoadFloat3(&(*TrailPointIter)), 1.f);
		/*XMStoreFloat3(&pVertices[iIndex].vPosition, XMLoadFloat3(&(*TrailPointIter)));
		XMStoreFloat2(&pVertices[iIndex].vTexcoord, XMLoadFloat2(&_float2((_float)iIndex / (m_TrailPoint.size() - 2), 1.f)));*/
		pVertices[iIndex].vPosition = *TrailPointIter;
		pVertices[iIndex].vTexcoord = _float2((_float)iIndex / (m_TrailPoint.size() - 2), 1.f);

		++TrailPointIter;

		vPos[iIndex + 1] = XMVectorSetW(XMLoadFloat3(&(*TrailPointIter)), 1.f);
		/*XMStoreFloat3(&pVertices[iIndex + 1].vPosition, XMLoadFloat3(&(*TrailPointIter)));
		XMStoreFloat2(&pVertices[iIndex + 1].vTexcoord, XMLoadFloat2(&_float2((_float)iIndex / (m_TrailPoint.size() - 2), 0.f)));*/
		pVertices[iIndex + 1].vPosition = (*TrailPointIter);
		pVertices[iIndex + 1].vTexcoord = _float2((_float)iIndex / (m_TrailPoint.size() - 2), 0.f);

		++TrailPointIter;
	}


#pragma region 보간

	//m_iCatmullRomCount = m_TrailPoint.size();
	//if (m_iCatmullRomCount >= 2)
	//{
	//XMVECTOR vec;
	//_float3	 vCat[80];

	////	if (m_TrailPoint.size() >= m_iLimitPointCount) //트레일갯수 다 채워졌을때
	////	{
	//_float4 vCatPos = { 0.f,0.f ,0.f ,0.f };

	////소드 트레일의 윗부분 보간
	//for (_int i = 19; i >= 10; i--) //보간 구간은 10으로 잡는다
	//{
	//	vec = XMVectorCatmullRom(vPos[11], vPos[9], vPos[17], vPos[19], _float((i - 10) / 20.f));
	//	XMStoreFloat4(&vCatPos, vec);
	//	/*XMStoreFloat3(&pVertices[(m_iLimitPointCount - 1 - i) * 2 + 1].vPosition, XMLoadFloat3(&_float3(vCatPos.x, vCatPos.y, vCatPos.z)));
	//	XMStoreFloat3(&vCat[(m_iLimitPointCount - 1 - i) * 2 + 1], XMLoadFloat3(&_float3(vCatPos.x, vCatPos.y, vCatPos.z)));*/
	//	pVertices[(m_iLimitPointCount - 1 - i) * 2 + 1].vPosition = _float3(vCatPos.x, vCatPos.y, vCatPos.z);
	//	vCat[(m_iLimitPointCount - 1 - i) * 2 + 1] = _float3(vCatPos.x, vCatPos.y, vCatPos.z);
	//}

	////소드 트레일의 아랫부분 보간
	//for (_int i = 19; i >= 10; i--) //보간 구간은 10으로 잡는다
	//{
	//	vec = XMVectorCatmullRom(vPos[10], vPos[12], vPos[20], vPos[18], _float((i - 10) / 20.f));
	//	XMStoreFloat4(&vCatPos, vec);
	//	/*XMStoreFloat3(&pVertices[(m_iLimitPointCount - 1 - i) * 2].vPosition, XMLoadFloat3(&_float3(vCatPos.x, vCatPos.y, vCatPos.z)));
	//	XMStoreFloat3(&vCat[(m_iLimitPointCount - 1 - i) * 2], XMLoadFloat3(&_float3(vCatPos.x, vCatPos.y, vCatPos.z)));*/
	//	pVertices[(m_iLimitPointCount - 1 - i) * 2].vPosition = _float3(vCatPos.x, vCatPos.y, vCatPos.z);
	//	vCat[(m_iLimitPointCount - 1 - i) * 2] = _float3(vCatPos.x, vCatPos.y, vCatPos.z);
	//}

	/*D3D11_MAPPED_SUBRESOURCE		SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));*/
	//VTXPOSTEX* pCatmallomVertices = (VTXPOSTEX*)SubResource.pData;

	vector<VTXPOSTEX> pCatmallomVertices;
	pCatmallomVertices.reserve(m_iNumCatmullRomVertices);

	/*VTXPOSTEX* pCatmallomVertices = new VTXPOSTEX[(m_iNumVertices - 2) * 10];
	ZeroMemory(pCatmallomVertices, sizeof(VTXPOSTEX) * (m_iNumVertices - 2) * 10);*/

	_uint iNumVertices = { 0 };

	for (_uint i = 0; i < m_iNumVertices; i += 2)
	{
		if (i >= m_iNumVertices - 4)
		{
			pCatmallomVertices.push_back(pVertices[i]);
			pCatmallomVertices.push_back(pVertices[i + 1]);
			continue;
		}

		VTXPOSTEX vHigh0, vHigh1, vHigh2, vHigh3;
		VTXPOSTEX vLow0, vLow1, vLow2, vLow3;

		if (i == 0)
		{
			vHigh0 = pVertices[i];
			vLow0 = pVertices[i + 1];
		}
		else
		{
			vHigh0 = pVertices[i - 2];
			vLow0 = pVertices[i - 1];
		}

		vHigh1 = pVertices[i];
		vHigh2 = pVertices[i + 2];
		vHigh3 = pVertices[i + 4];


		vLow1 = pVertices[i + 1];
		vLow2 = pVertices[i + 3];
		vLow3 = pVertices[i + 5];

		for (_uint j = 0; j < 10; ++j)
		{
			VTXPOSTEX VertexLow = {};
			XMStoreFloat3(&VertexLow.vPosition, XMVectorCatmullRom(
				XMLoadFloat3(&vLow0.vPosition)
				, XMLoadFloat3(&vLow1.vPosition)
				, XMLoadFloat3(&vLow2.vPosition)
				, XMLoadFloat3(&vLow3.vPosition)
				, j * 0.1f));

			XMStoreFloat2(&VertexLow.vTexcoord, XMVectorCatmullRom(
				XMLoadFloat2(&vLow0.vTexcoord)
				, XMLoadFloat2(&vLow1.vTexcoord)
				, XMLoadFloat2(&vLow2.vTexcoord)
				, XMLoadFloat2(&vLow3.vTexcoord)
				, j * 0.1f));

			pCatmallomVertices.push_back(VertexLow);

			VTXPOSTEX VertexHigh = {};
			XMStoreFloat3(&VertexHigh.vPosition, XMVectorCatmullRom(
				XMLoadFloat3(&vHigh0.vPosition)
				, XMLoadFloat3(&vHigh1.vPosition)
				, XMLoadFloat3(&vHigh2.vPosition)
				, XMLoadFloat3(&vHigh3.vPosition)
				, j * 0.1f));

			XMStoreFloat2(&VertexHigh.vTexcoord, XMVectorCatmullRom(
				XMLoadFloat2(&vHigh0.vTexcoord)
				, XMLoadFloat2(&vHigh1.vTexcoord)
				, XMLoadFloat2(&vHigh2.vTexcoord)
				, XMLoadFloat2(&vHigh3.vTexcoord)
				, j * 0.1f));

			pCatmallomVertices.push_back(VertexHigh);
		}

	}

	/*for (_uint i = m_iLimitPointCount - 6; i < m_iLimitPointCount; i += 2)
	{
		pCatmallomVertices[iNumVertices++] = pVertices[i];
		iNumVertices++;
		pCatmallomVertices[iNumVertices++] = pVertices[i + 1];
	}*/

	//	}
	//}

	/*m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);*/


#pragma endregion 전체 구간 보간

	D3D11_MAPPED_SUBRESOURCE SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	m_pContext->Map(m_pVB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	memcpy(SubResource.pData, pCatmallomVertices.data(), sizeof(VTXPOSTEX) * pCatmallomVertices.size());
	m_pContext->Unmap(m_pVB.Get(), 0);

	//m_pContext->Map(m_pVB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	//memcpy(SubResource.pData, pCatmallomVertices.data(), sizeof(VTXPOSTEX) * pCatmallomVertices.size());
	//// Unmaping :
	//m_pContext->Unmap(m_pVB.Get(), 0);
	/*D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	m_pContext->Map(m_pVB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	m_pContext->Unmap(m_pVB.Get(), 0);*/

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CVIBuffer_SwordTrail::Clear()
{
	m_TrailPoint.clear();

	return S_OK;
}

shared_ptr<CVIBuffer_SwordTrail> CVIBuffer_SwordTrail::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CVIBuffer_SwordTrail> pInstance = make_shared<CVIBuffer_SwordTrail>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_SwordTrail");
		pInstance.reset();
	}

	return pInstance;
}

shared_ptr<CComponent> CVIBuffer_SwordTrail::Clone(void* pArg)
{
	shared_ptr<CVIBuffer_SwordTrail> pInstance = make_shared<CVIBuffer_SwordTrail>(*this);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_SwordTrail");
		pInstance.reset();
	}

	return pInstance;
}

void CVIBuffer_SwordTrail::Free()
{
	__super::Free();
}
