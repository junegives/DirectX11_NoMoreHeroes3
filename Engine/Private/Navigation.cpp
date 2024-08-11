#include "Navigation.h"
#include "Cell.h"
#include "Shader.h"

#include "GameInstance.h"

_float4x4 CNavigation::m_WorldMatrix = {};

CNavigation::CNavigation(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CComponent(pDevice, pContext)
{
}

CNavigation::CNavigation(const CNavigation& rhs)
	: CComponent(rhs)
	, m_pShader(rhs.m_pShader)
	, m_Cells(rhs.m_Cells)
{
}

HRESULT CNavigation::Initialize_Prototype(const wstring& strNavigationDataFilePath)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	// 네비게이션 데이터 파일을 읽어서 사용한다.
	_ulong		dwByte = { 0 };
	HANDLE		hFile = CreateFile(strNavigationDataFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	while (true)
	{
		_float3		vPoints[CCell::POINT_END];

		// _float3 형식의 'POINT_END' 갯수만큼의 데이터를 읽어와서 vPoints 배열에 저장
		ReadFile(hFile, vPoints, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);

		if (0 == dwByte)
			break;

		shared_ptr<CCell> pCell = CCell::Create(m_pDevice, m_pContext, vPoints, (_uint)m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	CloseHandle(hFile);

	if (FAILED(Make_Neighbors()))
		return E_FAIL;

#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		m_iCurrentIndex = ((NAVI_DESC*)pArg)->iStartCellIndex;
	}

	return S_OK;
}

void CNavigation::Tick(_fmatrix TerrainWorldMatrix)
{
	// 매 틱마다 지형의 월드 행렬을 받아와서 저장한다.
	XMStoreFloat4x4(&m_WorldMatrix, TerrainWorldMatrix);
}

_bool CNavigation::isMove(_fvector vPosition)
{
	_int			iNeighborIndex = { -1 };
	_uint			iSearchCnt = 0;

	/* 현재 존재하고 있던 셀 안에서 움직였다. */
	if (true == m_Cells[m_iCurrentIndex]->isIn(vPosition, &iNeighborIndex))
		return true;

	/* 현재 셀을 나갔다. */
	else
	{
		// 셀의 인덱스가 유효한 값(-1이 아닌 값)이라면 이동을 시도한다.
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				iSearchCnt++;

				//if (500 < iSearchCnt) return false;

				// 이웃 셀의 인덱스가 -1이라면, 이동할 셀이 더 이상 없으므로 함수는 true를 반환한다.
				if (-1 == iNeighborIndex)
					return false;

				// 이웃 셀에 위치하는지 여부를 확인하고, 위치한다면 현재 셀을 갱신하고 함수는 true를 반환한다.
				if (true == m_Cells[iNeighborIndex]->isIn(vPosition, &iNeighborIndex))
				{
					m_iCurrentIndex = iNeighborIndex;
					return true;
				}
			}
		}

		else
			return false;
	}

	return false;
}

_float CNavigation::Compute_Height(_vector vPosition)
{
	shared_ptr<CCell> pCell = m_Cells[m_iCurrentIndex];

	_float3 PointA = pCell->Get_Point(CCell::POINT_A);
	_float3 PointB = pCell->Get_Point(CCell::POINT_B);
	_float3 PointC = pCell->Get_Point(CCell::POINT_C);

	_vector vA = XMVectorSetW(XMLoadFloat3(&PointA), 1.f);
	_vector vB = XMVectorSetW(XMLoadFloat3(&PointB), 1.f);
	_vector vC = XMVectorSetW(XMLoadFloat3(&PointC), 1.f);

	_vector vPlane = XMPlaneFromPoints(vA, vB, vC);


	_vector fTargetPos = vPosition;
	_float fx = XMVectorGetX(fTargetPos);
	_float fz = XMVectorGetZ(fTargetPos);

	_float fa = XMVectorGetX(vPlane);
	_float fb = XMVectorGetY(vPlane);
	_float fc = XMVectorGetZ(vPlane);
	_float fd = XMVectorGetW(vPlane);

	_float fResult = (-fa * fx - fc * fz - fd) / fb;
	return fResult;
}

_bool CNavigation::Compute_CurrentIdx(_vector vPosition)
{
	_int iNeighborCell;
	_int iCurrentIdx = -1;

	for (auto& pCell : m_Cells)
	{
		iCurrentIdx++;

		if (pCell->isIn(vPosition, &iNeighborCell))
		{
			m_iCurrentIndex = iCurrentIdx;
			return true;
		}
	}

	return false;
}

_vector CNavigation::CalcSlideVec(_vector vLook)
{
	_float3 vNormal = m_Cells[m_iCurrentIndex]->GetNormal();

	_vector vCellNormal = XMLoadFloat3(&vNormal);

	// 법선벡터에 정사영한 벡터
	_vector vProj = XMVector3Dot(vLook, vCellNormal) * vCellNormal;

	_vector vSlide = vLook - XMVector3Dot(vLook, vCellNormal) * vCellNormal;

	return vSlide;
}

void CNavigation::GetCellCenterPos(_vector* vCenterPos)
{
	m_Cells[m_iCurrentIndex]->GetCenterPos(vCenterPos);
}

void CNavigation::GetCellCenterPos(_uint iCellIdx, _vector* vCenterPos)
{
	m_Cells[iCellIdx]->GetCenterPos(vCenterPos);
}

#ifdef _DEBUG

HRESULT CNavigation::Render()
{
	_float4x4		 ViewMatrix, ProjMatrix;

	ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_VIEW);
	ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_PROJ);

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_float4		vColor = _float4(0.f, 0.f, 0.f, 1.f);
	_float4x4	WorldMatrix = m_WorldMatrix;

	if (-1 == m_iCurrentIndex)
	{
		vColor = _float4(0.f, 1.f, 0.f, 1.f);
	}
	else
	{
		WorldMatrix._42 += 0.1f;
		vColor = _float4(1.f, 0.f, 0.f, 1.f);
	}

	if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	m_pShader->Begin(0);

	if (-1 == m_iCurrentIndex)
	{
		for (auto& pCell : m_Cells)
		{
			if (nullptr != pCell)
				pCell->Render();
		}
	}
	else
		m_Cells[m_iCurrentIndex]->Render();

	return S_OK;
}

#endif

HRESULT CNavigation::Make_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
				pSourCell->Set_Neighbor(CCell::LINE_AB, pDestCell);
			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
				pSourCell->Set_Neighbor(CCell::LINE_BC, pDestCell);
			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
				pSourCell->Set_Neighbor(CCell::LINE_CA, pDestCell);
		}
	}

	return S_OK;
}

shared_ptr<CNavigation> CNavigation::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strNavigationDataFilePath)
{
	shared_ptr<CNavigation> pInstance = make_shared<CNavigation>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strNavigationDataFilePath)))
	{
		MSG_BOX("Failed to Created : CNavigation");
		pInstance.reset();
	}

	return pInstance;
}

shared_ptr<CComponent> CNavigation::Clone(void* pArg)
{
	shared_ptr<CNavigation> pInstance = make_shared<CNavigation>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNavigation");
		pInstance.reset();
	}

	return pInstance;
}

void CNavigation::Free()
{
}
