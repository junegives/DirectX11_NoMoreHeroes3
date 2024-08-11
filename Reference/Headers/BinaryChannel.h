#pragma once

#include "Engine_Defines.h"
#include "File_Manager.h"

BEGIN(Engine)

class CBinaryChannel final
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
	CBinaryChannel();
	~CBinaryChannel() = default;


public:
	// aiNodeAnim : ��忡 ���� �ִϸ��̼� ���� (NodeAnim == Channel)
	HRESULT		Initialize(shared_ptr<CFile_Manager> pFile, shared_ptr<class CBinaryModel> pModel);
	// ��ȯ ����� ����ϴ� �Լ� (TrackPosition : �ִϸ��̼� ä�ο��� ���� ��ġ �Ǵ� �ð� )
	void		Invalidate_TransformationMatrix(_double TrackPosition, _uint* pCurrentKeyFrame, const vector<shared_ptr<class CBinaryBone>>& Bones);

public:
	_uint		GetBoneIndex() { return m_iBoneIndex; }

public:
	KEYFRAME	GetCurrentKeyFrame() { return m_CurrentKeyFrame; }
	void		ResetCurKeyFrame() { m_CurrentKeyFrame = m_KeyFrames.front(); }

	
private:
	string				m_strName;
	_uint				m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>	m_KeyFrames;

	// �躹�Ǹ� �ȵǴ� ����
	// _uint			m_iCurrentKeyFrame = { 0 };
	_uint				m_iBoneIndex = { 0 };

	// �ٸ� �ִϸ��̼ǰ� �������� ������ ������ ���°�
	KEYFRAME			m_ChangePointKeyFrame;
	KEYFRAME			m_CurrentKeyFrame;

	_vector				m_vScale;
	_vector				m_vRotation;
	_vector				m_vTranslation;

public:
	static shared_ptr<CBinaryChannel> Create(shared_ptr<CFile_Manager> pFile, shared_ptr<class CBinaryModel> pModel);
	void	Free();
};

END