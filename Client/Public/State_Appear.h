#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CState_Appear final : public CState
{
public:
	CState_Appear(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CState_Appear() = default;

public:
	virtual HRESULT			Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg = nullptr);

	virtual STATE			Tick(_float fTimeDelta);
	virtual STATE			Late_Tick(_float fTimeDelta);
	virtual void			Reset_State();
	virtual void			Enter_State();

private:
	_bool					m_isIn = false;
	_bool					m_isLoop = false;
	_bool					m_isOut = false;

	_bool					m_isWeaponAttached = false;

	_bool					m_isLeopardon = false;
	_bool					m_isTriplePut = false;

	_bool					m_isBattle1Bone1 = false;
	_bool					m_isBattle1Bone2 = false;

	_bool					m_isBattle2Bone1 = false;
	_bool					m_isBattle2Bone2 = false;

	LEVEL					m_eNextLevel = LEVEL_END;

public:
	static shared_ptr<CState> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg = nullptr);
	virtual void Free();
};

END