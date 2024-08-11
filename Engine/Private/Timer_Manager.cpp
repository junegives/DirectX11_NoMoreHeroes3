#include "Timer_Manager.h"
#include "Timer.h"

CTimer_Manager::CTimer_Manager()
{

}

_float CTimer_Manager::Compute_TimeDelta(const wstring& strpTimerTag)
{
	shared_ptr<CTimer> pTimer = Find_Timer(strpTimerTag);
	if (nullptr == pTimer)
		return 0.0f;

	return pTimer->Compute_TimeDelta();
}

HRESULT CTimer_Manager::Add_Timer(const wstring& strTimerTag)
{
	shared_ptr<CTimer>		pTimer = Find_Timer(strTimerTag);

	if (nullptr != pTimer)
		return E_FAIL;

	pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	m_Timers.insert({ strTimerTag, pTimer });

	return S_OK;
}

shared_ptr<CTimer> CTimer_Manager::Find_Timer(const wstring& strTimerTag)
{
	auto	iter = m_Timers.find(strTimerTag);

	if (iter == m_Timers.end())
		return nullptr;

	return iter->second;
}

shared_ptr<CTimer_Manager> CTimer_Manager::Create()
{
	return make_shared<CTimer_Manager>();
}

void CTimer_Manager::Free(void)
{
	m_Timers.clear();
}
