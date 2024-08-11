#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CBinaryModel;
END

BEGIN(Client)

class CBoss : public CGameObject
{
public:
	CBoss(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CBoss(const CBoss& rhs);
	virtual ~CBoss() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CBinaryModel> m_pModelCom = { nullptr };

private:
	void	Key_Input();

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

private:
	_uint	m_iAnimationIndex = 0;
	_bool	m_bPressed[2] = { false };
	_bool	m_bAnimChanging = false;

	_float4	m_PrevTransPos = {};


	// 상태 제어
private:





public:
	static shared_ptr<CBoss> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void Free() override;
};

END