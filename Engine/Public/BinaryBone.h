#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CBinaryBone final
{
public:
	CBinaryBone();
	~CBinaryBone() = default;

public:
	const string& Get_BoneName() const {
		return m_strName;
	}

	_float4x4 Get_CombinedTransformationMatrix() const {
		return m_CombinedTransformationMatrix;
	}

	_float4x4* Get_CombindTransformationMatrixPtr() {
		return &m_CombinedTransformationMatrix;
	}

	void	Set_TranslationMatrix(_fmatrix TranslationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TranslationMatrix);
	}

	_vector& Get_RootPos() { return m_RootPos; }
	_vector& Get_RootLook() { return m_RootLook; }

public:
	HRESULT Initialize(const string& strBoneName, _int iParentBoneIndex, _int iBoneIndex, _fmatrix TransformMatrix, _fmatrix PivotMatrix);

	// m_CombinedTransformationMatrix�� ������Ʈ�ϴ� �Լ�
	void	Invalidate_CombinedTransformationMatrix(const vector<shared_ptr<CBinaryBone>>& Bones);

private:
	string		m_strName;							// Bone�� �̸�
	_float4x4	m_TransformationMatrix;				// �θ� �������� ǥ���� ������ ���� ���
	_float4x4	m_CombinedTransformationMatrix;		// ���� TransformationMatrix * �θ��� m_CombinedTransformationMatrix

	_int		m_iParentBoneIndex = { 0 };			// ���� Bone�� �θ� Bone�� �ε����� �����ϴ� ����
	_int		m_iBoneIndex = { 0 };				// ���� Bone�� �ε����� �����ϴ� ����

	_vector		m_RootPos = { 0.f, 0.f, 0.f, 1.f };	// ��Ʈ ���� ��ġ�� �����ϰ� �ִ� ����
	_vector		m_RootLook = { 0.f, 0.f, 0.f, 0.f };	// ��Ʈ ���� ��ġ�� �����ϰ� �ִ� ����

public:
	// aiNode : 3D ���� ���� ������ ��Ÿ���� Ŭ����
	// aiBone : ���̷��� �ִϸ��̼� �ý��ۿ��� ����(Bone)�� ��Ÿ���� Ŭ���� (���� �Ϻθ� ������Ű�� ����, ����ġ�� �Բ� ���Ǿ� ������ ����)
	// -> ������ ����X, ���� � �����鿡�� ������ �ִ���
	// aiNodeAnim : Ư�� ���� ���õ� �ִϸ��̼� �����͸� ��Ÿ���� Ŭ���� (Ư�� ����� �ִϸ��̼� ����)
	// -> �ִϸ��̼� �������� ���� ��ȯ (�ִϸ��̼��� ǥ���ϱ� ���ؼ� ���� ���� ����)
	static shared_ptr<CBinaryBone> Create(const string& strBoneName, _int iParentBoneIndex, _int iBoneIndex, _fmatrix TransformMatrix, _fmatrix PivotMatrix);
	shared_ptr<CBinaryBone> Clone();
	void Free();
};

END