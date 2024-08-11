#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CCamera_Event : public CCamera
{
public:
	CCamera_Event(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CCamera_Event(const CCamera_Event& rhs);
	virtual ~CCamera_Event() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	// 이벤트 시작 전 위치, 룩 세팅해주는 함수
	void			SettingBefore(_vector vPos, _vector vLook);
	void			SettingBefore(shared_ptr<CGameObject> pAttachObject, _float4x4* matBone, _matrix matPivot, _vector vLocalTrans);

	// 줌, 이동, 회전, 룩 변경 함수
	void			CamZoom(_float fPlayTime, _float fSpeed, _float fZoomAmount);
	void			CamMove(_float fPlayTime, _float fSpeed, _vector vTargetPos);
	void			CamRotation(_float fPlayTime, _float fSpeed, _float fRadian);
	void			CamLook(_float fPlayTime, _float fSpeed, _vector vTargetLook);

public:
	_bool			IsZooming() { return m_isZoom; }
	_bool			IsRotation() { return m_isRotation; }
	_bool			IsLookChanging() { return m_isLook; }

	void			SetAttach(_bool isAttach) { m_isAttach = isAttach; }

private:
	_float			m_fMoveSpeed;
	_float			m_fRotationSpeed;

	_bool			m_isZoom;
	_bool			m_isMove;
	_bool			m_isRotation;
	_bool			m_isLook;
	
	_float			m_fZoomTime;
	_float			m_fMoveTime;
	_float			m_fRotationTime;

	_float			m_fPlayTime;

	_vector			m_vTargetPos;
	_vector			m_vTargetLook;
	_float			m_fTargetZoom;
	_float			m_fTargetRotation;

	_bool			m_isAttach;
	_float4x4*		m_matAttach;
	_vector			m_vLocalTrans;
	shared_ptr<CGameObject>	m_pAttachObject;
	_matrix			m_matPivot;

	_uint			m_iX = 0;
	_uint			m_iY = 0;
	_uint			m_iZ = 0;


public:
	static shared_ptr<CCamera_Event> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
	virtual void Free() override;
};

END