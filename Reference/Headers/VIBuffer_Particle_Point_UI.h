#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Particle_Point_UI final : public CVIBuffer_Instancing
{
public:
	enum UI_TYPE { UI_PLAYERHP, UI_MONSTERHP, UI_BOSSHP, UI_TARGET, UI_SLASH, UI_END };

public:
	typedef struct : public CVIBuffer_Instancing::INSTANCE_DESC
	{
		UI_TYPE		eUIType;
	} INSTANCE_UI_DESC;

public:
	CVIBuffer_Particle_Point_UI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CVIBuffer_Particle_Point_UI(const CVIBuffer_Particle_Point_UI& rhs);
	virtual ~CVIBuffer_Particle_Point_UI() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iNumInstance);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool	Picking(_long iMouseX, _long iMouseY, _matrix matWorld, _float3* vPickPoint, _float* fMinDist) override;

private:
	UI_TYPE		m_eUIType = UI_END;

public:
	static shared_ptr<CVIBuffer_Particle_Point_UI> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iNumInstance);
	virtual shared_ptr<CComponent> Clone(void* pArg) override;
	virtual void Free() override;
};

END