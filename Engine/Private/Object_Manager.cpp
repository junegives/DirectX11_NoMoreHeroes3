#include "Object_Manager.h"
#include "GameObject.h"
#include "Layer.h"

CObject_Manager::CObject_Manager()
{
}

HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_pLayers = new LAYERS[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

void CObject_Manager::Priority_Tick(_float fTimeDelta)
{
	// Static Object는 항상 돌려주고
	for (auto& Pair : m_pLayers[0])
		Pair.second->Priority_Tick(fTimeDelta);

	// 아니면 현재 Tick 돌릴 애들만 돌려주기
	if (m_eCurLevel != 0)
	{
		for (auto& Pair : m_pLayers[m_eCurLevel])
			Pair.second->Priority_Tick(fTimeDelta);
	}

	/*for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Priority_Tick(fTimeDelta);
	}*/
}

void CObject_Manager::Tick(_float fTimeDelta)
{
	// Static Object는 항상 돌려주고
	for (auto& Pair : m_pLayers[0])
		Pair.second->Tick(fTimeDelta);

	// 아니면 현재 Tick 돌릴 애들만 돌려주기
	if (m_eCurLevel != 0)
	{
		for (auto& Pair : m_pLayers[m_eCurLevel])
			Pair.second->Tick(fTimeDelta);
	}
}

void CObject_Manager::Late_Tick(_float fTimeDelta)
{
	// Static Object는 항상 돌려주고
	for (auto& Pair : m_pLayers[0])
		Pair.second->Late_Tick(fTimeDelta);

	// 아니면 현재 Tick 돌릴 애들만 돌려주기
	if (m_eCurLevel != 0)
	{
		for (auto& Pair : m_pLayers[m_eCurLevel])
			Pair.second->Late_Tick(fTimeDelta);
	}
}

HRESULT CObject_Manager::Add_GameObject(_uint iLevelIndex, const LAYER_ID& eLayerID, const wstring& strObjectTag, shared_ptr<CGameObject> pGameObject)
{
	shared_ptr<CLayer> pLayer = Find_Layer(iLevelIndex, eLayerID);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		m_pLayers[iLevelIndex].emplace(eLayerID, pLayer);
	}

	pLayer->Add_GameObject(pGameObject);

	pGameObject->Set_ObjectTag(strObjectTag);

	return S_OK;
}

shared_ptr<class CGameObject> CObject_Manager::Find_GameObject(_uint iLevelIndex, const LAYER_ID& eLayerID, const wstring& strObjectTag)
{
	return m_pLayers[iLevelIndex].find(eLayerID)->second->Find_GameObject(strObjectTag);
}

//shared_ptr<class CGameObject> CObject_Manager::Clone_GameObject(_uint iLevelIndex, const LAYER_ID& eLayerID, const wstring& strObjectTag)
//{
//	shared_ptr<CGameObject> pOriginObject = Find_GameObject(iLevelIndex, eLayerID, strObjectTag);
//	if (nullptr == pOriginObject)
//		return nullptr;
//
//	return make_shared<CGameObject>(*pOriginObject);
//}

HRESULT CObject_Manager::Remove_Object(_uint iLevelIndex, const LAYER_ID& eLayerID, const wstring& strObjectTag)
{
	return m_pLayers[iLevelIndex].find(eLayerID)->second->Remove_Object(strObjectTag);
}

HRESULT CObject_Manager::Remove_Object(_uint iLevelIndex, const LAYER_ID& eLayerID, shared_ptr<class CGameObject> pGameObject)
{
	return m_pLayers[iLevelIndex].find(eLayerID)->second->Remove_Object(pGameObject);
}

HRESULT CObject_Manager::Remove_Layer(_uint iLevelIndex, const LAYER_ID& eLayerID)
{
	shared_ptr<CLayer> pLayer = Find_Layer(iLevelIndex, eLayerID);

	if (nullptr != pLayer)
	{
		pLayer->Clear();
	}

	return S_OK;
}

shared_ptr<class CComponent> CObject_Manager::Find_Component(_uint iLevelIndex, const LAYER_ID& eLayerID, const wstring& strComTag, _uint iIndex)
{
	shared_ptr<CLayer> pLayer = Find_Layer(iLevelIndex, eLayerID);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Find_Component(strComTag, iIndex);
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	m_pLayers[iLevelIndex].clear();
}

shared_ptr<class CLayer> CObject_Manager::Find_Layer(_uint iLevelIndex, const LAYER_ID& eLayerID)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto iter = m_pLayers[iLevelIndex].find(eLayerID);
	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

list<shared_ptr<CGameObject>> CObject_Manager::Get_Layer_Objects(_uint iLevelIndex, const LAYER_ID& eLayerID)
{
	shared_ptr<CLayer> pLayer = Find_Layer(iLevelIndex, eLayerID);

	if (!pLayer)
	{
		return list<shared_ptr<CGameObject>>();
	}
	return pLayer->Get_AllObjects();
}

shared_ptr<CObject_Manager> CObject_Manager::Create(_uint iNumLevels)
{
	shared_ptr<CObject_Manager> pInstance = make_shared<CObject_Manager>();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CObject_Manager");
		pInstance = nullptr;
	}

	return pInstance;
}

void CObject_Manager::Free()
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& iter : m_pLayers[i]) {
			iter.second->Free();
		}

		m_pLayers[i].clear();
	}

	Safe_Delete_Array(m_pLayers);
}
