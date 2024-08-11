#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CState_Attack final : public CState
{
public:
	CState_Attack(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CState_Attack() = default;

public:
	virtual HRESULT				Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg = nullptr);

	virtual STATE				Tick(_float fTimeDelta);
	virtual STATE				Late_Tick(_float fTimeDelta);
	virtual void				Reset_State();
	virtual void				Enter_State();

public:
	STATE						Key_Input(_float fTimeDelta);
	void						Lock_On();

	// ÄÞº¸
	STATE						Combo_Check();

	void						Block_RootMotion();
	void						Change_Look();

	void						SetOwnerAttackCollider(shared_ptr<class CCollider> pCollider) { m_pOwnerAttackCollider = pCollider; }
	shared_ptr<class CCollider>	GetOwnerAttackCollider() { return m_pOwnerAttackCollider.lock(); }

private:
	weak_ptr<class CCollider>	m_pOwnerAttackCollider;
	shared_ptr<CGameObject>		m_pTarget = nullptr;

	shared_ptr<class CWeapon>	m_pWeapon = nullptr;

	_bool						m_isPressed = false;
	_bool						m_isBlocked = false;

	_bool						m_isLockEnter = true;

	_bool						m_isAttackColSet = false;

	_uint						m_iCallCnt = 0;

	// ÄÞº¸ °ø°Ý
	_bool						m_isAttack1 = false;
	_bool						m_isAttack2 = false;
	_bool						m_isAttack3 = false;
	_bool						m_isAttack4 = false;
	_bool						m_isAttack5 = false;

	_bool						m_isStackCombo = false;

public:
	static shared_ptr<CState> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg = nullptr);
	virtual void Free();
};

END