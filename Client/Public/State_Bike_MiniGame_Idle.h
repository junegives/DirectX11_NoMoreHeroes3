#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CState_Bike_MiniGame_Idle final : public CState
{
public:
	CState_Bike_MiniGame_Idle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CState_Bike_MiniGame_Idle() = default;

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

	_bool					m_iOtherAnim = false;

	_bool					m_isIn = false;
	_bool					m_isOut = false;

	_bool					m_isFade = false;

	weak_ptr<class CBike>	m_pBike;

	_uint					m_isLine = 1;
	_float					m_fLineX[3];

public:
	static shared_ptr<CState> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg = nullptr);
	virtual void Free();
};

END