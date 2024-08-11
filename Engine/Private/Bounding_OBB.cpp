#include "Bounding_OBB.h"

CBounding_OBB::CBounding_OBB(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CBounding(pDevice, pContext)
{
}

HRESULT CBounding_OBB::Initialize(void* pArg)
{
	/* 초기상태는 로컬상태 */
	OBB_DESC* pDesc = (OBB_DESC*)pArg;

	_float4		vRotation;
	// RollPitchYaw : 회전 벡터 리턴해줌 (OBB 회전값은 쿼터니언으로)
	XMStoreFloat4(&vRotation, XMQuaternionRotationRollPitchYaw(pDesc->vRadians.x, pDesc->vRadians.y, pDesc->vRadians.z));

	m_pOriginalOBB = make_shared<BoundingOrientedBox>(pDesc->vCenter, pDesc->vExtents, vRotation);
	m_pOBB = make_shared<BoundingOrientedBox>(*m_pOriginalOBB);

	return S_OK;
}

void CBounding_OBB::Tick(_fmatrix WorldMatrix)
{
	m_isColl = false;

	m_pOriginalOBB->Transform(*m_pOBB, WorldMatrix);
}

HRESULT CBounding_OBB::Render(shared_ptr<PrimitiveBatch<VertexPositionColor>> pBatch)
{
	DX::Draw(pBatch.get(), *m_pOBB, m_isColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	return S_OK;
}

_bool CBounding_OBB::Intersect(CCollider::TYPE eType, shared_ptr<CBounding> pBounding)
{
	_bool isColl = false;

	void* pTargetBounding = pBounding->Get_Bounding();

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		isColl = m_pOBB->Intersects(*(BoundingBox*)pTargetBounding);
		break;
	case CCollider::TYPE_OBB:
		isColl = m_pOBB->Intersects(*(BoundingOrientedBox*)pTargetBounding);
		break;
	case CCollider::TYPE_SPHERE:
		isColl = m_pOBB->Intersects(*(BoundingSphere*)pTargetBounding);
		break;
	}

	if (isColl)
		m_isColl = isColl;

	return isColl;
}

CBounding_OBB::OBB_COL_DESC CBounding_OBB::Compute_OBBColDesc()
{
	OBB_COL_DESC			OBBDesc{};

	_float3		vPoints[8];

	m_pOBB->GetCorners(vPoints);
	// 뒤좌하, 뒤우하, 뒤우상, 뒤좌상, 앞좌하, 앞우하, 앞우상, 앞좌상

	// OBB의 축 (회전된 축)
	OBBDesc.vCenter = m_pOBB->Center;
	XMStoreFloat3(&OBBDesc.vCenterAxis[0], (XMLoadFloat3(&vPoints[5]) - XMLoadFloat3(&vPoints[4])) * 0.5f);
	XMStoreFloat3(&OBBDesc.vCenterAxis[1], (XMLoadFloat3(&vPoints[7]) - XMLoadFloat3(&vPoints[4])) * 0.5f);
	XMStoreFloat3(&OBBDesc.vCenterAxis[2], (XMLoadFloat3(&vPoints[0]) - XMLoadFloat3(&vPoints[4])) * 0.5f);

	// OBB의 정렬된 축 (노멀라이징)
	for (size_t i = 0; i < 3; i++)
	{
		XMStoreFloat3(&OBBDesc.vAlignAxis[i], XMVector3Normalize(XMLoadFloat3(&OBBDesc.vCenterAxis[i])));
	}

	return OBBDesc;
}

_bool CBounding_OBB::Intersect(shared_ptr<CBounding_OBB> pTargetBounding)
{
	OBB_COL_DESC		OBBDesc[2];

	// Sour 객체의 OBB
	OBBDesc[0] = Compute_OBBColDesc();
	// Dest 객체의 OBB
	OBBDesc[1] = pTargetBounding->Compute_OBBColDesc();

	_float			fDistance[3];

	for (size_t i = 0; i < 2; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			// 두 OBB의 중심전 간의 거리
			fDistance[0] = fabs(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenter) - XMLoadFloat3(&OBBDesc[0].vCenter),
				XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])).m128_f32[0]);

			// 첫 번째 OBB의 각 축과 두 번째 OBB의 정렬된 축 간의 거리를 계산
			fDistance[1] = fabs(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[0]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])).m128_f32[0]) +
				fabs(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[1]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])).m128_f32[0]) +
				fabs(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[2]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])).m128_f32[0]);

			// 두 번째 OBB의 각 축과 첫 번째 OBB의 정렬된 축 간의 거리 계산
			fDistance[2] = fabs(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[0]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])).m128_f32[0]) +
				fabs(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[1]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])).m128_f32[0]) +
				fabs(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[2]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])).m128_f32[0]);

			if (fDistance[0] > fDistance[1] + fDistance[2])
				return false;
		}
	}

	return true;
}

shared_ptr<CBounding_OBB> CBounding_OBB::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
	shared_ptr<CBounding_OBB> pInstance = make_shared<CBounding_OBB>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBounding_OBB");
		pInstance.reset();
	}

	return pInstance;
}

void CBounding_OBB::Free()
{
}
