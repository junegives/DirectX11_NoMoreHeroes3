#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CTransform;
class CSphericalCoordinates;
END

BEGIN(Client)

class CCamera_Attach : public CCamera
{
public:
	typedef struct : public CCamera::CAMERA_DESC
	{
		_float		fMouseSensor = { 0.0f };
	} CAMERA_ATTACH_DESC;

public:
	CCamera_Attach(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CCamera_Attach(const CCamera_Attach& rhs);
	virtual ~CCamera_Attach() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void			SetLock(_bool isLocked) { m_isLocked = isLocked; }

	void			ChangeTargetCamLerp(_vector vTargetDir, _float fLerpRatio, _vector vTargetPos);

private:
	_float			m_fMouseSensor = { 0 };

	_float3			m_vLookPos;	// 카메라가 바라볼 위치
	_float3			m_vCamPos;	// 카메라의 상대 위치

	weak_ptr<CTransform>				m_pTargetTransform;	// 타겟 트랜스폼
	shared_ptr<CSphericalCoordinates>	m_pSphereicalCoord;	// 구면 좌표계 클래스

	_vector			m_vTargetDir;
	_vector			m_vTargetPos;
	_float			m_fLerpRatio;

	_vector			m_vTargetObjectPos;

	_bool			m_isLocked = false;
	_float			m_fUnLockTime = 0.f;
	_float			m_fTargetScale = 0.f;

	shared_ptr<class CUI_Target>		m_pTargetUI = nullptr;


public:
	static shared_ptr<CCamera_Attach> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
	virtual void Free() override;
};

END