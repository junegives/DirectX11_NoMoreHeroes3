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

	// m_CombinedTransformationMatrix를 업데이트하는 함수
	void	Invalidate_CombinedTransformationMatrix(const vector<shared_ptr<CBone>>& Bones);

private:
	_char		m_szName[MAX_PATH] = "";			// Bone의 이름
	_float4x4	m_TransformationMatrix;				// 부모를 기준으로 표현된 나만의 상태 행렬
	_float4x4	m_CombinedTransformationMatrix;		// 나의 TransformationMatrix * 부모의 m_CombinedTransformationMatrix

	_int		m_iParentBoneIndex = { 0 };			// 현재 Bone의 부모 Bone의 인덱스를 저장하는 변수

public:
	// aiNode : 3D 모델의 계층 구조를 나타내는 클래스
	// aiBone : 스켈레톤 애니메이션 시스템에서 뼈대(Bone)를 나타내는 클래스 (모델의 일부를 변형시키는 역할, 가중치와 함께 사용되어 정점에 영향)
	// -> 움직임 상태X, 뼈가 어떤 정점들에게 영향을 주는지
	// aiNodeAnim : 특정 노드와 관련된 애니메이션 데이터를 나타내는 클래스 (특정 노드의 애니메이션 정보)
	// -> 애니메이션 내에서의 상태 변환 (애니메이션을 표현하기 위해서 뼈와 뼈의 관계)
	static shared_ptr<CBone> Create(const aiNode* pAIBone, _int iParentBoneIndex, _fmatrix PivotMatrix);
	shared_ptr<CBone> Clone();
	void Free();
};

END