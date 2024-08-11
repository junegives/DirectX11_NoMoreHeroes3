#include "pch.h"
//#include "SkyBox.h"
//#include"Export_Utility.h"
//
//CSkyBox::CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev)
//	: Engine::CGameObject(pGraphicDev)
//{
//}
//
//CSkyBox::CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev, const tstring& _keyName)
//	: Engine::CGameObject(pGraphicDev), m_strKeyName(_keyName)
//{
//}
//
//CSkyBox::CSkyBox(const CSkyBox& rhs)
//	: Engine::CGameObject(rhs)
//{
//}
//
//CSkyBox::~CSkyBox()
//{
//}
//
//HRESULT CSkyBox::ReadyGameObject()
//{
//	m_pTransformCom->SetScale(40.f, 40.f, 40.f);
//
//	return S_OK;
//}
//
//_int CSkyBox::UpdateGameObject(const _float& fTimeDelta)
//{
//	Engine::AddRenderGroup(RENDER_PRIORITY, shared_from_this());
//	_int	iExit = CGameObject::UpdateGameObject(fTimeDelta);
//
//	return iExit;
//}
//
//void CSkyBox::LateUpdateGameObject()
//{
//	_matrix		matCamWorld;
//	
//	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matCamWorld);
//	D3DXMatrixInverse(&matCamWorld, 0, &matCamWorld);
//	
//	m_pTransformCom->SetPosition(matCamWorld._41, matCamWorld._42 + 10.f, matCamWorld._43);
//}
//
//void CSkyBox::RenderGameObject()
//{
//	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());
//	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
//
//	m_pTextureCom->SetTextureKey(m_strKeyName, TEX_CUBE);
//	m_pTextureCom->SetTexture(0);
//
//	dynamic_pointer_cast<CCubeTex>(m_pBufferCom)->RenderBuffer();
//
//	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
//}
//
//void CSkyBox::AddComponent()
//{
//	shared_ptr<CComponent> pComponent;
//
//	pComponent = m_pBufferCom = (make_shared<CCubeTex>(m_pGraphicDev));
//	dynamic_pointer_cast<CCubeTex>(m_pBufferCom)->ReadyBuffer();
//	m_mapComponent[ID_STATIC].insert({ L"Com_Weald_SkyBoxTexture", pComponent });
//
//	pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
//	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });
//
//	pComponent = m_pTransformCom = make_shared<CTransform>(_vec3(0.f, 0.f, 0.f), _vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f));
//	//m_pTransformCom->ReadyTransform();
//	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });
//}
