#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CSwordTrail final : public CGameObject
{
public:
	typedef struct
	{
		shared_ptr<CBinaryModel> pModel = nullptr;
	} SWORDTRAIL_DESC;

public:
	CSwordTrail(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strTextureComTag);
	CSwordTrail(const CSwordTrail& rhs);
	virtual ~CSwordTrail() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr)	override;
	virtual void	Tick(_float fTimeDelta)				override;
	virtual void	Late_Tick(_float fTimeDelta)		override;
	virtual HRESULT Render()							override;

public:
	void	SetColor(_vector vColorBack, _vector vColorFront);
	void	UpdateTrail(_float3 vPointDown, _float3 vPointUp, _fmatrix vWeaponMatrix);
	void	ClearTrail();

private:
	SWORDTRAIL_DESC				m_TrailDesc;

	shared_ptr<CShader>			m_pShaderCom = nullptr;
	shared_ptr<CBinaryModel>	m_pModelCom = nullptr;
	shared_ptr<CTexture>		m_pTextureCom = nullptr;
	shared_ptr<CVIBuffer_SwordTrail>	m_pVIBufferCom = nullptr;

	wstring						m_strTextureComTag = L"";

	_float	m_fLifeTme = 0.0f;
	_int	m_iBoneIndex = 0;
	_int	m_iTextureIndex = 0;

	_float3		m_vUpperPos = { 0.f, 0.f, 0.f };
	_float3		m_vLowerPos = { 0.f, 0.f, 0.f };
	_float4x4	m_WeaponMatrix;

	_vector		m_vColorBack = XMVectorSet(1.f, 0.28f, 0.3f, 1.f);
	_vector		m_vColorFront = XMVectorSet(1.f, 1.f, 1.f, 1.f);

	_float		m_fAlpha = 1.f;

private:
	HRESULT			Add_Component();
	HRESULT			Bind_ShaderResources();

public:
	static shared_ptr<CSwordTrail> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strTextureComTag, void* pArg = nullptr);
	virtual void Free() override;
};

END