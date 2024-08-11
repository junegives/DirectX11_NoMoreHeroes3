#pragma once

#include "Client_Defines.h"
//#include "GameObject.h" 
//
//BEGIN(Engine)
//class CShader;
//class CTexture;
//class CTransform;
//class CVIBuffer_Cube;
//END
//
//BEGIN(Client)
//
//class CSkyBox final : public CGameObject
//{
//public:
//	CSkyBox(ComPtr< pGraphicDev);
//	CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev, const tstring& _keyName);
//	CSkyBox(const CSkyBox& rhs);
//	virtual ~CSkyBox();
//
//public:
//	virtual HRESULT ReadyGameObject() override;
//	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
//	virtual void LateUpdateGameObject() override;
//	virtual void RenderGameObject() override;
//
//private:
//	virtual void			AddComponent();
//
//private:
//	shared_ptr<CCubeTex> m_pBufferCom = nullptr;
//	shared_ptr<CTexture> m_pTextureCom = nullptr;
//	shared_ptr<CTransform> m_pTransformCom = nullptr;
//
//	_float		m_fSpeed = 10.f;
//	tstring		m_strKeyName;
//};
