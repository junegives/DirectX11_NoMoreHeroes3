#include "VIBuffer_Instancing.h"

CVIBuffer_Instancing::CVIBuffer_Instancing(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Instancing::CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs)
	: CVIBuffer(rhs)
	, m_pVBInstance(rhs.m_pVBInstance)
	, m_iInstanceStride(rhs.m_iInstanceStride)
	, m_iNumInstance(rhs.m_iNumInstance)
	, m_iIndexCountPerInstance(rhs.m_iIndexCountPerInstance)
	, m_RandomNumber(rhs.m_RandomNumber)
	, m_pSpeed(rhs.m_pSpeed)
{
}

HRESULT CVIBuffer_Instancing::Initialize_Prototype()
{
	// ���� �ѹ� ������ �ʱ�ȭ
	m_RandomNumber = mt19937_64(m_RandomDevice());

	// ��ü�� ����(���)�� ��� �ν��Ͻ� ���۴� Ŭ�еǸ� �ȵȴ�. (m_InstanceData)
	// -> ������Ʈ Ǯ��(��ƼŬ � �������� �����ؼ� �̸� ����)

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Initialize(void* pArg)
{
	// �ν��Ͻ� ��, �ӵ� ���� ������ �޾ƿͼ� �ν��Ͻ��� ���� ���� ����
	m_InstanceData = *(INSTANCE_DESC*)pArg;

	m_pSpeed	= new _float[m_iNumInstance];
	m_pLifeTime = new _float[m_iNumInstance];
	m_vDir		= new _float3[m_iNumInstance];

	// ��ü�� �ӵ��� ���� ������ ���� ���� �� ����
	// �Ǽ���(float) ������ ���� ������ ������ ����
	uniform_real_distribution<float>	SpeedRange(m_InstanceData.vSpeed.x, m_InstanceData.vSpeed.y);
	uniform_real_distribution<float>	TimeRange(m_InstanceData.vLifeTime.x, m_InstanceData.vLifeTime.y);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		m_pSpeed[i] = SpeedRange(m_RandomNumber);
		m_pLifeTime[i] = TimeRange(m_RandomNumber);
	}

	m_isFinished = false;

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Render()
{
	// �ν��Ͻ��� ����ϱ� ������ DrawIndexedInstanced �Լ��� ����Ͽ� ���� ���� ��ü�� ������
	// render ��� DrawIndexedInstanced �Լ� ���
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Bind_Buffers()
{
	// ���� ���ۿ� �ν��Ͻ� ���۸� �Բ� ����
	ComPtr<ID3D11Buffer> pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance
	};

	_uint				iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceStride	// ���(m_pVBInstance?) �ϳ��� ������ m_iInstanceStride
	};

	_uint				iOffsets[] = {
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers->GetAddressOf(), iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB.Get(), m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}

void CVIBuffer_Instancing::Reset(_vector vCenter, _vector vDir)
{
	m_isFinished = false;
	m_fTimeAcc = 0.f;

	XMStoreFloat3(&m_InstanceData.vDirRange, vDir);

	VTXINSTANCE* pVertices = new VTXINSTANCE[m_iNumInstance];
	ZeroMemory(pVertices, sizeof(VTXINSTANCE) * m_iNumInstance);

	uniform_real_distribution<float>	WidthRange(m_InstanceData.vRange.x * -0.5f, m_InstanceData.vRange.x * 0.5f);
	uniform_real_distribution<float>	HeightRange(m_InstanceData.vRange.y * -0.5f, m_InstanceData.vRange.y * 0.5f);
	uniform_real_distribution<float>	DepthRange(m_InstanceData.vRange.z * -0.5f, m_InstanceData.vRange.z * 0.5f);
	uniform_real_distribution<float>	SizeRange(m_InstanceData.vSize.x, m_InstanceData.vSize.y);
	uniform_real_distribution<float>	DirX, DirY, DirZ;

	if (m_InstanceData.vDirRange.x < 0.f)
		DirX = uniform_real_distribution<float>(m_InstanceData.vDirRange.x * m_InstanceData.vDirX.y, m_InstanceData.vDirRange.x * m_InstanceData.vDirX.x);
	else
		DirX = uniform_real_distribution<float>(m_InstanceData.vDirRange.x * m_InstanceData.vDirX.x, m_InstanceData.vDirRange.x * m_InstanceData.vDirX.y);

	if (m_InstanceData.vDirRange.y < 0.f)
		DirY = uniform_real_distribution<float>(m_InstanceData.vDirRange.y * m_InstanceData.vDirY.y, m_InstanceData.vDirRange.y * m_InstanceData.vDirY.x);
	else if (m_InstanceData.vDirRange.y > 0.f)
		DirY = uniform_real_distribution<float>(m_InstanceData.vDirRange.y * m_InstanceData.vDirY.x, m_InstanceData.vDirRange.y * m_InstanceData.vDirY.y);
	else
		DirY = uniform_real_distribution<float>(m_InstanceData.vDirRange.y + m_InstanceData.vDirY.x, m_InstanceData.vDirRange.y + m_InstanceData.vDirY.y);

	if (m_InstanceData.vDirRange.z < 0.f)	
		DirZ = uniform_real_distribution<float>(m_InstanceData.vDirRange.z * m_InstanceData.vDirZ.y, m_InstanceData.vDirRange.z * m_InstanceData.vDirZ.x);
	else
		DirZ = uniform_real_distribution<float>(m_InstanceData.vDirRange.z * m_InstanceData.vDirZ.x, m_InstanceData.vDirRange.z * m_InstanceData.vDirZ.y);


	// ��ü�� �ӵ��� ���� ������ ���� ���� �� ����
	// �Ǽ���(float) ������ ���� ������ ������ ����
	uniform_real_distribution<float>	SpeedRange(m_InstanceData.vSpeed.x, m_InstanceData.vSpeed.y);
	uniform_real_distribution<float>	TimeRange(m_InstanceData.vLifeTime.x, m_InstanceData.vLifeTime.y);

	m_fMaxSpeed = m_InstanceData.vSpeed.y;
	m_fMaxLifeTime = m_InstanceData.vLifeTime.y;

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		m_pSpeed[i] = SpeedRange(m_RandomNumber);
		m_pLifeTime[i] = TimeRange(m_RandomNumber);
		if (m_InstanceData.isSetDir)	m_vDir[i] = {DirX(m_RandomNumber), DirY(m_RandomNumber), DirZ(m_RandomNumber)};
		XMStoreFloat3(&m_InstanceData.vCenter, vCenter);
		_float		fSize = SizeRange(m_RandomNumber);

		_vector vSize = XMLoadFloat3(&m_vDir[i]) - vCenter;
		vSize = XMVector3Normalize(vSize);

		_vector		vDir = { m_vDir[i].x, m_vDir[i].y, m_vDir[i].z, 0.f };
		_float4		fDir;
		XMStoreFloat4(&fDir, XMVector4Normalize(vDir));

		pVertices[i].vDir = fDir;

		pVertices[i].vRight = _float4(vSize.m128_f32[0], 0.f, 0.f, 0.f);
		pVertices[i].vUp = _float4(0.f, vSize.m128_f32[1] / 10.f, 0.f, 0.f);
		pVertices[i].vLook = _float4(0.f, 0.f, vSize.m128_f32[2] / 10.f, 0.f);

		pVertices[i].vTranslation = _float4(
			m_InstanceData.vCenter.x + WidthRange(m_RandomNumber),
			m_InstanceData.vCenter.y + HeightRange(m_RandomNumber),
			m_InstanceData.vCenter.z + DepthRange(m_RandomNumber),
			1.f);

		pVertices[i].vColor = m_InstanceData.vColor;
	}

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVBInstance)))
		return;

	Safe_Delete_Array(pVertices);
}

