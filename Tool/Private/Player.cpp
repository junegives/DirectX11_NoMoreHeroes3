#include "pch.h"
#include "Player.h"

#include "GameInstance.h"

CPlayer::CPlayer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLandObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CLandObject(rhs)
{
}

HRESULT CPlayer::Initialize(void* pArg)
{
	m_fColRadius = 0.9f;

	LANDOBJ_DESC* pGameObjectDesc = (LANDOBJ_DESC*)pArg;

	pGameObjectDesc->fSpeedPerSec = 10.f;
	pGameObjectDesc->fRotationPerSec = XMConvertToRadians(150.0f);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pGameInstance->Set_Player(shared_from_this());

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ -6.05f, 3.3f, -8.4f, 1.f });

	_vector vStartPos = { 0.f, 0.f, 0.f, 0.f };

	m_pNavigationCom->GetCellCenterPos(&vStartPos);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vStartPos);

	m_iHP = 100;

	return S_OK;
}

void CPlayer::Priority_Tick(_float fTimeDelta)
{
}

void CPlayer::Tick(_float fTimeDelta)
{
	m_vPrevPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_fTimeDelta = fTimeDelta;

	Key_Input(fTimeDelta);

	SetUp_OnNavi(m_pTransformCom);

	// m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
		return;

#ifdef _DEBUG
	// m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
	//// m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CPlayer::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

		// 셰이더에 던지는 루프를 클라에서 돌리려면 메시가 몇 개 있는지 알아야 한다.
		// 셰이더에 값 던지는 기능을 클라에서 선택적으로 가능
		_uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType::aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			/*if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
				return E_FAIL;*/

			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(i)))
				return E_FAIL;
		}


	return S_OK;
}

HRESULT CPlayer::Add_Components()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_Travis"),
		TEXT("Com_Model"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Navigation */
	CNavigation::NAVI_DESC		NaviDesc{};

	NaviDesc.iStartCellIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

	/* Com_Collider */
	CBounding_Sphere::SPHERE_DESC		BoundingDesc{};

	BoundingDesc.vCenter = _float3(0.f, 1.f, 0.f);
	BoundingDesc.fRadius = m_fColRadius;

	/*if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider(LAYER_PLAYER, m_pColliderCom);
	m_pColliderCom->SetOwner(shared_from_this());*/

	return S_OK;
}

void CPlayer::Key_Input(_float fTimeDelta)
{
	// 이동
	{
		if (m_pGameInstance->Is_KeyPressing(DIK_UP))
		{
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
		}
		if (m_pGameInstance->Is_KeyPressing(DIK_LEFT))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
		}
		if (m_pGameInstance->Is_KeyPressing(DIK_DOWN))
		{
			m_pTransformCom->Go_Backward(fTimeDelta);
		}
		if (m_pGameInstance->Is_KeyPressing(DIK_RIGHT))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 1.f);
		}



		/*if (GetKeyState(DIKEYBOARD_LEFT) & 0x8000)
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
		}

		if (GetKeyState(DIKEYBOARD_RIGHT) & 0x8000)
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 1.f);
		}

		if (GetKeyState(DIKEYBOARD_DOWN) & 0x8000)
		{
			m_pTransformCom->Go_Backward(fTimeDelta);
		}

		if (GetKeyState(DIKEYBOARD_UP) & 0x8000)
		{
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
			m_iState |= STATE_RUN;
		}
		else
		{
			m_iState = 0x00000000;
			m_iState |= STATE_IDLE;
		}*/
	}
}

shared_ptr<CPlayer> CPlayer::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
	shared_ptr<CPlayer> pInstance = make_shared<CPlayer>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer");
		pInstance.reset();
	}

	return pInstance;
}

HRESULT CPlayer::Bind_ShaderResources()
{
	_float4x4		 ViewMatrix, ProjMatrix;

	ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_VIEW);
	ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_PROJ);

	_float4         CamPos;
	CamPos = m_pGameInstance->Get_CamPosition();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Free()
{
	__super::Free();
}

void CPlayer::Change_NaviCom()
{
	CNavigation::NAVI_DESC		NaviDesc{};

	NaviDesc.iStartCellIndex = 0;

	__super::Remove_Component(TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"));

	__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pNavigationCom), &NaviDesc);
	
	_vector vStartPos = { 0.f, 0.f, 0.f, 0.f };

	m_pNavigationCom->GetCellCenterPos(&vStartPos);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vStartPos);
}
