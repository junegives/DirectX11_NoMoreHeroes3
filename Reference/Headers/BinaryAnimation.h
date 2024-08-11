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

	// �ִϸ��̼��� ����ƴ��� Ȯ��
	//_bool	IsFinish(_float fTimeDelta, _bool isLoop);
	// ���� �ִϸ��̼����� �Ѿ�� ���� ����
	//void	SetNextAnim_Initailize();
	//// ���� �ִϸ��̼��� ù��° Ű���������� ��ȯ ����� �����ϴ� �Լ�
	//_bool	SetNextAnim_TransfomaionMatrix(_float fTimeDelta, _double Time, shared_ptr<CBinaryAnimation> pNextAnimation, const vector<shared_ptr<class CBinaryBone>>& Bones, _Out_ _float4x4* pRootTransform);
	void	SetStartKeyFrames(_uint iNumKeyFrame, _float fTimeDelta, vector<shared_ptr<CBinaryBone>>& Bones);

private:
	_uint		m_iNumAnimations = { 0 };	// �ִϸ��̼� ����
	string		m_strName;	// �ִϸ��̼� �̸�

	_double		m_Duration = { 0.0 };		// �ִϸ��̼��� �����ϴµ� �ɸ��� ��ü �ð� (���ٴ� �ִϸ��̼� ��ü�� �����ϴ� ����)
	_double		m_TickPerSecond = { 0.0 };	// �ִϸ��̼� ��� �ӵ�
	_double		m_TrackPosition = { 0.0 };	// ���� �� �ִϸ��̼��� ��� ��ġ

	_double		m_ChangeStartTime = { 0.0 };

	_float		m_fAnimSpeed;
	_bool		m_isPaused = false;
	_bool		m_isLoop = false;
	_bool		m_isFinished = false;
	_bool		m_isStop = false;

	/*
	m_fTimeAcc += pAnimation->mTicksPerSecond * fTimeDelta;
	mDuration <= m_fTimeAcc�̸� �ִϸ��̼� ���� ��
	*/

	_uint		m_iNumChannels = { 0 };		// �ִϸ��̼��� �����ϴµ� "�ʿ���" ���� ��
	vector<shared_ptr<class CBinaryChannel>>		m_Channels = {};	// �ִϸ��̼��� �����ϴµ� �ʿ��� ��
	vector<_uint>			m_iCurrentKeyFrames;

public:
	static shared_ptr<CBinaryAnimation> Create(shared_ptr<CFile_Manager> pFile, shared_ptr<class CBinaryModel> pModel);
	shared_ptr<CBinaryAnimation> Clone();
	void Free();
};

END