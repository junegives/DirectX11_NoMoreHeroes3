#pragma once

#include "Tool_Defines.h"
#include "LandObject.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CNavigation;
class CBinaryModel;
END

BEGIN(Tool)

class CPlayer final : public CLandObject
{
public:
	CPlayer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	shared_ptr<class CComponent> Get_PartObjectComponent(const wstring& strPartObjTag, const wstring& strComTag);

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	shared_ptr<CCollider>		m_pColliderCom = { nullptr };
	_uint						m_iState = { 0 };
	STATE						m_eCurState = STATE_END;

private:
	shared_ptr<CBinaryModel>	m_pBodyModel;
	shared_ptr<CBinaryModel>	m_pBodyTransModel;
	shared_ptr<CShader>			m_pShaderCom = { nullptr };
	shared_ptr<CBinaryModel>	m_pModelCom = { nullptr };


private:
	_float		m_fTimeDelta = 0.f;

	_vector		m_vPrevPos;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

	void	Key_Input(_float fTimeDelta);

public:
	void	Change_NaviCom();

public:
	static shared_ptr<CPlayer> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END