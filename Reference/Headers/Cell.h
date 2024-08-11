#pragma once

/* �׺���̼��� �����ϴ� �ϳ��� �ﰢ���̴�. */

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CCell final
{
public:
	enum POINTS { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINES { LINE_AB, LINE_BC, LINE_CA, LINE_END };

public:
	CCell(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	~CCell() = default;

public:
	_float3 Get_Point(POINTS ePoint) { return m_vPoints[ePoint]; }

	void	Set_Neighbor(LINES eLine, const shared_ptr<CCell> pNeighbor) {
		m_iNeighborIndices[eLine] = pNeighbor->m_iIndex;
	}

public:
	HRESULT Initialize(const _float3* pPoints, _uint iCellIndex);
	_bool	Compare_Points(const _float3& vSourPoint, const _float3& vDestPoint);
	_bool	isIn(_fvector vLocalPos, _int* pNeighborIndex);

	void	GetCenterPos(_vector* vPos);

public:
	_float3	GetNormal() { return m_vNormalForSlide; }

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	ComPtr<ID3D11Device>		m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>	m_pContext = { nullptr };

private:
	_float3						m_vPoints[POINT_END];						// Cell�� �����ϴ� �� 3��
	_uint						m_iIndex = { 0 };							// Navigation�� ���ϴ� Cell Index

	_float3						m_vNormals[LINE_END];							// ������ ��������
	_int						m_iNeighborIndices[LINE_END] = { -1, -1, -1 };	// �� ���� �����ϴ� �̿� ���� �ε���

	_float3						m_vNormalForSlide;							// �����̵� ����

#ifdef _DEBUG
private:
	shared_ptr<class CVIBuffer_Cell> m_pVIBuffer = { nullptr };	// Cell�� Ȯ���ϱ� ���ؼ� �ﰢ�� line strip���� �׸���.
#endif

public:
	static shared_ptr<CCell> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _float3* pPoints, _uint iCellIndex);
	void Free();
};

END