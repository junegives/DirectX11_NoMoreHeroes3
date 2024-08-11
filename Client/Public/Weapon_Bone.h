#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CBinaryModel;
END

BEGIN(Client)

class CWeapon_Bone : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		_float4x4* pSocketMatrix;
	}WEAPON_DESC;

public:
	CWeapon_Bone(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CWeapon_Bone(const CWeapon_Bone& rhs);
	virtual ~CWeapon_Bone() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void		Set_Attached(_bool isAttached) { m_isAttached = isAttached; }
	_bool		Get_Attached() { return m_isAttached; }

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CBinaryModel> m_pModelCom = { nullptr };
	shared_ptr<CCollider>	m_pColliderCom = { nullptr };

private:
	_float4x4* m_pSocketMatrix = { nullptr };

	_bool		m_isAttached = true;

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CWeapon_Bone> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END