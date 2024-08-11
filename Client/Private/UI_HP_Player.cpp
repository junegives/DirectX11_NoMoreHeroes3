#include "pch.h"
#include "UI_HP_Player.h"

CUI_HP_Player::CUI_HP_Player(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CUI(pDevice, pContext)
{
}

CUI_HP_Player::CUI_HP_Player(const CUI_HP_Player& rhs)
    : CUI(rhs)
{
}

CUI_HP_Player::~CUI_HP_Player()
{
    Free();
}

HRESULT CUI_HP_Player::Initialize(void* pArg)
{
    UIDESC* pUIDesc = (UIDESC*)pArg;

    m_UIDesc.fCX = pUIDesc->fCX;
    m_UIDesc.fCY = pUIDesc->fCY;
    m_UIDesc.fX = pUIDesc->fX;
    m_UIDesc.fY = pUIDesc->fY;

    m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == m_pTransformCom)
        return E_FAIL;

    CTransform::TRANSFORM_DESC TransDesc = {};

    TransDesc.fRotationPerSec = XMConvertToRadians(60.f);
    TransDesc.fSpeedPerSec = 10.f;

    if (FAILED(m_pTransformCom->Initialize(pArg)))
        return E_FAIL;

    m_Components.emplace(g_strTransformTag, m_pTransformCom);

    if (FAILED(Add_Component()))
        return E_FAIL;

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

    m_pTransformCom->Set_Scale(2.0f, 2.0f, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.fX - g_iWinSizeX * 0.5f, -m_UIDesc.fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

    return S_OK;
}

void CUI_HP_Player::Priority_Tick(_float fTimeDelta)
{
}

void CUI_HP_Player::Tick(_float fTimeDelta)
{
    if (!m_isVisible)
        return;

    if (m_pGameInstance->Is_KeyDown(DIK_5))
    {
        m_iHP++;

        if (m_iMaxHP < m_iHP)
            m_iHP = m_iMaxHP;
    }

    if (m_pGameInstance->Is_KeyDown(DIK_6))
    {
        if (0 >= m_iHP)
            m_iHP = 1;

        m_iHP--;
    }

    m_pVIBufferCom->Tick_HP(fTimeDelta, m_iHP, 0);
}

void CUI_HP_Player::Late_Tick(_float fTimeDelta)
{
    m_isVisible = true;

    __super::Late_Tick(fTimeDelta);
}

HRESULT CUI_HP_Player::Render()
{
    if (!m_isVisible)
        return S_OK;

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_HP_Player::Add_Component()
{
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxUIPointInstance"),
        TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
        return E_FAIL;

    CVIBuffer_Particle_Point_UI::INSTANCE_UI_DESC			InstanceDesc{};
    InstanceDesc.eUIType = CVIBuffer_Particle_Point_UI::UI_PLAYERHP;
    InstanceDesc.vPivot = _float3(0.f, 9.f, 0.f);
    InstanceDesc.vCenter = _float3(0.0f, 0.0f, 0.f);
    InstanceDesc.vRange = _float3(300.0f, 5.0f, 0.f);
    InstanceDesc.vSize = _float2(m_UIDesc.fCX, m_UIDesc.fCY);
    InstanceDesc.vSpeed = _float2(1.f, 3.f);
    InstanceDesc.vLifeTime = _float2(20.f, 50.f);
    InstanceDesc.isLoop = true;
    InstanceDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);

    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_UI_Point_58"),
        TEXT("Com_VIBuffer"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pVIBufferCom), &InstanceDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_HP_Player::Bind_ShaderResources()
{
    _float4		vColor = _float4(1.f, 0.f, 0.f, 1.f);

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

shared_ptr<CUI_HP_Player> CUI_HP_Player::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    shared_ptr<CUI_HP_Player> pInstance = make_shared<CUI_HP_Player>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CUI_HP_Player");
        pInstance.reset();
    }

    return pInstance;
}

void CUI_HP_Player::Free()
{
    m_pVIBufferCom->Free();
}