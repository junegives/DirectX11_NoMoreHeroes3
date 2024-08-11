#include "pch.h"
#include "Sky.h"

CSky::CSky(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CGameObject(pDevice, pContext)
{
}

CSky::CSky(const CSky& rhs)
    : CGameObject(rhs)
{
}

HRESULT CSky::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		CGameObject::GAMEOBJECT_DESC			GameObjectDesc = {};

		GameObjectDesc.iGameObjectData = 10;
		GameObjectDesc.fSpeedPerSec = 10.0f;
		GameObjectDesc.fRotationPerSec = XMConvertToRadians(60.0f);

		if (FAILED(__super::Initialize(&GameObjectDesc)))
			return E_FAIL;
	}

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

void CSky::Priority_Tick(_float fTimeDelta)
{
}

void CSky::Tick(_float fTimeDelta)
{
}

void CSky::Late_Tick(_float fTimeDelta)
{
	_float4 CamPos = m_pGameInstance->Get_CamPosition();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&CamPos));

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_PRIORITY, shared_from_this())))
		return;
}

HRESULT CSky::Render()
{
	_float4x4 ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_VIEW);
	_float4x4 ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_PROJ);

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 3)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 4)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CSky::Add_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky"),
		TEXT("Com_Texture"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"),
		TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CSky> CSky::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
	shared_ptr<CSky> pInstance = make_shared<CSky>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CSky");
		pInstance.reset();
	}

	return pInstance;
}

void CSky::Free()
{
	__super::Free();
}
