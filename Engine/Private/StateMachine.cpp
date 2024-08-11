#include "StateMachine.h"

#include "State.h"
#include "BinaryModel.h"

CStateMachine::CStateMachine(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CComponent(pDevice, pContext)
{
}

CStateMachine::CStateMachine(const CStateMachine& rhs)
    : CComponent(rhs)
{
}

HRESULT CStateMachine::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CStateMachine::Initialize(void* pArg)
{
	/* 초기상태는 로컬상태 */
	STATEMACHINE_DESC* pDesc = (STATEMACHINE_DESC*)pArg;

	m_pOwner			= pDesc->pOwner;
	m_pOwnerBody		= pDesc->pOwnerBody;

    return S_OK;
}

void CStateMachine::Tick(_float fTimeDelta)
{
    STATE eState = m_pCurState->Tick(fTimeDelta);

	if (eState != m_eCurState)
	{
		m_pCurState->Reset_State();
		Set_State(eState);
	}
}

void CStateMachine::Late_Tick(_float fTimeDelta)
{
	STATE eState = m_pCurState->Late_Tick(fTimeDelta);
}

HRESULT CStateMachine::Set_State(STATE eState)
{
	auto	iter = find_if(m_StateMap.begin(), m_StateMap.end(), [&](const pair<STATE, shared_ptr<CState>>& pair) {
		return eState == pair.first;
		});

	if (iter == m_StateMap.end())
		return E_FAIL;

	if (STATE::STATE_END == m_eCurState)
		m_ePreState = eState;
	else
		m_ePreState = m_eCurState;

	m_pCurState = iter->second;
	m_eCurState = eState;
	m_pCurState->Enter_State();

	return S_OK;
}

HRESULT CStateMachine::Add_State(STATE eState, shared_ptr<CState> pState)
{
	if (nullptr == pState || eState != pState->Get_State())
		return E_FAIL;

	m_StateMap.emplace(eState, pState);

	return S_OK;
}

shared_ptr<CState> CStateMachine::Get_State(STATE eState)
{
	auto	iter = find_if(m_StateMap.begin(), m_StateMap.end(), [&](const pair<STATE, shared_ptr<CState>>& pair) {
		return eState == pair.first;
		});

	if (iter == m_StateMap.end())
		return nullptr;

	return iter->second;
}

HRESULT CStateMachine::Render()
{
    return S_OK;
}

shared_ptr<CStateMachine> CStateMachine::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CStateMachine> pInstance = make_shared<CStateMachine>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStateMachine");
		pInstance.reset();
	}

	return pInstance;
}

shared_ptr<CComponent> CStateMachine::Clone(void* pArg)
{
	shared_ptr<CComponent> pInstance = make_shared<CStateMachine>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStateMachine");
		pInstance.reset();
	}

	return pInstance;
}

void CStateMachine::Free()
{
}