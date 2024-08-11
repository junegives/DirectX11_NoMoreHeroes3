#pragma once

#include "Engine_Defines.h"
#include "File_Manager.h"

BEGIN(Engine)

class CBinaryAnimation final
{
public:
	CBinaryAnimation();
	CBinaryAnimation(const CBinaryAnimation& rhs);
	~CBinaryAnimation() = default;

public:
	HRESULT Initialize(shared_ptr<CFile_Manager> pFile, shared_ptr<class CBinaryModel> pModel);
	_bool	Invalidate_TransformationMatrix(_float fTimeDelta, const vector<shared_ptr<class CBinaryBone>>& Bones);

	void	Reset();
	
	_bool	IsFinish() { return m_isFinished; }
	_bool	IsLoop() { return m_isLoop; }
	_bool	IsPaused() { return m_isPaused; }
	_float	GetAnimSpeed() { return m_fAnimSpeed; }

	void	SetFinish(_bool isFinish) { m_isFinished = isFinish; }
	void	SetLoop(_bool isLoop) { m_isLoop = isLoop; }
	void	SetPaused(_bool isPaused) { m_isPaused = isPaused; }
	void	SetAnimSpeed(_float fAnimSpeed) { m_fAnimSpeed = fAnimSpeed; }

	_uint	GetNumChannels() { return m_iNumChannels; }
	vector<shared_ptr<class CBinaryChannel>> GetChannels() { return m_Channels; }

	_float	GetCurTrackPosRatio() { return m_TrackPosition / m_Duration; }
	_float	GetTrackPosRatio(_float fTrackPos) { return fTrackPos / m_Duration; }
	_float	GetCurTrackPos() { return m_TrackPosition; }

	// 애니메이션이 종료됐는지 확인
	//_bool	IsFinish(_float fTimeDelta, _bool isLoop);
	// 다음 애니메이션으로 넘어가기 위한 세팅
	//void	SetNextAnim_Initailize();
	//// 다음 애니메이션의 첫번째 키프레임으로 변환 행렬을 갱신하는 함수
	//_bool	SetNextAnim_TransfomaionMatrix(_float fTimeDelta, _double Time, shared_ptr<CBinaryAnimation> pNextAnimation, const vector<shared_ptr<class CBinaryBone>>& Bones, _Out_ _float4x4* pRootTransform);
	void	SetStartKeyFrames(_uint iNumKeyFrame, _float fTimeDelta, vector<shared_ptr<CBinaryBone>>& Bones);

private:
	_uint		m_iNumAnimations = { 0 };	// 애니메이션 개수
	string		m_strName;	// 애니메이션 이름

	_double		m_Duration = { 0.0 };		// 애니메이션을 구동하는데 걸리는 전체 시간 (보다는 애니메이션 전체를 수행하는 길이)
	_double		m_TickPerSecond = { 0.0 };	// 애니메이션 재생 속도
	_double		m_TrackPosition = { 0.0 };	// 현재 내 애니메이션의 재생 위치

	_double		m_ChangeStartTime = { 0.0 };

	_float		m_fAnimSpeed;
	_bool		m_isPaused = false;
	_bool		m_isLoop = false;
	_bool		m_isFinished = false;
	_bool		m_isStop = false;

	/*
	m_fTimeAcc += pAnimation->mTicksPerSecond * fTimeDelta;
	mDuration <= m_fTimeAcc이면 애니메이션 끝난 것
	*/

	_uint		m_iNumChannels = { 0 };		// 애니메이션을 구동하는데 "필요한" 뼈의 수
	vector<shared_ptr<class CBinaryChannel>>		m_Channels = {};	// 애니메이션을 구동하는데 필요한 뼈
	vector<_uint>			m_iCurrentKeyFrames;

public:
	static shared_ptr<CBinaryAnimation> Create(shared_ptr<CFile_Manager> pFile, shared_ptr<class CBinaryModel> pModel);
	shared_ptr<CBinaryAnimation> Clone();
	void Free();
};

END