#include "pch.h"
#include "SwordTrail.h"

#include "Player.h"

CSwordTrail::CSwordTrail(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strTextureComTag)
    : CGameObject(pDevice, pContext)
{
	m_strTextureComTag = strTextureComTag;
}

CSwordTrail::CSwordTrail(const CSwordTrail& rhs)
    : CGameObject(rhs)
{
}

HRESULT CSwordTrail::Initialize(void* pArg)
{
	/*SWORDTRAIL_DESC* pTrailDesc = (SWORDTRAIL_DESC*)(pArg);

	m_TrailDesc = *pTrailDesc;*/

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

void CSwordTrail::Tick(_float fTimeDelta)
{
	CVIBuffer_SwordTrail::TRAIL_DESC TrailDesc;

	TrailDesc.vUpperPos = m_vUpperPos;
	TrailDesc.vLowerPos = m_vLowerPos;

	TrailDesc.vTrailMatrix = m_WeaponMatrix;

	m_pVIBufferCom->Update(TrailDesc);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_GLOW, shared_from_this())))
		return;

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, shared_from_this())))
		return;
}

void CSwordTrail::Late_Tick(_float fTimeDelta)
{
}

HRESULT CSwordTrail::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CSwordTrail::SetColor(_vector vColorBack, _vector vColorFront)
{
	m_vColorBack = vColorBack;
	m_vColorFront = vColorFront;
}

void CSwordTrail::UpdateTrail(_float3 vPointDown, _float3 vPointUp, _fmatrix vWeaponMatrix)
{
	m_vUpperPos = vPointDown;
	m_vLowerPos = vPointUp;

	XMStoreFloat4x4(&m_WeaponMatrix, vWeaponMatrix);
}

void CSwordTrail::ClearTrail()
{
	m_pVIBufferCom->Clear();
}

HRESULT CSwordTrail::Add_Component()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_EffectTexture"),
		TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_SwordTrail"),
		TEXT("Com_VIBuffer"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_strTextureComTag,
		TEXT("Com_Texture"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSwordTrail::Bind_ShaderResources()
{
	_float4x4		 ViewMatrix, ProjMatrix;

	ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_VIEW);
	ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_PROJ);

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorBack", &m_vColorBack, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorFront", &m_vColorFront, sizeof(_float4))))
		return E_FAIL;

	m_fAlpha = 1.f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CSwordTrail> CSwordTrail::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strTextureComTag, void* pArg)
{
	shared_ptr<CSwordTrail> pInstance = make_shared<CSwordTrail>(pDevice, pContext, strTextureComTag);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CSwordTrail");
		pInstance.reset();
	}
	return pInstance;
}

void CSwordTrail::Free()
{
}
