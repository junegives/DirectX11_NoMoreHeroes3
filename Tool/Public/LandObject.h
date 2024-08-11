#pragma once

#include "Tool_Defines.h"
#include "GameObject.h"


BEGIN(Engine)
class CTransform;
class CVIBuffer_Terrain;
END

BEGIN(Tool)

class CLandObject abstract : public CGameObject
{
public:
	typedef struct : public GAMEOBJECT_DESC
	{
		shared_ptr<CTransform> pTerrainTransform = { nullptr };
		shared_ptr<CVIBuffer_HeightTerrain> pTerrainVIBuffer = { nullptr };
	}LANDOBJ_DESC;

public:
	CLandObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CLandObject(const CLandObject& rhs);
	virtual ~CLandObject() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_uint					GetHP() { return m_iHP; }
	_bool					SetDamage(_uint& iDamage) {
		m_iHP -= iDamage;
		if (m_iHP <= 0) {
			m_iHP = 0;
			return false;
		}
		return true;
	}
	void					SetHeal(_uint& iHeal) {
		m_iHP += iHeal;
		if (m_iHP >= m_iMaxHP)
			m_iHP = m_iMaxHP;
	}

	_uint					GetDamage() { return m_iSaveDamage; }
	void					SaveDamage(_uint iSaveDamage) { m_iSaveDamage = iSaveDamage; }
	_bool					SetDamage() { m_iHP -= m_iSaveDamage; if (m_iHP <= 0) return false; else return true; }

	_float					GetColRadius() { return m_fColRadius; }
public:
	//HRESULT Add_Components();

protected:
	shared_ptr<CNavigation>		m_pNavigationCom = { nullptr };

	shared_ptr<CTransform>				m_pTerrainTransform = { nullptr };
	shared_ptr<CVIBuffer_HeightTerrain>	m_pTerrainVIBuffer = { nullptr };
	//shared_ptr<CNavigation>				m_pNavigationCom = { nullptr };

protected:
	HRESULT SetUp_OnTerrain(class shared_ptr<CTransform> pTargetTransform);

	_float	SetUp_OnNavi(class shared_ptr<CTransform> pTargetTransform);
	_float	Jump_OnNavi(class shared_ptr<CTransform> pTargetTransform);

public:
	_bool	IsValidPos(_vector vPos);

protected:
	_uint						m_iHP;
	_uint						m_iMaxHP;
	_uint						m_iAP;

	_uint						m_iSaveDamage;

	_float						m_fColRadius;

public:
	virtual void Free() override;
};

END