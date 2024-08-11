#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CAnimation final
{
public:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	~CAnimation() = default;

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, shared_ptr<class CModel> pModel);
	void	Invalidate_TransformationMatrix(_float fTimeDelta, const vector<shared_ptr<class CBone>>& Bones, _bool isLoop);

private:
	_uint		m_iNumAnimations = { 0 };	// 애니메이션 개수
	_char		m_szName[MAX_PATH] = "";	// 애니메이션 이름

	_double		m_Duration = { 0.0 };		// 애니메이션을 구동하는데 걸리는 전체 시간 (보다는 애니메이션 전체를 수행하는 길이)
	_double		m_TickPerSecond = { 0.0 };	// 애니메이션 재생 속도
	_double		m_TrackPosition = { 0.0 };	// 현재 내 애니메이션의 재생 위치

	/*
	m_fTimeAcc += pAnimation->mTicksPerSecond * fTimeDelta;
	mDuration <= m_fTimeAcc이면 애니메이션 끝난 것
	*/

	_uint		m_iNumChannels = { 0 };		// 애니메이션을 구동하는데 "필요한" 뼈의 수
	vector<shared_ptr<class CChannel>> m_Channels = {};	// 애니메이션을 구동하는데 필요한 뼈
	vector<_uint>			m_iCurrentKeyFrames;

	_bool		m_isFinished = { false };

public:
	static shared_ptr<CAnimation> Create(const aiAnimation* pAIAnimation, shared_ptr<class CModel> pModel);
	shared_ptr<CAnimation> Clone();
	void Free();
};

END