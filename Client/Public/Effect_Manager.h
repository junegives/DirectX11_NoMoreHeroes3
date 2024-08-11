#pragma once

#include "Client_Defines.h"

BEGIN(Engine)


END

BEGIN(Client)

class CEffect_Manager final
{
	DECLARE_SINGLETON_SMART(CEffect_Manager)
public:
	CEffect_Manager();
	~CEffect_Manager() = default;

public:
	void						AddEffect(const wstring& strEffectTag, shared_ptr<class CEffect> pEffect);
	void						RemoveEffect(const wstring& strEffectTag);
	shared_ptr<class CEffect>	GetEffect(const wstring& strEffectTag);

	void						PlayEffect(const wstring& strEffectTag, _uint iNum, _vector vCenter, _vector vUp, _vector vLook);
	void						PlayEffect(const wstring& strEffectTag, _uint iNum, _vector vCenter, _bool isRotate, _vector vAxis = { 0.f, 0.f, 0.f, 0.f }, _vector vRotation = { 0.f, 0.f, 0.f, 0.f });
	void						PlayEffect(const wstring& strEffectTag, shared_ptr<CTransform> pParentTransform);

	void						AddParticle(const wstring& strParticleTag, shared_ptr<class CParticle> pParticle);
	void						RemoveParticle(const wstring& strParticleTag);
	shared_ptr<class CParticle>	GetParticle(const wstring& strParticleTag);

	void						PlayParticle(const wstring& strParticleTag, _vector vCenter, _vector vDir = {0.f, 0.f, 0.f, 0.f});

private:
	shared_ptr<class CGameInstance>							m_pGameInstance = { nullptr };
	map<const wstring, shared_ptr<class CEffect>>			m_Effects;
	map<const wstring, shared_ptr<class CParticle>>			m_Particles;
};

END