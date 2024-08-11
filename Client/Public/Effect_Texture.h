#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CEffect_Texture : public CEffect
{
public:
	CEffect_Texture(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strTextureTag, const wstring& strMaskTextureTag);
	CEffect_Texture(const CEffect_Texture& rhs);
	~CEffect_Texture();

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Component();

public:
	// 이펙트 발생 위치, 이동 방향
	void	Play(_uint iNum, _vector vCenter, _vector vScale, _vector vUp, _vector vLook);
	void	Play(_uint iNum, _vector vCenter, _bool isRotate, _vector vAxis = {0.f, 0.f, 0.f, 0.f}, _vector vRotate = {0.f, 0.f, 0.f, 0.f});
	virtual void	Play(shared_ptr<CTransform> pParentTransform);

private:
	shared_ptr<CShader> m_pShaderCom = { nullptr };
	shared_ptr<CVIBuffer_Rect> m_pVIBufferCom = { nullptr };

	shared_ptr<CTexture> m_pTextureCom = { nullptr };
	shared_ptr<CTexture> m_pMaskTextureCom = { nullptr };

private:
	wstring m_strTextureTag;
	wstring m_strMaskTextureTag;
	_float	m_fActiveTime = 0.f;
	_float	m_fSpeed = 0.f;

	_vector	m_vStartPos;
	_vector m_vEndPos;

	_uint	m_iNum = 1;

	_float2	m_vUVPivot = { 0.f, 0.f };
	_float2	m_vUVScale = { 1.f, 1.f };
	_uint	m_iUVCnt = 0;
	_float	m_fFrameTime = 0.f;

	_uint	m_iUVIdx = 0;

	shared_ptr<CTransform>	m_pTransformArray[5];


public:
	static	shared_ptr<CEffect_Texture> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strTextureTag, const wstring& strMaskTextureTag, void* pArg = nullptr);
	virtual void Free() override;
};

END