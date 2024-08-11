#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CBone final
{
public:
	CBone();
	~CBone() = default;

public:
	const _char* Get_BoneName() const {
		return m_szName;
	}

	_float4x4 Get_CombinedTransformationMatrix() const {
		return m_CombinedTransformationMatrix;
	}

	void	Set_TranslationMatrix(_fmatrix TranslationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TranslationMatrix);
	}

public:
	HRESULT Initialize(const aiNode* pAIBone, _int iParentBoneIndex, _fmatrix PivotMatrix);

	// m_CombinedTransformationMatrix�� ������Ʈ�ϴ� �Լ�
	void	Invalidate_CombinedTransformationMatrix(const vector<shared_ptr<CBone>>& Bones);

private:
	_char		m_szName[MAX_PATH] = "";			// Bone�� �̸�
	_float4x4	m_TransformationMatrix;				// �θ� �������� ǥ���� ������ ���� ���
	_float4x4	m_CombinedTransformationMatrix;		// ���� TransformationMatrix * �θ��� m_CombinedTransformationMatrix

	_int		m_iParentBoneIndex = { 0 };			// ���� Bone�� �θ� Bone�� �ε����� �����ϴ� ����

public:
	// aiNode : 3D ���� ���� ������ ��Ÿ���� Ŭ����
	// aiBone : ���̷��� �ִϸ��̼� �ý��ۿ��� ����(Bone)�� ��Ÿ���� Ŭ���� (���� �Ϻθ� ������Ű�� ����, ����ġ�� �Բ� ���Ǿ� ������ ����)
	// -> ������ ����X, ���� � �����鿡�� ������ �ִ���
	// aiNodeAnim : Ư�� ���� ���õ� �ִϸ��̼� �����͸� ��Ÿ���� Ŭ���� (Ư�� ����� �ִϸ��̼� ����)
	// -> �ִϸ��̼� �������� ���� ��ȯ (�ִϸ��̼��� ǥ���ϱ� ���ؼ� ���� ���� ����)
	static shared_ptr<CBone> Create(const aiNode* pAIBone, _int iParentBoneIndex, _fmatrix PivotMatrix);
	shared_ptr<CBone> Clone();
	void Free();
};

END