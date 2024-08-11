#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point : public CVIBuffer
{
public:
	CVIBuffer_Point(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CVIBuffer_Point(const CVIBuffer_Point& rhs);
	virtual ~CVIBuffer_Point() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render();
	virtual HRESULT Bind_Buffers();

public:
	void			Reset(_vector vCenter, _vector vDir = { 0.f, 0.f, 0.f, 0.f });
	_bool			IsFinish();

protected:
	ComPtr<ID3D11Buffer>		m_pVBInstance = { nullptr };
	_uint						m_iInstanceStride = { 0 };
	_uint						m_iNumInstance = { 0 };
	_uint						m_iIndexCountPerInstance = { 0 };

	random_device				m_RandomDevice;
	mt19937_64					m_RandomNumber;
	INSTANCE_DESC				m_InstanceData;

	_float* m_pSpeed = { nullptr };
	_float3* m_vDir = { nullptr };
	_float* m_pLifeTime = { nullptr };

	_float						m_fTimeAcc = { 0.f };
	_bool						m_isFinished = { false };

	_float						m_fMaxLifeTime = 0.f;
	_float						m_fMaxSpeed = 0.f;


public:
	virtual shared_ptr<CComponent> Clone(void* pArg) = 0;
	virtual void Free() override;
};

END