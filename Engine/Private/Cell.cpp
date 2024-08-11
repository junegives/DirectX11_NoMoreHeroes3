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

	// �� AB�� �մ� ������ �븻 ����
	// 1. �� B���� �� A�� ���� ���� ���͸� ���Ѵ�.
	// 2. ���� ���⺤���� x,z ������ ��ȯ�ϰ� ��ȯ�� ���� x���п� -1�� ���Ѵ�.
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

// �� ���� �����ϴ��� Ȯ���ϴ� �Լ�
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

// ���� ��ġ�� ���� �ܺο� ��ġ�ϴ��� ���θ� �Ǵ��ϰ�, �ܺο� ��ġ�Ѵٸ� ���� ����� �̿� ���� �ε����� pNeighborIndex�� ����
_bool CCell::isIn(_fvector vLocalPos, _int* pNeighborIndex)
{
	for (size_t i = 0; i < LINE_END; i++)
	{
		// ���� ��ġ���� ���� �� �������� ���ϴ� ���� vDir
		_vector	vDir = XMVector3Normalize(vLocalPos - XMLoadFloat3(&m_vPoints[i]));

		// ������ �븻 ���Ϳ� ������ ���� vDir ������ ���� ���
		if (0 < XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_vNormals[i]), vDir)))
		{
			// �̵��Ϸ��� ��ġ�� ���� ���� ���̸� �����̵� ���Ϳ� �븻 ���� ����
			if (-1 == m_iNeighborIndices[i])
				m_vNormalForSlide = m_vNormals[i];

			// ����� �����, ���� �ܺο� ��ġ
			// pNeighborIndex�� ���� ���� ���� �̿� ���� �ε����� �����ϰ� false ��ȯ
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
