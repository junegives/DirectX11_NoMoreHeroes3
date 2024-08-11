#pragma once

#include "Client_Defines.h"
#include "StaticObject.h"

BEGIN(Engine)
class CShader;
class CBinaryModel;
END

BEGIN(Client)

class CSign : public CStaticObject
{
public:
	enum SIGN_ID { SIGN_BATTLE1, SIGN_BATTLE2, SIGN_BOSS, SIGN_MINIGAME, SIGN_END };

public:
	CSign(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag, const SIGN_ID& eSignID);
	CSign(const CSign& rhs);
	virtual ~CSign() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollision(LAYER_ID eColLayer, shared_ptr<class CCollider> pCollider) override;

private:
	_bool			m_bPicked = false;
	_bool			m_bCanPick = false;

public:
	void			SetSignID(SIGN_ID eSignID) { m_eSignID = eSignID; }
	SIGN_ID			GetSignID() { return m_eSignID; }

private:
	SIGN_ID			m_eSignID = SIGN_END;

private:
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CSign> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag, const SIGN_ID& eSignID, _bool isActive = true);
	virtual void Free() override;
};

END