#include "pch.h"
#include "Weapon.h"
#include "Trail.h"
#include "SwordTrail.h"
#include "MeshTrail.h"
#include "Player.h"

#include "Effect_Manager.h"

CWeapon::CWeapon(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CPartObject(pDevice, pContext)
{
}

CWeapon::CWeapon(const CWeapon& rhs)
    : CPartObject(rhs)
{
}

HRESULT CWeapon::Initialize(void* pArg)
{
	WEAPON_DESC* pWeaponDesc = (WEAPON_DESC*)pArg;

	m_pSocketMatrix = pWeaponDesc->pSocketMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;
	
	m_pModelCom->Set_Animation(8, false, 2.0f, 0.2f, 0);
	m_pModelCom->Play_Animation(0.016, m_pTransformCom);

	m_pTrailStartMatrix = m_pModelCom->Get_CombinedBoneMatrixPtr("FX_Trail_Start");
	m_pTrailEndMatrix = m_pModelCom->Get_CombinedBoneMatrixPtr("FX_Trail_End");

	m_pParticleFXMatrix = XMLoadFloat4x4(m_pModelCom->Get_CombinedBoneMatrixPtr("gimmick_04"));
	for (size_t i = 0; i < 3; i++)
	{
		m_pParticleFXMatrix.r[i] = XMVector3Normalize(m_pParticleFXMatrix.r[i]);
	}

	m_WeaponFXMatrix = XMLoadFloat4x4(m_pModelCom->Get_CombinedBoneMatrixPtr("FX_Beam"));

	for (size_t i = 0; i < 3; i++)
	{
		m_WeaponFXMatrix.r[i] = XMVector3Normalize(m_WeaponFXMatrix.r[i]);
	}

	m_vWeaponFXPos = m_WeaponFXMatrix.r[3];

	//m_pTransformCom->Set_Scale(0.1f, 0.1f, 0.1f);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.0f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.5f, 0.f, 0.f, 1.f));

	m_InitMatrix = m_pTransformCom->Get_WorldMatrix();

	m_vPrevPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	/*CTrail::TRAIL_DESC TrailDesc = { };
	TrailDesc.isTrail = false;
	TrailDesc.fAccGenTrail = 0.f;
	TrailDesc.fGenTrailTime = 1.f;
	TrailDesc.vDiffuseColor = { 1.f, 0.f, 0.f, 0.5f };
	TrailDesc.vUV_FlowSpeed = { 0.f, 0.f };
	TrailDesc.iUV_Cut = -1;
	TrailDesc.isDiffuse = false;
	TrailDesc.isAlpha = false;

	m_pTrail = CTrail::Create(m_pDevice, m_pContext, TrailDesc);
	if (m_pTrail == nullptr)
		return E_FAIL;

	m_pTrail->SetPos(XMVectorSet(0.5f, 0.5f, 0.5f, 1.f), XMVectorSet(-0.9f, -0.9f, -0.9f, 1.f));

	StopTrail();*/

	m_pSwordTrail = CSwordTrail::Create(m_pDevice, m_pContext, L"Prototype_Component_Texture_Trail");
	m_vUpperPos = { m_pTrailStartMatrix->m[3][0], m_pTrailStartMatrix->m[3][1] , m_pTrailStartMatrix->m[3][2] };
	m_vLowerPos = { m_pTrailEndMatrix->m[3][0], m_pTrailEndMatrix->m[3][1] , m_pTrailEndMatrix->m[3][2] };

	CMeshTrail::MESHTRAIL_DESC MeshTrail_Desc = {};
	MeshTrail_Desc.pModel = m_pEffectInModelCom;
	MeshTrail_Desc.iSize = 5;
	MeshTrail_Desc.fLifeTime = 1.f;
	MeshTrail_Desc.vColor = { 1.f, 1.f, 1.f };
	MeshTrail_Desc.ePassType = CMeshTrail::MESH_COLOR_ALPHA;

	m_pMeshTrail = CMeshTrail::Create(m_pDevice, m_pContext);
	m_pMeshTrail->SettingMeshTrail(MeshTrail_Desc);
	
	return S_OK;
}

void CWeapon::Priority_Tick(_float fTimeDelta)
{
}

void CWeapon::Tick(_float fTimeDelta)
{
	// m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	/*m_pTrail->Set_TransformMatrix(XMLoadFloat4x4(&m_WorldMatrix));
	m_pTrail->Tick(fTimeDelta);*/

	_matrix	BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
	{
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
	}

	shared_ptr<CPlayer> pPlayer = dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player());

	if (pPlayer->GetCurState() == STATE::STATE_ATTACK)
	{
		UpdateTrail(m_pTransformCom->Get_WorldMatrix());
		m_pSwordTrail->SetColor(XMVectorSet(1.f, 0.8f, 0.271f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
		m_pSwordTrail->Tick(fTimeDelta);
		m_ePrevState = STATE::STATE_ATTACK;

	}
	else if (pPlayer->GetCurState() == STATE::STATE_HEAVYATTACK)
	{
		UpdateTrail(m_pTransformCom->Get_WorldMatrix());
		m_pSwordTrail->SetColor(XMVectorSet(0.467f, 0.953f, 0.996f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
		//m_pSwordTrail->SetColor(XMVectorSet(0.941f, 0.38f, 0.969f, 1.f), XMVectorSet(0.439f, 0.0f, 0.627f, 1.f));
		m_pSwordTrail->Tick(fTimeDelta);
		m_ePrevState = STATE::STATE_ATTACK;
	}
	else if (STATE::STATE_ATTACK == m_ePrevState)
	{
		m_pSwordTrail->ClearTrail();
		m_ePrevState = STATE::STATE_IDLE;
	}

	m_pMeshTrail->Tick(fTimeDelta);
}

void CWeapon::Late_Tick(_float fTimeDelta)
{
	_matrix	BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
	{
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
	}

	XMStoreFloat4x4(&m_WorldMatrix, m_InitMatrix * BoneMatrix * m_pParentTransform->Get_WorldMatrix());

	m_pTransformCom->Set_WorldMatrix(m_WorldMatrix);

	if (!m_isAttached)
		return;
	//m_WorldMatrix.m[3][1] -= 2.8f;

	m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
		return;

#ifdef _DEBUG
	 m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

	 m_vPrevPos = m_vCurPos;
	 m_vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	 m_pMeshTrail->Late_Tick(fTimeDelta);

	 //m_pTrail->Late_Tick(fTimeDelta);
	 //m_pSwordTrail->Late_Tick(fTimeDelta);
}

HRESULT CWeapon::Render()
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

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	if (m_isWeaponGlow)
	{

	iNumMeshes = m_pEffectModelCom->Get_NumMeshes();

	_float4 vColor = { 1.f, 1.f, 1.f, 1.f };

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	m_WeaponFXMatrix *= XMMatrixRotationZ(XMConvertToRadians(3));

	m_WeaponFXMatrix.r[3] = m_vWeaponFXPos;
	m_WeaponFXMatrix.r[3].m128_f32[0] += 0.03f;

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pTrailEndMatrix->m[3][0] = -0.1f;
	_matrix RotateWorldMatrix = XMLoadFloat4x4(m_pTrailEndMatrix) * XMLoadFloat4x4(&m_WorldMatrix);
	_float4x4 RotatedWorld;
	XMStoreFloat4x4(&RotatedWorld, RotateWorldMatrix);

	XMStoreFloat4x4(&RotatedWorld, m_WeaponFXMatrix * m_pTransformCom->Get_WorldMatrix());

	/*m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	RotatedWorld.m[3][0] = vPos.m128_f32[0];
	RotatedWorld.m[3][1] = vPos.m128_f32[1];
	RotatedWorld.m[3][2] = vPos.m128_f32[2];
	RotatedWorld.m[3][3] = 1.f;*/

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &RotatedWorld)))
		return E_FAIL;

	/*for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pEffectModelCom->Render(i)))
			return E_FAIL;
	}*/

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pEffectInModelCom->Render(0)))
		return E_FAIL;

	if (FAILED(m_pEffectOutModelCom->Render(0)))
		return E_FAIL;

	if (0 == m_iGenerateAfterImage % 5)
	{
		m_iGenerateAfterImage = 0;
		CMeshTrail::MESHTRAIL_ELEMENT_DESC MeshTrail_Element_Desc = {};
		MeshTrail_Element_Desc.fLifeTime = 1.f;
		MeshTrail_Element_Desc.vColor = { 1.f, 1.f, 1.f };
		MeshTrail_Element_Desc.WorldMatrix = RotatedWorld;

		m_pMeshTrail->AddMeshTrail(MeshTrail_Element_Desc);
	}
	
	m_iGenerateAfterImage++;
	}

