#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CMonster_Leopardon final : public CMonster
{
public:
	CMonster_Leopardon(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CMonster_Leopardon(const CMonster_Leopardon& rhs);
	virtual ~CMonster_Leopardon() = default;

public:
	shared_ptr<class CComponent> Get_PartObjectComponent(const wstring& strPartObjTag, const wstring& strComTag);

	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	OnCollision(LAYER_ID eColLayer, shared_ptr<class CCollider> pCollider) override;

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();
	HRESULT Add_PartObjects();
	HRESULT Add_States();

	void Collision_ToPlayer();

public:
	_uint		GetNumWarp() { return (_uint)m_TeleportVec.size(); }

	void		AddTeleport(shared_ptr<class CLandObject_Leopardon_Teleport> pTeleport) { m_TeleportVec.push_back(pTeleport); }
	vector<shared_ptr<class CLandObject_Leopardon_Teleport>>	GetTeleportVec() { return m_TeleportVec; }
	shared_ptr<class CLandObject_Leopardon_Teleport>			GetRandomTeleport() { return m_TeleportVec[rand() % m_TeleportVec.size()]; }

	void		AddRing(shared_ptr<class CProjectile_Leopardon_Ring> pRing) { m_RingVec.push_back(pRing); }
	vector<shared_ptr<class CProjectile_Leopardon_Ring>>		GetRingVec() { return m_RingVec; }

public:
	_bool						IsVisible() { return m_isVisible; }
	_bool						SetDamage();

public:
	STATE							m_eCurState = STATE_END;
	shared_ptr<class CUI_HP_Boss>	m_pUI_HP = nullptr;
	_bool							m_isDead = false;

private:
	_bool						m_isVisible = true;
	shared_ptr<CCollider>		m_pAttackColliderCom = { nullptr };

	vector<shared_ptr<class CLandObject_Leopardon_Teleport>>	m_TeleportVec;
	vector<shared_ptr<class CProjectile_Leopardon_Ring>>		m_RingVec;

	map<const wstring, shared_ptr<class CPartObject>>			m_MonsterParts;


public:
	static shared_ptr<CMonster_Leopardon> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END