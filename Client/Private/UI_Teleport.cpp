#include "pch.h"
#include "UI_Teleport.h"
#include "UI_Manager.h"

CUI_Teleport::CUI_Teleport(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CUI(pDevice, pContext)
{
}

CUI_Teleport::CUI_Teleport(const CUI_Teleport& rhs)
    : CUI(rhs)
{
}

HRESULT CUI_Teleport::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    m_UIDesc.fY = 20.f;

    m_fY = -200.f;

    m_pTransformCom_Font->Set_Scale(m_UIDesc.fCX * 1.5f, 50.f, 1.f);

    m_vPosFontTop[0][0]     = XMVectorSet(m_UIDesc.fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f);
    m_vPosFontTop[1][0]     = m_vPosFontTop[0][0] + XMVectorSet(0.f, -60.f, 0.f, 0.f);
    m_vPosFontTop[2][0]     = m_vPosFontTop[1][0] + XMVectorSet(0.f, -60.f, 0.f, 0.f);
    m_vPosFontTop[3][0]     = m_vPosFontTop[2][0] + XMVectorSet(0.f, -60.f, 0.f, 0.f);

    m_vPosFontBottom[0][0]  = XMVectorSet(m_UIDesc.fX - g_iWinSizeX * 0.5f, m_fY - g_iWinSizeY * 0.5f, 0.f, 1.f);
    m_vPosFontBottom[1][0]  = m_vPosFontBottom[0][0] + XMVectorSet(0.f, 60.f, 0.f, 0.f);
    m_vPosFontBottom[2][0]  = m_vPosFontBottom[1][0] + XMVectorSet(0.f, 60.f, 0.f, 0.f);
    m_vPosFontBottom[3][0]  = m_vPosFontBottom[2][0] + XMVectorSet(0.f, 60.f, 0.f, 0.f);

    m_pTransformCom_Font->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.fX - g_iWinSizeX * 0.5f, -20.f + g_iWinSizeY * 0.5f, 0.f, 1.f));

    m_pTransformCom_Button->Set_Scale(m_UIDesc.fCX * 0.2f, 50.f, 1.f);
    m_pTransformCom_Button->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.fX * 0.2f - g_iWinSizeX * 0.5f, -100.f + g_iWinSizeY * 0.5f, 0.f, 1.f));
    //m_pTransformCom_Button->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.fX * 0.2f - g_iWinSizeX * 0.5f, -360, 0.f, 1.f));

    m_isActive = false;
    m_isVisible = false;
    m_isIn = true;

    return S_OK;
}

void CUI_Teleport::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Teleport::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (m_isIn)
    {
        if (!m_isVisible)
        {
            m_isIn = false;
            m_isLoop = false;
            m_isOut = true;
        }

        // 나타나기
        if (g_iWinSizeY * 0.5f + 90.f < m_vPosFontTop[0][0].m128_f32[1])
        {
            for (size_t i = 0; i < 4; i++)
            {
                m_vPosFontTop[i][0].m128_f32[1] -= 10.f;
                m_vPosFontTop[i][1].m128_f32[1] -= 10.f;
            }

            for (size_t i = 0; i < 4; i++)
            {
                m_vPosFontBottom[i][0].m128_f32[1] += 10.f;
                m_vPosFontBottom[i][1].m128_f32[1] += 10.f;
            }
        }

        else
        {
            m_isIn = false;
            m_isLoop = true;
        }
    }

    else if (m_isOut)
    {
        // 사라지기
        if (-m_fY + g_iWinSizeY * 0.5f > m_vPosFontTop[0][0].m128_f32[1])
        {
            for (size_t i = 0; i < 4; i++)
            {
                m_vPosFontTop[i][0].m128_f32[1] += 20.f;
                m_vPosFontTop[i][1].m128_f32[1] += 20.f;
            }

            for (size_t i = 0; i < 4; i++)
            {
                m_vPosFontBottom[i][0].m128_f32[1] -= 20.f;
                m_vPosFontBottom[i][1].m128_f32[1] -= 20.f;
            }
        }

        else
        {
            /*m_vPosFontTop[0][0].m128_f32[1] = -m_fY + g_iWinSizeY * 0.5f;
            m_vPosFontTop[1][0].m128_f32[1] = m_vPosFontTop[0][0].m128_f32[1] - 60.f;
            m_vPosFontTop[2][0].m128_f32[1] = m_vPosFontTop[1][0].m128_f32[1] - 60.f;
            m_vPosFontTop[3][0].m128_f32[1] = m_vPosFontTop[2][0].m128_f32[1] - 60.f;

            m_vPosFontBottom[0][0].m128_f32[1] = m_fY - g_iWinSizeY * 0.5f;
            m_vPosFontBottom[1][0].m128_f32[1] = m_vPosFontBottom[0][0].m128_f32[1] + 60.f;
            m_vPosFontBottom[2][0].m128_f32[1] = m_vPosFontBottom[1][0].m128_f32[1] + 60.f;
            m_vPosFontBottom[3][0].m128_f32[1] = m_vPosFontBottom[2][0].m128_f32[1] + 60.f;*/

            m_vPosFontTop[0][0] = XMVectorSet(m_UIDesc.fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f);
            m_vPosFontBottom[0][0] = XMVectorSet(m_UIDesc.fX - g_iWinSizeX * 0.5f, m_fY - g_iWinSizeY * 0.5f, 0.f, 1.f);

            for (size_t i = 1; i < 4; i++)
            {
                m_vPosFontTop[i][0] = m_vPosFontTop[i - 1][0] + XMVectorSet(0.f, -60.f, 0.f, 0.f);
                m_vPosFontBottom[i][0] = m_vPosFontBottom[i - 1][0] + XMVectorSet(0.f, 60.f, 0.f, 0.f);

                m_vPosFontTop[i][1].m128_f32[1] = -1000.f;
                m_vPosFontBottom[i][1].m128_f32[1] = -1000.f;
            }

            /*m_vPosFontTop[0][0] = XMVectorSet(m_UIDesc.fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f);
            m_vPosFontTop[1][0] = m_vPosFontTop[0][0] + XMVectorSet(0.f, -60.f, 0.f, 0.f);
            m_vPosFontTop[2][0] = m_vPosFontTop[1][0] + XMVectorSet(0.f, -60.f, 0.f, 0.f);
            m_vPosFontTop[3][0] = m_vPosFontTop[2][0] + XMVectorSet(0.f, -60.f, 0.f, 0.f);

            m_vPosFontBottom[0][0] = XMVectorSet(m_UIDesc.fX - g_iWinSizeX * 0.5f, m_fY - g_iWinSizeY * 0.5f, 0.f, 1.f);
            m_vPosFontBottom[1][0] = m_vPosFontBottom[0][0] + XMVectorSet(0.f, 60.f, 0.f, 0.f);
            m_vPosFontBottom[2][0] = m_vPosFontBottom[1][0] + XMVectorSet(0.f, 60.f, 0.f, 0.f);
            m_vPosFontBottom[3][0] = m_vPosFontBottom[2][0] + XMVectorSet(0.f, 60.f, 0.f, 0.f);*/

            m_isIn = true;
            m_isOut = false;
            m_isLoop = false;

            m_isActive = false;
        }
    }

    else
    {
        // 유지하기
        if (!m_isVisible)
        {
            m_isIn = false;
            m_isOut = true;
            m_isLoop = false;
        }
    }
}

