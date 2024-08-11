#include "Cell.h"
#include "VIBuffer_Cell.h"

CCell::CCell(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice(pDevice)
    , m_pContext(pContext)
{
}

HRESULT CCell::Initialize(const _float3* pPoints, _uint iCellIndex)
{
    for (size_t i = 0; i < POINT_END; i++)
        m_vPoints[i] = pPoints[i];

    m_iIndex = iCellIndex;
	_float3 vNormalTmp;

	// 점 AB를 잇는 선분의 노말 벡터
	// 1. 점 B에서 점 A를 빼서 방향 벡터를 구한다.
	// 2. 구한 방향벡터의 x,z 성분을 교환하고 교환된 후의 x성분에 -1을 곱한다.
	XMStoreFloat3(&m_vNormals[LINE_AB], XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A]));
	vNormalTmp = _float3(m_vNormals[LINE_AB].z * -1.f, 0.f, m_vNormals[LINE_AB].x);
	XMStoreFloat3(&m_vNormals[LINE_AB], XMVector3Normalize(XMLoadFloat3(&vNormalTmp)));

	XMStoreFloat3(&m_vNormals[LINE_BC], XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_B]));
	vNormalTmp = _float3(m_vNormals[LINE_BC].z * -1.f, 0.f, m_vNormals[LINE_BC].x);
	XMStoreFloat3(&m_vNormals[LINE_BC], XMVector3Normalize(XMLoadFloat3(&vNormalTmp)));

	XMStoreFloat3(&m_vNormals[LINE_CA], XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(&m_vPoints[POINT_C]));
	vNormalTmp = _float3(m_vNormals[LINE_CA].z * -1.f, 0.f, m_vNormals[LINE_CA].x);
	XMStoreFloat3(&m_vNormals[LINE_CA], XMVector3Normalize(XMLoadFloat3(&vNormalTmp)));


#ifdef _DEBUG
    m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
    if (nullptr == m_pVIBuffer)
        return E_FAIL;
#endif

    return S_OK;
}

// 두 점을 공유하는지 확인하는 함수
_bool CCell::Compare_Points(const _float3& vSourPoint, const _float3& vDestPoint)
{
	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&vSourPoint)))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&vDestPoint)))
			return true;
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&vDestPoint)))
			return true;
	}
	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&vSourPoint)))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&vDestPoint)))
			return true;
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&vDestPoint)))
			return true;
	}
	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&vSourPoint)))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&vDestPoint)))
			return true;
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&vDestPoint)))
			return true;
	}

	return false;
}

// 현재 위치가 셀의 외부에 위치하는지 여부를 판단하고, 외부에 위치한다면 가장 가까운 이웃 셀의 인덱스를 pNeighborIndex에 저장
_bool CCell::isIn(_fvector vLocalPos, _int* pNeighborIndex)
{
	for (size_t i = 0; i < LINE_END; i++)
	{
		// 현재 위치에서 선의 한 끝점으로 향하는 벡터 vDir
		_vector	vDir = XMVector3Normalize(vLocalPos - XMLoadFloat3(&m_vPoints[i]));

		// 선분의 노말 벡터와 위에서 구한 vDir 벡터의 내적 계산
		if (0 < XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_vNormals[i]), vDir)))
		{
			// 이동하려는 위치가 셀이 없는 곳이면 슬라이딩 벡터에 노말 벡터 저장
			if (-1 == m_iNeighborIndices[i])
				m_vNormalForSlide = m_vNormals[i];

			// 결과가 양수면, 셀의 외부에 위치
			// pNeighborIndex에 현재 선에 대한 이웃 셀의 인덱스를 저장하고 false 반환
			*pNeighborIndex = m_iNeighborIndices[i];
			return false;
		}
	}

	return true;
}

void CCell::GetCenterPos(_vector* vPos)
{
	for (size_t i = 0; i < POINT_END; i++)
	{
		vPos->m128_f32[0] += m_vPoints[i].x;
		vPos->m128_f32[1] += m_vPoints[i].y;
		vPos->m128_f32[2] += m_vPoints[i].z;
	}

	vPos->m128_f32[0] /= (_float)POINT_END;
	vPos->m128_f32[1] /= (_float)POINT_END;
	vPos->m128_f32[2] /= (_float)POINT_END;
	vPos->m128_f32[3] = 1.f;
}

#ifdef _DEBUG
HRESULT CCell::Render()
{
    m_pVIBuffer->Bind_Buffers();
    m_pVIBuffer->Render();

    return S_OK;
}
#endif

shared_ptr<CCell> CCell::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _float3* pPoints, _uint iCellIndex)
{
    shared_ptr<CCell>   pInstance = make_shared<CCell>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pPoints, iCellIndex)))
    {
        MSG_BOX("Failed to Created : CCell");
        pInstance.reset();
    }

    return pInstance;
}

void CCell::Free()
{
}