//#ifdef _DEBUG
//	m_pColliderCom->Render();
//#endif

	return S_OK;
}

void CWeapon::OnCollision(LAYER_ID eColLayer, shared_ptr<CCollider> pCollider)
{
	switch (eColLayer)
	{
	case Engine::LAYER_MONSTER:
		if (!m_pColliderCom->IsOnCollide())
			return;

		if (!pCollider->IsOnCollide())
			return;

		// 공격에 따라 방향 다르기
		
		{
			// R
			// Attack2, Attack5, HeavyAttack1(2)
			//	_vector vParticleDir = m_pParentTransform->Get_State(CTransform::STATE_RIGHT);

			_vector vDir = m_vCurPos - m_vPrevPos;
			_vector vLook = m_pParentTransform->Get_State(CTransform::STATE_LOOK);

			_vector vLookProj = vLook;
			XMVectorSetY(vLookProj, 0);

			_vector vOrthogonal = XMVector3Cross(vDir, vLookProj);
			vOrthogonal.m128_f32[1] = vDir.m128_f32[1];
			vOrthogonal = XMVector3Normalize(vOrthogonal);

			_vector vTest = {-vLook.m128_f32[2], vDir.m128_f32[1], vLook.m128_f32[0], 0.f};

			_vector vEffectGenPos =
				(m_InitMatrix * m_pParticleFXMatrix * m_pParentTransform->Get_WorldMatrix()).r[3];

			_vector vEffectGenPosTest =
				(m_pParticleFXMatrix * m_pParentTransform->Get_WorldMatrix()).r[3];

			_vector vEffectGenPosTestTest =
				(m_pParticleFXMatrix * m_pTransformCom->Get_WorldMatrix()).r[3];

			switch (m_iAttackType)
			{
			// 좌하
			case 0:
				vDir = -m_pParentTransform->Get_State(CTransform::STATE_RIGHT) - m_pParentTransform->Get_State(CTransform::STATE_UP);
				break;
			// 우상
			case 1:
				vDir = m_pParentTransform->Get_State(CTransform::STATE_RIGHT) + m_pParentTransform->Get_State(CTransform::STATE_UP);
				break;
			// 상
			case 2:
				vDir =m_pParentTransform->Get_State(CTransform::STATE_UP);
				break;
			// 우
			case 3:
				vDir = m_pParentTransform->Get_State(CTransform::STATE_RIGHT);
				break;
			// 좌
			case 4:
				vDir = -m_pParentTransform->Get_State(CTransform::STATE_RIGHT);
				break;

			default:
				break;
			}


			//CEffect_Manager::GetInstance()->PlayParticle(TEXT("GameObject_Particle_Attack"), vEffectGenPosTestTest, vDir);
			CEffect_Manager::GetInstance()->PlayParticle(TEXT("GameObject_Particle_Attack"), pCollider->GetBounding()->GetWorldCenter() - vLook, XMVector4Normalize(vDir));
			CEffect_Manager::GetInstance()->PlayEffect(TEXT("GameObject_Effect_Attack_Line"), 5, pCollider->GetBounding()->GetWorldCenter() - vLook, XMVector4Normalize(vDir), XMVector4Normalize(vLook));
			CEffect_Manager::GetInstance()->PlayEffect(TEXT("GameObject_Effect_Attack_Particle_Blue"), 1, pCollider->GetBounding()->GetWorldCenter(), XMVector4Normalize(vDir), XMVector4Normalize(vLook));
		}

		break;
	default:
		break;
	}
}