void CUI_Teleport::Late_Tick(_float fTimeDelta)
{
    CUI_Manager::GetInstance()->AllVisibleOff();

    m_isVisible = false;

    __super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Teleport::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Teleport::Add_Component()
{
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_TeleportFont"),
        TEXT("Com_Texture_Font"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pTextureCom_Font))))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_TeleportButton"),
        TEXT("Com_Texture_Button"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pTextureCom_Button))))
        return E_FAIL;

   m_pTransformCom_Font = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == m_pTransformCom_Font)
        return E_FAIL;

    m_pTransformCom_Button = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == m_pTransformCom_Button)
        return E_FAIL;

    m_pTransformCom_Font->TurnRadian({ 0.f, 0.f, 1.f, 0.f }, XMConvertToRadians(10));

    m_Components.emplace(TEXT("Com_Transform_Font"), m_pTransformCom_Font);
    m_Components.emplace(TEXT("Com_Transform_Button"), m_pTransformCom_Button);

    CTransform::TRANSFORM_DESC TransformDesc = {};
    TransformDesc.fSpeedPerSec = 5.f;
    TransformDesc.fRotationPerSec = 0.f;

    if (FAILED(m_pTransformCom_Font->Initialize(&TransformDesc)))
        return E_FAIL;

    if (FAILED(m_pTransformCom_Button->Initialize(&TransformDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Teleport::Bind_ShaderResources()
{
    _float4x4		ViewMatrix, ProjMatrix;

    XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&ProjMatrix, XMMatrixIdentity());

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    _bool   isTest = false;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_isTest", &isTest, sizeof(_bool))))
        return E_FAIL;

    // Font 출력 (상)
    for (size_t i = 0; i < 4; i++)
    {
        CalcCorner(m_vPosFontTop[i][0], m_pTransformCom_Font->Get_Scale());

        // 오른쪽으로 이동하는 애들
        if (i % 2 == 0)
        {
            // 왼쪽에 있던 텍스처의 왼쪽 끝이 보였을 경우
            if (0.f <= m_fLT.x)
            {
                m_vPosFontTop[i][1] = m_vPosFontTop[i][0] + XMVectorSet(-m_pTransformCom_Font->Get_Scale().x - 1.f, -tanf(XMConvertToRadians(10)) * m_pTransformCom_Font->Get_Scale().x, 0.f, 0.f);

                _vector vTemp = m_vPosFontTop[i][0];
                m_vPosFontTop[i][0] = m_vPosFontTop[i][1];
                m_vPosFontTop[i][1] = vTemp;
            }

            _float fSpeed = (i + 1) * 0.3f;

            m_vPosFontTop[i][0] += (XMVectorSet(fSpeed, tanf(XMConvertToRadians(10)) * fSpeed, 0.f, 0.f));
            m_vPosFontTop[i][1] += (XMVectorSet(fSpeed, tanf(XMConvertToRadians(10)) * fSpeed, 0.f, 0.f));
        }

        // 왼쪽으로 이동하는 애들
        else
        {
            // 오른쪽에 있던 텍스처의 오른쪽 끝이 보였을 경우
            if (g_iWinSizeX >= m_fRT.x)
            {
                m_vPosFontTop[i][1] = m_vPosFontTop[i][0] + XMVectorSet(m_pTransformCom_Font->Get_Scale().x + 1.f, tanf(XMConvertToRadians(10)) * m_pTransformCom_Font->Get_Scale().x, 0.f, 0.f);

                _vector vTemp = m_vPosFontTop[i][0];
                m_vPosFontTop[i][0] = m_vPosFontTop[i][1];
                m_vPosFontTop[i][1] = vTemp;
            }

            _float fSpeed = (i + 1) * 0.3f;

            m_vPosFontTop[i][0] += (XMVectorSet(-fSpeed, -tanf(XMConvertToRadians(10)) * fSpeed, 0.f, 0.f));
            m_vPosFontTop[i][1] += (XMVectorSet(-fSpeed, -tanf(XMConvertToRadians(10)) * fSpeed, 0.f, 0.f));
        }

        for (size_t j = 0; j < 2; j++)
        {
            m_pTransformCom_Font->Set_State(CTransform::STATE_POSITION, m_vPosFontTop[i][j]);

            if (FAILED(m_pShaderCom->Begin(0)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Bind_Buffers()))
                return E_FAIL;

            if (FAILED(m_pTransformCom_Font->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
                return E_FAIL;

            if (FAILED(m_pTextureCom_Font->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;
        }
    }

    // Font 출력 (하)
    for (size_t i = 0; i < 4; i++)
    {
        CalcCorner(m_vPosFontBottom[i][0], m_pTransformCom_Font->Get_Scale());

        // 왼쪽으로 이동하는 애들
        if (i % 2 == 0)
        {
            // 오른쪽에 있던 텍스처의 오른쪽 끝이 보였을 경우
            if (g_iWinSizeX >= m_fRT.x)
            {
                m_vPosFontBottom[i][1] = m_vPosFontBottom[i][0] + XMVectorSet(m_pTransformCom_Font->Get_Scale().x + 1.f, tanf(XMConvertToRadians(10)) * m_pTransformCom_Font->Get_Scale().x, 0.f, 0.f);

                _vector vTemp = m_vPosFontBottom[i][0];
                m_vPosFontBottom[i][0] = m_vPosFontBottom[i][1];
                m_vPosFontBottom[i][1] = vTemp;
            }

            _float fSpeed = (i + 1) * 0.3f;

            m_vPosFontBottom[i][0] += (XMVectorSet(-fSpeed, -tanf(XMConvertToRadians(10)) * fSpeed, 0.f, 0.f));
            m_vPosFontBottom[i][1] += (XMVectorSet(-fSpeed, -tanf(XMConvertToRadians(10)) * fSpeed, 0.f, 0.f));
        }

        // 오른쪽으로 이동하는 애들
        else
        {
            // 왼쪽에 있던 텍스처의 왼쪽 끝이 보였을 경우
            if (0.f <= m_fLT.x)
            {
                m_vPosFontBottom[i][1] = m_vPosFontBottom[i][0] + XMVectorSet(-m_pTransformCom_Font->Get_Scale().x - 1.f, -tanf(XMConvertToRadians(10)) * m_pTransformCom_Font->Get_Scale().x, 0.f, 0.f);

                _vector vTemp = m_vPosFontBottom[i][0];
                m_vPosFontBottom[i][0] = m_vPosFontBottom[i][1];
                m_vPosFontBottom[i][1] = vTemp;
            }

            _float fSpeed = (i + 1) * 0.3f;

            m_vPosFontBottom[i][0] += (XMVectorSet(fSpeed, tanf(XMConvertToRadians(10)) * fSpeed, 0.f, 0.f));
            m_vPosFontBottom[i][1] += (XMVectorSet(fSpeed, tanf(XMConvertToRadians(10)) * fSpeed, 0.f, 0.f));
        }

        for (size_t j = 0; j < 2; j++)
        {
            m_pTransformCom_Font->Set_State(CTransform::STATE_POSITION, m_vPosFontBottom[i][j]);

            if (FAILED(m_pShaderCom->Begin(0)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Bind_Buffers()))
                return E_FAIL;

            if (FAILED(m_pTransformCom_Font->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
                return E_FAIL;

            if (FAILED(m_pTextureCom_Font->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;
        }
    }

    return S_OK;
}

shared_ptr<CUI_Teleport> CUI_Teleport::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    shared_ptr<CUI_Teleport> pInstance = make_shared<CUI_Teleport>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CUI_Teleport");
        pInstance.reset();
    }

    return pInstance;
}
