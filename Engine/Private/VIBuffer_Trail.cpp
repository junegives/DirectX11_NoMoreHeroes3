#include "VIBuffer_Trail.h"


CVIBuffer_Trail::CVIBuffer_Trail(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iVtxCount)
    : CVIBuffer(pDevice, pContext)
{
    m_iVtxCount = iVtxCount;
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail& rhs)
    : CVIBuffer(rhs)
    , m_iVtxCount(rhs.m_iVtxCount)
{

}

HRESULT CVIBuffer_Trail::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CVIBuffer_Trail::Initialize(void* pArg)
{
    if (pArg == nullptr)
        return E_FAIL;

#pragma region VERTEXBUFFER

    m_iNumVertexBuffers = 1;
    m_iNumVertices = 10000;
    m_iVertexStride = sizeof(VTXPOSTEX);

    m_BufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = m_iVertexStride;

    VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iNumVertices);


    for (_uint i = 0; i < 10000; i += 2)
    {
        pVertices[i].vPosition = _float3(0.f, 0.f, 0.f);
        pVertices[i].vTexcoord = _float2(0.f, 0.f);

        pVertices[i + 1].vPosition = _float3(0.f, 0.f, 0.f);
        pVertices[i + 1].vTexcoord = _float2(0.f, 0.f);
    }

    m_InitialData.pSysMem = pVertices;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEXBUFFER
    m_iIndexStride = sizeof(TRIINDICES16);
    m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
    m_iNumIndices = m_iNumVertices - 2;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = 0;

    TRIINDICES16* pIndices = new TRIINDICES16[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(TRIINDICES16) * m_iNumIndices);

    for (_uint i = 0; i < m_iNumIndices; i += 6)
    {
        pIndices[i]._0 = i + 3;
        pIndices[i]._1 = i + 1;
        pIndices[i]._2 = i;

        pIndices[i + 1]._0 = i + 2;
        pIndices[i + 1]._1 = i + 3;
        pIndices[i + 1]._2 = i;
    }

    m_InitialData.pSysMem = pIndices;

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);

#pragma endregion


    return S_OK;
}

