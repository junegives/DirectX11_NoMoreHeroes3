#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CCollision_Manager final
{
public:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;

public:
	HRESULT Initialize();

public:
	void	Add_Collider(const LAYER_ID& eLayerID, shared_ptr<class CCollider> pGameObject);
	void	Check_Collision(const LAYER_ID& eLayerSour, const LAYER_ID& eLayerDest);
	
private:
	vector<shared_ptr<class CCollider>>	m_Colliders[LAYER_END];

public:
	static	shared_ptr<CCollision_Manager> Create();
	void	Free();
};

END