#pragma once

#include "Component.h"

/* 1. 월드스페이스 변환을 위한 행렬을 보유. */
/* 2. 월드상에서의 상태변환을 위한 기능 (움직인다.) */
/* 3. 객체를 생성하게되면 기본적으로 항상 추가되는 방식. */

BEGIN(Engine)

class ENGINE_DLL CTransform : public CComponent
{
public:
	enum STATE {STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

	typedef struct
	{
		_float		fSpeedPerSec = { 0.0f };
		_float		fRotationPerSec = { 0.0f };
	} TRANSFORM_DESC;

public:
	CTransform(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CTransform() = default;

public:
	_vector Get_State(STATE eState) {
		return XMLoadFloat4x4(&m_WorldMatrix).r[eState];
	}

	void Set_State(STATE eState, _fvector vState)
	{
		_float4		vTmp;
		XMStoreFloat4(&vTmp, vState);
		memcpy(&m_WorldMatrix.m[eState], &vTmp, sizeof(_float4));
	}

	_float3 Get_Scale() {
		return _float3(
			XMVectorGetX(XMVector3Length(Get_State(STATE_RIGHT))),
			XMVectorGetX(XMVector3Length(Get_State(STATE_UP))),
			XMVectorGetX(XMVector3Length(Get_State(STATE_LOOK)))
		);
	}

	void Set_Scale(_float fX, _float fY, _float fZ);

	void	Set_WorldMatrix(_float4x4 WorldMatrix) { m_WorldMatrix = WorldMatrix; }

	_matrix Get_WorldMatrix() {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	_matrix Get_WorldMatrix_Inverse() {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;

public:
	// 월드 변환 행렬을 셰이더의 변수에 연결하는 함수
	HRESULT Bind_ShaderResource(shared_ptr<class CShader> pShader, const _char* pContantName);

public:
	void Go_Straight(_float fTimeDelta, shared_ptr<class CNavigation> pNavigation = nullptr, _float fSpeedWeight = 1.f);
	void Go_Backward(_float fTimeDelta, shared_ptr<class CNavigation> pNavigation = nullptr, _float fSpeedWeight = 1.f);
	void Go_Left(_float fTimeDelta, shared_ptr<class CNavigation> pNavigation = nullptr, _float fSpeedWeight = 1.f);
	void Go_Right(_float fTimeDelta, shared_ptr<class CNavigation> pNavigation = nullptr, _float fSpeedWeight = 1.f);

	_bool Go_Dir_Slide(_float fTimeDelta, _vector vDir, shared_ptr<class CNavigation> pNavigation, _float fSpeedWeight = 1.f);

	void Go_Straight_Slide(_float fTimeDelta, shared_ptr<class CNavigation> pNavigation, _float fSpeedWeight = 1.f);
	_bool Go_Straight_Collide(_float fTimeDelta, shared_ptr<class CNavigation> pNavigation = nullptr, _float fSpeedWeight = 1.f);

	// XZ 평면에 대해 평행하게 이동
	void Go_StraightXZ(_float fTimeDelta);
	void Go_BackwardXZ(_float fTimeDelta);
	void Go_LeftXZ(_float fTimeDelta);
	void Go_RightXZ(_float fTimeDelta);

	void Turn(_fvector vAxis, _float fTimeDelta);
	void TurnRadian(_fvector vAxis, _float fRadian);
	void Rotation(_fvector vAxis, _float fRadian);

	_bool MoveTo(_fvector vPoint, _float fLimit, _float fTimeDelta);
	_bool MoveToCheckNavi(_fvector vPoint, shared_ptr<class CNavigation> pNavigation, _float fLimit, _float fTimeDelta);
	void LookAt(_fvector vPoint);
	void LookAt_ForLandObject(_fvector vPoint);

	void SetLook(_fvector _vLook);
	void SetLook_ForLandObject(_fvector _vLook);

	void UpDir(_fvector vDir, _fvector _vLook);

public:
	void			SetSpeedPerSec(_float fSpeed) { m_fSpeedPerSec = fSpeed; }
	void			ChangeSpeedPerSec(_float fSpeed) { m_fSpeedPerSec += fSpeed; }
	_float			GetSpeedPerSec() { return m_fSpeedPerSec; }

	void			SetRotationPerSec(_float fRotation) { m_fRotationPerSec = fRotation; }
	void			ChangeRotationPerSec(_float fRotation) { m_fRotationPerSec += fRotation; }
	_float			GetRotationPerSec() { return m_fRotationPerSec; }

private:
	_float4x4		m_WorldMatrix = {};

	_float			m_fSpeedPerSec = { 0.0f };
	_float			m_fRotationPerSec = { 0.0f };

public:
	static shared_ptr<CTransform> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual shared_ptr<CComponent> Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END