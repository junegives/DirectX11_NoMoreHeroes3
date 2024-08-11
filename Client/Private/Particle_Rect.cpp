#include "pch.h"
#include "..\Public\Particle_Rect.h"

#include "GameInstance.h"
#include "Particle_Rect.h"

CParticle_Rect::CParticle_Rect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject(pDevice, pContext)
{
}

CParticle_Rect::CParticle_Rect(const CParticle_Rect& rhs)
	: CGameObject(rhs)
{
}

CParticle_Rect::~CParticle_Rect()
{
	Free();
}

HRESULT CParticle_Rect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_Rect::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC			GameObjectDesc = {};

	GameObjectDesc.iGameObjectData = 10;
	GameObjectDesc.fSpeedPerSec = 10.0f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(60.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

void CParticle_Rect::Priority_Tick(_float fTimeDelta)
{
}

void CParticle_Rect::Tick(_float fTimeDelta)
{
	//m_pVIBufferCom->Tick_Drop(fTimeDelta);
	m_pVIBufferCom->Tick_Spread(fTimeDelta);
}

void CParticle_Rect::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, shared_from_this())))
		return;
}

HRESULT CParticle_Rect::Render()
{
	_float4x4 matView = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_VIEW);
	_float4x4 matProj = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_PROJ);


	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &matView)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &matProj)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Rect::Add_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Snow"),
		TEXT("Com_Texture"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
		return E_FAIL;

	CVIBuffer_Instancing::INSTANCE_DESC			InstanceDesc{};
	InstanceDesc.vCenter = _float3(0.0f, 10.f, 0.f);
	InstanceDesc.vRange = _float3(10.0f, 2.f, 10.f);
	InstanceDesc.vSize = _float2(0.2f, 0.7f);
	InstanceDesc.vSpeed = _float2(2.f, 5.f);
	InstanceDesc.vLifeTime = _float2(3.f, 5.f);
	InstanceDesc.isLoop = true;
	InstanceDesc.vColor = _float4(1.f, 0.f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pVIBufferCom), &InstanceDesc)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CParticle_Rect> CParticle_Rect::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
	shared_ptr<CParticle_Rect> pInstance = make_shared<CParticle_Rect>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CParticle_Rect");
		pInstance.reset();
	}

	return pInstance;
}

void CParticle_Rect::Free()
{
	__super::Free();

	m_pVIBufferCom->Free();
}
