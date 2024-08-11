#pragma once

#include "Client_Defines.h"
#include "Projectile.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CProjectile_Leopardon_Cube final : public CProjectile
{
public:
	CProjectile_Leopardon_Cube(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CProjectile_Leopardon_Cube(const CProjectile_Leopardon_Cube& rhs);
	virtual ~CProjectile_Leopardon_Cube() = default;

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
	shared_ptr<CTexture>		m_pMaskTextureGenerate = { nullptr };
	shared_ptr<CTexture>		m_pMaskTextureThrow = { nullptr };
	shared_ptr<CTexture>		m_pMaskTextureCube = { nullptr };

public:
	_bool						IsVisible() { return m_isVisible; }
	void						SetVisible(_bool isVisible) { m_isVisible = isVisible; }

public:
	void						ThrowCube();
	void						HideCube();

	void						GenerateCube();

private:
	_bool						m_isVisible = true;

private:
	_bool						m_isThrow = false;

	_float						m_fUVx = 0.f, m_fUVy = 0.f;
	_float						m_fMaskAlpha0 = 0.f, m_fMaskAlpha1 = 0.f;
	_float						m_fLifeTime = 0.f;

	_float3						m_vColor0 = { 0.9, 0.3, 0.3 }, m_vColor1 = { 0.5, 0.87, 0.96 };

	_float						m_fSoundDelay = 0.f;

public:
	static shared_ptr<CProjectile_Leopardon_Cube> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END