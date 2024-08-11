#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CEffect_Distortion : public CEffect
{
public:
	CEffect_Distortion(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag, const wstring& strMaskTextureComTag = L"");
	virtual ~CEffect_Distortion() = default;

public:
	virtual HRESULT	Initialize(void* pArg = nullptr) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	shared_ptr<CTexture>				m_pMaskTextureCom = nullptr;
	shared_ptr<CShader>					m_pShaderCom = nullptr;
	shared_ptr<CBinaryModel>			m_pModelCom = nullptr;

private:
	wstring								m_strMaskTextureComTag;
	wstring								m_strModelComTag;
	_float								m_fAlpha = 1.f;

	_bool								m_isFollow = false;
	_bool								m_isTest = true;

	shared_ptr<CTransform>				m_pParentTransformCom = nullptr;

	_float								m_fDistortionTimer = 0.f;
	_float								m_fDistortionSpeed = 0.f;

public:
	virtual void	Play(_uint iNum, _vector vCenter, _vector vScale, _vector vUp, _vector vLook);
	virtual void	Play(_uint iNum, _vector vCenter, _bool isRotate, _vector vAxis, _vector vRotate);
	virtual void	Play(shared_ptr<CTransform> pParentTransform);

protected:
	HRESULT			Add_Component();

public:
	static shared_ptr<CEffect_Distortion> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag, const wstring& strMaskTextureComTag = L"", void* pArg = nullptr);
	virtual void Free() override;
};

END