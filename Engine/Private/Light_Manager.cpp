#include "Light_Manager.h"
#include "Light.h"

CLight_Manager::CLight_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice(pDevice)
    , m_pContext(pContext)
{
}

const LIGHT_DESC* CLight_Manager::Get_LightDesc(_uint iIndex) const
{
    auto iter = m_Lights.begin();

    for (size_t i = 0; i < iIndex; i++)
        ++iter;

    return (*iter)->Get_LightDesc();
}

HRESULT CLight_Manager::Initialize()
{
    return S_OK;
}

HRESULT CLight_Manager::Add_Light(const LIGHT_DESC& LightDesc)
{
    shared_ptr<CLight> pLight = CLight::Create(m_pDevice, m_pContext, LightDesc);

    if (nullptr == pLight)
        return E_FAIL;

    m_Lights.push_back(pLight);

    return S_OK;
}

HRESULT CLight_Manager::Render(shared_ptr<class CShader> pShader, shared_ptr<class CVIBuffer_Rect> pVIBuffer)
{
    for (auto& pLight : m_Lights)
        pLight->Render(pShader, pVIBuffer);

    return S_OK;
}

shared_ptr<CLight_Manager> CLight_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    shared_ptr<CLight_Manager> pInstance = make_shared<CLight_Manager>(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLight_Manager");
        pInstance.reset();
    }

    return pInstance;
}

void CLight_Manager::Free()
{
    for (auto& pLight : m_Lights)
        pLight->Free();

    m_Lights.clear();

    m_pContext.Reset();
    m_pDevice.Reset();
}
