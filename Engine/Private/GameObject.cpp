#include "GameObject.h"
#include "GameInstance.h"

CGameObject::CGameObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice(pDevice)
    , m_pContext(pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
{
}

CGameObject::CGameObject(const CGameObject& rhs)
    : m_pDevice(rhs.m_pDevice)
    , m_pContext(rhs.m_pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		GAMEOBJECT_DESC* pGameDesc = (GAMEOBJECT_DESC*)pArg;
		m_iGameObjectData = pGameDesc->iGameObjectData;
	}

	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Initialize(pArg)))
		return E_FAIL;

	m_Components.emplace(g_strTransformTag, m_pTransformCom);

    return S_OK;
}

void CGameObject::Priority_Tick(_float fTimeDelta)
{
}

void CGameObject::Tick(_float fTimeDelta)
{
}

void CGameObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
    return S_OK;
}

void CGameObject::OnCollision(LAYER_ID eColLayer, shared_ptr<CCollider> pCollider)
{
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const wstring& strPrototypeTag, const wstring& strComTag, shared_ptr<CComponent>* ppOut, void* pArg)
{
	auto	iter = m_Components.find(strComTag);
	if (iter != m_Components.end())
		return E_FAIL;

	shared_ptr<CComponent> pComponent = m_pGameInstance->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(strComTag, pComponent);

	*ppOut = pComponent;

	return S_OK;
}

HRESULT CGameObject::Remove_Component(const wstring& strPrototypeTag, const wstring& strComTag)
{
	auto iter = m_Components.find(strComTag);
	if (iter != m_Components.end()) {
		m_Components.erase(iter);
	}

	return S_OK;
}

shared_ptr<class CComponent> CGameObject::Find_Component(const wstring& strComTag)
{
	return m_Components.find(strComTag)->second;
}

shared_ptr<CGameObject> CGameObject::Clone(void* pArg)
{
	return nullptr;
}

void CGameObject::Free()
{
}
