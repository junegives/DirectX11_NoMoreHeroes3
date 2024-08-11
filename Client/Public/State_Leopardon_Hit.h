#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CState_Leopardon_Hit final : public CState
{
public:
	CState_Leopardon_Hit(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CState_Leopardon_Hit() = default;

public:
	virtual HRESULT			Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg = nullptr);

	virtual STATE			Tick(_float fTimeDelta);
	virtual STATE			Late_Tick(_float fTimeDelta);
	virtual void			Reset_State();
	virtual void			Enter_State();

private:
	shared_ptr<CGameObject>	m_pTarget = nullptr;

	HIT_TYPE				m_eHitType = HIT_END;
	DIRECTION				m_eHitDir = DIR_END;

	_uint					m_iHitRand = 0;

	_bool					m_isPressed = false;

	_bool					m_isDowned = false;

	_bool					m_isFade = false;

public:
	static shared_ptr<CState> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg = nullptr);
	virtual void Free();
};

END