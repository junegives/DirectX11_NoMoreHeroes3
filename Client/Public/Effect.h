#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CEffect : public CGameObject
{
public:
	enum EFFECT_TYPE { EFFECT_TEXTURE, EFFECT_MESH, EFFECT_END };

public:
	typedef struct
	{
		EFFECT_TYPE		eEffectType;	// 이펙트 타입

		_float3			vCenter;		// 위치
		_float3			vSize;			// 크기
		
		_bool			isMove;			// 이동 여부
		_float3			vDir;			// 방향
		_float			fMoveSpeed;		// 이동 속도

		_bool			isRotate;		// 회전 여부
		_float3			vRotate;		// 회전 방향
		_float3			vRotateSpeed;	// 회전 속도

		_bool			isThorwColor = false;	// 색도 던지는지
		_float4			vColor;			// 색깔

		_float			fLifeTime;		// 수명
		_float			fDuration;		// 재생 시간

		_bool			isLoop;			// 반복

		_bool			isUseMask = false;		// 마스크 텍스처 쓰는지

		_bool			isSprite = false;	// 스프라이트 텍스처인지
		_uint			iUVx;			// UV 분할 개수
		_uint			iUVy;			// UV 분할 개수

		_uint			iShaderPass = 0;
	} EFFECT_DESC;

public:
	CEffect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CEffect(const CEffect& rhs);
	~CEffect();

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void	Play(_uint iNum, _vector vCenter, _vector vScale, _vector vUp, _vector vLook) = 0;
	virtual void	Play(_uint iNum, _vector vCenter, _bool isRotate, _vector vAxis, _vector vRotate) = 0;
	virtual void	Play(shared_ptr<CTransform> pParentTransform) = 0;

protected:
	EFFECT_DESC m_EffectDesc = {};
	_vector		m_vDir = { 0.f, 0.f, 0.f, 0.f };

protected:
	HRESULT Add_Component();


public:
	virtual void Free() override;
};

END