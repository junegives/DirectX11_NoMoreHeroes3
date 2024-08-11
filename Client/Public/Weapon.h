#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CBinaryModel;
END

BEGIN(Client)

class CWeapon : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		_float4x4* pSocketMatrix;
	}WEAPON_DESC;

public:
	CWeapon(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	OnCollision(LAYER_ID eColLayer, shared_ptr<CCollider> pCollider) override;

public:
	void		Set_Attached(_bool isAttached) { m_isAttached = isAttached; }
	_bool		Get_Attached() { return m_isAttached; }

	void		SetWeaponGlow(_bool isWeaponGlow) { m_isWeaponGlow = isWeaponGlow; }

	void		Set_AttackType(_uint iAttackType) { m_iAttackType = iAttackType; }
	_uint		Get_AttackType() { return m_iAttackType; }

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CBinaryModel> m_pModelCom = { nullptr };
	shared_ptr<CCollider>	m_pColliderCom = { nullptr };

	shared_ptr<CBinaryModel> m_pEffectModelCom = { nullptr };
	shared_ptr<CBinaryModel> m_pEffectOutModelCom = { nullptr };
	shared_ptr<CBinaryModel> m_pEffectInModelCom = { nullptr };

private:
	_matrix		m_InitMatrix;

	_float4x4* m_pSocketMatrix = { nullptr };
	_float4x4* m_pTrailStartMatrix = { nullptr };
	_float4x4* m_pTrailEndMatrix = { nullptr };

	_matrix		m_pParticleFXMatrix;
	_matrix		m_WeaponFXMatrix;
	_vector		m_vWeaponFXPos;

	_bool		m_isAttached = false;
	_bool		m_isWeaponGlow = false;

	_uint		m_iAttackType = 0;

	_vector		m_vPrevPos;
	_vector		m_vCurPos;

public:
	void SetTrailPos(_vector vHighPosition, _vector vLowPosition);
	void StartTrail(const wstring& strDiffuseTextureName, const wstring& strAlphaTextureName, const _float4& vColor, _uint iVertexCount);
	void StopTrail();

	void UpdateTrail(_matrix fSocketMatrix);

private:
	shared_ptr<class CTrail>		m_pTrail;
	shared_ptr<class CSwordTrail>	m_pSwordTrail;
	shared_ptr<class CMeshTrail>	m_pMeshTrail;

	_uint		m_iGenerateAfterImage = 0;

	_float3		m_vUpperPos;
	_float3		m_vLowerPos;

	STATE		m_ePrevState;

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	static shared_ptr<CWeapon> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END