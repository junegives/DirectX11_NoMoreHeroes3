#include "Bone.h"

CBone::CBone()
{ 
}

HRESULT CBone::Initialize(const aiNode* pAIBone, _int iParentBoneIndex, _fmatrix PivotMatrix)
{
	// Bone �̸� ����
	strcpy_s(m_szName, pAIBone->mName.data);

	// Bone�� ��ȯ ��� ����
	memcpy(&m_TransformationMatrix, &pAIBone->mTransformation, sizeof(_float4x4));

	// ��� ��ġ (�� ��� -> �� ���) Assmip�� �����Ϳ��� ����� �״�� ������ ���� ��ġ�� �ؾ��Ѵ�.
	//XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	if(-1 != iParentBoneIndex)
		XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	else
		// pivot�� ���� ������ ����Ǿ����.
		XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)) * PivotMatrix);

	// Bone�� ���� ��ȯ ��� �ʱ�ȭ (������ �θ� ���뿡 ���� ������� ��ȯ�� ��� �θ� ����� �Ž��� �ö󰡸鼭 ���� ��)
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	// �θ� Bone�� �ε��� ����
	m_iParentBoneIndex = iParentBoneIndex;

	return S_OK;
}

void CBone::Invalidate_CombinedTransformationMatrix(const vector<shared_ptr<CBone>>& Bones)
{
	// �� �ε����� -1�̸� (�θ� ���ٴ� ���̹Ƿ�) �θ� ���� ���� ������� ��ȯ�� �ʿ����� ����
	if (-1 == m_iParentBoneIndex)
		m_CombinedTransformationMatrix = m_TransformationMatrix;
	else
		// �θ� ���� �ִٸ�, �θ� ���� m_CombinedTransformationMatrix�� ���� ���� m_TransformationMatrix�� ��� �����Ͽ� ����� ����� ����
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
}

shared_ptr<CBone> CBone::Create(const aiNode* pAIBone, _int iParentBoneIndex, _fmatrix PivotMatrix)
{
	shared_ptr<CBone>		pInstance = make_shared<CBone>();

	if (FAILED(pInstance->Initialize(pAIBone, iParentBoneIndex, PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CBone");
		pInstance.reset();
	}

	return pInstance;
}

shared_ptr<CBone> CBone::Clone()
{
	return make_shared<CBone>(*this);
}

void CBone::Free()
{
}
