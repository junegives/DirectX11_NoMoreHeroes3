#include "pch.h"
#include "Trail.h"

CTrail::CTrail(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject(pDevice, pContext)
{
}

CTrail::CTrail(const CTrail& rhs)
	: CGameObject(rhs)
{

}

HRESULT CTrail::Initialize(void* pArg)
{
	TRAIL_DESC* pTrailDesc = (TRAIL_DESC*)(pArg);

	m_TrailDesc = *pTrailDesc;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

void CTrail::Tick(_float fTimeDelta)
{
	m_TrailDesc.vUVAcc.x += m_TrailDesc.vUV_FlowSpeed.x * fTimeDelta;
	m_TrailDesc.vUVAcc.y += m_TrailDesc.vUV_FlowSpeed.y * fTimeDelta;

	if (!m_TrailDesc.isTrail)
		return;

	m_TrailDesc.fAccGenTrail += fTimeDelta;
	if (m_TrailDesc.fAccGenTrail >= m_TrailDesc.fGenTrailTime)
		m_pVIBufferCom->Update_TrailBuffer(fTimeDelta, XMLoadFloat4x4(&m_TransformMatrix));
}

void CTrail::Late_Tick(_float fTimeDelta)
{
	if (m_TrailDesc.isTrail)
		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, shared_from_this())))
			return;
}

HRESULT CTrail::Render()
{
	_float4x4		 WorldMatrix, ViewMatrix, ProjMatrix;

	//XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrix());
	//XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_VIEW);
	ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_PROJ);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_TransformMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_TrailDesc.vDiffuseColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fUVFlow", &m_TrailDesc.vUVAcc, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iCutUV", &m_TrailDesc.iUV_Cut, sizeof(_int))))
		return E_FAIL;


	_uint iPassIndex = 0;
	{

		if (m_TrailDesc.isDiffuse)
		{
			if (FAILED(m_pDiffuseTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
				return E_FAIL;
		}


		if (m_TrailDesc.isAlpha)
		{
			if (FAILED(m_pAlphaTextureCom->Bind_ShaderResource(m_pShaderCom, "g_AlphaTexture", 0)))
				return E_FAIL;
		}


		// 둘다 없다면.
		if (!m_TrailDesc.isDiffuse && !m_TrailDesc.isAlpha)
			iPassIndex = 0;

		//  있다면.
		if (!m_TrailDesc.isDiffuse && m_TrailDesc.isAlpha)
			iPassIndex = 1;
	}

	if (FAILED(m_pShaderCom->Begin(iPassIndex)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CTrail::Set_DiffuseTexture(const wstring& strDiffuseTextureName)
{
	m_TrailDesc.isDiffuse = true;
	m_TrailDesc.strDiffuseTag = strDiffuseTextureName;
}

void CTrail::Set_AlphaTexture(const wstring& strAlphaTextureName)
{
	m_TrailDesc.isAlpha = true;
	m_TrailDesc.strAlphaTag = strAlphaTextureName;
}

HRESULT CTrail::Add_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Trail"),
		TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"),
		TEXT("Com_VIBuffer"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pVIBufferCom), &m_TrailDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Trail"),
		TEXT("Com_Diffuse_Texture"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pDiffuseTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Trail"),
		TEXT("Com_Alpha_Texture"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pAlphaTextureCom))))
		return E_FAIL;

	return S_OK;
}

void CTrail::SetPos(_vector vHighPosition, _vector vLowPosition)
{
	m_pVIBufferCom->SetPos(vHighPosition, vLowPosition);
}

void CTrail::StartTrail(_matrix TransformMatrix)
{
	m_TrailDesc.isTrail = true;

	m_TrailDesc.vUVAcc.x = 0.f;
	m_TrailDesc.vUVAcc.y = 0.f;

	m_pVIBufferCom->StartTrail(TransformMatrix);
}

void CTrail::StopTrail()
{
	m_TrailDesc.isTrail = false;
	m_pVIBufferCom->StopTrail();
}


shared_ptr<CTrail> CTrail::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const TRAIL_DESC& TrailDesc)
{
	shared_ptr<CTrail> pInstance = make_shared<CTrail>(pDevice, pContext);
	if (FAILED(pInstance->Initialize((void*)&TrailDesc)))
	{
		MSG_BOX("Create Failed : CTrail");
		pInstance.reset();
	}
	return pInstance;
}

void CTrail::Free()
{
	__super::Free();

}
