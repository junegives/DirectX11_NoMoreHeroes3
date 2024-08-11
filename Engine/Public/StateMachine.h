#pragma once

#include "Engine_Defines.h"
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CStateMachine : public CComponent
{
public:
	typedef struct
	{
		shared_ptr<class CGameObject>		pOwner;
		shared_ptr<class CGameObject>		pOwnerBody;
	}STATEMACHINE_DESC;

public:
	CStateMachine(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CStateMachine(const CStateMachine& rhs);
	virtual ~CStateMachine() = default;

public:
	HRESULT		Initialize_Prototype();
	HRESULT		Initialize(void* pArg) override;
	void		Tick(_float fTimeDelta);
	void		Late_Tick(_float fTimeDelta);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

public:
	HRESULT						Set_State(STATE eState);
	HRESULT						Add_State(STATE eState, shared_ptr<class CState> pState);
	shared_ptr<class CState>	Get_State(STATE eState);

public:
	STATE						Get_CurState() { return m_eCurState; }
	STATE						Get_PreState() { return m_ePreState; }

public:
	shared_ptr<class CGameObject>	Get_Owner() { return m_pOwner.lock(); }
	shared_ptr<class CGameObject>	Get_OwnerBody() { return m_pOwnerBody.lock(); }

	map<STATE, shared_ptr<class CState>>		Get_StateMap() { return m_StateMap; }

protected:

protected:
	map<STATE, shared_ptr<class CState>>		m_StateMap;
	shared_ptr<class CState>					m_pCurState = { nullptr };
	STATE										m_eCurState = { STATE::STATE_END };
	STATE										m_ePreState = { STATE::STATE_END };

protected:
	weak_ptr<class CGameObject>					m_pOwner;
	weak_ptr<class CGameObject>					m_pOwnerBody;

public:
	static shared_ptr<CStateMachine> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	virtual void Free() override;
};

END