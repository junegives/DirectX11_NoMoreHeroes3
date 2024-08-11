#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CPartObject : public CGameObject
{
public:
	typedef struct
	{
		shared_ptr<CTransform> pParentTransform;
	}PARTOBJ_DESC;

public:
	CPartObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CPartObject(const CPartObject& rhs);
	virtual ~CPartObject() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	shared_ptr<CTransform>	m_pParentTransform = { nullptr };
	_float4x4				m_WorldMatrix;

public:
	virtual void Free() override;
};

END
