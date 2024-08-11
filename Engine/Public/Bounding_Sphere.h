#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_Sphere final : public CBounding
{
public:
	typedef struct : public CBounding::BOUNDING_DESC
	{
		_float		fRadius;
	}SPHERE_DESC;

public:
	CBounding_Sphere(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CBounding_Sphere() = default;

public:
	virtual void* Get_Bounding() override {
		return m_pSphere.get();
	}

public:
	HRESULT Initialize(void* pArg);
	virtual void Tick(_fmatrix WorldMatrix) override;
	virtual HRESULT Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> pBatch) override;
	_bool Intersect(CCollider::TYPE eType, shared_ptr<CBounding> pBounding);

public:
	_float		GetRadius() { return m_fRadius; }

private:
	shared_ptr<BoundingSphere> m_pOriginalSphere = { nullptr };
	shared_ptr<BoundingSphere> m_pSphere = { nullptr };

	_float						m_fRadius = 0.f;

public:
	static shared_ptr<CBounding_Sphere> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
	virtual void Free() override;
};

END