#include "Transform.h"
#include "Shader.h"

#include "Navigation.h"

CTransform::CTransform(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CComponent(pDevice, pContext)
{
}

void CTransform::Set_Scale(_float fX, _float fY, _float fZ)
{
    Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * fX);
    Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * fY);
    Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * fZ);
}

HRESULT CTransform::Initialize_Prototype()
{
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

    return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return S_OK;

    TRANSFORM_DESC* pTransformDesc = (TRANSFORM_DESC*)pArg;

    m_fSpeedPerSec = pTransformDesc->fSpeedPerSec;
    m_fRotationPerSec = pTransformDesc->fRotationPerSec;

    return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(shared_ptr<class CShader> pShader, const _char* pContantName)
{
    return pShader->Bind_Matrix(pContantName, &m_WorldMatrix);
}

void CTransform::Go_Straight(_float fTimeDelta, shared_ptr<CNavigation> pNavigation, _float fSpeedWeight)
{
    _vector     vOriginPos = Get_State(STATE_POSITION);
    _vector     vLook = Get_State(STATE_LOOK);

    _vector vPos = vOriginPos + XMVector3Normalize(vLook) * m_fSpeedPerSec * fSpeedWeight * fTimeDelta;

    if (nullptr == pNavigation ||
        true == pNavigation->isMove(vPos))
    {
        Set_State(STATE_POSITION, vPos);
    }
    else if (false == pNavigation->isMove(vPos))
    {
        _vector vSlide = pNavigation->CalcSlideVec(vPos - vOriginPos);

        vPos = vOriginPos + vSlide;

        if (false == pNavigation->isMove(vPos))
        {
            return;
        }

        Set_State(STATE_POSITION, vPos);
    }
}

void CTransform::Go_Backward(_float fTimeDelta, shared_ptr<CNavigation> pNavigation, _float fSpeedWeight)
{
    _vector     vPosition = Get_State(STATE_POSITION);
    _vector     vLook = Get_State(STATE_LOOK);

    vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fSpeedWeight * fTimeDelta;

    if (nullptr == pNavigation ||
        true == pNavigation->isMove(vPosition))
    Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta, shared_ptr<CNavigation> pNavigation, _float fSpeedWeight)
{
    _vector     vPosition = Get_State(STATE_POSITION);
    _vector     vRight = Get_State(STATE_RIGHT);

    vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fSpeedWeight * fTimeDelta;

    if (nullptr == pNavigation ||
        true == pNavigation->isMove(vPosition))
    Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta, shared_ptr<CNavigation> pNavigation, _float fSpeedWeight)
{
    _vector     vPosition = Get_State(STATE_POSITION);
    _vector     vRight = Get_State(STATE_RIGHT);

    vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fSpeedWeight * fTimeDelta;

    if (nullptr == pNavigation ||
        true == pNavigation->isMove(vPosition))
    Set_State(STATE_POSITION, vPosition);
}

_bool CTransform::Go_Dir_Slide(_float fTimeDelta, _vector vDir, shared_ptr<class CNavigation> pNavigation, _float fSpeedWeight)
{
    _vector     vOriginPos = Get_State(STATE_POSITION);

    _vector vPos = vOriginPos + XMVector3Normalize(vDir) * m_fSpeedPerSec * fSpeedWeight * fTimeDelta;

    if (nullptr == pNavigation)
        return false;

    if (true == pNavigation->isMove(vPos))
    {
        Set_State(STATE_POSITION, vPos);
    }
    else
    {
        _vector vSlide = pNavigation->CalcSlideVec(vPos - vOriginPos);

        vPos = vOriginPos + vSlide;

        if (false == pNavigation->isMove(vPos))
        {
            return false;
        }

        Set_State(STATE_POSITION, vPos);
    }
}

void CTransform::Go_Straight_Slide(_float fTimeDelta, shared_ptr<class CNavigation> pNavigation, _float fSpeedWeight)
{
    _vector     vOriginPos = Get_State(STATE_POSITION);
    _vector     vLook = Get_State(STATE_LOOK);

    _vector vPos = vOriginPos + XMVector3Normalize(vLook) * m_fSpeedPerSec * fSpeedWeight * fTimeDelta;

    if (nullptr == pNavigation)
        return;

    if (true == pNavigation->isMove(vPos))
    {
        Set_State(STATE_POSITION, vPos);
    }
    else
    {
        _vector vSlide = pNavigation->CalcSlideVec(vPos - vOriginPos);

        vPos = vOriginPos + vSlide;

        if (false == pNavigation->isMove(vPos))
        {
            return;
        }

        Set_State(STATE_POSITION, vPos);
    }
}

_bool CTransform::Go_Straight_Collide(_float fTimeDelta, shared_ptr<class CNavigation> pNavigation, _float fSpeedWeight)
{
    _vector     vOriginPos = Get_State(STATE_POSITION);
    _vector     vLook = Get_State(STATE_LOOK);

    _vector vPos = vOriginPos + XMVector3Normalize(vLook) * m_fSpeedPerSec * fSpeedWeight * fTimeDelta;

    if (!pNavigation)
    {
        Set_State(STATE_POSITION, vPos);
        return true;
    }

    if (true == pNavigation->isMove(vPos))
    {
        Set_State(STATE_POSITION, vPos);
        return true;
    }

    return false;
}

void CTransform::Go_StraightXZ(_float fTimeDelta)
{
    _vector     vPosition = Get_State(STATE_POSITION);
    _vector     vLook = Get_State(STATE_LOOK);

    vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;
    vPosition.m128_f32[1] -= XMVector3Normalize(vLook).m128_f32[1] * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_BackwardXZ(_float fTimeDelta)
{
    _vector     vPosition = Get_State(STATE_POSITION);
    _vector     vLook = Get_State(STATE_LOOK);

    vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;
    vPosition.m128_f32[1] += XMVector3Normalize(vLook).m128_f32[1] * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_LeftXZ(_float fTimeDelta)
{
    _vector     vPosition = Get_State(STATE_POSITION);
    _vector     vRight = Get_State(STATE_RIGHT);

    vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;
    vPosition.m128_f32[1] += XMVector3Normalize(vRight).m128_f32[1] * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_RightXZ(_float fTimeDelta)
{
    _vector     vPosition = Get_State(STATE_POSITION);
    _vector     vRight = Get_State(STATE_RIGHT);

    vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;
    vPosition.m128_f32[1] -= XMVector3Normalize(vRight).m128_f32[1] * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE_POSITION, vPosition);
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
    _matrix     RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

    for (size_t i = STATE_RIGHT; i < STATE_POSITION; i++)
    {
        _vector     vStateDir = Get_State(STATE(i));

        Set_State(STATE(i), XMVector4Transform(vStateDir, RotationMatrix));
    }
}

void CTransform::TurnRadian(_fvector vAxis, _float fRadian)
{
    _matrix     RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

    for (size_t i = STATE_RIGHT; i < STATE_POSITION; i++)
    {
        _vector     vStateDir = Get_State(STATE(i));

        Set_State(STATE(i), XMVector4Transform(vStateDir, RotationMatrix));
    }
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
    _matrix     RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);
    _float3     vScale = Get_Scale();

    for (size_t i = STATE_RIGHT; i < STATE_POSITION; i++)
    {
        _float4     vTmp = _float4(0.f, 0.f, 0.f, 0.f);

        *((_float*)&vTmp + i) = 1.f * *((_float*)&vScale + i);

        _vector     vStateDir = XMLoadFloat4(&vTmp);

        Set_State(STATE(i), XMVector4Transform(vStateDir, RotationMatrix));
    }

}

_bool CTransform::MoveTo(_fvector vPoint, _float fLimit, _float fTimeDelta)
{
    _vector vPosition = Get_State(STATE_POSITION);
    _vector vDir = vPoint - vPosition;

    _float fDistance = XMVector3Length(vDir).m128_f32[0];

    if (fDistance > fLimit)
        vPosition += XMVector3Normalize(vDir) * m_fSpeedPerSec * fTimeDelta;
    else return false;

    Set_State(CTransform::STATE_POSITION, vPosition);

    return true;
}

_bool CTransform::MoveToCheckNavi(_fvector vPoint, shared_ptr<class CNavigation> pNavigation, _float fLimit, _float fTimeDelta)
{
    _vector vOriginPos = Get_State(STATE_POSITION);
    //_vector vDir = vPoint - vOriginPos;

    //_float fDistance = XMVector3Length(vDir).m128_f32[0];

    //if (fDistance < fLimit)
    //    return false;

    //_vector vPosition = vOriginPos + XMVector3Normalize(vDir) * m_fSpeedPerSec * fTimeDelta;

    //if (true == pNavigation->isMove(vPosition))
    //{
    //    Set_State(STATE_POSITION, vPosition);

    //    return true;
    //}
    //else
    //{
    //    _vector vSlide = pNavigation->CalcSlideVec(vPosition - vOriginPos);

    //    vPosition = vOriginPos + vSlide;

    //    if (false == pNavigation->isMove(vPosition))
    //    {
    //        return false;
    //    }

    //    Set_State(STATE_POSITION, vPosition);

    //    return true;
    //}

    _vector vPosition;

    if (true == pNavigation->isMove(vPoint))
    {
        Set_State(STATE_POSITION, vPoint);

        return true;
    }
    else
    {
        _vector vSlide = pNavigation->CalcSlideVec(vPoint - vOriginPos);

        vPosition = vOriginPos + vSlide;

        if (false == pNavigation->isMove(vPosition))
        {
            return false;
        }

        Set_State(STATE_POSITION, vPosition);

        return true;
    }
}

void CTransform::LookAt(_fvector vPoint)
{
    _float3 vScale = Get_Scale();

    _vector vPosition = Get_State(STATE_POSITION);
    _vector vLook = vPoint - vPosition;

    _vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
    _vector vUp = XMVector3Cross(vLook, vRight);

    Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
    Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
    Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::UpDir(_fvector vDir, _fvector _vLook)
{
    _float3 vScale = Get_Scale();

    _vector vUp = XMVector3Normalize(vDir);
    _vector vLook = XMVector3Normalize(_vLook);
    _vector vRight = XMVector3Cross(vUp, vLook);

    Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
    Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
    Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::LookAt_ForLandObject(_fvector vPoint)
{
    _float3 vScale = Get_Scale();

    _vector vPosition = Get_State(STATE_POSITION);
    //_vector vLook = vPoint - vPosition;

    // Lerp 넣기!!!!
    _vector vLook = XMVectorLerp(Get_State(CTransform::STATE_LOOK), vPoint - vPosition, 0.2);
    _vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
    vLook = XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f));

    Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
    Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::SetLook(_fvector _vLook)
{
    _float3     vScale = Get_Scale();

    _vector		vScaled = XMLoadFloat3(&vScale);

    _vector		vPosition = Get_State(STATE_POSITION);

    // Lerp 넣기!!!!
    /*_vector     vLook = XMVectorLerp(Get_State(CTransform::STATE_LOOK), _vLook, 0.3);
    _vector		vRight = XMVector3Normalize(XMVector3Cross(_vector{ 0.f, 1.f, 0.f, 0.f }, vLook));
    _vector		vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));*/

    _vector		vLook = XMVector3Normalize(_vLook) * vScaled.m128_f32[2];
    _vector		vRight = XMVector3Normalize(XMVector3Cross(_vector{ 0.f, 1.f, 0.f, 0.f }, vLook)) * vScaled.m128_f32[0];
    _vector		vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * vScaled.m128_f32[1];

    Set_State(STATE_RIGHT, vRight);
    Set_State(STATE_UP, vUp);
    Set_State(STATE_LOOK, vLook);
}

void CTransform::SetLook_ForLandObject(_fvector _vLook)
{
    _float3     vScale = Get_Scale();

    _vector		vScaled = XMLoadFloat3(&vScale);

    _vector		vPosition = Get_State(STATE_POSITION);

    _vector		vLook   = XMVector3Normalize(_vLook) * vScaled.m128_f32[2];
    _vector		vRight  = XMVector3Normalize(XMVector3Cross(_vector{ 0.f, 1.f, 0.f, 0.f }, vLook)) * vScaled.m128_f32[0];
    vLook               = XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f));

    Set_State(STATE_RIGHT, vRight);
    Set_State(STATE_LOOK, vLook);
}

shared_ptr<CTransform> CTransform::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    shared_ptr<CTransform> pInstance = make_shared<CTransform>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CTransform");
        pInstance.reset();
    }

    return pInstance;
}

shared_ptr<CComponent> CTransform::Clone(void* pArg)
{
    return nullptr;
}

void CTransform::Free()
{
    __super::Free();
}
