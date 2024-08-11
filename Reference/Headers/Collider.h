#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_SPHERE, TYPE_AABB, TYPE_OBB, TYPE_END };
	
public:
	CCollider(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() { int a = 3; }

public:
	virtual HRESULT Initialize_Prototype(TYPE eType);
	virtual HRESULT Initialize(void* pArg) override;

public:
	shared_ptr<class CGameObject>	GetOwner() { return m_pOwner.lock(); }
	void							SetOwner(shared_ptr<class CGameObject> pOwner) { m_pOwner = pOwner; }

public:
	void	Tick(_fmatrix WorldMatrix);
	HRESULT Render();

public:
	_bool	Intersect(shared_ptr<CCollider> pTargetCollider);

	_bool						IsOnCollide() { return m_isOnCol; }
	void						SetOnCollide(_bool isOnCol) { m_isOnCol = isOnCol; }

	_bool						IsActive() { return m_isActive; }
	void						SetActive(_bool isActive) { m_isActive = isActive; }

	_bool						IsAlive() { return m_isAlive; }
	void						SetAlive(_bool isAlive) { m_isAlive = isAlive; }

public:
	_uint						GetHitType() { return m_iHitType; }
	void						SetHitType(_uint iHitType) { m_iHitType = iHitType; }
	_uint						GetAP() { return m_iAP; }
	void						SetAP(_uint iAP) { m_iAP = iAP; }

	shared_ptr<class CBounding>	GetBounding() { return m_pBounding; }


private:
	TYPE						m_eType = { TYPE_END };
	shared_ptr<class CBounding> m_pBounding = { nullptr };

	weak_ptr<class CGameObject>	m_pOwner;
	_bool						m_isActive = true;
	_bool						m_isAlive = true;
	_bool						m_isOnCol = true;

	_uint						m_iHitType = 0;
	_uint						m_iAP = 0;


#ifdef _DEBUG
private:
	// 포지션과 색을 가진 PrimitiveBatch를 만들어서 넘겨주면 정점이 생성되는것
	shared_ptr<PrimitiveBatch<VertexPositionColor>> m_pBatch = { nullptr };
	// 그리려면 셰이더도 있어야함(내가 짠 셰이더 아니다, 기본 제공 셰이더)
	shared_ptr<BasicEffect> m_pEffect = { nullptr };
	ComPtr<ID3D11InputLayout> m_pInputLayout = { nullptr };

#endif

public:
	static shared_ptr<CCollider> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, TYPE eType);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	virtual void Free() override;
};

END