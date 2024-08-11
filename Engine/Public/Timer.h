#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CTimer final
{
public:
	CTimer();
	virtual ~CTimer() = default;

public:
	HRESULT	Ready_Timer(void);
	_float Compute_TimeDelta(void);

private:
	LARGE_INTEGER			m_FrameTime;
	LARGE_INTEGER			m_FixTime;
	LARGE_INTEGER			m_LastTime;
	LARGE_INTEGER			m_CpuTick;

	_float					m_fTimeDelta;

public:
	static shared_ptr<CTimer>	Create(void);

private:
	virtual void	Free();
};

END