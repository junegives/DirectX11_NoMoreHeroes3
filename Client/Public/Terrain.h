#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Terrain;
class CVIBuffer_HeightTerrain;
END

BEGIN(Client)

class CTerrain : public CGameObject
{
	enum TEXTURE_TYPE { TEXTURE_DIFFUSE, TEXTURE_MASK, TEXTURE_END };

public:
	CTerrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	shared_ptr<CShader>		m_pShaderCom = { nullptr };
	shared_ptr<CTexture>	m_pTextureCom[TEXTURE_END] = {nullptr};
	shared_ptr<CNavigation>	m_pNavigationCom = { nullptr };
	shared_ptr<CVIBuffer_HeightTerrain> m_pVIBufferCom = { nullptr };

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CTerrain> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void Free() override;
};

END