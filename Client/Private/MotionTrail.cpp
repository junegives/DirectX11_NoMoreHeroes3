#include "pch.h"
#include "MotionTrail.h"

CMotionTrail::CMotionTrail(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CGameObject(pDevice, pContext)
{
}

CMotionTrail::CMotionTrail(const CMotionTrail& rhs)
    : CGameObject(rhs)
{
}

HRESULT CMotionTrail::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

void CMotionTrail::Tick(_float fTimeDelta)
{
    //m_MotionTrailList.fLifeTime -= fTimeDelta;

    //if (m_TrailDesc.fLifeTime < 0.f)
    //{
    //    
    //}
}

void CMotionTrail::Late_Tick(_float fTimeDelta)
{
}

HRESULT CMotionTrail::Render()
{
    return E_NOTIMPL;
}

HRESULT CMotionTrail::Add_Component()
{
    return E_NOTIMPL;
}

HRESULT CMotionTrail::Bind_ShaderResources()
{
    return E_NOTIMPL;
}

shared_ptr<CMotionTrail> CMotionTrail::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    return shared_ptr<CMotionTrail>();
}

void CMotionTrail::Free()
{
}
