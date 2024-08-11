#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CState_DeathKick final : public CState
{
public:
	CState_DeathKick(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CState_DeathKick() = default;

public:
	virtual HRESULT			Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg = nullptr);

	virtual STATE			Tick(_float fTimeDelta);
	virtual STATE			Late_Tick(_float fTimeDelta);
	virtual void			Reset_State();
	virtual void			Enter_State();

public:
	STATE					Key_Input(_float fTimeDelta);
	STATE					State_Check();

	void						SetOwnerAttackCollider(shared_ptr<class CCollider> pCollider) { m_pOwnerAttackCollider = pCollider; }
	shared_ptr<class CCollider>	GetOwnerAttackCollider() { return m_pOwnerAttackCollider.lock(); }

private:
	weak_ptr<class CCollider>	m_pOwnerAttackCollider;
	_bool						m_isMouseMoved = false;

	_bool						m_isRandom = false;

private:
	_bool						m_isIn = false;
	_bool						m_isOut = false;
	shared_ptr<CGameObject>		m_pTarget = nullptr;

public:
	static shared_ptr<CState> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg = nullptr);
	virtual void Free();
};

END