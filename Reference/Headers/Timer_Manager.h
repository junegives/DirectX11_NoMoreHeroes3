#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CTimer_Manager final
{
public:
	CTimer_Manager();
	~CTimer_Manager() = default;

public:	
	_float Compute_TimeDelta(const wstring& pTimerTag);

public:
	HRESULT Add_Timer(const wstring& pTimerTag);

private:
	shared_ptr<class CTimer> Find_Timer(const wstring& pTimerTag);

private:
	map<const wstring, shared_ptr<class CTimer>>		m_Timers;

public:
	static shared_ptr<CTimer_Manager> Create();
	virtual void Free();
};

END