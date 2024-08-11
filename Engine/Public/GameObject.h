#pragma once

#include "Transform.h"

/* 클라이언트에서 제작될 다양한객체들의 부모가 되는 클래스. */

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public enable_shared_from_this<CGameObject>
{
public:
	typedef struct : public CTransform::TRANSFORM_DESC
	{
		_uint iGameObjectData = { 0 };
	} GAMEOBJECT_DESC;

public:
	CGameObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Tick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void	OnCollision(LAYER_ID eColLayer, shared_ptr<class CCollider> pCollider);

	shared_ptr<class CTransform> Get_TransformCom() { return m_pTransformCom; }
	shared_ptr<class CComponent> Find_Component(const wstring& strComTag);

public:
	const wstring&				Get_ObjectTag() { return m_strObjectTag; };
	void						Set_ObjectTag(const wstring& strObjectTag) { m_strObjectTag = strObjectTag; }

	_bool						IsActive() { return m_isActive; }
	void						SetActive(_bool isActive) { m_isActive = isActive; }
	_bool						IsAlive() { return m_isAlive; }
	void						SetAlive(_bool isAlive) { m_isAlive = isAlive; }

	_bool						IsOnCollide() { return m_isOnCol; }
	void						SetOnCollide(_bool isOnCol) { m_isOnCol = isOnCol; }

protected:
	ComPtr<ID3D11Device> m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };

	shared_ptr<class CGameInstance> m_pGameInstance = { nullptr };
	shared_ptr<class CTransform>	m_pTransformCom = { nullptr };

protected:
	map<const wstring, shared_ptr<class CComponent>>		m_Components;
	_uint													m_iGameObjectData = { 0 };

protected:
	wstring			m_strObjectTag = L"";

	_bool			m_isActive = true;
	_bool			m_isAlive = true;

	_bool			m_isOnCol = true;

protected:
	HRESULT Add_Component(_uint iLevelIndex, const wstring& strPrototypeTag, const wstring& strComTag, shared_ptr<CComponent>* ppOut, void* pArg = nullptr);
	HRESULT Remove_Component(const wstring& strPrototypeTag, const wstring& strComTag);

public:
	virtual shared_ptr<CGameObject> Clone(void* pArg);
	virtual void Free();
};

END