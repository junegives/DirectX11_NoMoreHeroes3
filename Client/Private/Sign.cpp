#include "pch.h"
#include "SIgn.h"

#include "Player.h"

#include "GameInstance.h"

CSign::CSign(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag, const SIGN_ID& eSignID)
    : CStaticObject(pDevice, pContext, strModelComTag, LEVEL_VILLAGE)
    , m_eSignID(eSignID)
{
}

CSign::CSign(const CSign& rhs)
    : CStaticObject(rhs)
{
}

HRESULT CSign::Initialize(void* pArg)
{
    CGameObject::GAMEOBJECT_DESC			GameObjectDesc = {};

    GameObjectDesc.iGameObjectData = 10;
    GameObjectDesc.fSpeedPerSec = 10.0f;
    GameObjectDesc.fRotationPerSec = XMConvertToRadians(40.0f);

    if (FAILED(__super::Initialize(&GameObjectDesc)))
        return E_FAIL;

    switch (m_eSignID)
    {
    case Client::CSign::SIGN_BATTLE1:
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-186.2f, 30.f, 214.5f, 1.f));
        break;
    case Client::CSign::SIGN_BATTLE2:
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-53.8f, 50.f, 274.f, 1.f));
        break;
    case Client::CSign::SIGN_MINIGAME:
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-38.5f, 120.f, 38.8f, 1.f));
        m_pTransformCom->Set_Scale(3.f, 3.f, 3.f);
        break;
    case Client::CSign::SIGN_BOSS:
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-144.f, 20.f, 204.f, 1.f));
        break;
    default:
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
        break;
    }

    return S_OK;
}

void CSign::Priority_Tick(_float fTimeDelta)
{
}

void CSign::Tick(_float fTimeDelta)
{
    if (!m_isActive)
        return;

    m_pTransformCom->Turn({ 0.f, 1.f, 0.f, 0.f }, fTimeDelta);
}

void CSign::Late_Tick(_float fTimeDelta)
{
    if (!m_isActive)
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
        return;

#ifdef _DEBUG
    // m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CSign::Render()
{
    if (!m_isActive)
        return S_OK;

    return __super::Render();
}

void CSign::OnCollision(LAYER_ID eColLayer, shared_ptr<class CCollider> pCollider)
{
    if (!m_isActive)
        return;
}

HRESULT CSign::Bind_ShaderResources()
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

shared_ptr<CSign> CSign::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag, const SIGN_ID& eSignID, _bool isActive)
{
    shared_ptr<CSign> pInstance = make_shared<CSign>(pDevice, pContext, strModelComTag, eSignID);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CSign");
        pInstance.reset();
    }

    pInstance->SetActive(isActive);
    pInstance->Set_CanPick(false);

    return pInstance;
}

void CSign::Free()
{
    __super::Free();
}
