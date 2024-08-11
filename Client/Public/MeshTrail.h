#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CMeshTrail final : public CGameObject
{
public:
	enum PASS_TYPE { MESH_DEFAULT, MESH_COLOR_NONALPHA, MESH_COLOR_ALPHA, MESH_MASK_ALPHA, MESH_END };

public:
	typedef struct
	{
		_float4x4					WorldMatrix;
		_float						fLifeTime = 0.f;

		_float						fUVSpeedX = 0.f;
		_float						fUVSpeedY = 0.f;
		
		_float3						vColor = { 1.f, 1.f, 1.f };
	} MESHTRAIL_ELEMENT_DESC;

	typedef struct
	{
		shared_ptr<CBinaryModel>	pModel;
		_uint						iSize;

		_float						fLifeTime = 1.f;
		_float3						vColor = { 1.f, 1.f, 1.f };

		PASS_TYPE					ePassType = MESH_COLOR_ALPHA;

		_float						fUVx = 0.f;
		_float						fUVy = 0.f;

		_float						fUVSpeedRangeX = 0.f;
		_float						fUVSpeedRangeY = 0.f;

		wstring						strDiffuseTexture = L"";
		wstring						strMaskTexture = L"";
	} MESHTRAIL_DESC;

public:
	CMeshTrail(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CMeshTrail(const CMeshTrail& rhs);
	virtual ~CMeshTrail() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr)	override;
	virtual void	Tick(_float fTimeDelta)				override;
	virtual void	Late_Tick(_float fTimeDelta)		override;
	virtual HRESULT Render();

private:
	HRESULT Add_Component();
	HRESULT	Bind_ShaderResources();

public:
	void	SettingMeshTrail(MESHTRAIL_DESC MeshTrail_Desc);
	void	AddMeshTrail(MESHTRAIL_ELEMENT_DESC MeshTrail_Desc);
	void	ClearMeshTrail();

private:
	shared_ptr<CShader>				m_pShaderCom = { nullptr };
	shared_ptr<CBinaryModel>		m_pModelCom = nullptr;

	shared_ptr<CTexture>			m_pDiffuseTextureCom = nullptr;
	shared_ptr<CTexture>			m_pMaskTextureCom = nullptr;

	MESHTRAIL_DESC					m_MeshTraliDesc;
	list<MESHTRAIL_ELEMENT_DESC>	m_MeshTrailList;

public:
	static shared_ptr<CMeshTrail> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END