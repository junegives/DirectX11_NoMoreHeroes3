#pragma once

#include "Engine_Defines.h"

/* 1. ������ü���� �����Ͽ� ���󳪴�. */
/* 2. ������ ������ü�� �����Ͽ� �纻��ü�� �����Ѵ�. */
/* 3. ������ �纻��ü�� ���̾ �߰��Ͽ� ������ ������ ������ ������ �����Ѵ�.(������ü�� ����) */

BEGIN(Engine)

class CObject_Manager final
{
public:
	CObject_Manager();
	virtual ~CObject_Manager() = default;
public:
	HRESULT Initialize(_uint iNumLevels);
	void	Priority_Tick(_float fTimeDelta);
	void	Tick(_float fTimeDelta);
	void	Late_Tick(_float fTimeDelta);

public:
	HRESULT Add_GameObject(_uint iLevelIndex, const LAYER_ID& eLayerID, const wstring& strObjectTag, shared_ptr<class CGameObject> pGameObject);
	shared_ptr<class CGameObject> Find_GameObject(_uint iLevelIndex, const LAYER_ID& eLayerID, const wstring& strObjectTag);
	shared_ptr<class CLayer>		Find_Layer(_uint iLevelIndex, const LAYER_ID& eLayerID);
	list<shared_ptr<class CGameObject>>	Get_Layer_Objects(_uint iLevelIndex, const LAYER_ID& eLayerID);
	//shared_ptr<class CGameObject> Clone_GameObject(_uint iLevelIndex, const LAYER_ID& eLayerID, const wstring& strObjectTag);
	HRESULT Remove_Object(_uint iLevelIndex, const LAYER_ID& eLayerID, const wstring& strObjectTag);
	HRESULT Remove_Object(_uint iLevelIndex, const LAYER_ID& eLayerID, shared_ptr<class CGameObject> pGameObject);
	HRESULT Remove_Layer(_uint iLevelIndex, const LAYER_ID& eLayerID);
	shared_ptr<class CComponent> Find_Component(_uint iLevelIndex, const LAYER_ID& eLayerID, const wstring& strComTag, _uint iIndex);
	void	Clear(_uint iLevelIndex);

	void	Set_CurLevel(_uint eCurLevel) { m_eCurLevel = eCurLevel; }
	_uint	Get_CurLevel() { return m_eCurLevel; }

private:
	_uint													m_iNumLevels = { 0 };
	_uint													m_eCurLevel = 0;

	map<const LAYER_ID, shared_ptr<class CLayer>>*			m_pLayers = { nullptr };
	typedef map<const LAYER_ID, shared_ptr<class CLayer>>	LAYERS;

private:

public:
	static shared_ptr<CObject_Manager> Create(_uint iNumLevels);
	virtual void Free();
};

END

