#include "Texture.h"
#include "Shader.h"

CTexture::CTexture(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CComponent(pDevice, pContext)
{
}

CTexture::CTexture(const CTexture& rhs)
    : CComponent(rhs)
    , m_SRVs(rhs.m_SRVs)
    , m_iNumTextures(rhs.m_iNumTextures)
{
}

HRESULT CTexture::Initialize_Prototype(const wstring& strTextureFilePath, _uint iNumTextures)
{
    m_iNumTextures = iNumTextures;

    _tchar		szEXT[MAX_PATH] = TEXT("");

    _wsplitpath_s(strTextureFilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);


    for (size_t i = 0; i < iNumTextures; i++)
    {
        _tchar		szTextureFilePath[MAX_PATH] = TEXT("");

        wsprintf(szTextureFilePath, strTextureFilePath.c_str(), i);

        HRESULT			hr = { 0 };

        ComPtr<ID3D11ShaderResourceView> pSRV = { nullptr };

        if (false == lstrcmp(szEXT, TEXT(".dds")))
            hr = CreateDDSTextureFromFile(m_pDevice.Get(), szTextureFilePath, nullptr, &pSRV);

        else if (false == lstrcmp(szEXT, TEXT(".tga")))
            hr = E_FAIL;

        else
            hr = CreateWICTextureFromFile(m_pDevice.Get(), szTextureFilePath, nullptr, &pSRV);

        if (FAILED(hr))
            return E_FAIL;

        m_SRVs.push_back(pSRV);
    }

    return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(shared_ptr<class CShader> pShader, const _char* pConstantName, _uint iIndex)
{
    if (iIndex >= m_iNumTextures)
        return E_FAIL;

    return pShader->Bind_SRV(pConstantName, m_SRVs[iIndex]);
}

HRESULT CTexture::Bind_ShaderResources(shared_ptr<class CShader> pShader, const _char* pConstantName)
{
    return pShader->Bind_SRVs(pConstantName, m_SRVs[0].GetAddressOf(), m_iNumTextures);
}

shared_ptr<CTexture> CTexture::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strTextureFilePath, _uint iNumTextures)
{
    shared_ptr<CTexture> pInstance = make_shared<CTexture>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(strTextureFilePath, iNumTextures)))
    {
        MSG_BOX("Failed to Created : CTexture");
        pInstance.reset();
    }

    return pInstance;
}

shared_ptr<CComponent> CTexture::Clone(void* pArg)
{
    shared_ptr<CTexture> pInstance = make_shared<CTexture>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CShader");
        pInstance.reset();
    }

    return pInstance;
}

void CTexture::Free()
{
    m_SRVs.clear();
}
