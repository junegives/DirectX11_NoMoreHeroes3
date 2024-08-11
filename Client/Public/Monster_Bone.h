#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CMonster_Bone : public CMonster
{
public:
    CMonster_Bone(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMonster_Bone(const CMonster_Bone& rhs);
    virtual ~CMonster_Bone() = default;

public:
    shared_ptr<class CComponent> Get_PartObjectComponent(const wstring& strPartObjTag, const wstring& strComTag);

    virtual HRESULT Initialize(void* pArg = nullptr) override;
    virtual void	Priority_Tick(_float fTimeDelta) override;
    virtual void	Tick(_float fTimeDelta) override;
    virtual void	Late_Tick(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    virtual void	OnCollision(LAYER_ID eColLayer, shared_ptr<class CCollider> pCollider) override;

private:
    HRESULT         Add_Component();
    HRESULT         Bind_ShaderResources();
    HRESULT         Add_PartObjects();
    HRESULT         Add_States();

    void            Collision_ToPlayer();
public:
    _bool			IsVisible() { return m_isVisible; }
    _bool           SetDamage();

public:
    STATE						m_eCurState = STATE_END;

private:
    _bool						m_isVisible = true;
    shared_ptr<CCollider>		m_pAttackColliderCom = { nullptr };
    map<const wstring, shared_ptr<class CPartObject>>			m_MonsterParts;

    shared_ptr<class CUI_HP_Monster>  m_pUI_HP = nullptr;


public:
    static shared_ptr<CMonster_Bone> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg = nullptr);
    virtual void Free() override;
};

/*
    STATE_LEOPARDON_END,
    
    // Monster_Bone
    // ATK : 2
    STATE_BONE_ATTACK, STATE_BONE_ATTACK2,
    
    // DMG : 14
    STATE_BONE_DOWN_B, STATE_BONE_DOWN_OUT_B, STATE_BONE_DOWN, STATE_BONE_DOWN_OUT,
    STATE_BONE_HIT_HEAVY_B, STATE_BONE_HIT_HEAVY, STATE_BONE_HIT_HEAVY_L, STATE_BONE_HIT_HEAVY_R,
    STATE_BONE_HIT_LIGHT, STATE_BONE_HIT_LIGHT_L, STATE_BONE_HIT_LIGHT_R,
    STATE_BONE_HIT_STRUCKDOWN_B, STATE_BONE_HIT_STRUCKDOWN,
    STATE_BONE_THROW,
    
    // DTH : 5
    STATE_BONE_DOWN_DEATH_B, STATE_BONE_DOWN_DEATH,
    STATE_BONE_DOWN_FREEZE_B, STATE_BONE_DOWN_FREEZE, STATE_BONE_THROW_DEATH,
    
    // IDL : 5
    STATE_BONE_IDLE_APPEAR, STATE_BONE_IDLE_APPEAR2, STATE_BONE_IDLE,
    STATE_BONE_IDLE_FIND, STATE_BONE_IDLE_FIND2,
    
    // MOV : 5
    STATE_BONE_RUN, STATE_BONE_WALK_B, STATE_BONE_WALK, STATE_BONE_WALK_L, STATE_BONE_WALK_R,
    
    STATE_BONE_END,
*/

END