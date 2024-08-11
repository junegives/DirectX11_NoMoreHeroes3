#include "pch.h"
#include "Body_Player.h"
#include "Player.h"

#include "Effect_Manager.h"

CBody_Player::CBody_Player(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CPartObject(pDevice, pContext)
{
}

CBody_Player::CBody_Player(const CBody_Player& rhs)
    : CPartObject(rhs)
{
}

HRESULT CBody_Player::Initialize(void* pArg)
{
    BODY_PLAYER_DESC* pBodyPlayerDesc = (BODY_PLAYER_DESC*)pArg;

    m_pPlayerState = pBodyPlayerDesc->pPlayerState;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;
    
    m_pModelCom->Set_Animation(5, true, 2.0f, 0.2f);

    XMStoreFloat4(&m_PrevTransPos, { 0.f, 0.f, 0.f, 0.f });

	return S_OK;
}

void CBody_Player::Priority_Tick(_float fTimeDelta)
{
}

void CBody_Player::Tick(_float fTimeDelta)
{
    if (m_isUseAll)
    {
        m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);
        m_pTransModelCom->Play_Animation(fTimeDelta, m_pTransformCom);
        m_pModelCom->Apply_RootMotion(fTimeDelta, m_pParentTransform, m_pNavigationCom);
        m_pTransModelCom->Apply_RootMotion(fTimeDelta, m_pParentTransform, m_pNavigationCom);
    }

    else
    {
        if (m_isTransformed)
        {
            m_pTransModelCom->Play_Animation(fTimeDelta, m_pTransformCom);
            m_pTransModelCom->Apply_RootMotion(fTimeDelta, m_pParentTransform, m_pNavigationCom);
        }

        else
        {
            m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);
            m_pModelCom->Apply_RootMotion(fTimeDelta, m_pParentTransform, m_pNavigationCom);
        }
    }

    // 디버깅
    if (m_pGameInstance->Is_KeyDown(DIK_Y))
    {
        m_isAfterImage = !m_isAfterImage;

        CEffect_Manager::GetInstance()->PlayEffect(TEXT("GameObject_Effect_Attack_Line"), 1, XMVectorSet(m_WorldMatrix.m[3][0], m_WorldMatrix.m[3][1], m_WorldMatrix.m[3][2], 1.f), true, {0.f, 0.f, 1.f, 0.f});
        /*m_isTransformed = false;
        m_isUseAll = false;*/
    }

    m_pModelCom->SetBlockRootMotion(false);
}

void CBody_Player::Late_Tick(_float fTimeDelta)
{
    XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_pParentTransform->Get_WorldMatrix());

    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
        return;
}

HRESULT CBody_Player::Render()
{
    //m_pGameInstance->Draw2DText(L"Hello, World!", g_iWinSizeX / 2.f, 30.f);

    _vector vTest = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (m_isTransformed)
    {
        // 셰이더에 던지는 루프를 클라에서 돌리려면 메시가 몇 개 있는지 알아야 한다.
        // 셰이더에 값 던지는 기능을 클라에서 선택적으로 가능
        _uint   iNumMeshes = m_pTransModelCom->Get_NumMeshes();

        for (_uint i = 0; i < iNumMeshes; i++)
        {
            if (FAILED(m_pTransModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType::aiTextureType_DIFFUSE, "g_DiffuseTexture")))
                return E_FAIL;

            if (FAILED(m_pTransModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(0)))
                return E_FAIL;

            if (FAILED(m_pTransModelCom->Render(i)))
                return E_FAIL;
        }
    }

    else
    {
        // 셰이더에 던지는 루프를 클라에서 돌리려면 메시가 몇 개 있는지 알아야 한다.
        // 셰이더에 값 던지는 기능을 클라에서 선택적으로 가능
        _uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

        AFTERIMAGE_DESC AfterImageDesc = {};
        map<_uint, _float4x4*> mapNewBoneMatrix;

        for (_uint i = 0; i < iNumMeshes; i++)
        {
            if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType::aiTextureType_DIFFUSE, "g_DiffuseTexture")))
                return E_FAIL;

            if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
                return E_FAIL;

            if (m_isAfterImage)
            {
                if (3 < m_AfterImageList.size())
                    m_AfterImageList.pop_front();

                _float4x4 BoneMatrix[256];

                if (FAILED(m_pModelCom->Copy_BoneMatrix(i, BoneMatrix)))
                    return E_FAIL;

                mapNewBoneMatrix.emplace(i, BoneMatrix);
                //mapNewBoneMatrix.emplace(i, BoneMatrix);
            }

            if (m_isAfterImage)
            {
                _float fAlpha = 0.1f;
                _float3 vColor = { 0.851, 0.471, 0.212 };

                for (auto iter : m_AfterImageList)
                {
                    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fAlpha, sizeof(_float))))
                        return E_FAIL;

                    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float3))))
                        return E_FAIL;

                    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &iter.WorldMatrix)))
                        return E_FAIL;

                    if (FAILED(m_pModelCom->Bind_PrevBoneMatrices(m_pShaderCom, "g_BoneMatrices", iter.BoneMatrix.find(i)->second)))
                        return E_FAIL;

                    if (FAILED(m_pShaderCom->Begin(1)))
                        return E_FAIL;

                    if (FAILED(m_pModelCom->Render(i)))
                        return E_FAIL;

                    fAlpha += 0.2f;
                }
            }
            
            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(0)))
                return E_FAIL;

            if (FAILED(m_pModelCom->Render(i)))
                return E_FAIL;
        }

        if (m_isAfterImage)
        {
            AfterImageDesc.BoneMatrix = mapNewBoneMatrix;
            AfterImageDesc.WorldMatrix = m_WorldMatrix;

            m_AfterImageList.push_back(AfterImageDesc);
        }
    }


    return S_OK;
}

void CBody_Player::Set_Animation(_uint iAnimIndex, _bool isLoop, _float fAnimSpeed, _float fChangeDuration, _uint iStartNumKeyFrames)
{
    m_pModelCom->Set_Animation(iAnimIndex, isLoop, fAnimSpeed, fChangeDuration, iStartNumKeyFrames);
}

void CBody_Player::Set_Animation_TransModel(_uint iAnimIndex, _bool isLoop, _float fAnimSpeed, _float fChangeDuration, _uint iStartNumKeyFrames)
{
    m_pTransModelCom->Set_Animation(iAnimIndex, isLoop, fAnimSpeed, fChangeDuration, iStartNumKeyFrames);
}

void CBody_Player::ChangeModel(_bool isToUpgrade)
{
    m_isTransformed = isToUpgrade;
}

void CBody_Player::PlayAllModel(_bool isUseAll)
{
    m_isUseAll = isUseAll;
}

HRESULT CBody_Player::Add_Component()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_Travis"),
        TEXT("Com_Model"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pModelCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Player_Travis_Armor"),
        TEXT("Com_Model_Trans"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pTransModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Player::Bind_ShaderResources()
{
    _float4x4		 ViewMatrix, ProjMatrix;

    ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_VIEW);
    ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeline::D3DTS_PROJ);

    _float4         CamPos;
    CamPos = m_pGameInstance->Get_CamPosition();

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
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

shared_ptr<CBody_Player> CBody_Player::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    shared_ptr<CBody_Player> pInstance = make_shared<CBody_Player>(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CBody_Player");
        pInstance.reset();
    }

    return pInstance;
}

void CBody_Player::Free()
{
}
