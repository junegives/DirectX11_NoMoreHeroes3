#include "pch.h"
#include "Projectile.h"

#include "Player.h"

#include "GameInstance.h"

CProjectile::CProjectile(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CGameObject(pDevice, pContext)
{
}

CProjectile::CProjectile(const CProjectile& rhs)
    : CGameObject(rhs)
{
}

HRESULT CProjectile::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

void CProjectile::Priority_Tick(_float fTimeDelta)
{
}

void CProjectile::Tick(_float fTimeDelta)
{

    
}

void CProjectile::Late_Tick(_float fTimeDelta)
{
    m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

    Collision_ToPlayer();

    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_GLOW, shared_from_this())))
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, shared_from_this())))
        return;
}

HRESULT CProjectile::Render()
{
//    _vector vTest = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
//
//    if (FAILED(Bind_ShaderResources()))
//        return E_FAIL;
//
//    // 셰이더에 던지는 루프를 클라에서 돌리려면 메시가 몇 개 있는지 알아야 한다.
//    // 셰이더에 값 던지는 기능을 클라에서 선택적으로 가능
//    _uint   iNumMeshes = m_pModelCom->Get_NumMeshes();
//
//    for (size_t i = 0; i < iNumMeshes; i++)
//    {
//        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType::aiTextureType_DIFFUSE, "g_DiffuseTexture")))
//            return E_FAIL;
//
//        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
//            return E_FAIL;
//
//        if (FAILED(m_pShaderCom->Begin(0)))
//            return E_FAIL;
//
//        if (FAILED(m_pModelCom->Render(i)))
//            return E_FAIL;
//    }
//
//#ifdef _DEBUG
//    m_pColliderCom->Render();
//#endif

    return S_OK;
}

HRESULT CProjectile::Add_Component()
{
    return S_OK;
}

void CProjectile::Collision_ToPlayer()
{
    shared_ptr<CPlayer> pPlayer = dynamic_pointer_cast<CPlayer>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, LAYER_PLAYER, TEXT("GameObject_Player")));
    shared_ptr<CCollider> pPlayerCollider = dynamic_pointer_cast<CCollider>(pPlayer->Get_PartObjectComponent(TEXT("Part_Weapon"), TEXT("Com_Collider")));

    /*CCollider*		pPlayerCollider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(LEVEL_STATIC, LAYER_PLAYER, TEXT("Com_Collider")));
    if (nullptr == pPlayerCollider)
        return;
        */

    m_pColliderCom->Intersect(pPlayerCollider);
}

_bool CProjectile::Projectile_Go_Straight(_float fTimeDelta)
{
    return m_pTransformCom->Go_Straight_Collide(fTimeDelta, m_pNavigationCom);
}

shared_ptr<CProjectile> CProjectile::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    shared_ptr<CProjectile> pInstance = make_shared<CProjectile>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CProjectile");
        pInstance.reset();
    }

    return pInstance;
}

void CProjectile::Free()
{
    __super::Free();
}
