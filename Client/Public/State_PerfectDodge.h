#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CState_PerfectDodge final : public CState
{
public:
	CState_PerfectDodge(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CState_PerfectDodge() = default;

public:
	virtual HRESULT			Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg = nullptr);

	virtual STATE			Tick(_float fTimeDelta);
	virtual STATE			Late_Tick(_float fTimeDelta);
	virtual void			Reset_State();
	virtual void			Enter_State();

public:
	STATE					Key_Input(_float fTimeDelta);

private:
	_bool					m_isPressed = false;
	_bool					m_isPressMove = false;

	_bool					m_isIn = false;
	_bool					m_isLoop = false;
	_bool					m_isOut = false;

private:
	shared_ptr<CGameObject>	m_pTarget = nullptr;
	_vector					m_vOriginPos;
	_vector					m_vTargetCenter;

public:
	static shared_ptr<CState> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg = nullptr);
	virtual void Free();
};

END