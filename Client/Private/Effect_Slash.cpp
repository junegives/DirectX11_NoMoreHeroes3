#include "pch.h"
#include "Effect_Slash.h"

CEffect_Slash::CEffect_Slash(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strTextureComTag)
	: CEffect(pDevice, pContext)
{
	m_strTextureComTag = strTextureComTag;
}

CEffect_Slash::CEffect_Slash(const CEffect_Slash& rhs)
	: CEffect(rhs)
{
}

CEffect_Slash::~CEffect_Slash()
{
}

HRESULT CEffect_Slash::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return E_NOTIMPL;
}

void CEffect_Slash::Priority_Tick(_float fTimeDelta)
{
}

void CEffect_Slash::Tick(_float fTimeDelta)
{
}

void CEffect_Slash::Late_Tick(_float fTimeDelta)
{
}

HRESULT CEffect_Slash::Render()
{
	return E_NOTIMPL;
}

void CEffect_Slash::Play(_uint iNum, _vector vCenter, _vector vScale, _vector vUp, _vector vLook)
{
}

void CEffect_Slash::Play(_uint iNum, _vector vCenter, _bool isRotate, _vector vAxis, _vector vRotate)
{
}

void CEffect_Slash::Play(shared_ptr<CTransform> pParentTransform)
{
}

HRESULT CEffect_Slash::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, m_strTextureComTag,
		TEXT("Com_Texture"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_EffectTexture"),
		TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pVIBufferCom))))
		return E_FAIL;
}

shared_ptr<CEffect_Slash> CEffect_Slash::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strTextureComTag, void* pArg)
{
	shared_ptr<CEffect_Slash> pInstance = make_shared<CEffect_Slash>(pDevice, pContext, strTextureComTag);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CEffect_Slash");
		pInstance.reset();
	}

	return pInstance;
}

void CEffect_Slash::Free()
{
}
