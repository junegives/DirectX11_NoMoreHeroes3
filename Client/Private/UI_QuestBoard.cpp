#include "pch.h"
#include "UI_QuestBoard.h"

CUI_QuestBoard::CUI_QuestBoard(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CUI(pDevice, pContext)
{
}

CUI_QuestBoard::CUI_QuestBoard(const CUI_QuestBoard& rhs)
    : CUI(rhs)
{
}

HRESULT CUI_QuestBoard::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    m_QuestMap.emplace(QUEST_BATTLE, L"버거형 : 지정 매치를 찾아 도전해 봐.");
    m_QuestMap.emplace(QUEST_GOLD, L"버거형 : 랭킹전 참가비를 마련해 봐.");
    m_QuestMap.emplace(QUEST_BOSS, L"버거형 : 랭킹전에 도전해 봐.");

    SetQuest(QUEST_BATTLE);

    return S_OK;
}

void CUI_QuestBoard::Priority_Tick(_float fTimeDelta)
{
}

void CUI_QuestBoard::Tick(_float fTimeDelta)
{
    if (!m_isVisible)
        return;

    __super::Tick(fTimeDelta);

    if (!m_isMoving)
    {
        if (m_pGameInstance->Is_KeyDown(DIK_E))
        {
            if (m_isFold)
            {
                MoveTo(false, XMVectorSet(m_UIDesc.fCX * 0.5f, m_UIDesc.fCY * 1.2f, 0.f, 1.f), 0.f, 0.15f);
            }

            else
            {
                MoveTo(false, XMVectorSet(-160.f, m_UIDesc.fCY * 1.2f, 0.f, 1.f), 0.f, 0.15f);
            }

            m_isFold = !m_isFold;
        }
    }
}

void CUI_QuestBoard::Late_Tick(_float fTimeDelta)
{
    m_isVisible = true;

    __super::Late_Tick(fTimeDelta);
}

HRESULT CUI_QuestBoard::Render()
{
    if (!m_isVisible)
        return S_OK;

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    CalcCorner();

    if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_NexonSmall"), m_strCurrentQuest, _float2(m_fLT.x + 17.f, m_fLT.y + 30.f), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;
 

    return S_OK;
}

HRESULT CUI_QuestBoard::Add_Component()
{
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QuestBoard"),
        TEXT("Com_Texture"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_QuestBoard::Bind_ShaderResources()
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

    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

shared_ptr<CUI_QuestBoard> CUI_QuestBoard::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    shared_ptr<CUI_QuestBoard> pInstance = make_shared<CUI_QuestBoard>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CUI_QuestBoard");
        pInstance.reset();
    }

    return pInstance;
}
