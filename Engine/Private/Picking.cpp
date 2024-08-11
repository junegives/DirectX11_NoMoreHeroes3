#include "Picking.h"
#include "GameInstance.h"

CPicking::CPicking(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice(pDevice)
    , m_pContext(pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT CPicking::Initialize()
{
    return S_OK;
}

void CPicking::RayToWorld(_long iScreenX, _long iScreenY)
{
    _matrix matProj = m_pGameInstance->Get_Transform_Matrix(CPipeline::D3DTS_PROJ);

    D3D11_VIEWPORT Viewport;

    _uint iNumViewPorts = 1;

    m_pContext->RSGetViewports(&iNumViewPorts, &Viewport);

    // 스크린 좌표 View Space로 내리기
    _float fViewX = (2.f * iScreenX / Viewport.Width - 1.f) / matProj.r[0].m128_f32[0];
    _float fViewY = (-2.f * iScreenY / Viewport.Height + 1.f) / matProj.r[1].m128_f32[1];

    // View Space의 Ray 생성
    _vector vRayOrigin = XMVectorSet(0.f, 0.f, 0.f, 1.f);
    _vector vRayDir = XMVectorSet(fViewX, fViewY, 1.f, 0.f);

    // Ray를 World Space로 내리기
    _matrix matViewInv = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeline::D3DTS_VIEW);
    vRayOrigin = XMVector3TransformCoord(vRayOrigin, matViewInv);
    vRayDir = XMVector3TransformNormal(vRayDir, matViewInv);

    // 교차 판정을 위해 Ray의 방향 벡터를 단위 길이로
    vRayDir = XMVector3Normalize(vRayDir);

    XMStoreFloat3(&m_vRayPosWorld, vRayOrigin);
    XMStoreFloat3(&m_vRayDirWorld, vRayDir);

    m_fMinDist = -1.f;
}

void CPicking::RayToLocal(_matrix matWorldInv)
{
    // Ray를 Local Space로 내리기
    _vector vRayPosLocal = XMVector3TransformCoord(XMLoadFloat3(&m_vRayPosWorld), matWorldInv);
    _vector vRayDirLocal = XMVector3TransformNormal(XMLoadFloat3(&m_vRayDirWorld), matWorldInv);

    XMStoreFloat3(&m_vRayPosLocal, vRayPosLocal);
    XMStoreFloat3(&m_vRayDirLocal, vRayDirLocal);
}

_bool CPicking::IntersectWorld(_float3 vPointA, _float3 vPointB, _float3 vPointC, _float3* pOut, _float* fMinDist)
{
    //Intersects(_In_ FXMVECTOR Origin, _In_ FXMVECTOR Direction, _In_ FXMVECTOR V0, _In_ GXMVECTOR V1, _In_ HXMVECTOR V2, _Out_ float& Dist)
    if (true == TriangleTests::Intersects(XMLoadFloat3(&m_vRayPosWorld), XMLoadFloat3(&m_vRayDirWorld), XMLoadFloat3(&vPointA), XMLoadFloat3(&vPointB), XMLoadFloat3(&vPointC), *fMinDist))
    {
        XMStoreFloat3(pOut, XMLoadFloat3(&m_vRayPosWorld) + XMLoadFloat3(&m_vRayDirWorld) * (*fMinDist));

        return true;
    }

    return false;
}

_bool CPicking::IntersectLocal(_float3 vPointA, _float3 vPointB, _float3 vPointC, _float3* pOut)
{
    _float fDist;

    _vector vDir = XMLoadFloat3(&m_vRayDirLocal);

    if (true == TriangleTests::Intersects(XMLoadFloat3(&m_vRayPosLocal), vDir, XMLoadFloat3(&vPointA), XMLoadFloat3(&vPointB), XMLoadFloat3(&vPointC), fDist))
    {
        XMStoreFloat3(pOut, XMLoadFloat3(&m_vRayPosLocal) + XMLoadFloat3(&m_vRayDirLocal) * fDist);

        return true;
    }

    return false;
}

shared_ptr<CPicking> CPicking::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const GRAPHIC_DESC& GraphicDesc)
{
    shared_ptr<CPicking> pInstance = make_shared<CPicking>(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CPicking_Manager");
        pInstance.reset();
    }

    return pInstance;
}

void CPicking::Free()
{
}
