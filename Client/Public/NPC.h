#pragma once

#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CNPC : public CLandObject
{
public:
	CNPC(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CNPC(const CNPC& rhs);
	virtual ~CNPC() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_float					GetDistanceToPlayer();
	void					SetYChange(_bool isYChange) { m_isYChange = isYChange; }

	_bool					IsHit() { return m_isHit; }
	void					SetHit(_bool isHit) { m_isHit = isHit; }

	HIT_TYPE				GetHitType() { return m_eHitType; }
	DIRECTION				GetHitDir() { return m_eHitDir; }

	void					SetHitType(HIT_TYPE eHitType) { m_eHitType = eHitType; }
	void					SetAP(_uint iAP) { m_iSaveDamage = iAP; }

	virtual void			OnCollision(LAYER_ID eColLayer, shared_ptr<class CCollider> pCollider) override;

protected:
	shared_ptr<CShader>			m_pShaderCom = { nullptr };
	shared_ptr<CBinaryModel>	m_pModelCom = { nullptr };
	shared_ptr<CCollider>		m_pColliderCom = { nullptr };
	shared_ptr<CStateMachine>	m_pStateMachineCom = { nullptr };

protected:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

	void	Collision_ToPlayer();

protected:
	_float						m_fDistanceToPlayer;
	_bool						m_isYChange = false;

	HIT_TYPE					m_eHitType = HIT_LIGHT;
	DIRECTION					m_eHitDir = DIR_F;
	_bool						m_isHit = false;

public:
	static shared_ptr<CNPC> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END