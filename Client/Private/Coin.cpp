#include "pch.h"
#include "Coin.h"

#include "UI_Manager.h"
#include "Sound_Manager.h"

#include "GameInstance.h"

CCoin::CCoin(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag, const COIN_ID& eCoinID)
    : CStaticObject(pDevice, pContext, strModelComTag, LEVEL_MINIGAME)
    , m_eCoinID(eCoinID)
{
}

CCoin::CCoin(const CCoin& rhs)
    : CStaticObject(rhs)
{
}

HRESULT CCoin::Initialize(void* pArg)
{
    CGameObject::GAMEOBJECT_DESC			GameObjectDesc = {};

    GameObjectDesc.iGameObjectData = 10;
    GameObjectDesc.fSpeedPerSec = 10.0f;
    GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(&GameObjectDesc)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    _vector* vCreatePos = (_vector*)pArg;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, *vCreatePos);

    switch (m_eCoinID)
    {
    case Client::CCoin::COIN_DEFAULT:
        m_iValue = 20;
        //m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-186.2f, 0.5f, 214.5f, 1.f));
        break;
    case Client::CCoin::COIN_COPPER:
        m_iValue = 10;
        //m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-53.8f, 8.f, 274.f, 1.f));
        break;
    case Client::CCoin::COIN_SILVER:
        m_iValue = 20;
        //m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-144.f, 7.f, 204.f, 1.f));
        break;
    case Client::CCoin::COIN_GOLD:
        m_iValue = 100;
        //m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
        break;
    default:
        m_iValue = 0;
        //m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
        break;
    }

    return S_OK;
}

void CCoin::Priority_Tick(_float fTimeDelta)
{
}

void CCoin::Tick(_float fTimeDelta)
{
    if (!m_isActive)
        return;

    m_pTransformCom->Turn({ 0.f, 1.f, 0.f, 0.f }, fTimeDelta);
}

void CCoin::Late_Tick(_float fTimeDelta)
{
    if (!m_isActive)
        return;

    m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
        return;

#ifdef _DEBUG
    // m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CCoin::Render()
{
    if (!m_isActive)
        return S_OK;

    return __super::Render();
}

void CCoin::OnCollision(LAYER_ID eColLayer, shared_ptr<class CCollider> pCollider)
{
    if (!m_isActive)
        return;

    switch (eColLayer)
    {
    case Engine::LAYER_PLAYER:
        if (!m_pColliderCom->IsOnCollide())
            return;

        CSound_Manager::GetInstance()->StopSound(CSound_Manager::CHANNELID::PROP1);
        CSound_Manager::GetInstance()->PlaySound(L"MiniGame_Coin_[cut_0sec].wav", CSound_Manager::CHANNELID::PROP1, 1.f);

        // UI ¶ß±â
        CUI_Manager::GetInstance()->GainCoin(m_iValue);

        m_isAlive = false;
    }
}

HRESULT CCoin::Add_Component()
{
    /* Com_Collider */
    CBounding_Sphere::SPHERE_DESC		BoundingDesc{};

    BoundingDesc.vCenter = _float3(0.f, 1.f, 0.f);
    BoundingDesc.fRadius = 0.8f;

    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pColliderCom), &BoundingDesc)))
        return E_FAIL;

    m_pGameInstance->Add_Collider(LAYER_COIN, m_pColliderCom);
    m_pColliderCom->SetOwner(shared_from_this());

    return S_OK;
}

HRESULT CCoin::Bind_ShaderResources()
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

shared_ptr<CCoin> CCoin::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag, const COIN_ID& eCoinID, _vector vCreatePos, _bool isActive)
{
    shared_ptr<CCoin> pInstance = make_shared<CCoin>(pDevice, pContext, strModelComTag, eCoinID);

    if (FAILED(pInstance->Initialize(&vCreatePos)))
    {
        MSG_BOX("Failed to Created : CCoin");
        pInstance.reset();
    }

    pInstance->SetActive(isActive);
    pInstance->Set_CanPick(false);

    return pInstance;
}

void CCoin::Free()
{
    __super::Free();
}
