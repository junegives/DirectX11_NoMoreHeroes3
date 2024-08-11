#pragma once

#include "VIBuffer.h"
#include <deque>

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Trail final : public CVIBuffer
{
public:
    CVIBuffer_Trail(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iVtxCount);
    CVIBuffer_Trail(const CVIBuffer_Trail& rhs);
    virtual ~CVIBuffer_Trail() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);

    void Update_TrailBuffer(_float fTimedelta, _matrix TransformMatrix);
    virtual HRESULT Render() override;


public:
    void SetPos(_vector vHighPosition, _vector vLowPosition);
    void StartTrail(_matrix TransformMatrix);
    void StopTrail();

public:
    void Set_VtxCount(_uint iCount) { m_iVtxCount = iCount; }

private:
    _float4 m_vHighPosition = { 0.f, .5f, 0.f, 1.f };
    _float4 m_vLowPosition = { 0.f, -.5f, 0.f, 1.f };


    deque<VTXPOSTEX> m_TrailVertices;

    _uint m_iVtxCount = 0;
    _uint m_iEndIndex = 0;
    _uint m_iCatmullRomCount = 5;
    _uint m_iCatmullRomIndex[4] = { 0, 0, 0, 0 };

public:
    static shared_ptr<CVIBuffer_Trail> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iVtxCount);
    virtual shared_ptr<CComponent> Clone(void* pArg);
    virtual void Free();
};

END