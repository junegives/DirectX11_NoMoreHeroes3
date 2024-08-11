#include "Collision_Manager.h"

#include "GameObject.h"
#include "Collider.h"

CCollision_Manager::CCollision_Manager()
{
}

HRESULT CCollision_Manager::Initialize()
{
	return S_OK;
}

void CCollision_Manager::Add_Collider(const LAYER_ID& eLayerID, shared_ptr<class CCollider> pGameObject)
{
	if (eLayerID >= LAYER_END)
		return;

	m_Colliders[eLayerID].push_back(pGameObject);
}

void CCollision_Manager::Check_Collision(const LAYER_ID& eLayerSour, const LAYER_ID& eLayerDest)
{
	vector<shared_ptr<CCollider>>& pSourList = m_Colliders[eLayerSour];
	vector<shared_ptr<CCollider>>& pDestList = m_Colliders[eLayerDest];

	shared_ptr<CGameObject>	pSourObject;
	shared_ptr<CGameObject>	pDestObject;

	for (auto pSourCollider = pSourList.begin(); pSourCollider != pSourList.end(); )
	{
		if (!(*pSourCollider)->IsActive() || !(*pSourCollider)->IsAlive())
		{
			++pSourCollider;
			continue;
		}

		pSourObject = dynamic_pointer_cast<CGameObject>((*pSourCollider)->GetOwner());

		for (auto pDestCollider = pDestList.begin(); pDestCollider != pDestList.end(); )
		{
			if (!(*pDestCollider)->IsActive() || !(*pDestCollider)->IsAlive())
			{
				++pDestCollider;
				continue;
			}

			pDestObject = dynamic_pointer_cast<CGameObject>((*pDestCollider)->GetOwner());

			// 그냥 충돌이 아니라,
			// 처음 충돌인지 아닌지
			// 저번에도 충돌을 했지만 충돌을 계속 유지할지 (OnCollision()을 호출할지) 아니면 충돌을 종료할지
			// 두 가지를 나눠서 판단해야한다.
			if ((*pSourCollider)->Intersect(*pDestCollider))
			{
				pSourObject->OnCollision(eLayerDest, *pDestCollider);
				pDestObject->OnCollision(eLayerSour, *pSourCollider);
			}

			// 탐색하려는 오브젝트가 이미 죽어있다면 지우고 다음 오브젝트로 continue
			if (!pDestObject || !pDestObject->IsAlive())
			{
				pDestCollider = pDestList.erase(pDestCollider);
				continue;
			}

			++pDestCollider;
		}

		// 탐색하려는 오브젝트가 죽었다면 지우고 다음 오브젝트로 continue
		if (!pSourObject || !pSourObject->IsAlive())
		{
			pSourCollider = pSourList.erase(pSourCollider);
			continue;
		}

		++pSourCollider;
	}
}

shared_ptr<CCollision_Manager> CCollision_Manager::Create()
{
	shared_ptr<CCollision_Manager> pInstance = make_shared<CCollision_Manager>();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CCollision_Manager");
		pInstance.reset();
	}

	return pInstance;
}

void CCollision_Manager::Free()
{
	for (size_t i = 0; i < LAYER_END; i++)
	{
		m_Colliders[i].clear();
	}
}
