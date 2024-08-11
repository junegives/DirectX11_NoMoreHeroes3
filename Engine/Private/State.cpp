#include "State.h"
#include "StateMachine.h"

#include "GameObject.h"
#include "GameInstance.h"

CState::CState(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT CState::Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine)
{
	m_eState = eState;
	m_pOwnerMachine = pOwnerMachine;

	m_pOwnerObject = m_pOwnerMachine.lock()->Get_Owner();
	m_pOwnerBody = m_pOwnerMachine.lock()->Get_OwnerBody();
	m_pOwnerTransform = m_pOwnerObject.lock()->Get_TransformCom();

	return S_OK;
}

void CState::Free()
{
}