void CVIBuffer_Trail::Update_TrailBuffer(_float fTimedelta, _matrix TransformMatrix)
{

    vector<VTXPOSTEX> Vertices;
    Vertices.reserve(m_iNumVertices);

    TransformMatrix.r[0] = XMVector4Normalize(TransformMatrix.r[0]);
    TransformMatrix.r[1] = XMVector4Normalize(TransformMatrix.r[1]);
    TransformMatrix.r[2] = XMVector4Normalize(TransformMatrix.r[2]);

    _vector vWorldHigh = XMVector3TransformCoord(XMLoadFloat4(&m_vHighPosition), TransformMatrix);
    _vector vWorldLow = XMVector3TransformCoord(XMLoadFloat4(&m_vLowPosition), TransformMatrix);


    while (m_TrailVertices.size() < m_iVtxCount)
    {
        VTXPOSTEX Vertex = {};
        XMStoreFloat3(&Vertex.vPosition, vWorldHigh);
        m_TrailVertices.push_back(Vertex);

        XMStoreFloat3(&Vertex.vPosition, vWorldLow);
        m_TrailVertices.push_back(Vertex);
    }


    while (m_iVtxCount - 2 < m_TrailVertices.size())
        m_TrailVertices.pop_front();

    VTXPOSTEX Vertex = {};
    XMStoreFloat3(&Vertex.vPosition, vWorldHigh);
    m_TrailVertices.push_back(Vertex);

    XMStoreFloat3(&Vertex.vPosition, vWorldLow);
    m_TrailVertices.push_back(Vertex);



    for (_uint i = 0; i < m_TrailVertices.size(); i++)
    {
        _float _iVtxCount = _float(m_TrailVertices.size());

        if (i % 2 == 0)
            m_TrailVertices[i].vTexcoord = { ((_float)i) / ((_float)_iVtxCount), 0.f };
        else
            m_TrailVertices[i].vTexcoord = { ((_float)i) / ((_float)_iVtxCount), 1.f };
        Vertices.push_back(m_TrailVertices[i]);
    }

    vector<VTXPOSTEX> CatmallomVertices;
    CatmallomVertices.reserve((m_iVtxCount - 2) * 10);
    for (_uint i = 0; i < Vertices.size(); i += 2)
    {
        if (i >= Vertices.size() - 4)
        {
            CatmallomVertices.push_back(Vertices[i]);
            CatmallomVertices.push_back(Vertices[i + 1]);
            continue;
        }

        VTXPOSTEX vHigh0, vHigh1, vHigh2, vHigh3;
        VTXPOSTEX vLow0, vLow1, vLow2, vLow3;

        if (i == 0)
        {
            vHigh0 = Vertices[i];
            vLow0 = Vertices[i + 1];
        }
        else
        {
            vHigh0 = Vertices[i - 2];
            vLow0 = Vertices[i - 1];
        }

        vHigh1 = Vertices[i];
        vHigh2 = Vertices[i + 2];
        vHigh3 = Vertices[i + 4];


        vLow1 = Vertices[i + 1];
        vLow2 = Vertices[i + 3];
        vLow3 = Vertices[i + 5];

        for (_uint j = 0; j < 10; ++j)
        {
            VTXPOSTEX VertexHigh = {};
            XMStoreFloat3(&VertexHigh.vPosition, XMVectorCatmullRom(
                XMLoadFloat3(&vHigh0.vPosition)
                , XMLoadFloat3(&vHigh1.vPosition)
                , XMLoadFloat3(&vHigh2.vPosition)
                , XMLoadFloat3(&vHigh3.vPosition)
                , j * 0.1f));

            XMStoreFloat2(&VertexHigh.vTexcoord, XMVectorCatmullRom(
                XMLoadFloat2(&vHigh0.vTexcoord)
                , XMLoadFloat2(&vHigh1.vTexcoord)
                , XMLoadFloat2(&vHigh2.vTexcoord)
                , XMLoadFloat2(&vHigh3.vTexcoord)
                , j * 0.1f));

            CatmallomVertices.push_back(VertexHigh);


            VTXPOSTEX VertexLow = {};
            XMStoreFloat3(&VertexLow.vPosition, XMVectorCatmullRom(
                XMLoadFloat3(&vLow0.vPosition)
                , XMLoadFloat3(&vLow1.vPosition)
                , XMLoadFloat3(&vLow2.vPosition)
                , XMLoadFloat3(&vLow3.vPosition)
                , j * 0.1f));

            XMStoreFloat2(&VertexLow.vTexcoord, XMVectorCatmullRom(
                XMLoadFloat2(&vLow0.vTexcoord)
                , XMLoadFloat2(&vLow1.vTexcoord)
                , XMLoadFloat2(&vLow2.vTexcoord)
                , XMLoadFloat2(&vLow3.vTexcoord)
                , j * 0.1f));

            CatmallomVertices.push_back(VertexLow);
        }

    }

    m_iNumIndices = CatmallomVertices.size() - 2;

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    m_pContext->Map(m_pVB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy(mappedResource.pData, CatmallomVertices.data(), sizeof(VTXPOSTEX) * CatmallomVertices.size());
    m_pContext->Unmap(m_pVB.Get(), 0);
}

HRESULT CVIBuffer_Trail::Render()
{
    __super::Render();

    return S_OK;
}

void CVIBuffer_Trail::SetPos(_vector vHighPosition, _vector vLowPosition)
{
    XMStoreFloat4(&m_vHighPosition, vHighPosition);
    XMStoreFloat4(&m_vLowPosition, vLowPosition);

}

void CVIBuffer_Trail::StartTrail(_matrix TransformMatrix)
{
    m_TrailVertices.clear();
    for (_uint i = 0; i < m_iNumVertices; i += 2)
    {
        VTXPOSTEX High = {};
        XMStoreFloat3(&High.vPosition, XMVector3TransformCoord(XMLoadFloat4(&m_vHighPosition), TransformMatrix));
        VTXPOSTEX Low = {};
        XMStoreFloat3(&Low.vPosition, XMVector3TransformCoord(XMLoadFloat4(&m_vLowPosition), TransformMatrix));

        m_TrailVertices.push_back(High);
        m_TrailVertices.push_back(Low);
    }
}

void CVIBuffer_Trail::StopTrail()
{
    m_TrailVertices.clear();
}

shared_ptr<CVIBuffer_Trail> CVIBuffer_Trail::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iVtxCount)
{
    shared_ptr<CVIBuffer_Trail> pInstance = make_shared<CVIBuffer_Trail>(pDevice, pContext, iVtxCount);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Trail");
        pInstance.reset();
    }

    return pInstance;
}

shared_ptr<CComponent> CVIBuffer_Trail::Clone(void* pArg)
{
    shared_ptr<CVIBuffer_Trail> pInstance = make_shared<CVIBuffer_Trail>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CVIBuffer_Trail");
        pInstance.reset();
    }

    return pInstance;
}

void CVIBuffer_Trail::Free()
{
    __super::Free();
}