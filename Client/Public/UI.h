#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI : public CGameObject
{
public:
	typedef struct
	{
		_float					fX, fY, fCX, fCY;
	} UIDESC;

public:
	CUI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_vector						ChangeToWorldPos(_vector vPos);
	_vector						ChangeToWindowPos(_vector vPos);
	void						WorldToScreenPos(_vector vPos, _float fTop);

	void						SetVisible(_bool isVisible) { m_isVisible = isVisible; }
	_bool						GetVisible() { return m_isVisible; }

	void						MoveTo(_bool isDir, _fvector vTarget, _float fSpeed, _float fTime);
	void						MoveTo(_fvector vPos);

	void						CalcCorner();
	void						CalcCorner(_vector vPos, _float3 fSize);

protected:
	shared_ptr<CShader>			m_pShaderCom = { nullptr };
	shared_ptr<CVIBuffer_Rect>	m_pVIBufferCom = { nullptr };

protected:
	UIDESC						m_UIDesc;
	_float2						m_fLT, m_fRT, m_fLB, m_fRB;

	_float4x4					m_ViewMatrix, m_ProjMatrix;
	const wstring				m_strTextureComTag = {};

protected:
	_bool						m_isVisible;
	_float						m_fTimeDelta;

	_bool						m_isMoving = false;
	_bool						m_isMovToDir = false;
	_float						m_fMoveTime = 0.f;
	_bool						m_fMoveSpeed = 0.f;

	_vector						m_vMovTarget;

protected:
	HRESULT Add_Component();

public:
	static shared_ptr<CUI> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END