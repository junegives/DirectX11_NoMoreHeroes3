#include "Component_Manager.h"

CComponent_Manager::CComponent_Manager()
{
}

HRESULT CComponent_Manager::Initialize(_uint iNumLevels)
{
    m_iNumLevels = iNumLevels;

    m_pPrototypes = new PROTOTYPES[iNumLevels];
    m_pComponents  = new PROTOTYPES[iNumLevels];

    return S_OK;
}

HRESULT CComponent_Manager::Add_Component(_uint iLevelIndex, const wstring& strComponentTag, shared_ptr<class CComponent> pComponent)
{
    if (nullptr != Find_Component(iLevelIndex, strComponentTag))
        return E_FAIL;

    m_pComponents[iLevelIndex].emplace(strComponentTag, pComponent);

    return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, shared_ptr<class CComponent> pPrototype)
{
    if (nullptr != Find_Prototype(iLevelIndex, strPrototypeTag))
        return E_FAIL;

    m_pPrototypes[iLevelIndex].emplace(strPrototypeTag, pPrototype);

    return S_OK;
}

HRESULT CComponent_Manager::Remove_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag)
{
    if (iLevelIndex >= m_iNumLevels)
        return E_FAIL;

    auto iter = m_pPrototypes[iLevelIndex].find(strPrototypeTag);
    if (iter != m_pPrototypes[iLevelIndex].end()) {
        m_pPrototypes[iLevelIndex].erase(iter);
    }

    return S_OK;
}

shared_ptr<class CComponent> CComponent_Manager::Find_Component(_uint iLevelIndex, const wstring& strComponentTag)
{
    if (iLevelIndex >= m_iNumLevels)
        return nullptr;

    auto	iter = m_pComponents[iLevelIndex].find(strComponentTag);
    if (iter == m_pComponents[iLevelIndex].end())
        return nullptr;

    return iter->second;
}

shared_ptr<class CComponent> CComponent_Manager::Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg)
{
    shared_ptr<CComponent> pPrototype = Find_Prototype(iLevelIndex, strPrototypeTag);
    if (nullptr == pPrototype)
        return nullptr;

    shared_ptr<CComponent> pComponent = pPrototype->Clone(pArg);
    if (nullptr == pComponent)
        return nullptr;

    return pComponent;
}

void CComponent_Manager::Clear(_uint iLevelIndex)
{
    m_pPrototypes[iLevelIndex].clear();
    m_pComponents[iLevelIndex].clear();
}

shared_ptr<class CComponent> CComponent_Manager::Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag)
{
    if (iLevelIndex >= m_iNumLevels)
        return nullptr;

    auto	iter = m_pPrototypes[iLevelIndex].find(strPrototypeTag);
    if (iter == m_pPrototypes[iLevelIndex].end())
        return nullptr;

    return iter->second;
}

shared_ptr<CComponent_Manager> CComponent_Manager::Create(_uint iNumLevels)
{
    shared_ptr<CComponent_Manager> pInstance = make_shared<CComponent_Manager>();

    if (FAILED(pInstance->Initialize(iNumLevels)))
    {
        MSG_BOX("Failed to Created : CComponent_Manager");
        pInstance.reset();
    }

    return pInstance;
}

void CComponent_Manager::Free()
{
    for (size_t i = 0; i < m_iNumLevels; i++)
    {
        m_pPrototypes[i].clear();
        m_pComponents[i].clear();
    }

    Safe_Delete_Array(m_pPrototypes);
    Safe_Delete_Array(m_pComponents);
}
