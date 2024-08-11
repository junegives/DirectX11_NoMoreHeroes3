#include "BinaryBone.h"

CBinaryBone::CBinaryBone()
{ 
}

HRESULT CBinaryBone::Initialize(const string& strBoneName, _int iParentBoneIndex, _int iBoneIndex, _fmatrix TransformMatrix, _fmatrix PivotMatrix)
{
	// Bone 이름 복사
	m_strName = strBoneName;

	// 부모 Bone의 인덱스 설정
	m_iParentBoneIndex = iParentBoneIndex;

	// 부모 Bone의 인덱스 설정
	m_iBoneIndex = iBoneIndex;

	// Bone의 변환 행렬 설정
	memcpy(&m_TransformationMatrix, &TransformMatrix, sizeof(_float4x4));

	// 행렬 전치 (행 기반 -> 열 기반) Assmip용 데이터에서 행렬을 그대로 꺼내올 때는 전치를 해야한다.
	//XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	if(-1 != iParentBoneIndex)
		XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	else
		// pivot은 최초 뼈에만 적용되어야함.
		XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)) * PivotMatrix);

	// Bone의 누적 변환 행렬 초기화 (뼈대의 부모 뼈대에 대한 상대적인 변환을 모든 부모 뼈대로 거슬러 올라가면서 곱한 것)
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());


	return S_OK;
}

void CBinaryBone::Invalidate_CombinedTransformationMatrix(const vector<shared_ptr<CBinaryBone>>& Bones)
{
	// 본 인덱스가 -1이면 (부모가 없다는 뜻이므로) 부모 뼈에 대해 상대적인 변환이 필요하지 않음
	if (-1 == m_iParentBoneIndex)
		m_CombinedTransformationMatrix = m_TransformationMatrix;
	else
	{
		// 부모 뼈가 있다면, 부모 뼈의 m_CombinedTransformationMatrix와 현재 뼈의 m_TransformationMatrix를 행렬 곱셈하여 계산한 결과를 저장
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));

		if ("root" == m_strName)
		{
			memcpy(&m_RootPos, &m_CombinedTransformationMatrix.m[3], sizeof(_vector));
			memcpy(&m_RootLook, &m_CombinedTransformationMatrix.m[2], sizeof(_vector));

			_vector vDefaultPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
			_vector vDefaultLook = XMVectorSet(0.f, 0.f, -1.f, 0.f);
			memcpy(&m_CombinedTransformationMatrix.m[3], &vDefaultPos, sizeof(_vector));
			//memcpy(&m_CombinedTransformationMatrix.m[2], &vDefaultLook, sizeof(_vector));
		}
	}
}

shared_ptr<CBinaryBone> CBinaryBone::Create(const string& strBoneName, _int iParentBoneIndex, _int iBoneIndex, _fmatrix TransformMatrix, _fmatrix PivotMatrix)
{
	shared_ptr<CBinaryBone>		pInstance = make_shared<CBinaryBone>();

	if (FAILED(pInstance->Initialize(strBoneName, iParentBoneIndex, iBoneIndex, TransformMatrix, PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CBone");
		pInstance.reset();
	}

	return pInstance;
}

shared_ptr<CBinaryBone> CBinaryBone::Clone()
{
	return make_shared<CBinaryBone>(*this);
}

void CBinaryBone::Free()
{
}
