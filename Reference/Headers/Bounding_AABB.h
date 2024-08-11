#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_AABB final : public CBounding
{
public:
	typedef struct : public CBounding::BOUNDING_DESC
	{
		_float3		vExtents;
	}AABB_DESC;

public:
	CBounding_AABB(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CBounding_AABB() = default;

public:
	virtual void* Get_Bounding() override {
		return m_pAABB.get();
	}

public:
	HRESULT Initialize(void* pArg);
	virtual void Tick(_fmatrix WorldMatrix) override;
	virtual HRESULT Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> pBatch) override;
	virtual _bool Intersect(CCollider::TYPE eType, shared_ptr<CBounding> pBounding) override;

public:
	_float3		GetExtents() { return m_vExtents; }

private:
	shared_ptr<BoundingBox> m_pOriginalAABB = { nullptr };
	shared_ptr<BoundingBox> m_pAABB = { nullptr };

	_float3					m_vExtents;

private:
	_bool Intersect(shared_ptr<BoundingBox> pTargetAABB);

public:
	static shared_ptr<CBounding_AABB> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
	virtual void Free() override;
};

END