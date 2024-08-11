#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CComponent abstract
{
public:
	CComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render() { return S_OK; }

protected:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };
	shared_ptr<class CGameInstance> m_pGameInstance = { nullptr };

	_bool							m_isCloned = { false };

public:
	virtual shared_ptr<CComponent> Clone(void* pArg) = 0;
	virtual void Free();
};

END