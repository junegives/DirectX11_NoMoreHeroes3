#pragma once

#include "Model.h"
#include "Shader.h"
#include "Texture.h"
#include "Collider.h"
#include "Transform.h"
#include "Navigation.h"
#include "BinaryModel.h"
#include "StateMachine.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Trail.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_RushTrail.h"
#include "VIBuffer_SwordTrail.h"
#include "VIBuffer_HeightTerrain.h"
#include "VIBuffer_Particle_Rect.h"
#include "VIBuffer_Particle_Point.h"
#include "VIBuffer_Particle_Point_UI.h"

#pragma region FOR_COLLIDER
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#pragma endregion

/* 컴포넌트 원형을 레벨별로 보관한다. */
/* 지정한 원형을 복제하여 사본을 생성하고 리턴한다. */

BEGIN(Engine)

class CComponent_Manager final
{
public:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Component(_uint iLevelIndex, const wstring& strComponentTag, shared_ptr<class CComponent> pComponent);
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, shared_ptr<class CComponent> pPrototype);
	HRESULT Remove_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);
	shared_ptr<class CComponent> Find_Component(_uint iLevelIndex, const wstring& strComponentTag);
	shared_ptr<class CComponent> Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);
	shared_ptr<class CComponent> Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg);
	void Clear(_uint iLevelIndex);

private:
	_uint				m_iNumLevels = { 0 };

	map<const wstring, shared_ptr<class CComponent>>* m_pPrototypes = { nullptr };
	map<const wstring, shared_ptr<class CComponent>>* m_pComponents = { nullptr };
	typedef map<const wstring, shared_ptr<class CComponent>>	PROTOTYPES;

public:
	static shared_ptr<CComponent_Manager> Create(_uint iNumLevels);
	virtual void Free();
};

END