_bool CVIBuffer_Instancing::IsFinish()
{
	return m_isFinished && !m_InstanceData.isLoop;
}

void CVIBuffer_Instancing::Tick_Drop(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_InstanceData.fDuration <= m_fTimeAcc)
		m_isFinished = true;

	// ���ҽ� ������ ���� ���� ����
	D3D11_MAPPED_SUBRESOURCE			SubResource{};

	uniform_real_distribution<float>	TimeRange(m_InstanceData.vLifeTime.x, m_InstanceData.vLifeTime.y);
	uniform_real_distribution<float>	WidthRange(m_InstanceData.vRange.x * -0.5f, m_InstanceData.vRange.x * 0.5f);
	uniform_real_distribution<float>	HeightRange(m_InstanceData.vRange.y * -0.5f, m_InstanceData.vRange.y * 0.5f);
	uniform_real_distribution<float>	DepthRange(m_InstanceData.vRange.z * -0.5f, m_InstanceData.vRange.z * 0.5f);

	// �ν��Ͻ� ���� ����. GPU���� ������ �޸𸮸� CPU�� �а� �� �� �ֵ��� �ϴ� �۾�.
	// D3D11_MAP_WRITE_NO_OVERWRITE : ������ �����͸� ����� �ʰ� ������ ���� ���⸦ ����ϴ� �ɼ�
	m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		// �ν��Ͻ��� Y�� ��ġ(vTranslation.y)�� �ð� ����(fTimeDelta)�� �ش� �ν��Ͻ��� �ӵ�(m_pSpeed[i])�� ���� ����ŭ �Ʒ��� �̵�
		((VTXINSTANCE*)SubResource.pData)[i].vTranslation.y -= m_pSpeed[i] * fTimeDelta;

		m_pLifeTime[i] -= fTimeDelta;
		if (0.0f >= m_pLifeTime[i])
		{
			if (true == m_InstanceData.isLoop)
			{
				m_pLifeTime[i] = TimeRange(m_RandomNumber);
				((VTXINSTANCE*)SubResource.pData)[i].vTranslation = _float4(
					m_InstanceData.vCenter.x + WidthRange(m_RandomNumber),
					m_InstanceData.vCenter.y + HeightRange(m_RandomNumber),
					m_InstanceData.vCenter.z + DepthRange(m_RandomNumber),
					1.f);
			}
			else
			{
				((VTXINSTANCE*)SubResource.pData)[i].vColor.w = 0.f;
			}
		}
	}

	// �ν��Ͻ� ���۸� ���
	// CPU�� ������ �޸� ������ ������ GPU�� �ٽ� ������� �ѱ�� �۾�
	m_pContext->Unmap(m_pVBInstance.Get(), 0);
}

