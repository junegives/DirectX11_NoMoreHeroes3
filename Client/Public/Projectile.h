#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CProjectile : public CGameObject
{
public:
	CProjectile(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CProjectile(const CProjectile& rhs);
	virtual ~CProjectile() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool		IsLaunched() { return m_isLaunched; }
	void		SetLaunched(_bool isLaunched) { m_isLaunched = isLaunched; }

	_bool		IsCollided() { return m_isCollided; }
	void		SetCollided(_bool isCollided) { m_isCollided = isCollided; }

	HIT_TYPE	Get_HitType() { return m_eHitType; }
	_uint		Get_AP() { return m_iAP; }

protected:
	shared_ptr<CCollider>		m_pColliderCom = { nullptr };
	shared_ptr<CNavigation>		m_pNavigationCom = { nullptr };

protected:
	_bool						m_isLaunched = false;
	_bool						m_isCollided = false;

protected:
	HIT_TYPE					m_eHitType = HIT_END;
	_uint						m_iAP = 0;

protected:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

	void	Collision_ToPlayer();
	_bool	Projectile_Go_Straight(_float fTimeDelta);

public:
	static shared_ptr<CProjectile> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END