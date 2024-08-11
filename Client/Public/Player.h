#pragma once

#include "Client_Defines.h"
#include "LandObject.h"

#include "Sound_Manager.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CNavigation;
class CBinaryModel;
class CStateMachine;
END

BEGIN(Client)

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

public:
	virtual void OnCollision(LAYER_ID eColLayer, shared_ptr<class CCollider> pCollider) override;

public:
	shared_ptr<CBinaryModel>	GetBodyModel() { return m_pBodyModel; }

	void						SetBike(shared_ptr<class CBike> pBike);
	shared_ptr<class CBike>		GetBike() { return m_pBike; }

	void						SetWeapon(shared_ptr<class CWeapon> pWeapon) { m_pWeapon = pWeapon; }
	shared_ptr<class CWeapon>	GetWeapon() { return m_pWeapon; }

	_bool						GetClearBattle1() { return m_isClearBattle1; }
	void						SetClearBattle1(_bool isClear) { m_isClearBattle1 = isClear; }
	void						SetClearBattle2(_bool isClear) { m_isClearBattle2 = isClear; }
	_bool						IsClearSubBattle() { return m_isClearBattle1 && m_isClearBattle2; }

	void						SetWeaponAttach(_bool isWeaponAttached);
	_bool						IsWeaponAttached() { return m_isWeaponAttached; }

	_bool						SetDamage();

	STATE						GetCurState() { return m_eCurState; }

	void						SetBattleEnd(_bool isBattleEnd) { m_isBattleEnd = isBattleEnd; }
	_bool						IsBattleEnd() { return m_isBattleEnd; }

	void						SetCurBGM(const wstring& strBGM) { m_strCurBGM = strBGM; }

private:
	shared_ptr<CCollider>		m_pColliderCom = { nullptr };
	shared_ptr<CStateMachine>	m_pStateMachineCom = { nullptr };
	_uint						m_iState = { 0 };
	STATE						m_eCurState = STATE_END;

	wstring						m_strCurBGM = L"";

private:
	map<const wstring, shared_ptr<class CPartObject>>		m_PlayerParts;
	shared_ptr<class CBody_Player>	m_pBody;
	shared_ptr<CBinaryModel>		m_pBodyModel;
	shared_ptr<CBinaryModel>		m_pBodyTransModel;

	_bool							m_isWeaponAttached = false;

	shared_ptr<CGameObject>			m_pTarget = nullptr;

	shared_ptr<class CBike>			m_pBike = nullptr;
	shared_ptr<CTransform>			m_pBikeTransformCom = nullptr;

	shared_ptr<class CWeapon>		m_pWeapon = nullptr;
	shared_ptr<CCollider>			m_pWeaponCollider;

	_bool							m_isBattleEnd = false;

	_bool							m_isColTrigger = false;
	_bool							m_isPrevColTrigger = false;

private:
	HIT_TYPE	m_eHitType = HIT_LIGHT;
	DIRECTION	m_eHitDir = DIR_F;
	_bool		m_isHit = false;

	LEVEL		m_eNextLevel = LEVEL_END;
	_bool		m_isSceneChange = false;

	_float		m_fTimeDelta = 0.f;

	_bool		m_isOnPerfectDodge = false;

	_bool		m_isOnBattle = true;
	_bool		m_isOnBattleNow = false;
	_bool		m_isOnBike = false;
	_bool		m_isOnBikeNow = false;

	_bool		m_isNextTickTurn = false;
	_float		m_fNextTickTurnRadian = 0.f;
	_vector		m_vNextTickTurnAxis = { 0.f, 0.f, 0.f, 0.f };

	_vector		m_vPrevPos;

	_bool		m_isClearBattle1 = false;
	_bool		m_isClearMiniGame = false;
	_bool		m_isClearBattle2 = false;

	_bool		m_isOnMiniGameNow = false;
	_bool		m_isJump = false;

	_uint		m_iGold = 0;

public:
	HIT_TYPE	GetHitType() { return m_eHitType; }
	DIRECTION	GetHitDir() { return m_eHitDir; }

	_bool		IsHit() { return m_isHit; }
	void		SetHit(_bool isHit) { m_isHit = isHit; }

	_bool		IsOnPerfectDodge() { return m_isOnPerfectDodge; }
	void		SetOnPerfectDodge(_bool isOnPerfectDodge) { m_isOnPerfectDodge = isOnPerfectDodge; }

	_bool		IsOnBike() { return m_isOnBike; }
	void		SetOnBike(_bool isOnBike) { m_isOnBike = isOnBike; }

	_bool		IsOnBikeNow() { return m_isOnBikeNow; }
	void		SetOnBikeNow(_bool isOnBikeNow);

	_bool		IsOnMiniGameNow() { return m_isOnMiniGameNow; }
	void		SetOnMiniGameNow(_bool isOnMiniGameNow);

	_bool		IsOnBattle() { return m_isOnBattle; }
	void		SetOnBattle(_bool isOnBattle) { m_isOnBattle = isOnBattle; }

	_bool		IsOnBattleNow() { return m_isOnBattleNow; }
	void		SetOnBattleNow(_bool isOnBattleNow) { m_isOnBattleNow = isOnBattleNow; }

	void		SetAnimLerp(_float fTime) { m_fAnimLerpTime = fTime; }
	_float		GetAnimLerp() { if (m_isAnimLerpChange) { m_isAnimLerpChange = false; return m_fAnimLerpTime; } else return -1.f; }

	void		SetAnimLerpChange(_bool isAnimLerpChange) { m_isAnimLerpChange = isAnimLerpChange; }
	_bool		IsAnimLerpChange() { return m_isAnimLerpChange; }

	void		SetSceneChange(_bool isSceneChange) { m_isSceneChange = isSceneChange; }
	_bool		IsSceneChange() { return m_isSceneChange; }

	void		SetNextLevel(LEVEL eLevel) { m_eNextLevel = eLevel; }
	LEVEL		GetNextLevel() { return m_eNextLevel; }

	void		SetNextTickTurn(_fvector vAxis, _float fRadian);

	void		SetJump(_bool isJump) { m_isJump = isJump; }
	_bool		IsJump() { return m_isJump; }

	void						SetTarget(shared_ptr<CGameObject> pTarget) { m_pTarget = pTarget; }
	shared_ptr<CGameObject>		GetTarget() { return m_pTarget; }
	shared_ptr<CGameObject>		SearchTarget();

	void						SetGold(_uint iGold) { m_iGold = iGold; }
	void						GainGold(_uint iGold) { m_iGold += iGold; }
	_bool						UseGold(_uint iGold) { if (m_iGold <= iGold) return false; m_iGold -= iGold; return true; }
	_uint						GetGold() { return m_iGold; }

public:
	void	Change_NaviCom(const LEVEL& eCurLevel);

private:
	HRESULT Add_Components();
	HRESULT Add_PartObjects();
	HRESULT Add_States();

	void	Key_Input(_float fTimeDelta);

public:
	_bool						m_isAnimLerpChange = false;
	_float						m_fAnimLerpTime = 0.2f;

public:
	static shared_ptr<CPlayer> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END