#include "pch.h"
#include "Effect.h"

CEffect::CEffect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffect::CEffect(const CEffect& rhs)
	: CGameObject(rhs)
{
}

CEffect::~CEffect()
{
}

HRESULT CEffect::Initialize(void* pArg)
{
	m_EffectDesc = *(EFFECT_DESC*)pArg;

	CGameObject::GAMEOBJECT_DESC			GameObjectDesc = {};

	GameObjectDesc.iGameObjectData = 10;
	GameObjectDesc.fSpeedPerSec = 10.0f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(60.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

void CEffect::Priority_Tick(_float fTimeDelta)
{
}

void CEffect::Tick(_float fTimeDelta)
{
}

void CEffect::Late_Tick(_float fTimeDelta)
{
}

HRESULT CEffect::Render()
{
	return S_OK;
}

HRESULT CEffect::Add_Component()
{
	return S_OK;
}

void CEffect::Free()
{
}
