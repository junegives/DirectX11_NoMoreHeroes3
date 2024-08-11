#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CTrail final : public CGameObject
{
public:
	typedef struct
	{
		_bool	isTrail = false;
		
		_float	fAccGenTrail = 0.f;
		_float	fGenTrailTime = 0.01f;

		_float4	vDiffuseColor = { 0.f, 0.f, 0.f, 1.f };

		_float2	vUVAcc = { 0.f, 0.f };
		_float2	vUV_FlowSpeed = { 0.f, 0.f };

		_int	iUV_Cut = 1;

		_bool	isDiffuse = false;
		_bool	isAlpha = false;

		wstring strDiffuseTag;
		wstring strAlphaTag;
	} TRAIL_DESC;

public:
	CTrail(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CTrail(const CTrail& rhs);
	virtual ~CTrail() = default;

public:
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void SetPos(_vector vHighPosition, _vector vLowPosition);
	void StartTrail(_matrix TransformMatrix);
	void StopTrail();


public:
	void Set_TransformMatrix(_matrix TransformMatrix)
	{
		XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix);
	}

	void Set_Color(_float4 vColor) { m_TrailDesc.vDiffuseColor = vColor; m_TrailDesc.isAlpha = false; m_TrailDesc.isDiffuse = false; }
	void Set_DiffuseTexture(const wstring& strDiffuseTextureName);
	void Set_AlphaTexture(const wstring& strAlphaTextureName);

	const TRAIL_DESC& Get_TrailDesc() { return m_TrailDesc; }
	void Set_TrailDesc(const TRAIL_DESC& TrailDesc) { m_TrailDesc = TrailDesc; }

	void Set_VtxCount(_uint iCount) { m_pVIBufferCom->Set_VtxCount(iCount); }
public:
	void Set_UV_Speed(_float2 vUVSpeed) { m_vTrailUVSpeed = vUVSpeed; }

private:
	HRESULT Add_Component();


private:
	_float4x4 m_TransformMatrix;

private:
	shared_ptr<CShader>					m_pShaderCom = nullptr;
	shared_ptr<CTexture>				m_pDiffuseTextureCom = nullptr;
	shared_ptr<CTexture>				m_pAlphaTextureCom = nullptr;
	shared_ptr<CVIBuffer_Trail>			m_pVIBufferCom = nullptr;

private:
	_float2 m_vTrailUVSpeed = { 0.f, 0.f };

private:
	TRAIL_DESC m_TrailDesc;

public:
	static shared_ptr<CTrail> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const TRAIL_DESC& TrailDesc);
	virtual void Free() override;
};

END