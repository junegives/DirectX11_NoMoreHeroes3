#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CState_Armor_Missile final : public CState
{
public:
	CState_Armor_Missile(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CState_Armor_Missile() = default;

public:
	virtual HRESULT			Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg = nullptr);

	virtual STATE			Tick(_float fTimeDelta);
	virtual STATE			Late_Tick(_float fTimeDelta);
	virtual void			Reset_State();
	virtual void			Enter_State();

public:
	STATE					Key_Input(_float fTimeDelta);
	void					Set_OwnerTransModel(shared_ptr<class CBinaryModel> pOwnerModel) { m_pOwnerTransModel = pOwnerModel; }

private:
	_bool					m_isPressed = false;
	_bool					m_isPressMove = false;

	_bool					m_isIn = false;
	_bool					m_isLoop = false;
	_bool					m_isOut = false;

	_float					m_fWaitTime = 0.f;

	weak_ptr<class CBinaryModel>	m_pOwnerTransModel;

public:
	static shared_ptr<CState> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg = nullptr);
	virtual void Free();
};

END