#include "pch.h"
#include "UI_Fade.h"

CUI_Fade::CUI_Fade(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CUI(pDevice, pContext)
{
}

CUI_Fade::CUI_Fade(const CUI_Fade& rhs)
    : CUI(rhs)
{
}

HRESULT CUI_Fade::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

void CUI_Fade::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Fade::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    m_fFadeTime -= fTimeDelta;

    if (m_isFadeOut)
    {
        m_fMaskAlpha -= (fTimeDelta / m_fMaxFadeTime);

        if (0.f > m_fFadeTime)
        {
            m_isFadeOut = false;
            m_isFadeIn = false;
        }
    }

    else if (m_isFadeIn)
    {
        m_fMaskAlpha += (fTimeDelta / m_fMaxFadeTime);

        if (0.f > m_fFadeTime)
        {
            m_isFadeOut = false;
            m_isFadeIn = false;

            m_isVisible = false;
            m_isActive = false;
        }
    }
}

void CUI_Fade::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Fade::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    CalcCorner();

    return S_OK;
}

void CUI_Fade::FadeOut(_float fFadeTime)
{
    m_isFadeOut = true;
    m_isFadeIn = false;

    m_isActive = true;

    m_fMaskAlpha = 1.f;
    m_fMaxFadeTime = fFadeTime;
    m_fFadeTime = fFadeTime;
}

void CUI_Fade::FadeIn(_float fFadeTime)
{
    m_isFadeOut = false;
    m_isFadeIn = true;

    m_fMaskAlpha = 0.f;
    m_fMaxFadeTime = fFadeTime;
    m_fFadeTime = fFadeTime;
}

HRESULT CUI_Fade::Add_Component()
{
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Fade"),
        TEXT("Com_Texture"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Fade::Bind_ShaderResources()
{
    _float4x4		ViewMatrix, ProjMatrix;

    XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&ProjMatrix, XMMatrixIdentity());

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaskAlpha", &m_fMaskAlpha, sizeof(_float))))
        return E_FAIL;

    _bool   isTest = true;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_isTest", &isTest, sizeof(_bool))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(1)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

shared_ptr<CUI_Fade> CUI_Fade::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    shared_ptr<CUI_Fade> pInstance = make_shared<CUI_Fade>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CUI_Fade");
        pInstance.reset();
    }

    return pInstance;
}
