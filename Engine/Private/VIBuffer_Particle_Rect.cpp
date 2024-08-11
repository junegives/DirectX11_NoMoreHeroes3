#include "VIBuffer_Particle_Rect.h"

CVIBuffer_Particle_Rect::CVIBuffer_Particle_Rect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CVIBuffer_Instancing(pDevice, pContext)
{
}

CVIBuffer_Particle_Rect::CVIBuffer_Particle_Rect(const CVIBuffer_Particle_Rect& rhs)
    : CVIBuffer_Instancing(rhs)
{
}

HRESULT CVIBuffer_Particle_Rect::Initialize_Prototype(_uint iNumInstance)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumVertexBuffers = 2;				// 정점 버퍼의 개수 설정 (2개의 정점 버퍼 사용)
	m_iVertexStride = sizeof(VTXPOSTEX);	// 정점 하나의 크기
	m_iNumVertices = 4;						// 정점의 총 개수 (사각형 형성을 위해 4개의 정점 필요)

	m_iNumInstance = iNumInstance;		// m_iNumInstance : 그릴 (네모) 개수
	m_iIndexCountPerInstance = 6;		// m_iIndexCountPerInstance : 인스턴스 하나당 몇 개 그릴건지 (사용할 인덱스의 개수, 사각형을 형성하기 위해 6개의 인덱스)
	m_iIndexStride = 2;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_iNumIndices = m_iIndexCountPerInstance * m_iNumInstance;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	/* 정점버퍼를 만든다.*/
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexcoord = _float2(0.f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexcoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexcoord = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexcoord = _float2(0.f, 1.f);

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	/* 인덱스버퍼를 만든다.*/
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	_uint			iNumIndices = { 0 };

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		pIndices[iNumIndices++] = 0;
		pIndices[iNumIndices++] = 1;
		pIndices[iNumIndices++] = 2;

		pIndices[iNumIndices++] = 0;
		pIndices[iNumIndices++] = 2;
		pIndices[iNumIndices++] = 3;
	}

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Particle_Rect::Initialize(void* pArg)
{
#pragma region INSTANCE_BUFFER

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_iInstanceStride = sizeof(VTXINSTANCE);

	/* 정점버퍼를 만든다.*/
	m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXINSTANCE* pVertices = new VTXINSTANCE[m_iNumInstance];
	ZeroMemory(pVertices, sizeof(VTXINSTANCE) * m_iNumInstance);

	uniform_real_distribution<float>	WidthRange(m_InstanceData.vRange.x * -0.5f, m_InstanceData.vRange.x * 0.5f);
	uniform_real_distribution<float>	HeightRange(m_InstanceData.vRange.y * -0.5f, m_InstanceData.vRange.y * 0.5f);
	uniform_real_distribution<float>	DepthRange(m_InstanceData.vRange.z * -0.5f, m_InstanceData.vRange.z * 0.5f);
	uniform_real_distribution<float>	SizeRange(m_InstanceData.vSize.x, m_InstanceData.vSize.y);



	/*uniform_real_distribution<float>	RandomRange(0.1f, m_InstancingDesc.fRange);
	uniform_real_distribution<float>	RandomRotation(0.0f, XMConvertToRadians(360.0f));
	uniform_real_distribution<float>	RandomScale(m_InstancingDesc.vScale.x, m_InstancingDesc.vScale.y);
	uniform_real_distribution<float>	RandomSpeed(m_InstancingDesc.vSpeed.x, m_InstancingDesc.vSpeed.y);
	uniform_real_distribution<float>	RandomLifeTime(m_InstancingDesc.vLifeTime.x, m_InstancingDesc.vLifeTime.y);*/


	// 각 파티클 인스턴스의 속성을 무작위로 생성하고 인스턴스 버퍼에 저장
	// 위치, 크기 등의 속성을 무작위로 생성
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		_float		fSize = SizeRange(m_RandomNumber);

		pVertices[i].vRight = _float4(fSize, 0.f, 0.f, 0.f);
		pVertices[i].vUp = _float4(0.f, fSize, 0.f, 0.f);
		pVertices[i].vLook = _float4(0.f, 0.f, fSize, 0.f);
		pVertices[i].vTranslation = _float4(
			m_InstanceData.vCenter.x + WidthRange(m_RandomNumber),
			m_InstanceData.vCenter.y + HeightRange(m_RandomNumber),
			m_InstanceData.vCenter.z + DepthRange(m_RandomNumber),
			1.f);

		pVertices[i].vColor = m_InstanceData.vColor;
	}

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);


#pragma endregion

	return S_OK;
}

_bool CVIBuffer_Particle_Rect::Picking(_long iMouseX, _long iMouseY, _matrix matWorld, _float3* vPickPoint, _float* fMinDist)
{
	return _bool();
}

shared_ptr<CVIBuffer_Particle_Rect> CVIBuffer_Particle_Rect::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iNumInstance)
{
	shared_ptr<CVIBuffer_Particle_Rect> pInstance = make_shared<CVIBuffer_Particle_Rect>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumInstance)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Particle_Rect");
		pInstance.reset();
	}

	return pInstance;
}

shared_ptr<CComponent> CVIBuffer_Particle_Rect::Clone(void* pArg)
{
	shared_ptr<CVIBuffer_Particle_Rect> pInstance = make_shared<CVIBuffer_Particle_Rect>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Particle_Rect");
		pInstance.reset();
	}

	return pInstance;
}

void CVIBuffer_Particle_Rect::Free()
{
	__super::Free();
}
