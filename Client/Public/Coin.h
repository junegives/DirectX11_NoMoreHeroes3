#pragma once

#include "Client_Defines.h"
#include "StaticObject.h"

BEGIN(Engine)
class CShader;
class CBinaryModel;
END

BEGIN(Client)

class CCoin : public CStaticObject
{
public:
	enum COIN_ID { COIN_DEFAULT, COIN_COPPER, COIN_SILVER, COIN_GOLD, COIN_END };

public:
	CCoin(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag, const COIN_ID& eCoinID);
	CCoin(const CCoin& rhs);
	virtual ~CCoin() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollision(LAYER_ID eColLayer, shared_ptr<class CCollider> pCollider) override;

private:
	shared_ptr<CCollider> m_pColliderCom = { nullptr };

private:
	_uint			m_iValue = 0;

public:
	void				SetCoinID(COIN_ID eCoinID) { m_eCoinID = eCoinID; }
	COIN_ID			GetCoinID() { return m_eCoinID; }

private:
	COIN_ID			m_eCoinID = COIN_END;

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CCoin> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag, const COIN_ID& eCoinID, _vector vCreatePos, _bool isActive = true);
	virtual void Free() override;
};

END