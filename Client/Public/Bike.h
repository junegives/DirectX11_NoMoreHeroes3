#pragma once

#include "Client_Defines.h"
#include "LandObject.h"

#include "Sound_Manager.h"

BEGIN(Engine)
class CShader;
class CBinaryModel;
END

BEGIN(Client)

class CBike : public CLandObject
{
public:
	typedef struct
	{
		shared_ptr<CTransform> pParentTransform;
		_float4x4* pSocketMatrix;
	}BIKE_DESC;

public:
	CBike(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CBike(const CBike& rhs);
	virtual ~CBike() = default;
	
public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void			Set_Attached(_bool isAttached);
	_bool			Get_Attached() { return m_isAttached; }

	void			Set_Speed(_float fSpeed) { m_fSpeed = fSpeed; }
	_float			Get_Speed() { return m_fSpeed; }

	void			Speed_Up(_float fSpeed) { m_fSpeed += fSpeed; if (m_fSpeed >= m_fMaxSpeed) m_fSpeed = m_fMaxSpeed; };
	void			Speed_Down(_float fSpeed) { if (m_fSpeed >= fSpeed) m_fSpeed = 0; else m_fSpeed -= fSpeed; };

	void			SetNaviCom(shared_ptr<CNavigation> pNavigation) { m_pNavigationCom = pNavigation; }

	_float4x4*		Get_SocketMatrix() { return m_pSocketMatrix; }
	_float4x4*		Get_RootMatrix() { return m_pRootMatrix; }

	void			SetOnMiniGame(_bool isOnMiniGame);
	_bool			IsOnMiniGame() { return m_isOnMiniGame; }

	void			SetOnMiniGameNow(_bool isOnMiniGameNow);
	_bool			IsOnMiniGameNow() { return m_isOnMiniGameNow; }

	void			SetGameStart(_bool isGameStart) { m_isGameStart = isGameStart; }
	_bool			IsGameStart() { return m_isGameStart; }

	void			SetRun(_bool isRun) { m_isRun = isRun; }
	_bool			IsRun() { return m_isRun; }

private:
	shared_ptr<CShader>			m_pShaderCom = { nullptr };
	shared_ptr<CBinaryModel>	m_pModelCom = { nullptr };
	shared_ptr<CCollider>		m_pColliderCom = { nullptr };
	shared_ptr<CStateMachine>	m_pStateMachineCom = { nullptr };

private:
	_uint						m_iState = { 0 };
	STATE						m_eCurState = STATE_END;

private:
	_float4x4*					m_pSocketMatrix = { nullptr };
	_float4x4*					m_pRootMatrix = { nullptr };
	_bool						m_isAttached = false;
	_bool						m_isRotated = false;

	_float						m_fSpeed = 0.f;
	_float						m_fSpeedChange = 1.f;
	_float						m_fMaxSpeed = 20.f;

	_bool						m_isOnMiniGame = false;
	_bool						m_isOnMiniGameNow = false;

	_bool						m_isGameStart = false;
	_bool						m_isRun = false;

public:
	HRESULT						Add_Components();
	HRESULT						Add_States();
	HRESULT						Bind_ShaderResources();

	void						Key_Input(_float fTimeDelta);
	void						Change_NaviCom(_uint iStartCellIdx, shared_ptr<CNavigation> pNavigation);

public:
	static shared_ptr<CBike>	Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg = nullptr);
	virtual void				Free() override;
};

END