#pragma once

#include "Engine_Defines.h"
#include "File_Manager.h"

BEGIN(Engine)

class CBinaryChannel final
{
public:
	
	// 키프레임 : 본 움직임의 상태들 (중간 중간 상태값을 갖고 그걸 선형보간)
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
	// aiNodeAnim : 노드에 대한 애니메이션 정보 (NodeAnim == Channel)
	HRESULT		Initialize(shared_ptr<CFile_Manager> pFile, shared_ptr<class CBinaryModel> pModel);
	// 변환 행렬을 계산하는 함수 (TrackPosition : 애니메이션 채널에서 현재 위치 또는 시간 )
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

	// 얕복되면 안되는 변수
	// _uint			m_iCurrentKeyFrame = { 0 };
	_uint				m_iBoneIndex = { 0 };

	// 다른 애니메이션과 선형보간 시작할 시점의 상태값
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