#include "pch.h"
#include "UI_Coin.h"

CUI_Coin::CUI_Coin(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CUI(pDevice, pContext)
{
}

CUI_Coin::CUI_Coin(const CUI_Coin& rhs)
    : CUI(rhs)
{
}

HRESULT CUI_Coin::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

void CUI_Coin::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Coin::Tick(_float fTimeDelta)
{
    if (!m_isVisible)
        return;

    if (LEVEL_BATTLE1 == m_pGameInstance->Get_CurLevel() || LEVEL_BATTLE2 == m_pGameInstance->Get_CurLevel())
        return;

    __super::Tick(fTimeDelta);
}

void CUI_Coin::Late_Tick(_float fTimeDelta)
{
    if (LEVEL_BATTLE1 == m_pGameInstance->Get_CurLevel() || LEVEL_BATTLE2 == m_pGameInstance->Get_CurLevel())
        return;

    m_isVisible = true;

    __super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Coin::Render()
{
    if (!m_isVisible)
        return S_OK;

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Coin::Add_Component()
{
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Coin"),
        TEXT("Com_Texture"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pTextureCom))))
        return E_FAIL;

    // 숫자 텍스처
    {
        if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Coin_Num0"),
            TEXT("Com_Texture_Num0"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pTextureCom_Num[0]))))
            return E_FAIL;

        if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Coin_Num1"),
            TEXT("Com_Texture_Num1"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pTextureCom_Num[1]))))
            return E_FAIL;

        if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Coin_Num2"),
            TEXT("Com_Texture_Num2"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pTextureCom_Num[2]))))
            return E_FAIL;

        if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Coin_Num3"),
            TEXT("Com_Texture_Num3"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pTextureCom_Num[3]))))
            return E_FAIL;

        if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Coin_Num4"),
            TEXT("Com_Texture_Num4"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pTextureCom_Num[4]))))
            return E_FAIL;

        if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Coin_Num5"),
            TEXT("Com_Texture_Num5"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pTextureCom_Num[5]))))
            return E_FAIL;

        if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Coin_Num6"),
            TEXT("Com_Texture_Num6"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pTextureCom_Num[6]))))
            return E_FAIL;

        if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Coin_Num7"),
            TEXT("Com_Texture_Num7"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pTextureCom_Num[7]))))
            return E_FAIL;

        if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Coin_Num8"),
            TEXT("Com_Texture_Num8"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pTextureCom_Num[8]))))
            return E_FAIL;

        if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Coin_Num9"),
            TEXT("Com_Texture_Num9"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pTextureCom_Num[9]))))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CUI_Coin::Bind_ShaderResources()
{
    m_pTransformCom->Set_Scale(m_UIDesc.fCX, m_UIDesc.fCY, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.fX - g_iWinSizeX * 0.5f, -m_UIDesc.fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

    _float4x4		ViewMatrix, ProjMatrix;

    XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&ProjMatrix, XMMatrixIdentity());

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    // 로고 출력
    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    _bool   isTest = false;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_isTest", &isTest, sizeof(_bool))))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    // 코인 숫자 출력
    {
        m_pTransformCom->Set_Scale(m_UIDesc.fCX * 0.5f, m_UIDesc.fCY * 0.5f, 1.f);

        // 1000, 100, 10, 1자리 숫자 출력
        for (size_t i = 0; i < 4; i++)
        {
            if (0 == i)
                m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(15.f, 0.f, 0.f, 0.f));

            m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(22.f, 0.f, 0.f, 0.f));

            if (FAILED(m_pShaderCom->Begin(0)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_isTest", &isTest, sizeof(_bool))))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Bind_Buffers()))
                return E_FAIL;

            if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
                return E_FAIL;

            _uint iTest = (_uint)(m_iCoin / (_uint)pow(10, 3 - i));
            _uint iTest2 = ((_uint)(m_iCoin / (_uint)pow(10, 3 - i))) % 10;

            if (FAILED(m_pTextureCom_Num[((_uint)(m_iCoin / (_uint)pow(10, 3 - i))) % 10]->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;
        }
    }

    return S_OK;
}

shared_ptr<CUI_Coin> CUI_Coin::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    shared_ptr<CUI_Coin> pInstance = make_shared<CUI_Coin>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CUI_Coin");
        pInstance.reset();
    }

    return pInstance;
}
