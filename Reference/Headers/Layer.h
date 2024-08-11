#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CLayer final
{
public:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Initialize();
	void Priority_Tick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	HRESULT Clear();

public:
	list<shared_ptr<class CGameObject>>	Get_AllObjects();
	HRESULT Add_GameObject(shared_ptr<class CGameObject> pGameObject);
	shared_ptr<class CGameObject> Find_GameObject(const wstring& strObjectTag);
	HRESULT Remove_Object(const wstring& strObjectTag);
	HRESULT Remove_Object(shared_ptr<class CGameObject> pGameObject);
	shared_ptr<class CComponent> Find_Component(const wstring& strComTag, _uint iIndex);


private:
	list<shared_ptr<class CGameObject>>		m_GameObjects;

public:
	static shared_ptr<CLayer> Create();
	virtual void Free();
};
END