#pragma once

#include "Client_Defines.h"
#include "Monster.h"

#include "Sound_Manager.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CMonster_TriplePut final : public CMonster
{
public:
	CMonster_TriplePut(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CMonster_TriplePut(const CMonster_TriplePut& rhs);
	virtual ~CMonster_TriplePut() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	OnCollision(LAYER_ID eColLayer, shared_ptr<class CCollider> pCollider) override;

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();
	HRESULT Add_States();

	void Collision_ToPlayer();

public:
	_bool						IsVisible() { return m_isVisible; }
	_bool						SetDamage();

	void						SetDead(_bool isDead) { m_isDead = isDead; }
	_bool						IsDead() { return m_isDead; }

public:
	STATE						m_eCurState = STATE_END;
	shared_ptr<CCollider>		m_pAttack1ColliderCom = { nullptr };
	shared_ptr<CCollider>		m_pAttack2ColliderCom = { nullptr };

	shared_ptr<class CUI_HP_Monster>  m_pUI_HP = nullptr;
	_bool						m_isDead = false;
	_float						m_fHitAfter = 0.0f;
	_bool						m_isHitAfter = false;

private:
	_bool						m_isVisible = true;

public:
	static shared_ptr<CMonster_TriplePut> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END