void CVIBuffer_Instancing::Tick_Spread(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_InstanceData.fDuration <= m_fTimeAcc)
		m_isFinished = true;

	D3D11_MAPPED_SUBRESOURCE			SubResource{};

	uniform_real_distribution<float>	TimeRange(m_InstanceData.vLifeTime.x, m_InstanceData.vLifeTime.y);
	uniform_real_distribution<float>	WidthRange(m_InstanceData.vRange.x * -0.5f, m_InstanceData.vRange.x * 0.5f);
	uniform_real_distribution<float>	HeightRange(m_InstanceData.vRange.y * -0.5f, m_InstanceData.vRange.y * 0.5f);
	uniform_real_distribution<float>	DepthRange(m_InstanceData.vRange.z * -0.5f, m_InstanceData.vRange.z * 0.5f);

	m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		VTXINSTANCE* pVertices = ((VTXINSTANCE*)SubResource.pData);

		_vector		vDir = XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_InstanceData.vPivot);
		vDir = XMVectorSetW(vDir, 0.f);

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeed[i] * fTimeDelta);

		m_pLifeTime[i] -= fTimeDelta;
		if (0.0f >= m_pLifeTime[i])
		{
			if (true == m_InstanceData.isLoop)
			{
				m_pLifeTime[i] = TimeRange(m_RandomNumber);
				((VTXINSTANCE*)SubResource.pData)[i].vTranslation = _float4(
					m_InstanceData.vCenter.x + WidthRange(m_RandomNumber),
					m_InstanceData.vCenter.y + HeightRange(m_RandomNumber),
					m_InstanceData.vCenter.z + DepthRange(m_RandomNumber),
					1.f);
			}
			else
			{
				((VTXINSTANCE*)SubResource.pData)[i].vColor.w = 0.f;
			}
		}
	}

	m_pContext->Unmap(m_pVBInstance.Get(), 0);
}

