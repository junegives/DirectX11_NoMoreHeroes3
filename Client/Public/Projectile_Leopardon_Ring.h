#pragma once

#include "Client_Defines.h"
#include "Projectile.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CProjectile_Leopardon_Ring final : public CProjectile
{
public:
	CProjectile_Leopardon_Ring(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CProjectile_Leopardon_Ring(const CProjectile_Leopardon_Ring& rhs);
	virtual ~CProjectile_Leopardon_Ring() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollision(LAYER_ID eColLayer, shared_ptr<class CCollider> pCollider) override;

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

private:
	shared_ptr<CShader>			m_pShaderCom = { nullptr };
	shared_ptr<CBinaryModel>	m_pModelCom = { nullptr };

	shared_ptr<CTexture>		m_pDiffuseTexture = { nullptr };
	shared_ptr<CTexture>		m_pMaskTextureCom = { nullptr };

	shared_ptr<class CMeshTrail>	m_pMeshTrail;

	_uint					m_iGenerateAfterImage = 0;

	_bool					m_isSoundOn = false;

	_float					m_fLifeTime = 5.0f;

public:
	_bool						IsVisible()					{ return m_isVisible; }
	void						SetVisible(_bool isVisible)	{ m_isVisible = isVisible; }

	void						SetAttackPower(_int iAP)	{ m_iAP = iAP; }
	_uint						GetAttackPower()			{ return m_iAP; }

	void						SetIdx(_int iIdx)			{ m_iIdx = iIdx; }
	_uint						GetIdx()					{ return m_iIdx; }

	void						SetSpreadFirst()			{ m_isSpreadFirst = true; }
	void						SetSpreadSecond()			{ m_isSpreadSecond = true; }

public:
	void						Throw();
	void						Hide();

private:
	_uint						m_iIdx = 0;
	_bool						m_isVisible = true;

	_bool						m_isSpreadFirst = false;
	_bool						m_isSpreadSecond = false;
	
	_float						m_fSpreadTime = 0.f;
	_float						m_fSpreadFirstMaxTime = 0.25f;
	_float						m_fSpreadSecondMaxTime = 0.25f;

public:
	static shared_ptr<CProjectile_Leopardon_Ring> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END