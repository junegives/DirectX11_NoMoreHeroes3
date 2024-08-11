#include "pch.h"

#include "Effect_Manager.h"
#include "Particle.h"
#include "Effect.h"

IMPLEMENT_SINGLETON_SMART(CEffect_Manager)

CEffect_Manager::CEffect_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
}

void CEffect_Manager::AddEffect(const wstring& strEffectTag, shared_ptr<class CEffect> pEffect)
{
	m_Effects.emplace(strEffectTag, pEffect);
}

void CEffect_Manager::RemoveEffect(const wstring& strEffectTag)
{
	auto iter = m_Effects.find(strEffectTag);

	if (iter != m_Effects.end())
		m_Effects.erase(iter);
}

shared_ptr<CEffect> CEffect_Manager::GetEffect(const wstring& strEffectTag)
{
	return m_Effects.find(strEffectTag)->second;
}

void CEffect_Manager::PlayEffect(const wstring& strEffectTag, _uint iNum, _vector vCenter, _vector vUp, _vector vLook)
{
	shared_ptr<CEffect>	pEffect = m_Effects.find(strEffectTag)->second;

	pEffect->Play(iNum, vCenter, vCenter, vUp, vLook);
	//pEffect->Play(vCenter, vDir);
}

void CEffect_Manager::PlayEffect(const wstring& strEffectTag , _uint iNum, _vector vCenter, _bool isRotate, _vector vAxis, _vector vRotation)
{
	shared_ptr<CEffect> pEffect = m_Effects.find(strEffectTag)->second;

	pEffect->Play(iNum, vCenter, isRotate, vAxis, vRotation);
}

void CEffect_Manager::PlayEffect(const wstring& strEffectTag, shared_ptr<CTransform> pParentTransform)
{
	shared_ptr<CEffect> pEffect = m_Effects.find(strEffectTag)->second;
	
	pEffect->Play(pParentTransform);
}

void CEffect_Manager::AddParticle(const wstring& strParticleTag, shared_ptr<class CParticle> pParticle)
{
	m_Particles.emplace(strParticleTag, pParticle);
}

void CEffect_Manager::RemoveParticle(const wstring& strParticleTag)
{
	auto iter = m_Particles.find(strParticleTag);

	if (iter != m_Particles.end())
		m_Particles.erase(iter);
}

shared_ptr<CParticle> CEffect_Manager::GetParticle(const wstring& strParticleTag)
{
	return m_Particles.find(strParticleTag)->second;
}

void CEffect_Manager::PlayParticle(const wstring& strParticleTag, _vector vCenter, _vector vDir)
{
	shared_ptr<CParticle>	pParticle = m_Particles.find(strParticleTag)->second;

	pParticle->Play(vCenter, vDir);
}
