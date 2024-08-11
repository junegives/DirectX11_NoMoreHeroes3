#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Particle_Rect;
END

BEGIN(Client)

class CParticle_Rect final : public CGameObject
{
public:
	CParticle_Rect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CParticle_Rect(const CParticle_Rect& rhs);
	~CParticle_Rect();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	shared_ptr<CShader>						m_pShaderCom = { nullptr };
	shared_ptr<CTexture>					m_pTextureCom = { nullptr };
	shared_ptr<CVIBuffer_Particle_Rect>		m_pVIBufferCom = { nullptr };

private:
	HRESULT Add_Component();


public:
	static shared_ptr<CParticle_Rect>	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END