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
	// 랜덤 넘버 생성기 초기화
	m_RandomNumber = mt19937_64(m_RandomDevice());

	// 객체의 정보(행렬)를 담는 인스턴스 버퍼는 클론되면 안된다. (m_InstanceData)
	// -> 오브젝트 풀링(파티클 몇개 생성될지 예상해서 미리 생성)

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Initialize(void* pArg)
{
	// 인스턴스 수, 속도 등의 정보를 받아와서 인스턴스에 대한 정보 설정
	m_InstanceData = *(INSTANCE_DESC*)pArg;

	m_pSpeed	= new _float[m_iNumInstance];
	m_pLifeTime = new _float[m_iNumInstance];
	m_vDir		= new _float3[m_iNumInstance];

	// 객체의 속도에 대한 균일한 분포 생성 및 저장
	// 실수형(float) 변수에 대한 균일한 분포를 생성
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
	// 인스턴싱을 사용하기 때문에 DrawIndexedInstanced 함수를 사용하여 여러 개의 객체를 렌더링
	// render 대신 DrawIndexedInstanced 함수 사용
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Bind_Buffers()
{
	// 정점 버퍼와 인스턴스 버퍼를 함께 설정
	ComPtr<ID3D11Buffer> pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance
	};

	_uint				iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceStride	// 행렬(m_pVBInstance?) 하나의 사이즈 m_iInstanceStride
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


	// 객체의 속도에 대한 균일한 분포 생성 및 저장
	// 실수형(float) 변수에 대한 균일한 분포를 생성
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

	// 리소스 매핑을 위한 정보 저장
	D3D11_MAPPED_SUBRESOURCE			SubResource{};

	uniform_real_distribution<float>	TimeRange(m_InstanceData.vLifeTime.x, m_InstanceData.vLifeTime.y);
	uniform_real_distribution<float>	WidthRange(m_InstanceData.vRange.x * -0.5f, m_InstanceData.vRange.x * 0.5f);
	uniform_real_distribution<float>	HeightRange(m_InstanceData.vRange.y * -0.5f, m_InstanceData.vRange.y * 0.5f);
	uniform_real_distribution<float>	DepthRange(m_InstanceData.vRange.z * -0.5f, m_InstanceData.vRange.z * 0.5f);

	// 인스턴스 버퍼 매핑. GPU에서 버퍼의 메모리를 CPU가 읽고 쓸 수 있도록 하는 작업.
	// D3D11_MAP_WRITE_NO_OVERWRITE : 기존의 데이터를 덮어쓰지 않고 버퍼의 끝에 쓰기를 허용하는 옵션
	m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		// 인스턴스의 Y축 위치(vTranslation.y)를 시간 간격(fTimeDelta)과 해당 인스턴스의 속도(m_pSpeed[i])를 곱한 값만큼 아래로 이동
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

	// 인스턴스 버퍼를 언맵
	// CPU가 버퍼의 메모리 접근을 끝내고 GPU에 다시 제어권을 넘기는 작업
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
