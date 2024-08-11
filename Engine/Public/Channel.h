#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CChannel final
{
public:
	
	// Ű������ : �� �������� ���µ� (�߰� �߰� ���°��� ���� �װ� ��������)
	typedef struct
	{
		_float3		vScale, vPosition;
		_float4		vRotation;
		_double		Time;
	} KEYFRAME;

public:
	CChannel();
	~CChannel() = default;

public:
	// aiNodeAnim : ��忡 ���� �ִϸ��̼� ���� (NodeAnim == Channel)
	HRESULT		Initialize(const aiNodeAnim* pAIChannel, shared_ptr<class CModel> pModel);
	// ��ȯ ����� ����ϴ� �Լ� (TrackPosition : �ִϸ��̼� ä�ο��� ���� ��ġ �Ǵ� �ð� )
	void		Invalidate_TransformationMatrix(_double TrackPosition, _uint* pCurrentKeyFrame, const vector<shared_ptr<class CBone>>& Bones);

private:
	_char				m_szName[MAX_PATH] = "";
	_uint				m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>	m_KeyFrames;

	// �躹�Ǹ� �ȵǴ� ����
	// _uint			m_iCurrentKeyFrame = { 0 };
	_uint				m_iBoneIndex = { 0 };

public:
	static shared_ptr<CChannel> Create(const aiNodeAnim* pAIChannel, shared_ptr<class CModel> pModel);
	void	Free();
};

END