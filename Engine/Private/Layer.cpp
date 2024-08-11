#include "Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Initialize()
{
    return S_OK;
}

list<shared_ptr<CGameObject>> CLayer::Get_AllObjects()
{
	return m_GameObjects;
}

HRESULT CLayer::Add_GameObject(shared_ptr<class CGameObject> pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}

shared_ptr<class CGameObject> CLayer::Find_GameObject(const wstring& strObjectTag)
{
	auto iter = find_if(m_GameObjects.begin(), m_GameObjects.end(), [&](shared_ptr<CGameObject> pGameObject) {
		return strObjectTag == pGameObject->Get_ObjectTag();
	});

	if (iter == m_GameObjects.end())
		return nullptr;

	return *iter;
}

HRESULT CLayer::Remove_Object(const wstring& strObjectTag)
{
	auto iter = find_if(m_GameObjects.begin(), m_GameObjects.end(), [&](shared_ptr<CGameObject> pGameObject) {
		return strObjectTag == pGameObject->Get_ObjectTag();
		});

	if (iter == m_GameObjects.end())
		return E_FAIL;

	m_GameObjects.erase(iter);

	return S_OK;
}

HRESULT CLayer::Remove_Object(shared_ptr<class CGameObject> pGameObject)
{
	auto iter = find_if(m_GameObjects.begin(), m_GameObjects.end(), [&](shared_ptr<CGameObject> pFindGameObject) {
		return pGameObject == pFindGameObject;
		});

	if (iter == m_GameObjects.end())
		return E_FAIL;

	m_GameObjects.erase(iter);

	return S_OK;
}

shared_ptr<class CComponent> CLayer::Find_Component(const wstring& strComTag, _uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	return (*iter)->Find_Component(strComTag);
}

void CLayer::Priority_Tick(_float fTimeDelta)
{
	for (auto& iter : m_GameObjects) {
		if (!iter->IsActive())
			continue;
		iter->Priority_Tick(fTimeDelta);
	}
}

void CLayer::Tick(_float fTimeDelta)
{
	for (auto& iter : m_GameObjects) {
		if (!iter->IsActive())
			continue;
		iter->Tick(fTimeDelta);
	}
}

void CLayer::Late_Tick(_float fTimeDelta)
{
	for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end(); )
	{
		if (!(*iter)->IsActive())
		{
			++iter;
			continue;
		}

		(*iter)->Late_Tick(fTimeDelta);
		if (!(*iter)->IsAlive())
		{
			iter = m_GameObjects.erase(iter);
		}
		else
			++iter;
	}
}

HRESULT CLayer::Clear()
{
	m_GameObjects.clear();

	return S_OK;
}

shared_ptr<CLayer> CLayer::Create()
{
	shared_ptr<CLayer> pInstance = make_shared<CLayer>();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLayer");
		pInstance.reset();
	}

	return pInstance;
}

void CLayer::Free()
{
	m_GameObjects.clear();
}
