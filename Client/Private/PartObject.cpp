#include "pch.h"
#include "PartObject.h"

CPartObject::CPartObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CGameObject(pDevice, pContext)
{
}

CPartObject::CPartObject(const CPartObject& rhs)
    : CGameObject(rhs)
{
}

HRESULT CPartObject::Initialize(void* pArg)
{
	PARTOBJ_DESC* pPartObjDesc = (PARTOBJ_DESC*)pArg;

	m_pParentTransform = pPartObjDesc->pParentTransform;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPartObject::Priority_Tick(_float fTimeDelta)
{
}

void CPartObject::Tick(_float fTimeDelta)
{
}

void CPartObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CPartObject::Render()
{
    return S_OK;
}

void CPartObject::Free()
{
    __super::Free();
}
