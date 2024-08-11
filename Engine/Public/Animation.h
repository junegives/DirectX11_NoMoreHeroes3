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
	_uint		m_iNumAnimations = { 0 };	// �ִϸ��̼� ����
	_char		m_szName[MAX_PATH] = "";	// �ִϸ��̼� �̸�

	_double		m_Duration = { 0.0 };		// �ִϸ��̼��� �����ϴµ� �ɸ��� ��ü �ð� (���ٴ� �ִϸ��̼� ��ü�� �����ϴ� ����)
	_double		m_TickPerSecond = { 0.0 };	// �ִϸ��̼� ��� �ӵ�
	_double		m_TrackPosition = { 0.0 };	// ���� �� �ִϸ��̼��� ��� ��ġ

	/*
	m_fTimeAcc += pAnimation->mTicksPerSecond * fTimeDelta;
	mDuration <= m_fTimeAcc�̸� �ִϸ��̼� ���� ��
	*/

	_uint		m_iNumChannels = { 0 };		// �ִϸ��̼��� �����ϴµ� "�ʿ���" ���� ��
	vector<shared_ptr<class CChannel>> m_Channels = {};	// �ִϸ��̼��� �����ϴµ� �ʿ��� ��
	vector<_uint>			m_iCurrentKeyFrames;

	_bool		m_isFinished = { false };

public:
	static shared_ptr<CAnimation> Create(const aiAnimation* pAIAnimation, shared_ptr<class CModel> pModel);
	shared_ptr<CAnimation> Clone();
	void Free();
};

END