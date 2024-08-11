#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CState abstract
{
public:
	CState(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CState() = default;

public:
	virtual HRESULT			Initialize(STATE eState, shared_ptr<class CStateMachine> pOwnerMachine);

	virtual STATE			Tick(_float fTimeDelta) = 0;
	virtual STATE			Late_Tick(_float fTimeDelta) = 0;
	virtual void			Reset_State() = 0;
	virtual void			Enter_State() = 0;

	virtual	STATE			Get_State() { return m_eState; }

	virtual void			Set_OwnerModel(shared_ptr<class CBinaryModel> pOwnerModel) { m_pOwnerModel = pOwnerModel; }

	void					Set_OwnerCollider(shared_ptr<class CCollider> pOwnerCollider) { m_pOwnerCollider = pOwnerCollider; }
	void					Set_NavigationCom(shared_ptr<class CNavigation> pNavigationCom) { m_pNavigation = pNavigationCom; }

protected:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };

	weak_ptr<class CTransform>		m_pOwnerTransform;
	shared_ptr<class CNavigation>	m_pNavigation;

	weak_ptr<class CStateMachine>	m_pOwnerMachine;
	weak_ptr<class CGameObject>		m_pOwnerObject;
	weak_ptr<class CGameObject>		m_pOwnerBody;
	weak_ptr<class CBinaryModel>	m_pOwnerModel;

	weak_ptr<class CCollider>		m_pOwnerCollider;

	shared_ptr<class CGameInstance>	m_pGameInstance = { nullptr };

	// 어떤 상태인지 정해주는 변수
	STATE							m_eState = { STATE_END };

	// 처음들어오면 
	_bool							m_bEnter = { false };

public:
	virtual void Free();
};

END