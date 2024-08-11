#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CState_TriplePut_Spin final : public CState
{
public:
	CState_TriplePut_Spin(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CState_TriplePut_Spin() = default;

public:
	virtual HRESULT			Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg = nullptr);

	virtual STATE			Tick(_float fTimeDelta);
	virtual STATE			Late_Tick(_float fTimeDelta);
	virtual void			Reset_State();
	virtual void			Enter_State();

public:
	STATE							Check_State();
	void							SetOwnerAttackCollider(shared_ptr<class CCollider> pCollider) { m_pOwnerAttackCollider = pCollider; }
	shared_ptr<class CCollider>		GetOwnerAttackCollider() { return m_pOwnerAttackCollider.lock(); }

public:
	static shared_ptr<CState> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg = nullptr);
	virtual void Free();

private:

private:
	_bool			m_isSetSuperDodgeTime = false;
	_bool			m_isOnAttack = false;
	_bool			m_isEndAttack = false;

	weak_ptr<class CCollider>	m_pOwnerAttackCollider;
};

END