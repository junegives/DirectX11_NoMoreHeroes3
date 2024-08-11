#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CState_Guard : public CState
{
public:
	CState_Guard(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CState_Guard() = default;

public:
	virtual HRESULT			Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg = nullptr);

	virtual STATE			Tick(_float fTimeDelta);
	virtual STATE			Late_Tick(_float fTimeDelta);
	virtual void			Reset_State();
	virtual void			Enter_State();
	
public:
	STATE					Key_Input(_float fTimeDelta);
	void					Lock_On();

private:
	shared_ptr<CGameObject>	m_pTarget = nullptr;

	_bool					m_isPressed = false;
	_bool					m_isUnPressed = false;
	_bool					m_isPressMove = false;

	// 가드 피격

	// 걷기 모션
	_bool					m_isWalking = false;

	_bool					m_isGuardIn = false;

	// 방향 이동 검사
	_bool					m_isF = false;
	_bool					m_isFL = false;
	_bool					m_isFR = false;

	_bool					m_isB = false;
	_bool					m_isBL = false;
	_bool					m_isBR = false;

	_bool					m_isL = false;
	_bool					m_isR = false;

	_bool					m_isWalkingOut = false;

	// 가드 풀기 모션
	_bool					m_isGuardEnd = false;

	_bool					m_isHit = false;

public:
	void					Check_Guard_State();

public:
	static shared_ptr<CState> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg = nullptr);
	virtual void Free();
};

END