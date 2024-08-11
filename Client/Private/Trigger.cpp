#include "pch.h"
#include "Trigger.h"

#include "Player.h"
#include "Effect_Pillar.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "UI.h"
#include "UI_Fade.h"

#include "Effect_Pillar.h"
#include "Sound_Manager.h"

CTrigger::CTrigger(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag, const TRIGGER_ID& eTriggerID)
    : CStaticObject(pDevice, pContext, strModelComTag, LEVEL_VILLAGE)
    , m_eTriggerID(eTriggerID)
{
}

CTrigger::CTrigger(const CTrigger& rhs)
    : CStaticObject(rhs)
{
}

HRESULT CTrigger::Initialize(void* pArg)
{    
    CGameObject::GAMEOBJECT_DESC			GameObjectDesc = {};

    GameObjectDesc.iGameObjectData = 10;
    GameObjectDesc.fSpeedPerSec = 10.0f;
    GameObjectDesc.fRotationPerSec = XMConvertToRadians(40.0f);

    if (FAILED(__super::Initialize(&GameObjectDesc)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    _vector vPos;

    switch (m_eTriggerID)
    {
    case Client::CTrigger::TRIGGER_BATTLE1:
        vPos = XMVectorSet(-186.2f, 0.5f, 214.5f, 1.f);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-186.2f, 0.5f, 214.5f, 1.f));
        break;
    case Client::CTrigger::TRIGGER_BATTLE2:
        vPos = XMVectorSet(-53.8f, 8.f, 274.f, 1.f);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-53.8f, 8.f, 274.f, 1.f));
        break;
    case Client::CTrigger::TRIGGER_MINIGAME:
        vPos = XMVectorSet(-38.6f, 7.0f, 38.5f, 1.0f);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-38.6f, 7.0f, 38.5f, 1.0f));
        break;
    case Client::CTrigger::TRIGGER_BOSS:
        vPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
        break;
    default:
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
        break;
    }

    CEffect::EFFECT_DESC effectDesc = {};
    {
        effectDesc.eEffectType = CEffect::EFFECT_MESH;
        effectDesc.vCenter = { 0.f, 0.f, 0.f };
        effectDesc.vSize = { 4.f, 4.f, 1.f };
        effectDesc.isMove = false;
        effectDesc.vDir = { 0.f, 0.f, 0.f };
        effectDesc.fMoveSpeed = 0.01f;
        effectDesc.isRotate = false;
        effectDesc.isThorwColor = true;
        effectDesc.vColor = { 0.435f, 0.99f, 0.99f, 1.f };
        effectDesc.isLoop = true;
        effectDesc.iShaderPass = 4;
        effectDesc.isSprite = false;
        effectDesc.isUseMask = true;

        m_pEffectTriggerPillar = CEffect_Pillar::Create(m_pDevice, m_pContext, &effectDesc);
        m_pEffectTriggerPillar->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vPos + XMVectorSet(0.f, -10.f, 0.f, 0.f));
        m_pEffectTriggerPillar->Get_TransformCom()->Set_Scale(0.3f, 1.f, 0.3f);
    }

    return S_OK;
}

void CTrigger::Priority_Tick(_float fTimeDelta)
{
}

void CTrigger::Tick(_float fTimeDelta)
{
    if (!m_isActive)
        return;

    m_pTransformCom->Turn({ 0.f, 1.f, 0.f, 0.f }, fTimeDelta);
    m_pEffectTriggerPillar->Tick(fTimeDelta);
}

void CTrigger::Late_Tick(_float fTimeDelta)
{
    if (!m_isActive)
        return;

    m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
    m_pEffectTriggerPillar->Late_Tick(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
        return;

#ifdef _DEBUG
     m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CTrigger::Render()
{
    if (!m_isActive)
        return S_OK;

    return __super::Render();
}

void CTrigger::OnCollision(LAYER_ID eColLayer, shared_ptr<class CCollider> pCollider)
{
    if (!m_isActive)
        return;

    switch (eColLayer)
    {
    case Engine::LAYER_PLAYER:
        if (!m_pColliderCom->IsOnCollide())
            return;

        if (STATE::STATE_RISING != dynamic_pointer_cast<CPlayer>(pCollider->GetOwner())->GetCurState())
        {
            // UI ¶ß±â
            shared_ptr<CUI> pUITeleport = CUI_Manager::GetInstance()->GetUI(TEXT("GameObject_UI_Teleport"));
            pUITeleport->SetVisible(true);
            pUITeleport->SetActive(true);
        }

        if (m_pGameInstance->Is_MouseDown(DIM_LB))
        {
            CSound_Manager::GetInstance()->StopAll(); 

            shared_ptr<CPlayer> pPlayer = dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player());

            switch (m_eTriggerID)
            {
            case TRIGGER_BATTLE1:
                pPlayer->SetSceneChange(true);
                pPlayer->SetNextLevel(LEVEL_BATTLE1);
                pPlayer->SetCurBGM(L"BGM_Battle_EZ.mp3");
                CSound_Manager::GetInstance()->PlaySound(L"Village_Warp2.wav", CSound_Manager::CHANNELID::PROP1, 1.f);

                /*m_pGameInstance->Set_CurLevel(LEVEL_BATTLE1);
                dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player())->Change_NaviCom(LEVEL_BATTLE1);*/
                break;

            case TRIGGER_BATTLE2:
                pPlayer->SetSceneChange(true);
                pPlayer->SetNextLevel(LEVEL_BATTLE2);
                pPlayer->SetCurBGM(L"BGM_Battle1.mp3");
                CSound_Manager::GetInstance()->PlaySound(L"Trigger_Move.wav", CSound_Manager::CHANNELID::PROP1, 1.f);

                /*m_pGameInstance->Set_CurLevel(LEVEL_BATTLE2);
                dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player())->Change_NaviCom(LEVEL_BATTLE2);*/
                break;

            case TRIGGER_MINIGAME:
                pPlayer->SetSceneChange(true);
                pPlayer->SetNextLevel(LEVEL_MINIGAME);
                pPlayer->SetCurBGM(L"BGM_Nyan_Origin.mp3");
                CSound_Manager::GetInstance()->PlaySound(L"Village_Warp2.wav", CSound_Manager::CHANNELID::PROP1, 1.f);
                //m_pGameInstance->Set_CurLevel();
                //dynamic_pointer_cast<CPlayer>(m_pGameInstance->Get_Player())->Change_NaviCom(LEVEL_BATTLE2);
                break;

            case TRIGGER_BOSS:
                break;

            default:
                break;
            }
        }

        break;
    }
}

HRESULT CTrigger::Add_Component()
{
    /* Com_Collider */
    CBounding_Sphere::SPHERE_DESC		BoundingDesc{};

    BoundingDesc.vCenter = _float3(0.f, 1.f, 0.f);
    BoundingDesc.fRadius = 0.8f;

    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pColliderCom), &BoundingDesc)))
        return E_FAIL;

    m_pGameInstance->Add_Collider(LAYER_TELEPORT, m_pColliderCom);
    m_pColliderCom->SetOwner(shared_from_this());

    return S_OK;
}

HRESULT CTrigger::Bind_ShaderResources()
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

shared_ptr<CTrigger> CTrigger::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag, const TRIGGER_ID& eTriggerID, _bool isActive)
{
    shared_ptr<CTrigger> pInstance = make_shared<CTrigger>(pDevice, pContext, strModelComTag, eTriggerID);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CTrigger");
        pInstance.reset();
    }

    pInstance->SetActive(isActive);
    pInstance->Set_CanPick(false);

    return pInstance;
}

void CTrigger::Free()
{
    __super::Free();
}
