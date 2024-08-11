#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_OBB final : public CBounding
{
public:
	typedef struct
	{
		_float3		vCenter;			// OBB�� �߽���
		_float3		vCenterAxis[3];		// OBB�� ���� ��ǥ�迡���� 3���� ����
		_float3		vAlignAxis[3];		// OBB�� ���� ��ǥ�迡�� ���ĵ� ��
	}OBB_COL_DESC;

	typedef struct : public CBounding::BOUNDING_DESC
	{
		_float3		vExtents; /* OBB�� �߽ɿ��� �� �������� �Ÿ��� ��Ÿ���� ���� */
		_float3		vRadians; /* x : x�� ���� ȸ�� ����, y : y�� ���� ȸ�� ����, z : z�� ���� ȸ�� ���� */
	}OBB_DESC;

public:
	CBounding_OBB(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CBounding_OBB() = default;

public:
	virtual void* Get_Bounding() override {
		return m_pOBB.get();
	}

public:
	HRESULT Initialize(void* pArg);
	virtual void Tick(_fmatrix WorldMatrix) override;
	virtual HRESULT Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> pBatch) override;
	virtual _bool Intersect(CCollider::TYPE eType, shared_ptr<CBounding> pBounding) override;

public:
	OBB_COL_DESC Compute_OBBColDesc();

private:
	shared_ptr<BoundingOrientedBox> m_pOriginalOBB = { nullptr };
	shared_ptr<BoundingOrientedBox> m_pOBB = { nullptr };

private:
	_bool Intersect(shared_ptr<CBounding_OBB> pTargetBounding);

public:
	static shared_ptr<CBounding_OBB> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
	virtual void Free() override;
};

END