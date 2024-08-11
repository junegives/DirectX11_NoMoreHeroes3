#pragma once

#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Terrain;
END

BEGIN(Tool)

class CTerrain : public CGameObject
{
	enum TEXTURE_TYPE { TEXTURE_DIFFUSE, TEXTURE_MASK, TEXTURE_END };

public:
	CTerrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain();

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void SetTerrainSize(_uint* iTerrainSize) {
		m_iTerrainSize[0] = iTerrainSize[0];
		m_iTerrainSize[1] = iTerrainSize[1];
	}

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CTexture> m_pTextureCom[TEXTURE_END] = { nullptr };
	shared_ptr<CVIBuffer_Terrain> m_pVIBufferCom = { nullptr };

private:
	_bool		m_bPicked = false;
	_uint		m_iTerrainSize[2] = { 0, 0 };

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CTerrain> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
	virtual void Free() override;
};

END