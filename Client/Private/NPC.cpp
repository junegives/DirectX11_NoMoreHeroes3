#include "pch.h"
#include "NPC.h"
#include "Player.h"

#include "GameInstance.h"

CNPC::CNPC(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CLandObject(pDevice, pContext)
{
}

CNPC::CNPC(const CNPC& rhs)
    : CLandObject(rhs)
{
}

HRESULT CNPC::Initialize(void* pArg)
{
    LANDOBJ_DESC* pGameObjectDesc = (LANDOBJ_DESC*)pArg;

    pGameObjectDesc->fSpeedPerSec = 10.f;
    pGameObjectDesc->fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

void CNPC::Priority_Tick(_float fTimeDelta)
{
}

void CNPC::Tick(_float fTimeDelta)
{
    m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);

    //SetUp_OnTerrain(m_pTransformCom);
    if (STATE::STATE_JUMP != m_pStateMachineCom->Get_CurState() && !m_isYChange)
        SetUp_OnNavi(m_pTransformCom);
    else
        Jump_OnNavi(m_pTransformCom);

    m_pModelCom->Apply_RootMotion(fTimeDelta, m_pTransformCom, m_pNavigationCom, m_isYChange);
}

void CNPC::Late_Tick(_float fTimeDelta)
{
    m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

    Collision_ToPlayer();

    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
        return;
}

HRESULT CNPC::Render()
{
    _vector vTest = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    // 셰이더에 던지는 루프를 클라에서 돌리려면 메시가 몇 개 있는지 알아야 한다.
    // 셰이더에 값 던지는 기능을 클라에서 선택적으로 가능
    _uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

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

#ifdef _DEBUG
    m_pColliderCom->Render();
#endif

    return S_OK;
}

void CNPC::OnCollision(LAYER_ID eColLayer, shared_ptr<class CCollider> pCollider)
{
}

HRESULT CNPC::Add_Component()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CNPC::Bind_ShaderResources()
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

void CNPC::Collision_ToPlayer()
{
    shared_ptr<CPlayer> pPlayer = dynamic_pointer_cast<CPlayer>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, LAYER_PLAYER, TEXT("GameObject_Player")));
    shared_ptr<CCollider> pPlayerCollider = dynamic_pointer_cast<CCollider>(pPlayer->Get_PartObjectComponent(TEXT("Part_Weapon"), TEXT("Com_Collider")));

    /*CCollider*		pPlayerCollider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(LEVEL_STATIC, LAYER_PLAYER, TEXT("Com_Collider")));
    if (nullptr == pPlayerCollider)
        return;
        */
    m_pColliderCom->Intersect(pPlayerCollider);
}

_float CNPC::GetDistanceToPlayer()
{
    shared_ptr<CTransform> pPlayerTransform = dynamic_pointer_cast<CTransform>(m_pGameInstance->Find_Component(LEVEL_STATIC, LAYER_PLAYER, g_strTransformTag));

    m_fDistanceToPlayer = XMVectorGetX(XMVector4Length(pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

    return m_fDistanceToPlayer;
}

shared_ptr<CNPC> CNPC::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    shared_ptr<CNPC> pInstance = make_shared<CNPC>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CNPC");
        pInstance.reset();
    }

    return pInstance;
}

void CNPC::Free()
{
    __super::Free();
}
