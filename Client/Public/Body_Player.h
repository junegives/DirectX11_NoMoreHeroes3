#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

/* 플레이어의 각 파츠를 모아서 가지고 있는 객체. */

BEGIN(Engine)
class CShader;
class CBinaryModel;
END

BEGIN(Client)

class CBody_Player : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		_uint* pPlayerState;
	}BODY_PLAYER_DESC;

public:
	typedef struct
	{
		map<_uint, _float4x4*> BoneMatrix;
		_float4x4	WorldMatrix;
	}AFTERIMAGE_DESC;

public:
	CBody_Player(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CBody_Player(const CBody_Player& rhs);
	virtual ~CBody_Player() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	SetNaviCom(shared_ptr<CNavigation> pNavigation) { m_pNavigationCom = pNavigation; }
	void	Set_Animation(_uint iAnimIndex, _bool isLoop, _float fAnimSpeed, _float fChangeDuration, _uint iStartNumKeyFrames = 0);

	void	Set_Animation_TransModel(_uint iAnimIndex, _bool isLoop, _float fAnimSpeed, _float fChangeDuration, _uint iStartNumKeyFrames = 0);

public:
	void	ChangeModel(_bool isToUpgrade);
	void	PlayAllModel(_bool isUseAll);

public:
	void	Change_NaviCom(shared_ptr<CNavigation> pNavigation) { m_pNavigationCom = pNavigation; }

public:
	void	SetAfterImage(_bool isAfterImage) { m_isAfterImage = isAfterImage; }
	_bool	IsAfterImage() { return m_isAfterImage; }

private:
	shared_ptr<CShader>			m_pShaderCom = { nullptr };
	shared_ptr<CBinaryModel>	m_pModelCom = { nullptr };
	shared_ptr<CBinaryModel>	m_pTransModelCom = { nullptr };
	shared_ptr<CNavigation>		m_pNavigationCom = { nullptr };

private:
	_uint*	m_pPlayerState = { nullptr };
	_uint	m_prevPlayerState = STATE_IDLE;

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

private:
	_uint	m_iAnimationIndex = 0;
	_bool	m_bPressed[2] = { false };
	_bool	m_bAnimChanging = false;

	_float4	m_PrevTransPos = {};

// 상태 제어
private:
	_bool	m_isTransformed = false;
	_bool	m_isUseAll = false;

private:
	_bool								m_isAfterImage = false;
	list<AFTERIMAGE_DESC>				m_AfterImageList;


public:
	static shared_ptr<CBody_Player> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END