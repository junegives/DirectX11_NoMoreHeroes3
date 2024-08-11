#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CEffect_Slash final : public CEffect
{
public:
	CEffect_Slash(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strTextureComTag);
	CEffect_Slash(const CEffect_Slash& rhs);
	~CEffect_Slash();

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void	Play(_uint iNum, _vector vCenter, _vector vScale, _vector vUp, _vector vLook);
	virtual void	Play(_uint iNum, _vector vCenter, _bool isRotate, _vector vAxis, _vector vRotate) override;
	virtual void	Play(shared_ptr<CTransform> pParentTransform);

private:
	shared_ptr<CTexture>				m_pTextureCom = nullptr;
	shared_ptr<CShader>					m_pShaderCom = nullptr;
	shared_ptr<CVIBuffer_Rect>			m_pVIBufferCom = nullptr;

private:
	wstring								m_strTextureComTag = L"";

protected:
	HRESULT Add_Component();

public:
	static shared_ptr<CEffect_Slash> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strTextureComTag, void* pArg = nullptr);
	virtual void Free() override;
};

END