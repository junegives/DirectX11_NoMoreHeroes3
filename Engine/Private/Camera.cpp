#include "Camera.h"
#include "GameInstance.h"

CCamera::CCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CGameObject(pDevice, pContext)
{
}

CCamera::CCamera(const CCamera& rhs)
    : CGameObject(rhs)
{
}

HRESULT CCamera::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    CAMERA_DESC* pCameraDesc = (CAMERA_DESC*)pArg;

    m_fFovy = pCameraDesc->fFovy;
    m_fNear = pCameraDesc->fNear;
    m_fFar = pCameraDesc->fFar;
    m_fAspect = pCameraDesc->fAspect;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pCameraDesc->vEye));
    m_pTransformCom->LookAt(XMLoadFloat4(&pCameraDesc->vAt));

    return S_OK;
}

void CCamera::Priority_Tick(_float fTimeDelta)
{
}

void CCamera::Tick(_float fTimeDelta)
{
}

void CCamera::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
    return S_OK;
}

HRESULT CCamera::SetUp_TransformMatrices()
{
    m_pGameInstance->Set_Transform(CPipeline::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
    m_pGameInstance->Set_Transform(CPipeline::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));

    return S_OK;
}

void CCamera::Free()
{
}
