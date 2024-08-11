#include "pch.h"
#include "Particle.h"

CParticle::CParticle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject(pDevice, pContext)
{
}

CParticle::CParticle(const CParticle& rhs)
	: CGameObject(rhs)
{
}

CParticle::~CParticle()
{
	Free();
}

HRESULT CParticle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle::Initialize(void* pArg)
{
	m_ParticleDesc = *(PARTICLE_DESC*)pArg;

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

void CParticle::Priority_Tick(_float fTimeDelta)
{
}

void CParticle::Tick(_float fTimeDelta)
{
	//m_pVIBufferCom->Tick_Drop(fTimeDelta);
	 m_pVIBufferCom->Tick_Splash(fTimeDelta);
	 if (m_pVIBufferCom->IsFinish())
		 m_isActive = false;
}

void CParticle::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, shared_from_this())))
		return;
}

HRESULT CParticle::Render()
{
	_float4x4	matView = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_VIEW);
	_float4x4	matProj = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_PROJ);
	_float4		vCamPos = m_pGameInstance->Get_CamPosition();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &matView)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &matProj)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &vCamPos, sizeof(_float4))))
		return E_FAIL;

	if (m_ParticleDesc.isTexture)
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_ParticleDesc.iShaderPass)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CParticle::Play(_vector vCenter, _vector vDir)
{
	m_pVIBufferCom->Reset(vCenter, vDir);
	m_isActive = true;
}

void CParticle::Play(_vector vCenter)
{
	m_pVIBufferCom->Reset(vCenter);
	m_isActive = true;
}

HRESULT CParticle::Add_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Snow"),
		TEXT("Com_Texture"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
		return E_FAIL;
	
	CVIBuffer_Instancing::INSTANCE_DESC			InstanceDesc{};
	InstanceDesc.vPivot = m_ParticleDesc.vPivot;
	InstanceDesc.vCenter = m_ParticleDesc.vCenter;
	InstanceDesc.vRange = m_ParticleDesc.vRange;
	InstanceDesc.vSize = m_ParticleDesc.vSize;
	InstanceDesc.vSpeed = m_ParticleDesc.vSpeed;
	InstanceDesc.vLifeTime = m_ParticleDesc.vLifeTime;
	InstanceDesc.fDuration = m_ParticleDesc.fDuration;
	InstanceDesc.isLoop = m_ParticleDesc.isLoop;
	InstanceDesc.vColor = m_ParticleDesc.vColor;
	InstanceDesc.isSetDir = m_ParticleDesc.isSetDir;
	InstanceDesc.vDirX	= m_ParticleDesc.vDirX;
	InstanceDesc.vDirY	= m_ParticleDesc.vDirY;
	InstanceDesc.vDirZ	= m_ParticleDesc.vDirZ;
	if (m_ParticleDesc.isSetDir)
		InstanceDesc.vDirRange = m_ParticleDesc.vDirRange;

	wstring strPrototypeTag = TEXT("Prototype_Component_VIBuffer_Particle_Point") + to_wstring(m_ParticleDesc.iNumInstance);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, strPrototypeTag, CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext, m_ParticleDesc.iNumInstance))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, strPrototypeTag, TEXT("Com_VIBuffer"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pVIBufferCom), &InstanceDesc)))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CParticle> CParticle::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
	shared_ptr<CParticle> pInstance = make_shared<CParticle>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CParticle");
		pInstance.reset();
	}

	return pInstance;
}	

void CParticle::Free()
{
	m_pVIBufferCom->Free();
}
