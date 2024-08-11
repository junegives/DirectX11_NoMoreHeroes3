#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CChannel final
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
	CChannel();
	~CChannel() = default;

public:
	// aiNodeAnim : 노드에 대한 애니메이션 정보 (NodeAnim == Channel)
	HRESULT		Initialize(const aiNodeAnim* pAIChannel, shared_ptr<class CModel> pModel);
	// 변환 행렬을 계산하는 함수 (TrackPosition : 애니메이션 채널에서 현재 위치 또는 시간 )
	void		Invalidate_TransformationMatrix(_double TrackPosition, _uint* pCurrentKeyFrame, const vector<shared_ptr<class CBone>>& Bones);

private:
	_char				m_szName[MAX_PATH] = "";
	_uint				m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>	m_KeyFrames;

	// 얕복되면 안되는 변수
	// _uint			m_iCurrentKeyFrame = { 0 };
	_uint				m_iBoneIndex = { 0 };

public:
	static shared_ptr<CChannel> Create(const aiNodeAnim* pAIChannel, shared_ptr<class CModel> pModel);
	void	Free();
};

END