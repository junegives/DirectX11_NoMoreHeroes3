#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_SwordTrail final : public CVIBuffer
{
public:
	typedef struct
	{
		_float3 vUpperPos;
		_float3 vLowerPos;

		_float4x4 vTrailMatrix;
	} TRAIL_DESC;

public:
	CVIBuffer_SwordTrail(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CVIBuffer_SwordTrail(const CVIBuffer_SwordTrail& rhs);
	virtual ~CVIBuffer_SwordTrail() = default;

private:
	_int				m_iLimitPointCount = 30;
	list<_float3>		m_TrailPoint;

	_uint				m_iEndIndex = 0;
	_uint				m_iCatmullRomCount = 0;
	_uint				m_iVtxCount = 0;
	_uint				m_iCatmullRomIndex[4] = { 0 };

	_uint				m_iNumCatmullRomVertices = 0;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	HRESULT Update(TRAIL_DESC TrailPoint);
	HRESULT Clear();

public:
	static shared_ptr<CVIBuffer_SwordTrail> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CComponent> Clone(void* pArg = nullptr);
	virtual void Free() override;

};

END