void CVIBuffer_Instancing::Tick_Splash(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_InstanceData.fDuration <= m_fTimeAcc)
		m_isFinished = true;

	D3D11_MAPPED_SUBRESOURCE			SubResource{};

	uniform_real_distribution<float>	TimeRange(m_InstanceData.vLifeTime.x, m_InstanceData.vLifeTime.y);
	uniform_real_distribution<float>	WidthRange(m_InstanceData.vRange.x * -0.5f, m_InstanceData.vRange.x * 0.5f);
	uniform_real_distribution<float>	HeightRange(m_InstanceData.vRange.y * -0.5f, m_InstanceData.vRange.y * 0.5f);
	uniform_real_distribution<float>	DepthRange(m_InstanceData.vRange.z * -0.5f, m_InstanceData.vRange.z * 0.5f);

	m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		VTXINSTANCE* pVertices = ((VTXINSTANCE*)SubResource.pData);

		_vector		vDir = XMLoadFloat3(&m_vDir[i]);
		vDir = XMVectorSetW(vDir, 0.f);

		if (m_fMaxLifeTime / 2.f > m_pLifeTime[i])
			m_pSpeed[i] -= 0.1f;

		if (0.f >= m_pSpeed[i])
			m_pSpeed[i] = 0.f;

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeed[i] * m_pLifeTime[i] * 10.f * fTimeDelta);

		m_pLifeTime[i] -= fTimeDelta;

		if (0.0f >= m_pLifeTime[i])
		{
			m_pLifeTime[i] = 0.0f;

			if (true == m_InstanceData.isLoop)
			{
				m_pLifeTime[i] = TimeRange(m_RandomNumber);
				((VTXINSTANCE*)SubResource.pData)[i].vTranslation = _float4(
					m_InstanceData.vCenter.x + WidthRange(m_RandomNumber),
					m_InstanceData.vCenter.y + HeightRange(m_RandomNumber),
					m_InstanceData.vCenter.z + DepthRange(m_RandomNumber),
					1.f);
			}
			else
			{
				//((VTXINSTANCE*)SubResource.pData)[i].vColor.w -= (1.f - (m_fTimeAcc / m_InstanceData.fDuration));
				((VTXINSTANCE*)SubResource.pData)[i].vColor.w -= (1.f - (m_fMaxLifeTime / m_InstanceData.fDuration));
			}
		}
	}

	m_pContext->Unmap(m_pVBInstance.Get(), 0);
}

void CVIBuffer_Instancing::Tick_HP(_float fTimeDelta, _uint iHP, _uint iDecrease)
{
	D3D11_MAPPED_SUBRESOURCE			SubResource{};

	m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		VTXINSTANCE* pVertices = ((VTXINSTANCE*)SubResource.pData);

		if (iHP < m_iNumInstance - i)
		{
			((VTXINSTANCE*)SubResource.pData)[i].vColor = { 0.f, 0.f, 0.f, 1.f };
			//XMStoreFloat4(((VTXINSTANCE*)SubResource.pData)[i].vColor, XMVectorSet(1.f, 1.f, 1.f, 1.f));
		}

		else
		{
			((VTXINSTANCE*)SubResource.pData)[i].vColor = { 1.f, 0.f, 0.f, 1.f };
			//XMStoreFloat4(&pVertices[i].vColor, XMVectorSet(1.f, 0.f, 0.f, 1.f));
		}
	}

	m_pContext->Unmap(m_pVBInstance.Get(), 0);
}

void CVIBuffer_Instancing::Tick_Target(_float fTimeDelta, _bool isOnAttack)
{
	D3D11_MAPPED_SUBRESOURCE			SubResource{};

	m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		VTXINSTANCE* pVertices = ((VTXINSTANCE*)SubResource.pData);

		if (isOnAttack)
			((VTXINSTANCE*)SubResource.pData)[i].vColor = { 1.f, 0.f, 0.f, 1.f };

		else
			((VTXINSTANCE*)SubResource.pData)[i].vColor = { 0.f, 1.f, 1.f, 1.f };
	}

	m_pContext->Unmap(m_pVBInstance.Get(), 0);
}

void CVIBuffer_Instancing::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pSpeed);
	Safe_Delete_Array(m_vDir);
	Safe_Delete_Array(m_pLifeTime);
}
