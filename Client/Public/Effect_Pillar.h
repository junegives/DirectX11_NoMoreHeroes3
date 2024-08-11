#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CEffect_Pillar final : public CEffect
{
public:
	CEffect_Pillar(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CEffect_Pillar(const CEffect_Pillar& rhs);
	~CEffect_Pillar();

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	shared_ptr<CTexture>				m_pMaskTextureCom = nullptr;
	shared_ptr<CShader>					m_pShaderCom = nullptr;
	shared_ptr<CBinaryModel>			m_pModelCom = nullptr;

	wstring								m_strMaskTextureComTag;
	_float2								m_vUVPivot = { 0.f, 0.f };
	_float								m_fAlpha = 1.f;

public:
	virtual void	Play(_uint iNum, _vector vCenter, _vector vScale, _vector vUp, _vector vLook);
	virtual void	Play(_uint iNum, _vector vCenter, _bool isRotate, _vector vAxis, _vector vRotate) override;
	virtual void	Play(shared_ptr<CTransform> pParentTransform);

protected:
	HRESULT Add_Component();

public:
	static shared_ptr<CEffect_Pillar> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END