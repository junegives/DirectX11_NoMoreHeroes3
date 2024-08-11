#pragma once

#include "Engine_Defines.h"
#include "DebugDraw.h"
#include "Collider.h"

BEGIN(Engine)

class CBounding abstract
{
public:
	typedef struct
	{
		_float3		vCenter;
	}BOUNDING_DESC;

public:
	CBounding(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CBounding() = default;

public:
	virtual void* Get_Bounding() = 0;

public:
	_float3			GetCenter() { return m_vCenter; }
	_vector			GetWorldCenter() { return m_vWorldCenter; }

public:
	virtual void Tick(_fmatrix WorldMatrix) = 0;
	virtual HRESULT Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> pBatch) = 0;
	virtual _bool Intersect(CCollider::TYPE eType, shared_ptr<CBounding> pBounding) = 0;

	_bool						m_isColl = { false };

protected:
	ComPtr<ID3D11Device> m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };

	_float3						m_vCenter;
	_vector						m_vWorldCenter;

public:
	virtual void Free();
};

END