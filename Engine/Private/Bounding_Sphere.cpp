#include "Bounding_Sphere.h"

CBounding_Sphere::CBounding_Sphere(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CBounding(pDevice, pContext)
{
}

HRESULT CBounding_Sphere::Initialize(void* pArg)
{
	/* 초기상태는 로컬상태 */
	SPHERE_DESC* pSphereDesc = (SPHERE_DESC*)pArg;

	m_pOriginalSphere = make_shared<BoundingSphere>(pSphereDesc->vCenter, pSphereDesc->fRadius);
	m_pSphere = make_shared<BoundingSphere>(*m_pOriginalSphere);

	m_vCenter = pSphereDesc->vCenter;
	m_fRadius = pSphereDesc->fRadius;

	return S_OK;
}

void CBounding_Sphere::Tick(_fmatrix WorldMatrix)
{
	m_isColl = false;

	m_pOriginalSphere->Transform(*m_pSphere, WorldMatrix);

	m_vWorldCenter = XMVector3TransformCoord(XMVectorSet(m_vCenter.x, m_vCenter.y, m_vCenter.z, 0.f), WorldMatrix);
}

HRESULT CBounding_Sphere::Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> pBatch)
{
	DX::Draw(pBatch.get(), *m_pSphere, m_isColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	return S_OK;
}

_bool CBounding_Sphere::Intersect(CCollider::TYPE eType, shared_ptr<CBounding> pBounding)
{
	_bool isColl = false;

	void* pTargetBounding = pBounding->Get_Bounding();

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		isColl = m_pSphere->Intersects(*(BoundingBox*)pTargetBounding);
		break;
	case CCollider::TYPE_OBB:
		isColl = m_pSphere->Intersects(*(BoundingOrientedBox*)pTargetBounding);
		break;
	case CCollider::TYPE_SPHERE:
		isColl = m_pSphere->Intersects(*(BoundingSphere*)pTargetBounding);
		break;
	}

	if (isColl)
		m_isColl = isColl;

	return isColl;
}

shared_ptr<CBounding_Sphere> CBounding_Sphere::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
	shared_ptr<CBounding_Sphere> pInstance = make_shared<CBounding_Sphere>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBounding_Sphere");
		pInstance.reset();
	}

	return pInstance;
}

void CBounding_Sphere::Free()
{
}
