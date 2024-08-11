#include "pch.h"
#include "Effect_Distortion.h"

CEffect_Distortion::CEffect_Distortion(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag, const wstring& strMaskTextureComTag)
	: CEffect(pDevice, pContext)
	, m_strModelComTag(strModelComTag)
	, m_strMaskTextureComTag(strMaskTextureComTag)
{
}

HRESULT CEffect_Distortion::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_fAlpha = 1.f;
	m_fDistortionSpeed = 1.f;

	return S_OK;
}

void CEffect_Distortion::Priority_Tick(_float fTimeDelta)
{
}

void CEffect_Distortion::Tick(_float fTimeDelta)
{
	if (!m_isTest)
	{
		m_EffectDesc.fLifeTime -= fTimeDelta;

		if (0.f > m_EffectDesc.fLifeTime)
			m_isActive = false;
	}

	if (m_isFollow && m_pParentTransformCom)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pParentTransformCom->Get_State(CTransform::STATE_POSITION));
}

void CEffect_Distortion::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_DISTORTION, shared_from_this())))
		return;
}

HRESULT CEffect_Distortion::Render()
{
	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix;

	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrix());
	XMStoreFloat4x4(&ViewMatrix, m_pGameInstance->Get_Transform_Matrix(CPipeline::D3DTS_VIEW));
	XMStoreFloat4x4(&ProjMatrix, m_pGameInstance->Get_Transform_Matrix(CPipeline::D3DTS_PROJ));

	_float4		vCamPos = m_pGameInstance->Get_CamPosition();

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	if (m_EffectDesc.isUseMask)
	{
		m_fDistortionTimer += m_pGameInstance->Compute_TimeDelta(TEXT("Timer_60"));
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDistortionTimer", &m_fDistortionTimer, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDistortionSpeed", &m_fDistortionSpeed, sizeof(_float))))
			return E_FAIL;
	}
		if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
			return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_EffectDesc.iShaderPass)))
		return E_FAIL;

	if (FAILED(m_pModelCom->Render(0)))
		return E_FAIL;

	return S_OK;
}

void CEffect_Distortion::Play(_uint iNum, _vector vCenter, _vector vScale, _vector vUp, _vector vLook)
{
}

void CEffect_Distortion::Play(_uint iNum, _vector vCenter, _bool isRotate, _vector vAxis, _vector vRotate)
{
	m_isFollow = false;
	m_isActive = true;

	m_EffectDesc.fLifeTime = m_EffectDesc.fDuration;

	m_pTransformCom->Set_Scale(m_EffectDesc.vSize.x, m_EffectDesc.vSize.y, m_EffectDesc.vSize.z);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCenter);
}

void CEffect_Distortion::Play(shared_ptr<CTransform> pParentTransform)
{
	m_isFollow = true;
	m_pParentTransformCom = pParentTransform;

	m_pTransformCom->Set_Scale(m_EffectDesc.vSize.x, m_EffectDesc.vSize.y, m_EffectDesc.vSize.z);

	m_isActive = true;
}

HRESULT CEffect_Distortion::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, m_strMaskTextureComTag,
		TEXT("Com_Texture"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pMaskTextureCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_EffectMesh"),
		TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, m_strModelComTag,
		TEXT("Com_Model"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pModelCom))))
		return E_FAIL;
}

shared_ptr<CEffect_Distortion> CEffect_Distortion::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag, const wstring& strMaskTextureComTag, void* pArg)
{
	shared_ptr<CEffect_Distortion> pInstance = make_shared<CEffect_Distortion>(pDevice, pContext, strModelComTag, strMaskTextureComTag);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CEffect_Distortion");
		pInstance.reset();
	}

	return pInstance;
}

void CEffect_Distortion::Free()
{
	__super::Free();
}