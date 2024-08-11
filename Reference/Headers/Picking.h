#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CPicking final
{
public:
	CPicking(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	~CPicking() = default;

public:
	HRESULT Initialize();

public:
	// Ray to World
	void RayToWorld(_long iScreenX, _long iScreenY);
	// Ray to Local
	void RayToLocal(_matrix matWorldInv);

	_bool IntersectWorld(_float3 vPointA, _float3 vPointB, _float3 vPointC, _float3* pOut, _float* fMinDist);
	_bool IntersectLocal(_float3 vPointA, _float3 vPointB, _float3 vPointC, _float3* pOut);

protected:
	shared_ptr<class CGameInstance> m_pGameInstance = { nullptr };

	_float3 m_vRayDirWorld, m_vRayPosWorld;
	_float3 m_vRayDirLocal, m_vRayPosLocal;

	_float	m_fMinDist;

protected:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>		m_pContext = { nullptr };

public:
	static shared_ptr<CPicking> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const GRAPHIC_DESC& GraphicDesc);
	void Free();
};

END
