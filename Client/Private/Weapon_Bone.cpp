#include "pch.h"
#include "Weapon_Bone.h"

CWeapon_Bone::CWeapon_Bone(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CPartObject(pDevice, pContext)
{
}

CWeapon_Bone::CWeapon_Bone(const CWeapon_Bone& rhs)
	: CPartObject(rhs)
{
}

HRESULT CWeapon_Bone::Initialize(void* pArg)
{
	WEAPON_DESC* pWeaponDesc = (WEAPON_DESC*)pArg;

	m_pSocketMatrix = pWeaponDesc->pSocketMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	//m_pTransformCom->Set_Scale(0.1f, 0.1f, 0.1f);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.0f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.5f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CWeapon_Bone::Priority_Tick(_float fTimeDelta)
{
}

void CWeapon_Bone::Tick(_float fTimeDelta)
{
}

void CWeapon_Bone::Late_Tick(_float fTimeDelta)
{
	if (!m_isAttached)
		return;

	_matrix	BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
	{
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
	}


	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * BoneMatrix * m_pParentTransform->Get_WorldMatrix());
	//m_WorldMatrix.m[3][1] -= 2.8f;

	m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
		return;
}

HRESULT CWeapon_Bone::Render()
{
	if (!m_isAttached)
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType::aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

//#ifdef _DEBUG
//	m_pColliderCom->Render();
//#endif

	return S_OK;
}

HRESULT CWeapon_Bone::Add_Component()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Weapon_Bone"),
		TEXT("Com_Model"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pModelCom))))
		return E_FAIL;


	/* Com_Collider */
	CBounding_OBB::OBB_DESC		BoundingDesc{};

	BoundingDesc.vExtents = _float3(0.7f, 0.3f, 0.3f);
	BoundingDesc.vCenter = _float3(0.5f, 0.f, 0.f);
	BoundingDesc.vRadians = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Bone::Bind_ShaderResources()
{
	_float4x4		 ViewMatrix, ProjMatrix;

	ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_VIEW);
	ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_PROJ);

	_float4         CamPos;
	CamPos = m_pGameInstance->Get_CamPosition();

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	/*if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &CamPos, sizeof(_float4))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;*/

	return S_OK;
}

shared_ptr<CWeapon_Bone> CWeapon_Bone::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
	shared_ptr<CWeapon_Bone> pInstance = make_shared<CWeapon_Bone>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CWeapon_Bone");
		pInstance.reset();
	}

	return pInstance;
}

void CWeapon_Bone::Free()
{
}
