#include "pch.h"
#include "StaticObject.h"

#include "GameInstance.h"

CStaticObject::CStaticObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag, const LEVEL& eLevel)
    : CGameObject(pDevice, pContext)
    , m_strModelComTag(strModelComTag)
    , m_eLevel(eLevel)
{
}

CStaticObject::CStaticObject(const CStaticObject& rhs)
    : CGameObject(rhs)
{
}

HRESULT CStaticObject::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

void CStaticObject::Priority_Tick(_float fTimeDelta)
{
}

void CStaticObject::Tick(_float fTimeDelta)
{
}

void CStaticObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CStaticObject::Render()
{
    _vector vTest = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    // 셰이더에 던지는 루프를 클라에서 돌리려면 메시가 몇 개 있는지 알아야 한다.
    // 셰이더에 값 던지는 기능을 클라에서 선택적으로 가능
    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType::aiTextureType_DIFFUSE, "g_DiffuseTexture")))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CStaticObject::Add_Component()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(m_eLevel, m_strModelComTag,
        TEXT("Com_Model"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CStaticObject::Bind_ShaderResources()
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

void CStaticObject::Free()
{
    __super::Free();
}
