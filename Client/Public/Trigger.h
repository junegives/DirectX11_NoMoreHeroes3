#pragma once

#include "Client_Defines.h"
#include "StaticObject.h"

BEGIN(Engine)
class CShader;
class CBinaryModel;
END

BEGIN(Client)

class CTrigger : public CStaticObject
{
public:
	enum TRIGGER_ID { TRIGGER_BATTLE1, TRIGGER_BATTLE2, TRIGGER_BOSS, TRIGGER_MINIGAME, TRIGGER_END };

public:
	CTrigger(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag, const TRIGGER_ID& eTriggerID);
	CTrigger(const CTrigger& rhs);
	virtual ~CTrigger() = default;

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
	_bool				m_bPicked = false;
	_bool				m_bCanPick = false;

public:
	void				SetTriggerID(TRIGGER_ID eTriggerID) { m_eTriggerID = eTriggerID; }
	TRIGGER_ID			GetTriggerID() { return m_eTriggerID; }

private:
	TRIGGER_ID			m_eTriggerID = TRIGGER_END;

	shared_ptr<class CEffect_Pillar> m_pEffectTriggerPillar = nullptr;

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CTrigger> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strModelComTag, const TRIGGER_ID& eTriggerID, _bool isActive = true);
	virtual void Free() override;
};

END