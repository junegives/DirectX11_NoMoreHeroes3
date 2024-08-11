#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CState_TriplePut_Rush final : public CState
{
public:
	CState_TriplePut_Rush(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CState_TriplePut_Rush() = default;

public:
	virtual HRESULT			Initialize(STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg = nullptr);

	virtual STATE			Tick(_float fTimeDelta);
	virtual STATE			Late_Tick(_float fTimeDelta);
	virtual void			Reset_State();
	virtual void			Enter_State();

public:
	STATE							Check_State(_float fTimeDelta);
	void							SetOwnerAttackCollider(shared_ptr<class CCollider> pCollider) { m_pOwnerAttackCollider = pCollider; }
	shared_ptr<class CCollider>		GetOwnerAttackCollider() { return m_pOwnerAttackCollider.lock(); }

	void							SetTrail();

private:
	_bool					m_isIn;
	_bool					m_isLoop;
	_bool					m_isOut;

	_vector					m_vDest;
	_float					m_fPrevDist = 100.f;
	_uint					m_iFarCnt = 0;

	weak_ptr<class CCollider>	m_pOwnerAttackCollider;

	_float4x4*				m_pTrailHandL1Matrix = { nullptr };
	_float4x4*				m_pTrailHandL2Matrix = { nullptr };

	_float4x4*				m_pTrailHandR1Matrix = { nullptr };
	_float4x4*				m_pTrailHandR2Matrix = { nullptr };

	_float4x4*				m_pTrailFootLMatrix = { nullptr };
	_float4x4*				m_pTrailFootRMatrix = { nullptr };

	shared_ptr<class CRushTrail>	m_pTrailHandL;
	shared_ptr<class CRushTrail>	m_pTrailHandR;
	shared_ptr<class CRushTrail>	m_pTrailFootL;
	shared_ptr<class CRushTrail>	m_pTrailFootR;

	_float3					m_vUpperPos;
	_float3					m_vLowerPos;


public:
	static shared_ptr<CState> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, STATE eState, shared_ptr<CStateMachine> pOwnerMachine, void* pArg = nullptr);
	virtual void Free();
};

END