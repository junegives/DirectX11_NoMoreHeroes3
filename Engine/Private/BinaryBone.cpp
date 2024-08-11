#include "BinaryBone.h"

CBinaryBone::CBinaryBone()
{ 
}

HRESULT CBinaryBone::Initialize(const string& strBoneName, _int iParentBoneIndex, _int iBoneIndex, _fmatrix TransformMatrix, _fmatrix PivotMatrix)
{
	// Bone �̸� ����
	m_strName = strBoneName;

	// �θ� Bone�� �ε��� ����
	m_iParentBoneIndex = iParentBoneIndex;

	// �θ� Bone�� �ε��� ����
	m_iBoneIndex = iBoneIndex;

	// Bone�� ��ȯ ��� ����
	memcpy(&m_TransformationMatrix, &TransformMatrix, sizeof(_float4x4));

	// ��� ��ġ (�� ��� -> �� ���) Assmip�� �����Ϳ��� ����� �״�� ������ ���� ��ġ�� �ؾ��Ѵ�.
	//XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	if(-1 != iParentBoneIndex)
		XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	else
		// pivot�� ���� ������ ����Ǿ����.
		XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)) * PivotMatrix);

	// Bone�� ���� ��ȯ ��� �ʱ�ȭ (������ �θ� ���뿡 ���� ������� ��ȯ�� ��� �θ� ����� �Ž��� �ö󰡸鼭 ���� ��)
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());


	return S_OK;
}

void CBinaryBone::Invalidate_CombinedTransformationMatrix(const vector<shared_ptr<CBinaryBone>>& Bones)
{
	// �� �ε����� -1�̸� (�θ� ���ٴ� ���̹Ƿ�) �θ� ���� ���� ������� ��ȯ�� �ʿ����� ����
	if (-1 == m_iParentBoneIndex)
		m_CombinedTransformationMatrix = m_TransformationMatrix;
	else
	{
		// �θ� ���� �ִٸ�, �θ� ���� m_CombinedTransformationMatrix�� ���� ���� m_TransformationMatrix�� ��� �����Ͽ� ����� ����� ����
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
