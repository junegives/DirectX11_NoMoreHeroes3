#include "VIBuffer_Particle_Point_UI.h"

CVIBuffer_Particle_Point_UI::CVIBuffer_Particle_Point_UI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CVIBuffer_Instancing(pDevice, pContext)
{
}

CVIBuffer_Particle_Point_UI::CVIBuffer_Particle_Point_UI(const CVIBuffer_Particle_Point_UI& rhs)
    : CVIBuffer_Instancing(rhs)
{
}

HRESULT CVIBuffer_Particle_Point_UI::Initialize_Prototype(_uint iNumInstance)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumVertexBuffers = 2;
	m_iVertexStride = sizeof(VTXPOINT);
	m_iNumVertices = 1;

	m_iNumInstance = iNumInstance;
	m_iIndexCountPerInstance = 1;
	m_iIndexStride = 2;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_iNumIndices = m_iIndexCountPerInstance * m_iNumInstance;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region VERTEX_BUFFER
	/* 정점버퍼를 만든다.*/
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXPOINT* pVertices = new VTXPOINT[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iNumVertices);

	pVertices[0].vPosition = _float3(0.0f, 0.0f, 0.f);
	pVertices[0].vPSize = _float2(1.f, 1.f);

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

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Particle_Point_UI::Initialize(void* pArg)
{
#pragma region INSTANCE_BUFFER

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eUIType = ((INSTANCE_UI_DESC*)pArg)->eUIType;

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


	for (_int i = 0; i < m_iNumInstance; ++i)
	{
		_float		fSize = SizeRange(m_RandomNumber);

		pVertices[i].vRight = _float4(fSize, 0.f, 0.f, 0.f);
		pVertices[i].vUp = _float4(0.f, fSize, 0.f, 0.f);
		pVertices[i].vLook = _float4(0.f, 0.f, fSize, 0.f);
		/*pVertices[i].vTranslation = _float4(
			m_InstanceData.vCenter.x + WidthRange(m_RandomNumber),
			m_InstanceData.vCenter.y + HeightRange(m_RandomNumber),
			m_InstanceData.vCenter.z + DepthRange(m_RandomNumber),
			1.f);*/

		switch (m_eUIType)
		{
		case Engine::CVIBuffer_Particle_Point_UI::UI_PLAYERHP:

			if (0 <= i  && 3 > i)
			{
				pVertices[i].vTranslation = _float4(
					m_InstanceData.vCenter.x - m_InstanceData.vSize.x * (3 - i) - (3 - i),
					m_InstanceData.vCenter.y + m_InstanceData.vSize.y * 4 + 4,
					m_InstanceData.vCenter.z,
					1.f);
			}

			else if (3 <= i && 6 > i)
			{
				pVertices[i].vTranslation = _float4(
					m_InstanceData.vCenter.x + m_InstanceData.vSize.x * (i - 2) + (i - 2),
					m_InstanceData.vCenter.y + m_InstanceData.vSize.y * 4 + 4,
					m_InstanceData.vCenter.z,
					1.f);
			}

			else if (6 <= i && 15 > i)
			{
				pVertices[i].vTranslation = _float4(
					m_InstanceData.vCenter.x - m_InstanceData.vSize.x * (4 - (i - 6)) - (4 - (i - 6)),
					m_InstanceData.vCenter.y + m_InstanceData.vSize.y * 3 + 3,
					m_InstanceData.vCenter.z,
					1.f);
			}

			else if (15 <= i && 24 > i)
			{
				pVertices[i].vTranslation = _float4(
					m_InstanceData.vCenter.x - m_InstanceData.vSize.x * (4 - (i - 15)) - (4 - (i - 15)),
					m_InstanceData.vCenter.y + m_InstanceData.vSize.y * 2 + 2,
					m_InstanceData.vCenter.z,
					1.f);
			}

			else if (24 <= i && 33 > i)
			{
				pVertices[i].vTranslation = _float4(
					m_InstanceData.vCenter.x - m_InstanceData.vSize.x * (4 - (i - 24)) - (4 - (i - 24)),
					m_InstanceData.vCenter.y + m_InstanceData.vSize.y * 1 + 1,
					m_InstanceData.vCenter.z,
					1.f);
			}

			else if (33 <= i && 42 > i)
			{
				pVertices[i].vTranslation = _float4(
					m_InstanceData.vCenter.x - m_InstanceData.vSize.x * (4 - (i - 33)) - (4 - (i - 33)),
					m_InstanceData.vCenter.y,
					m_InstanceData.vCenter.z,
					1.f);
			}

			else if (42 <= i && 49 > i)
			{
				pVertices[i].vTranslation = _float4(
					m_InstanceData.vCenter.x - m_InstanceData.vSize.x * (4 - (i - 41)) - (4 - (i - 41)),
					m_InstanceData.vCenter.y - m_InstanceData.vSize.y * 1 - 1,
					m_InstanceData.vCenter.z,
					1.f);
			}

			else if (49 <= i && 54 > i)
			{
				pVertices[i].vTranslation = _float4(
					m_InstanceData.vCenter.x - m_InstanceData.vSize.x * (4 - (i - 47)) - (4 - (i - 47)),
					m_InstanceData.vCenter.y - m_InstanceData.vSize.y * 2 - 2,
					m_InstanceData.vCenter.z,
					1.f);
			}

			else if (54 <= i && 57 > i)
			{
				pVertices[i].vTranslation = _float4(
					m_InstanceData.vCenter.x - m_InstanceData.vSize.x * (4 - (i - 51)) - (4 - (i - 51)),
					m_InstanceData.vCenter.y - m_InstanceData.vSize.y * 3 - 3,
					m_InstanceData.vCenter.z,
					1.f);
			}

			else if (57 == i)
			{
				pVertices[i].vTranslation = _float4(
					m_InstanceData.vCenter.x,
					m_InstanceData.vCenter.y - m_InstanceData.vSize.y * 4 - 4,
					m_InstanceData.vCenter.z,
					1.f);
			}
			else
			{
				pVertices[i].vColor.w = 0.f;
			}
			break;

		case Engine::CVIBuffer_Particle_Point_UI::UI_MONSTERHP:

			if (0 == i)
			{
				pVertices[i].vTranslation = _float4(
					m_InstanceData.vCenter.x - m_InstanceData.vSize.x * (1 - i) - (1 - i),
					m_InstanceData.vCenter.y + m_InstanceData.vSize.y * 2 + 2,
					m_InstanceData.vCenter.z,
					1.f);
			}

			else if (1 == i)
			{
				pVertices[i].vTranslation = _float4(
					m_InstanceData.vCenter.x + m_InstanceData.vSize.x * i + i,
					m_InstanceData.vCenter.y + m_InstanceData.vSize.y * 2 + 2,
					m_InstanceData.vCenter.z,
					1.f);
			}

			else if (2 <= i && 7 > i)
			{
				pVertices[i].vTranslation = _float4(
					m_InstanceData.vCenter.x - m_InstanceData.vSize.x * (2 - (i - 2)) - (2 - (i - 2)),
					m_InstanceData.vCenter.y + m_InstanceData.vSize.y * 1 + 1,
					m_InstanceData.vCenter.z,
					1.f);
			}

			else if (7 <= i && 12 > i)
			{
				pVertices[i].vTranslation = _float4(
					m_InstanceData.vCenter.x - m_InstanceData.vSize.x * (2 - (i - 7)) - (2 - (i - 7)),
					m_InstanceData.vCenter.y,
					m_InstanceData.vCenter.z,
					1.f);
			}

			else if (12 <= i && 15 > i)
			{
				pVertices[i].vTranslation = _float4(
					m_InstanceData.vCenter.x - m_InstanceData.vSize.x * (2 - (i - 11)) - (2 - (i - 11)),
					m_InstanceData.vCenter.y - m_InstanceData.vSize.y * 1 - 1,
					m_InstanceData.vCenter.z,
					1.f);
			}

			else if (15 == i)
			{
				pVertices[i].vTranslation = _float4(
					m_InstanceData.vCenter.x,
					m_InstanceData.vCenter.y - m_InstanceData.vSize.y * 2 - 2,
					m_InstanceData.vCenter.z,
					1.f);
			}
			else
			{
				pVertices[i].vColor.w = 0.f;
			}
			break;

		case Engine::CVIBuffer_Particle_Point_UI::UI_BOSSHP:

			pVertices[i].vTranslation = _float4(
				m_InstanceData.vCenter.x + m_InstanceData.vSize.x * (-i / 3 + 25) + (-i / 3 + 25),
				m_InstanceData.vCenter.y + m_InstanceData.vSize.y * (1 - i % 3) + (1 - i % 3),
				m_InstanceData.vCenter.z,
				1.f);

			break;

		case Engine::CVIBuffer_Particle_Point_UI::UI_TARGET:

			// 원 만들기
			pVertices[i].vTranslation = _float4(
				m_InstanceData.vCenter.x + cosf(360 / m_iNumInstance * i) * 20.f,
				m_InstanceData.vCenter.y + sinf(360 / m_iNumInstance * i) * 20.f,
				m_InstanceData.vCenter.z,
				1.f);

			break;
		case Engine::CVIBuffer_Particle_Point_UI::UI_SLASH:
			break;
		default:
			break;
		}

		pVertices[i].vColor = m_InstanceData.vColor;
	}

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);


#pragma endregion

	return S_OK;
}

_bool CVIBuffer_Particle_Point_UI::Picking(_long iMouseX, _long iMouseY, _matrix matWorld, _float3* vPickPoint, _float* fMinDist)
{
	return _bool();
}

shared_ptr<CVIBuffer_Particle_Point_UI> CVIBuffer_Particle_Point_UI::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iNumInstance)
{
	shared_ptr<CVIBuffer_Particle_Point_UI> pInstance = make_shared<CVIBuffer_Particle_Point_UI>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumInstance)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Particle_Point_UI");
		pInstance.reset();
	}

	return pInstance;
}

shared_ptr<CComponent> CVIBuffer_Particle_Point_UI::Clone(void* pArg)
{
	shared_ptr<CVIBuffer_Particle_Point_UI> pInstance = make_shared<CVIBuffer_Particle_Point_UI>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Particle_Point_UI");
		pInstance.reset();
	}

	return pInstance;
}

void CVIBuffer_Particle_Point_UI::Free()
{
	__super::Free();
}
