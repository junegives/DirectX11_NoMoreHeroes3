#include "Component.h"
#include "GameInstance.h"

CComponent::CComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice(pDevice)
    , m_pContext(pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
    , m_isCloned(false)
{
}

CComponent::CComponent(const CComponent& rhs)
    : m_pDevice(rhs.m_pDevice)
    , m_pContext(rhs.m_pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
    , m_isCloned(true)
{
}

HRESULT CComponent::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CComponent::Initialize(void* pArg)
{
    return S_OK;
}

void CComponent::Free()
{
}
