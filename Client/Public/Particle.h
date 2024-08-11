#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Particle_Point;
END

BEGIN(Client)

class CParticle final : public CGameObject
{
public:
	enum PARTICLE_TYPE { PARTICLE_SLASH, PARTICLE_SLASH_LONG, PARTICLE_END };

public:
	typedef struct
	{
		PARTICLE_TYPE	eParticleType;	// 파티클 타입
		_uint			iNumInstance;	// 파티클 개수

		_float3			vPivot;
		_float3			vCenter;
		_float3			vRange;
		_float2			vSize;
		_float2			vSpeed;

		_float2			vLifeTime;		// 수명
		_bool			isLoop;			// 반복
		_float4			vColor;			// 색깔
		_float			fDuration;		// 움직이는 시간

		_bool			isSetDir = false;
		_float3			vDirRange = { 0.f, 0.f, 0.f };
		_float2			vDirX = { 0.f, 0.f };
		_float2			vDirY = { 0.f, 0.f };
		_float2			vDirZ = { 0.f, 0.f };

		_uint			iShaderPass = 0;
		_bool			isTexture = false;


	} PARTICLE_DESC;

public:
	CParticle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CParticle(const CParticle& rhs);
	~CParticle();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	// 파티클 발생 위치(월드), 파티클 이동 방향 (월드)
	void	Play(_vector vCenter, _vector vDir);
	void	Play(_vector vCenter);

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CTexture> m_pTextureCom = { nullptr };
	shared_ptr<CVIBuffer_Particle_Point> m_pVIBufferCom = { nullptr };

private:
	PARTICLE_DESC m_ParticleDesc = {};

private:
	HRESULT Add_Component();


public:
	static	shared_ptr<CParticle> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END