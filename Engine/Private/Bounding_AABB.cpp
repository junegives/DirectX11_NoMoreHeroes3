#include "Bounding_AABB.h"

CBounding_AABB::CBounding_AABB(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CBounding(pDevice, pContext)
{
}

HRESULT CBounding_AABB::Initialize(void* pArg)
{
	/* 초기상태는 로컬상태 */
	AABB_DESC* pDesc = (AABB_DESC*)pArg;

	m_pOriginalAABB = make_shared<BoundingBox>(pDesc->vCenter, pDesc->vExtents);
	m_pAABB = make_shared<BoundingBox>(*m_pOriginalAABB);

	m_vCenter = pDesc->vCenter;
	m_vExtents = pDesc->vExtents;

	return S_OK;
}

void CBounding_AABB::Tick(_fmatrix WorldMatrix)
{
	m_isColl = false;

	_matrix		TransformMatrix = WorldMatrix;

	TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[0]).m128_f32[0];
	TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[1]).m128_f32[0];
	TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(TransformMatrix.r[2]).m128_f32[0];

	m_pOriginalAABB->Transform(*m_pAABB, TransformMatrix);
}

HRESULT CBounding_AABB::Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> pBatch)
{
	DX::Draw(pBatch.get(), *m_pAABB, m_isColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	return S_OK;
}

_bool CBounding_AABB::Intersect(CCollider::TYPE eType, shared_ptr<CBounding> pBounding)
{
	_bool isColl = false;

	void* pTargetBounding = pBounding->Get_Bounding();

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		isColl = m_pAABB->Intersects(*(BoundingBox*)pTargetBounding);
		break;
	case CCollider::TYPE_OBB:
		isColl = m_pAABB->Intersects(*(BoundingOrientedBox*)pTargetBounding);
		break;
	case CCollider::TYPE_SPHERE:
		isColl = m_pAABB->Intersects(*(BoundingSphere*)pTargetBounding);
		break;
	}

	if (isColl)
		m_isColl = isColl;

	return isColl;
}

// 기본 제공되는 intersect가 있지만 직접 만들어봄
_bool CBounding_AABB::Intersect(shared_ptr<BoundingBox> pTargetAABB)
{
	_float3		vSourMin, vSourMax;
	_float3		vDestMin, vDestMax;

	vSourMin = _float3(m_pAABB->Center.x - m_pAABB->Extents.x, m_pAABB->Center.y - m_pAABB->Extents.y, m_pAABB->Center.z - m_pAABB->Extents.z);
	vSourMax = _float3(m_pAABB->Center.x + m_pAABB->Extents.x, m_pAABB->Center.y + m_pAABB->Extents.y, m_pAABB->Center.z + m_pAABB->Extents.z);
	vDestMin = _float3(pTargetAABB->Center.x - pTargetAABB->Extents.x, pTargetAABB->Center.y - pTargetAABB->Extents.y, pTargetAABB->Center.z - pTargetAABB->Extents.z);
	vDestMax = _float3(pTargetAABB->Center.x + pTargetAABB->Extents.x, pTargetAABB->Center.y + pTargetAABB->Extents.y, pTargetAABB->Center.z + pTargetAABB->Extents.z);

	/* 너비에 대한 비교. */
	if (max(vSourMin.x, vDestMin.x) > min(vSourMax.x, vDestMax.x))
		return false;

	/* 높이에 대한 비교. */
	if (max(vSourMin.y, vDestMin.y) > min(vSourMax.y, vDestMax.y))
		return false;

	/* 깊이에 대한 비교. */
	if (max(vSourMin.z, vDestMin.z) > min(vSourMax.z, vDestMax.z))
		return false;

	return true;
}

shared_ptr<CBounding_AABB> CBounding_AABB::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
	shared_ptr<CBounding_AABB> pInstance = make_shared<CBounding_AABB>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBounding_AABB");
		pInstance.reset();
	}

	return pInstance;
}

void CBounding_AABB::Free()
{
}
