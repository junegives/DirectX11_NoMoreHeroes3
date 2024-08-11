#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CMotionTrail final : public CGameObject
{
public:
	typedef struct
	{
		shared_ptr<CBinaryModel>	pModel = nullptr;
		_float4x4					WorldMatrix;
		_float						fLifeTime = 0.f;
	} MOTIONTRAIL_DESC;

public:
	CMotionTrail(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CMotionTrail(const CMotionTrail& rhs);
	virtual ~CMotionTrail() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr)	override;
	virtual void	Tick(_float fTimeDelta)				override;
	virtual void	Late_Tick(_float fTimeDelta)		override;
	virtual HRESULT Render();

private:
	HRESULT Add_Component();
	HRESULT	Bind_ShaderResources();

private:
	MOTIONTRAIL_DESC				m_MotionTrailList;

public:
	static shared_ptr<CMotionTrail> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void Free() override;
};

END