void CWeapon::SetTrailPos(_vector vHighPosition, _vector vLowPosition)
{
	m_pTrail->SetPos(vHighPosition, vLowPosition);
}

void CWeapon::StartTrail(const wstring& strDiffuseTextureName, const wstring& strAlphaTextureName, const _float4& vColor, _uint iVertexCount)
{
	_matrix	BoneMatrix = XMLoadFloat4x4(m_pTrailEndMatrix);

	for (size_t i = 0; i < 3; i++)
	{
		BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
	}

	XMStoreFloat4x4(&m_WorldMatrix, m_InitMatrix * BoneMatrix * m_pParentTransform->Get_WorldMatrix());

	m_pTransformCom->Set_WorldMatrix(m_WorldMatrix);

	if (L"" != strDiffuseTextureName)
		m_pTrail->Set_DiffuseTexture(strDiffuseTextureName);

	if (L"" != strAlphaTextureName)
	m_pTrail->Set_AlphaTexture(strAlphaTextureName);

	CTrail::TRAIL_DESC TrailDesc = m_pTrail->Get_TrailDesc();
	TrailDesc.vDiffuseColor = vColor;
	m_pTrail->Set_TrailDesc(TrailDesc);

	m_pTrail->Set_VtxCount(iVertexCount);
	m_pTrail->StartTrail(m_pTransformCom->Get_WorldMatrix());
}

void CWeapon::StopTrail()
{
	m_pTrail->StopTrail();
}

HRESULT CWeapon::Add_Component()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Weapon_BeamKatana"),
		TEXT("Com_Model"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pModelCom))))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effect_Sword"),
		TEXT("Com_Effect_Mesh"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pEffectModelCom))))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effect_Sword_In"),
		TEXT("Com_Effect_In_Mesh"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pEffectInModelCom))))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effect_Sword_Out"),
		TEXT("Com_Effect_Out_Mesh"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pEffectOutModelCom))))
		return E_FAIL;


	/* Com_Collider */
	/*CBounding_OBB::OBB_DESC		BoundingDesc{};

	BoundingDesc.vExtents = _float3(1.0f, 1.3f, 2.f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);
	BoundingDesc.vRadians = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;*/

	CBounding_Sphere::SPHERE_DESC		BoundingDesc{};

	/*BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);
	BoundingDesc.fRadius = ;*/

	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.8f);
	BoundingDesc.fRadius = 0.4f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	m_pColliderCom->SetOwner(shared_from_this());

	m_pGameInstance->Add_Collider(LAYER_PLAYER_ATTACK, m_pColliderCom);

	return S_OK;
}

HRESULT CWeapon::Bind_ShaderResources()
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

void CWeapon::UpdateTrail(_matrix fSocketMatrix)
{
	m_pSwordTrail->UpdateTrail(m_vUpperPos, m_vLowerPos, fSocketMatrix);
}

shared_ptr<CWeapon> CWeapon::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
	shared_ptr<CWeapon> pInstance = make_shared<CWeapon>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CWeapon");
		pInstance.reset();
	}

	return pInstance;
}

void CWeapon::Free()
{
}
