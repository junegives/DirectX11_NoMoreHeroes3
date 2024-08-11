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
		EFFECT_TYPE		eEffectType;	// ����Ʈ Ÿ��

		_float3			vCenter;		// ��ġ
		_float3			vSize;			// ũ��
		
		_bool			isMove;			// �̵� ����
		_float3			vDir;			// ����
		_float			fMoveSpeed;		// �̵� �ӵ�

		_bool			isRotate;		// ȸ�� ����
		_float3			vRotate;		// ȸ�� ����
		_float3			vRotateSpeed;	// ȸ�� �ӵ�

		_bool			isThorwColor = false;	// ���� ��������
		_float4			vColor;			// ����

		_float			fLifeTime;		// ����
		_float			fDuration;		// ��� �ð�

		_bool			isLoop;			// �ݺ�

		_bool			isUseMask = false;		// ����ũ �ؽ�ó ������

		_bool			isSprite = false;	// ��������Ʈ �ؽ�ó����
		_uint			iUVx;			// UV ���� ����
		_uint			iUVy;			// UV ���� ����

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