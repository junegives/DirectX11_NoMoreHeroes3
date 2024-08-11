#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

// 행렬(정점이 아니라 파티클의 상태)을 담는 새로운 Vertex Buffer
class ENGINE_DLL CVIBuffer_Instancing abstract : public CVIBuffer
{
public:
	typedef struct
	{
		_float3			vPivot;
		_float3			vCenter;
		_float3			vRange;
		_float2			vSize;
		_float2			vSpeed;

		_float2			vLifeTime;
		_bool			isLoop;
		_float4			vColor;
		_float			fDuration;

		_bool			isSetDir = false;
		_float3			vDirRange = { 0.f, 0.f, 0.f };
		_float2			vDirX = { 0.f, 0.f };
		_float2			vDirY = { 0.f, 0.f };
		_float2			vDirZ = { 0.f, 0.f };
	}INSTANCE_DESC;

public:
	CVIBuffer_Instancing(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs);
	virtual ~CVIBuffer_Instancing() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render();
	virtual HRESULT Bind_Buffers();

public:
	void			Reset(_vector vCenter, _vector vDir = {0.f, 0.f, 0.f, 0.f});
	_bool			IsFinish();

public:
	void			Tick_Drop(_float fTimeDelta);
	void			Tick_Spread(_float fTimeDelta);
	void			Tick_Splash(_float fTimeDelta);

	// UI
	void			Tick_HP(_float fTimeDelta, _uint iHP, _uint iDecrease);
	void			Tick_Target(_float fTimeDelta, _bool isOnAttack);

protected:
	ComPtr<ID3D11Buffer>		m_pVBInstance = { nullptr };
	_uint						m_iInstanceStride = { 0 };
	_uint						m_iNumInstance = { 0 };
	_uint						m_iIndexCountPerInstance = { 0 };

	random_device				m_RandomDevice;
	mt19937_64					m_RandomNumber;
	INSTANCE_DESC				m_InstanceData;

	_float*						m_pSpeed = { nullptr };
	_float3*					m_vDir = { nullptr };
	_float*						m_pLifeTime = { nullptr };

	_float						m_fTimeAcc = { 0.f };
	_bool						m_isFinished = { false };

	_float						m_fMaxLifeTime = 0.f;
	_float						m_fMaxSpeed = 0.f;


public:
	virtual shared_ptr<CComponent> Clone(void* pArg) = 0;
	virtual void Free() override;
};

END