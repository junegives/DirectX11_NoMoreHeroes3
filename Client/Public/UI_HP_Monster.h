#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_HP_Monster : public CUI
{
public:
	CUI_HP_Monster(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CUI_HP_Monster(const CUI_HP_Monster& rhs);
	virtual ~CUI_HP_Monster();

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_uint		GetHP() { return m_iHP; }
	void		SetHP(_uint iHP) { m_iHP = iHP; }
	void		GainHP(_uint iHP) { m_iHP += iHP; if (m_iMaxHP < m_iHP) m_iHP = m_iMaxHP; }

	void		SetMaxHP(_uint iHP) { m_iMaxHP = iHP; m_iHP = m_iMaxHP; }
	void		FullHP() { m_iHP = m_iMaxHP; }
	_bool		LoseHP(_int iHP) {
		if (0 >= m_iHP - iHP) {
			m_iHP = 0; return false;
		}
		else
		{
			m_iHP -= iHP; return true;
		}
	}

private:
	shared_ptr<CVIBuffer_Particle_Point_UI>	m_pVIBufferCom = { nullptr };

	_int								m_iHP = 16;
	_int								m_iMaxHP = 16;

protected:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CUI_HP_Monster> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END