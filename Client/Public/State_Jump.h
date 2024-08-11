#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CState_Jump final : public CState
{
public:
	CState_Jump(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CState_Jump() = default;

public:
	virtual HRESULT			Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg = nullptr);

	virtual STATE			Tick(_float fTimeDelta);
	virtual STATE			Late_Tick(_float fTimeDelta);
	virtual void			Reset_State();
	virtual void			Enter_State();

public:
	STATE					Key_Input(_float fTimeDelta);
	void					Lock_On();

	STATE					Jump_Check();
	_float					Jump();

private:
	shared_ptr<CGameObject>	m_pTarget = nullptr;

	_bool					m_isPressed = false;
	_bool					m_isIn = false;
	_bool					m_isLoop = false;
	_bool					m_isLanding = false;

	_bool					m_isLockEnter = true;

	_bool					m_isFalling = false;

	const _float			m_fGravity = 9.8f;
	_float					m_fMaxSpeed = 0.3f;
	_float					m_fJumpTime = 0.f;

public:
	static shared_ptr<CState> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg = nullptr);
	virtual void Free();
};

END