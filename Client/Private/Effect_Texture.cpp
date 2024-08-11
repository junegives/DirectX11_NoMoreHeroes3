#include "pch.h"
#include "Effect_Texture.h"

CEffect_Texture::CEffect_Texture(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strTextureTag, const wstring& strMaskTextureTag)
	: CEffect(pDevice, pContext)
{
	m_strTextureTag = strTextureTag;
	m_strMaskTextureTag = strMaskTextureTag;
}

CEffect_Texture::CEffect_Texture(const CEffect_Texture& rhs)
	: CEffect(rhs)
{
}

CEffect_Texture::~CEffect_Texture()
{
}

HRESULT CEffect_Texture::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	for (size_t i = 0; i < 5; i++)
	{
		m_pTransformArray[i] = CTransform::Create(m_pDevice, m_pContext);
	}

	if (m_EffectDesc.isSprite)
	{
		m_iUVCnt = m_EffectDesc.iUVx * m_EffectDesc.iUVy;
		m_vUVScale = { (_float)1 / m_EffectDesc.iUVx, (_float)1/m_EffectDesc.iUVy };
		m_fFrameTime = m_EffectDesc.fLifeTime / (_float)m_iUVCnt;
	}

	return S_OK;
}

void CEffect_Texture::Priority_Tick(_float fTimeDelta)
{

}

void CEffect_Texture::Tick(_float fTimeDelta)
{
	m_fActiveTime -= fTimeDelta;

	if (m_EffectDesc.fLifeTime - m_fActiveTime > m_fFrameTime * m_iUVIdx)
	{
		m_iUVIdx++;
		m_vUVPivot.x += m_vUVScale.x;

		if (1 <= m_vUVPivot.x)
		{
			m_vUVPivot.x = 0;
			m_vUVPivot.y += m_vUVScale.y;
		}
	}

	if (m_fActiveTime < 0)
		m_isActive = false;
}

void CEffect_Texture::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, shared_from_this())))
		return;
}

HRESULT CEffect_Texture::Render()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &vCamPos, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fLifeTime", &m_fActiveTime, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDuration", &m_EffectDesc.fLifeTime, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (m_EffectDesc.isUseMask)
		if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
			return E_FAIL;

	if (m_EffectDesc.isSprite)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVPivot", &m_vUVPivot, sizeof(_float2))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVScale", &m_vUVScale, sizeof(_float2))))
			return E_FAIL;
	}

	if (2 == m_EffectDesc.iShaderPass)
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_EffectDesc.vColor, sizeof(_float4))))
			return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_EffectDesc.iShaderPass)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (1 < m_iNum)
	{
		for (_uint i = 0; i < m_iNum - 1; ++i)
		{
			_float4x4	WorldRandMatrix;

			XMStoreFloat4x4(&WorldRandMatrix, m_pTransformArray[i]->Get_WorldMatrix());

			if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldRandMatrix)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(m_EffectDesc.iShaderPass)))
				return E_FAIL;

			if (FAILED(m_pVIBufferCom->Bind_Buffers()))
				return E_FAIL;

			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
	}

	return S_OK;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
HRESULT CEffect_Texture::Add_Component()
{
	if (m_strTextureTag != L"")
	{
		if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, m_strTextureTag,
			TEXT("Com_DiffuseTexture"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pTextureCom))))
			return E_FAIL;
	}

	if (m_EffectDesc.isUseMask)
	{
		if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, m_strMaskTextureTag,
			TEXT("Com_MaskTexture"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pMaskTextureCom))))
			return E_FAIL;
	}

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_EffectTexture"),
		TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

void CEffect_Texture::Play(_uint iNum, _vector vCenter, _vector vScale, _vector vUp, _vector vLook)
{
	m_iNum = iNum;

	m_pTransformCom->Set_Scale(m_EffectDesc.vSize.x, m_EffectDesc.vSize.y, m_EffectDesc.vSize.z);

	if (1 < m_iNum)
	{
		for (size_t i = 0; i < m_iNum - 1; i++)
		{
			_uint iRandScale = rand() % 20;
			_uint iRandPos = rand() % 5;
			_float fRandScale = _float(iRandScale) / (10.f) - 1.5f;
			_float fRandPos = _float(iRandPos) / (10.f) - 0.25f;

			m_pTransformArray[i]->Set_Scale(m_EffectDesc.vSize.x + fRandScale, m_EffectDesc.vSize.y + fRandScale, m_EffectDesc.vSize.z);
			m_pTransformArray[i]->Set_State(CTransform::STATE_POSITION, vCenter + XMVector3Cross(vUp, vLook) * fRandPos);
			m_pTransformArray[i]->UpDir(vUp, vLook);
		}
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCenter);
	m_pTransformCom->UpDir(vUp, vLook);

	if (m_EffectDesc.isSprite)
		m_vUVPivot = { 0.f, 0.f };

	m_fActiveTime = m_EffectDesc.fLifeTime;
	m_isActive = true;
	m_iUVIdx = 0;
}

void CEffect_Texture::Play(_uint iNum, _vector vCenter, _bool isRotate, _vector vAxis, _vector vRotate)
{
	m_iNum = iNum;

	m_pTransformCom->Set_Scale(m_EffectDesc.vSize.x, m_EffectDesc.vSize.y, m_EffectDesc.vSize.z);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCenter);

	if (isRotate)
	{
		m_pTransformCom->TurnRadian({ 1.f, 0.f, 0.f, 0.f }, vRotate.m128_f32[0]);
		m_pTransformCom->TurnRadian({ 0.f, 1.f, 0.f, 0.f }, vRotate.m128_f32[1]);
		m_pTransformCom->TurnRadian({ 0.f, 0.f, 1.f, 0.f }, vRotate.m128_f32[2]);
	}

	m_fActiveTime = m_EffectDesc.fLifeTime;
	m_isActive = true;
}

void CEffect_Texture::Play(shared_ptr<CTransform> pParentTransform)
{
}

shared_ptr<CEffect_Texture> CEffect_Texture::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strTextureTag, const wstring& strMaskTextureTag, void* pArg)
{
	shared_ptr<CEffect_Texture> pInstance = make_shared<CEffect_Texture>(pDevice, pContext, strTextureTag, strMaskTextureTag);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CEffect_Texture");
		pInstance.reset();
	}

	return pInstance;
}

void CEffect_Texture::Free()
{
}
