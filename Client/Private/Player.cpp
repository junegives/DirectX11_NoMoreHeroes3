#include "pch.h"
#include "Player.h"
#include "PartObject.h"
#include "Body_Player.h"
#include "Weapon.h"
#include "Projectile.h"
#include "Bike.h"

#include "UI_Manager.h"
#include "UI_HP_Player.h"
#include "UI_QuestBoard.h"

#include "Trigger.h"
#include "Sign.h"

#include "Effect_Manager.h"

#include "StateMachine.h"
#include "State_Run.h"
#include "State_Idle.h"
#include "State_Attack.h"
#include "State_Dodge.h"
#include "State_Jump.h"
#include "State_KillerSlash.h"
#include "State_DeathKick.h"
#include "State_HeavyAttack.h"
#include "State_DashAttack.h"
#include "State_Guard.h"
#include "State_PerfectDodge.h"
#include "State_Hit.h"
#include "State_Transform.h"
#include "State_Armor_Missile.h"
#include "State_Throw.h"
#include "State_Rising.h"
#include "State_Appear.h"

#include "Sound_Manager.h"


// Vehicle
#include "State_Vehicle_Idle.h"
#include "State_Vehicle_Run.h"
#include "State_Vehicle_Call.h"
#include "State_Vehicle_Turn.h"
#include "State_Vehicle_Back.h"
#include "State_Vehicle_Jump.h"

#include "GameInstance.h"
#include "Layer.h"

CPlayer::CPlayer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLandObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CLandObject(rhs)
{
}

shared_ptr<class CComponent> CPlayer::Get_PartObjectComponent(const wstring& strPartObjTag, const wstring& strComTag)
{
	auto	iter = m_PlayerParts.find(strPartObjTag);
	if (iter == m_PlayerParts.end())
		return nullptr;

	return iter->second->Find_Component(strComTag);
}

HRESULT CPlayer::Initialize(void* pArg)
{
	m_fColRadius = 0.9f;

	LANDOBJ_DESC* pGameObjectDesc = (LANDOBJ_DESC*)pArg;

	pGameObjectDesc->fSpeedPerSec = 10.f;
	pGameObjectDesc->fRotationPerSec = XMConvertToRadians(150.0f);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_States()))
		return E_FAIL;

	m_pGameInstance->Set_Player(shared_from_this());

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ -6.05f, 3.3f, -8.4f, 1.f });
	
	_vector vStartPos = { 0.f, 0.f, 0.f, 0.f };

	m_pNavigationCom->GetCellCenterPos(&vStartPos);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vStartPos);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-53.8f, 8.f, 274.f, 1.f));

	m_iHP = 100;

	return S_OK;
}

void CPlayer::Priority_Tick(_float fTimeDelta)
{
	for (auto& Pair : m_PlayerParts)
		(Pair.second)->Priority_Tick(fTimeDelta);
}

void CPlayer::Tick(_float fTimeDelta)
{
	m_vPrevPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	   
	m_fTimeDelta = fTimeDelta;

	Key_Input(fTimeDelta);

	if (m_isNextTickTurn)
	{
		m_pTransformCom->TurnRadian(m_vNextTickTurnAxis, m_fNextTickTurnRadian);
		m_isNextTickTurn = false;
	}

	_vector vLookOrigin = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	m_pStateMachineCom->Tick(fTimeDelta);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	m_eCurState = m_pStateMachineCom->Get_CurState();
	vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	for (auto& Pair : m_PlayerParts)
		(Pair.second)->Tick(fTimeDelta);
	vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	//SetUp_OnTerrain(m_pTransformCom);
	if (STATE::STATE_JUMP != m_pStateMachineCom->Get_CurState()
		&& STATE::STATE_VEHICLE_IDLE != m_pStateMachineCom->Get_CurState()
		&& STATE::STATE_VEHICLE_RUN != m_pStateMachineCom->Get_CurState()
		&& STATE::STATE_VEHICLE_FAST != m_pStateMachineCom->Get_CurState()
		&& STATE::STATE_VEHICLE_BACK_L != m_pStateMachineCom->Get_CurState()
		&& STATE::STATE_RISING != m_pStateMachineCom->Get_CurState())
	{
		SetUp_OnNavi(m_pTransformCom);

		// 오토바이 y값 조정
		/*_vector vPivot = { 0.f, 1.f, 0.f, 0.f };
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vPivot);*/
	}
	else
		Jump_OnNavi(m_pTransformCom);
	vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	if (STATE_VEHICLE_IDLE == m_eCurState || STATE_VEHICLE_FAST == m_eCurState || STATE_VEHICLE_RUN == m_eCurState ||
			STATE_VEHICLE_BACK_L == m_eCurState || STATE_VEHICLE_FALL == m_eCurState)
	{
		if (m_pBike)
		{
			if (m_pBike->Get_Attached())
			{
				_float4x4 WorldMatrix;

				_matrix	BoneMatrix = XMLoadFloat4x4(m_pBike->Get_SocketMatrix());
				_matrix RootMatrix = XMLoadFloat4x4(m_pBike->Get_RootMatrix());

				for (size_t i = 0; i < 3; i++)
				{
					RootMatrix.r[i] = XMVector3Normalize(RootMatrix.r[i]);
				}

				RootMatrix.r[3] = BoneMatrix.r[3];
				RootMatrix.r[3] += XMVectorSet(0.f, 0.f, -0.274f, 0.f);

				XMStoreFloat4x4(&WorldMatrix, RootMatrix  * m_pBikeTransformCom->Get_WorldMatrix());
				
				m_pTransformCom->Set_WorldMatrix(WorldMatrix);

				
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(WorldMatrix.m[3][0], WorldMatrix.m[3][1] + 0.35f, WorldMatrix.m[3][2], 1.f));
				m_pTransformCom->TurnRadian({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180));
			}

			else
			{
				int b = 3;
				
			}
		}
	}

	vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);



	if (m_isBattleEnd)
	{
		m_isBattleEnd = false;

		SetWeaponAttach(false);
		m_pGameInstance->Set_CurLevel(LEVEL_VILLAGE);
		Change_NaviCom(LEVEL_VILLAGE);
		CSound_Manager::GetInstance()->StopAll();
		CSound_Manager::GetInstance()->PlayBGM(L"BGM_Village_Casual.mp3", 0.6f);
		m_isOnBattle = false;

		// 배틀 2였을 때
		if (m_isClearBattle1 && m_isClearMiniGame)
		{

		}

		// 미니게임이었을 때
		else if (m_isClearBattle1)
		{
			m_isClearMiniGame = true;

			dynamic_pointer_cast<CUI_QuestBoard>(m_pGameInstance->Find_GameObject(LEVEL_VILLAGE, LAYER_UI, TEXT("GameObject_UI_QuestBoard")))->SetQuest(CUI_QuestBoard::QUEST_BOSS);

			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_VILLAGE, LAYER_TELEPORT, TEXT("GameObject_Trigger_Battle2"),
				CTrigger::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Trigger"), CTrigger::TRIGGER_BATTLE2, true))))
				return;

			/*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_VILLAGE, LAYER_ENVIRONMENT, TEXT("GameObject_Sign_Battle2"),
				CSign::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Sign_SpaceShip"), CSign::SIGN_BATTLE2, true))))
				return;*/
		}

		// 배틀 1이었을 때
		else
		{
			m_isClearBattle1 = true;

			dynamic_pointer_cast<CUI_QuestBoard>(m_pGameInstance->Find_GameObject(LEVEL_VILLAGE, LAYER_UI, TEXT("GameObject_UI_QuestBoard")))->SetQuest(CUI_QuestBoard::QUEST_GOLD);

			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_VILLAGE, LAYER_TELEPORT, TEXT("GameObject_Trigger_MiniGame"),
				CTrigger::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Trigger"), CTrigger::TRIGGER_MINIGAME, true))))
				return;

			/*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_VILLAGE, LAYER_ENVIRONMENT, TEXT("GameObject_Sign_MiniGame"),
				CSign::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Model_Sign_Coin"), CSign::SIGN_MINIGAME, true))))
				return;*/
		}
	}
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
	//m_pWeaponCollider->Tick(m_pTransformCom->Get_WorldMatrix());

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
		return;
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	m_pStateMachineCom->Late_Tick(fTimeDelta);
	vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	for (auto& Pair : m_PlayerParts)
		(Pair.second)->Late_Tick(fTimeDelta);
	vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

#ifdef _DEBUG
	// m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
	// m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
	vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	m_isPrevColTrigger = m_isColTrigger;
	m_isColTrigger = false;
}

HRESULT CPlayer::Render()
{
	if (m_isPrevColTrigger && !m_isColTrigger)
	{
		CSound_Manager::GetInstance()->StopSound(CSound_Manager::CHANNELID::BGM);
		CSound_Manager::GetInstance()->PlayBGM(m_strCurBGM.c_str(), 0.7f);
	}

#ifdef _DEBUG
	/*m_pNavigationCom->Render();
	m_pColliderCom->Render();*/

	wstring strState;

	switch (m_eCurState)
	{
	case Engine::STATE_ATTACK:
		strState = TEXT("STATE_ATTACK");
		break;
	case Engine::STATE_DASHATTACK:
		strState = TEXT("STATE_DASHATTACK");
		break;
	case Engine::STATE_EAT:
		strState = TEXT("STATE_EAT");
		break;
	case Engine::STATE_HEAVYATTACK:
		strState = TEXT("STATE_HEAVYATTACK");
		break;
	case Engine::STATE_KILLERSLASH:
		strState = TEXT("STATE_KILLERSLASH");
		break;
	case Engine::STATE_GUARD_BREAK:
		strState = TEXT("STATE_GUARD_BREAK");
		break;
	case Engine::STATE_GUARD_HIT:
		strState = TEXT("STATE_GUARD_HIT");
		break;
	case Engine::STATE_GUARD:
		strState = TEXT("STATE_GUARD");
		break;
	case Engine::STATE_BLOWNOFF_IN:
		strState = TEXT("STATE_BLOWNOFF");
		break;
	case Engine::STATE_FORCE:
		strState = TEXT("STATE_FORCE");
		break;
	case Engine::STATE_HEAVY_HIT:
		strState = TEXT("STATE_HEAVY_HIT");
		break;
	case Engine::STATE_LAUNCHED_B:
		strState = TEXT("STATE_LAUNCHED");
		break;
	case Engine::STATE_HIT:
		strState = TEXT("STATE_HIT");
		break;
	case Engine::STATE_LIGHT_HIT:
		strState = TEXT("STATE_LIGHT_HIT");
		break;
	case Engine::STATE_VEHICLE_FALL:
		strState = TEXT("STATE_VEHICLE_FALL");
		break;
	case Engine::STATE_DEATH:
		strState = TEXT("STATE_DEATH");
		break;
	case Engine::STATE_ITEMGET:
		strState = TEXT("STATE_ITEMGET");
		break;
	case Engine::STATE_PICKTRASH:
		strState = TEXT("STATE_PICKTRASH");
		break;
	case Engine::STATE_SCORPION:
		strState = TEXT("STATE_SCORPION");
		break;
	case Engine::STATE_TOCOMBAT:
		strState = TEXT("STATE_TOCOMBAT");
		break;
	case Engine::STATE_TOIDLE:
		strState = TEXT("STATE_TOIDLE");
		break;
	case Engine::STATE_IDLE:
		strState = TEXT("STATE_IDLE");
		break;
	case Engine::STATE_TRANSFORM:
		strState = TEXT("STATE_TRANSFORM");
		break;
	case Engine::STATE_VEHICLE_IDLE:
		strState = TEXT("STATE_VEHICLE_IDLE");
		break;
	case Engine::STATE_VEHICLE_IN_L:
		strState = TEXT("STATE_VEHICLE_IN_L");
		break;
	case Engine::STATE_VEHICLE_OUT_L:
		strState = TEXT("STATE_VEHICLE_OUT_L");
		break;
	case Engine::STATE_VEHICLE_IN_R:
		strState = TEXT("STATE_VEHICLE_IN_R");
		break;
	case Engine::STATE_VEHICLE_OUT_R:
		strState = TEXT("STATE_VEHICLE_OUT_R");
		break;
	case Engine::STATE_VEHICLE_CALL:
		strState = TEXT("STATE_VEHICLE_CALL");
		break;
	case Engine::STATE_DODGE:
		strState = TEXT("STATE_DODGE");
		break;
	case Engine::STATE_PERFECTDODGE:
		strState = TEXT("STATE_PERFECTDODGE");
		break;
	case Engine::STATE_GRABWEAPON:
		strState = TEXT("STATE_GRABWEAPON");
		break;
	case Engine::STATE_SHAKEWEAPON:
		strState = TEXT("STATE_SHAKEWEAPON");
		break;
	case Engine::STATE_GUARDWALK:
		strState = TEXT("STATE_GUARDWALK");
		break;
	case Engine::STATE_JUMP:
		strState = TEXT("STATE_JUMP");
		break;
	case Engine::STATE_STEPDOWN:
		strState = TEXT("STATE_STEPDOWN");
		break;
	case Engine::STATE_STEPUP:
		strState = TEXT("STATE_STEPUP");
		break;
	case Engine::STATE_RUN:
		strState = TEXT("STATE_RUN");
		break;
	case Engine::STATE_VEHICLE_BACK_L:
		strState = TEXT("STATE_VEHICLE_BACK_L");
		break;
	case Engine::STATE_VEHICLE_BACK_R:
		strState = TEXT("STATE_VEHICLE_BACK_R");
		break;
	case Engine::STATE_VEHICLE_FAST:
		strState = TEXT("STATE_VEHICLE_FAST");
		break;
	case Engine::STATE_VEHICLE_RUN:
		strState = TEXT("STATE_VEHICLE_RUN");
		break;
	case Engine::STATE_VEHICLE_RUN_START:
		strState = TEXT("STATE_VEHICLE_RUN_START");
		break;
	case Engine::STATE_VEHICLE_RUN_STOP:
		strState = TEXT("STATE_VEHICLE_RUN_STOP");
		break;
	case Engine::STATE_WALK:
		strState = TEXT("STATE_WALK");
		break;
	case Engine::STATE_DEATHKICK:
		strState = TEXT("STATE_DEATHKICK");
		break;
	case Engine::STATE_END:
		strState = TEXT("STATE_END");
		break;
	default:
		break;
	}

	/*if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Default"), strState, _float2(0.f, 0.f), XMVectorSet(1.f, 1.f, 0.5f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Default"), L"Player HP : " + std::to_wstring(m_iHP), _float2(0.f, 30.f), XMVectorSet(1.f, 1.f, 0.5f, 1.f))))
		return E_FAIL;*/
#endif

	return S_OK;
}

void CPlayer::OnCollision(LAYER_ID eColLayer, shared_ptr<class CCollider> pCollider)
{
	_vector vHitDir, vCurPos, vColObjPos, vCurLook;
	_float fRadian;

	wstring outputString;

	_float	fMonRadius, fPlayerRadius, fDistance;
	_vector	vMonCenter, vPlayerCenter, vSlidePos;

	_uint	iLoopExit = 100;


	switch (eColLayer)
	{
	case Engine::LAYER_MONSTER:

		//// 상태에 따라서 위치 변환 해줄 지 말지 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//if ((STATE_ATTACK || STATE_HEAVYATTACK || STATE_KILLERSLASH || STATE_DASHATTACK || STATE_DEATHKICK || STATE_TRANSFORM || STATE_ARMOR_MISSILE_IN) == m_eCurState)
		//{
		//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPrevPos);
		//	return;
		//}

		if (!m_pColliderCom->IsOnCollide())
			return;

		// 1. 플레이어 위치, 몬스터 위치, 플레이어 radius, 몬스터 radius 구하기
		vMonCenter = pCollider->GetOwner()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		vPlayerCenter = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		fMonRadius = dynamic_pointer_cast<CBounding_Sphere>(pCollider->GetBounding())->GetRadius();
		fPlayerRadius = dynamic_pointer_cast<CBounding_Sphere>(m_pColliderCom->GetBounding())->GetRadius();

		// 2. 몬스터 위치 - 플레이어 위치의 거리가 플레이어 radius + 몬스터 radius와 비교해서 먼지 가까운지 (왜함?)

		fDistance = XMVectorGetX(XMVector3Length(vMonCenter - vPlayerCenter));

		// 3. 만약 가깝거나 같다면, 플레이어의 위치를 보정해준다 (당연함..)

		// 4. ((플레이어 위치 - 몬스터 위치) Normalize 후 * (플레이어 radius + 몬스터 radius) + 몬스터 위치)로 이동
		vSlidePos = (XMVector3Normalize(vPlayerCenter - vMonCenter) * (fPlayerRadius + fMonRadius)) + vMonCenter;

		// 5. 이동한 포지션에 네비게이션 체크 해야됨 (이동 불가능이면 prevpos로 보정해줘야한당)
		if (!m_pTransformCom->MoveToCheckNavi(vSlidePos, m_pNavigationCom, 0.001f, m_fTimeDelta))
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPrevPos);
		}

		//// 플레이어 & 몬스터 충돌 -> 슬라이딩 벡터
		////vMonCenter = pCollider->GetOwner()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		////vPlayerCenter = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		////fMonRadius = dynamic_pointer_cast<CBounding_Sphere>(pCollider->GetBounding())->GetRadius();
		////fPlayerRadius = dynamic_pointer_cast<CBounding_Sphere>(m_pColliderCom->GetBounding())->GetRadius();
		////
		////vCollisionVector = XMVectorSubtract(vMonCenter, vPlayerCenter);
		////fDistance = XMVectorGetX(XMVector3Length(vCollisionVector));
		////fMinDistance = fPlayerRadius + fMonRadius;

		////// 충돌 벡터 계산
		////vCollisionDir	= XMVector3Normalize(vCollisionVector);
		////vSlideVector	= XMVectorScale(vCollisionDir, fMinDistance - fDistance);

		////// 슬라이딩 벡터 계산
		////vSlideDir = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		//////m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorAdd(m_pTransformCom->Get_State(CTransform::STATE_POSITION), vSlideDirScaled));
		////m_pTransformCom->Go_Dir_Slide(m_fTimeDelta, vSlideDir, m_pNavigationCom, 1.f);


		//if ((STATE_ATTACK || STATE_HEAVYATTACK || STATE_KILLERSLASH || STATE_DASHATTACK || STATE_DEATHKICK || STATE_TRANSFORM || STATE_ARMOR_MISSILE_IN) == m_eCurState)
		//{
		//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPrevPos);
		//	return;
		//}
		///*
		//void SlidePlayer(float& playerX, float& playerZ, float monsterX, float monsterZ) {
		//	// 몬스터의 위치로 플레이어를 이동시킴
		//	float directionX = playerX - monsterX;
		//	float directionZ = playerZ - monsterZ;
		//	float magnitude = sqrt(directionX * directionX + directionZ * directionZ);
		//	directionX /= magnitude;
		//	directionZ /= magnitude;

		//	playerX = monsterX + directionX * (playerRadius + monsterRadius);
		//	playerZ = monsterZ + directionZ * (playerRadius + monsterRadius);
		//}
		//*/

		//vMonCenter = pCollider->GetOwner()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		//vPlayerCenter = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPrevPos);

		//fMonRadius = dynamic_pointer_cast<CBounding_Sphere>(pCollider->GetBounding())->GetRadius();
		//fPlayerRadius = dynamic_pointer_cast<CBounding_Sphere>(m_pColliderCom->GetBounding())->GetRadius();

		//vCollisionVector = XMVectorSubtract(vMonCenter, vPlayerCenter);
		//
		//// 플레이어 -> 몬스터 충돌 방향의 Left 벡터 구하기 (접선 벡터)
		//fDistance = vCollisionVector.m128_f32[0];
		//vCollisionVector.m128_f32[0] = -vCollisionVector.m128_f32[2];
		//vCollisionVector.m128_f32[2] = fDistance;

		//// 충돌로 파고든 양 구하기
		//fDistance = fPlayerRadius + fMonRadius - XMVectorGetX(XMVector3Length(vCollisionVector));

		//vCollisionVector.m128_f32[1] = 0;

		//// 충돌 벡터 계산
		//vCollisionDir = XMVector3Normalize(vCollisionVector);
		//vCollisionDir.m128_f32[0] = sinf(vCollisionDir.m128_f32[0]);
		//vCollisionDir.m128_f32[2] = sinf(vCollisionDir.m128_f32[2]);
		////vSlideVector = XMVectorScale(vCollisionDir, fMinDistance - fDistance);

		//// 계산한 슬라이딩 벡터로 이동시키기
		//m_pTransformCom->Go_Dir_Slide(m_fTimeDelta, vCollisionVector, m_pNavigationCom, fDistance);

		//

		//// 만약 이동시킨 곳도 충돌중이라면?
		//while (fMonRadius + fPlayerRadius > XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vMonCenter)))
		//{
		//	// 더 이동시켜!!!!!
		//	if (!m_pTransformCom->Go_Dir_Slide(m_fTimeDelta, vCollisionVector, m_pNavigationCom, fDistance))
		//		break;

		//	iLoopExit--;

		//	if (iLoopExit <= 0)
		//		break;
		//}

		break;
	case Engine::LAYER_PROJECTILE:
		// 플레이어 & Projectile 충돌 -> 히트판정


		if (!m_pColliderCom->IsOnCollide())
			return;

		if (!pCollider->IsOnCollide())
			return;

		outputString = L"플레이어 충돌\n";
		OutputDebugStringW(outputString.c_str());

		m_pColliderCom->SetOnCollide(false);
		m_isHit = true;

		vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vColObjPos = pCollider->GetOwner()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

		vCurLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		vHitDir = XMVector4Normalize(vCurPos - vColObjPos);

		fRadian = XMVectorGetX(XMVector3Dot(vCurLook, vHitDir));

		// 정면 충돌 (플레이어 기준)
		if (0 > fRadian)
			m_eHitDir = DIR_F;

		// 후면 충돌 (플레이어 기준)
		else
			m_eHitDir = DIR_B;

		m_isHit = true;
		/*m_eHitType = dynamic_pointer_cast<CProjectile>(pCollider->GetOwner())->Get_HitType();

		m_iSaveDamage = dynamic_pointer_cast<CProjectile>(pCollider->GetOwner())->Get_AP();*/

		m_eHitType = (HIT_TYPE)pCollider->GetHitType();
		m_iSaveDamage = pCollider->GetAP();

		// 체력을 깎는 조건
		// 스킬 사용중이 아니여야하고
		// 가드 중일 때는, 공격 타입에 따라 다르고
		// 피격 애니메이션 중에는 체력을 깎으면 안된다,,,,,,,,,,,,,,,,,

		// 아예 각 state에서 체력 깎일 조건을 확인하고 체력을 깎아주자
		// hit으로 넘어가는 애들은 hit에서 깎고, 아닌 애들은 각 state에서

		//m_iHP -= dynamic_pointer_cast<CProjectile>(pCollideObject)->Get_AP();



		break;
	case Engine::LAYER_MONSTER_ATTACK:

		if (!m_pColliderCom->IsOnCollide())
			return;

		if (!pCollider->IsOnCollide())
			return;

		// 몬스터 공격 맞았을 때
		outputString = L"플레이어 충돌\n";
		OutputDebugStringW(outputString.c_str());

		m_pColliderCom->SetOnCollide(false);
		m_isHit = true;

		vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vColObjPos = pCollider->GetOwner()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

		vCurLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		vHitDir = XMVector4Normalize(vCurPos - vColObjPos);

		fRadian = XMVectorGetX(XMVector3Dot(vCurLook, vHitDir));

		// 정면 충돌 (플레이어 기준)
		if (0 > fRadian)
			m_eHitDir = DIR_F;

		// 후면 충돌 (플레이어 기준)
		else
			m_eHitDir = DIR_B;

		m_isHit = true;
		m_eHitType = (HIT_TYPE)pCollider->GetHitType();
		m_iSaveDamage = pCollider->GetAP();

		break;

	case Engine::LAYER_TELEPORT:
		outputString = L"플레이어 충돌\n";

		m_isColTrigger = true;

		if (!m_isPrevColTrigger)
		{
			CSound_Manager::GetInstance()->StopAll();
			CSound_Manager::GetInstance()->PlayBGM(L"BGM_InUFO.mp3", 1.f);
			// 디스토션 테스트 출력
			CEffect_Manager::GetInstance()->PlayEffect(TEXT("GameObject_Effect_Distortion_Test"), pCollider->GetOwner()->Get_TransformCom());
		}
		// 플레이어 텔레포트 충돌 -> 텔레포트 탈건지 UI
		break;
	case Engine::LAYER_ENVIRONMENT:
		// 플레이어 프롭 충돌 -> 슬라이딩 벡터
		break;
	default:
		break;
	}
}

void CPlayer::SetBike(shared_ptr<class CBike> pBike)
{
	m_pBike = pBike;
	m_pBikeTransformCom = m_pBike->Get_TransformCom();
}

void CPlayer::SetWeaponAttach(_bool isWeaponAttached)
{
	m_isWeaponAttached = isWeaponAttached;

	if (m_isWeaponAttached)
	{
		dynamic_pointer_cast<CWeapon>(m_PlayerParts.find(TEXT("Part_Weapon"))->second)->Set_Attached(true);
	}

	else
	{
		dynamic_pointer_cast<CWeapon>(m_PlayerParts.find(TEXT("Part_Weapon"))->second)->Set_Attached(false);
	}
}

_bool CPlayer::SetDamage()
{
	CUI_Manager::GetInstance()->LoseHP(m_iSaveDamage);

	_int iRand = rand() % 3;

	if (m_iSaveDamage >= 5)
	{
		CSound_Manager::GetInstance()->PlaySound(L"Player_Hit_H.wav", CSound_Manager::CHANNELID::PLAYER_ATTACK, 1.f);
	}
	else if (m_iSaveDamage >= 3)
	{
		if (iRand)
			CSound_Manager::GetInstance()->PlaySound(L"vo_TRV_DMG_M_Body_03.wav", CSound_Manager::CHANNELID::PLAYER_ATTACK, 1.f);
		else
			CSound_Manager::GetInstance()->PlaySound(L"vo_TRV_DMG_M_Body_02.wav", CSound_Manager::CHANNELID::PLAYER_ATTACK, 1.f);
	}
	else
	{
		if (0 == iRand)
			CSound_Manager::GetInstance()->PlaySound(L"vo_TRV_DMG_S_10.wav", CSound_Manager::CHANNELID::PLAYER_ATTACK, 1.f);
		else if (1 == iRand)
			CSound_Manager::GetInstance()->PlaySound(L"vo_TRV_DMG_S_03.wav", CSound_Manager::CHANNELID::PLAYER_ATTACK, 1.f);
		else
			CSound_Manager::GetInstance()->PlaySound(L"vo_TRV_DMG_S_00.wav", CSound_Manager::CHANNELID::PLAYER_ATTACK, 1.f);
	}

	return true;
}

void CPlayer::SetOnBikeNow(_bool isOnBikeNow)
{
	m_isOnBikeNow = isOnBikeNow;

	if (m_isOnBikeNow)
	{
		_float4x4 WorldMatrix;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pBikeTransformCom->Get_State(CTransform::STATE_POSITION));

		_matrix	BoneMatrix = XMLoadFloat4x4(m_pBike->Get_RootMatrix());

		for (size_t i = 0; i < 3; i++)
		{
			BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
		}

		//BoneMatrix.r[3] += -(vRight * 1.5f);
		BoneMatrix.r[3].m128_f32[0] += 1.67f;
		BoneMatrix.r[3].m128_f32[2] += 1.f;

		XMStoreFloat4x4(&WorldMatrix, BoneMatrix * m_pBikeTransformCom->Get_WorldMatrix());
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(WorldMatrix.m[3][0], WorldMatrix.m[3][1], WorldMatrix.m[3][2], 1.f));

		_vector vLookPoint = m_pBike->Get_TransformCom()->Get_State(CTransform::STATE_POSITION) - (XMVectorSet(WorldMatrix.m[2][0], WorldMatrix.m[2][1], WorldMatrix.m[2][2], 0.f) * 1.2f);
		m_pTransformCom->LookAt(vLookPoint);
		
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, (m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.8f, 0.f, 0.f)));

		//m_pTransformCom->LookAt_ForLandObject(m_pBike->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));

		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, (m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pBikeTransformCom->Get_State(CTransform::STATE_LOOK) * 0.4f));

		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, (m_pTransformCom->Get_State(CTransform::STATE_POSITION) - XMVectorSet(0.f, 0.f, 0.4f, 0.f)));
	}
}

void CPlayer::SetOnMiniGameNow(_bool isOnMiniGameNow)
{
}

HRESULT CPlayer::Add_Components()
{
	/* Com_Navigation */
	CNavigation::NAVI_DESC		NaviDesc{};

	NaviDesc.iStartCellIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_VILLAGE, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

	dynamic_pointer_cast<CBody_Player>(m_PlayerParts.find(TEXT("Part_Body"))->second)->SetNaviCom(m_pNavigationCom);

	/* Com_Collider */
	CBounding_Sphere::SPHERE_DESC		BoundingDesc{};

	BoundingDesc.vCenter = _float3(0.f, 1.f, 0.f);
	BoundingDesc.fRadius = m_fColRadius;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider(LAYER_PLAYER, m_pColliderCom);
	m_pColliderCom->SetOwner(shared_from_this());

	/* Com_StateMachine */
	CStateMachine::STATEMACHINE_DESC	StateMachineDesc{};

	StateMachineDesc.pOwner = shared_from_this();
	StateMachineDesc.pOwnerBody = m_PlayerParts.find(TEXT("Part_Body"))->second;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"),
		TEXT("Com_StateMachine"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pStateMachineCom), &StateMachineDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Add_PartObjects()
{
	CBody_Player::BODY_PLAYER_DESC BodyPlayerDesc{};

	BodyPlayerDesc.pParentTransform = m_pTransformCom;
	BodyPlayerDesc.pPlayerState = &m_iState;

	shared_ptr<CPartObject> pBody_Player = CBody_Player::Create(m_pDevice, m_pContext, &BodyPlayerDesc);
	m_pBody = dynamic_pointer_cast<CBody_Player>(pBody_Player);

	/* For.Prototype_GameObject_Body_Player */
	/*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, LAYER_PLAYER, TEXT("GameObject_Body_Player"),
		pBody_Player)))
		return E_FAIL;*/
	m_PlayerParts.emplace(TEXT("Part_Body"), pBody_Player);

	m_pBodyModel = dynamic_pointer_cast<CBinaryModel>(pBody_Player->Find_Component(TEXT("Com_Model")));
	if (nullptr == m_pBodyModel)
		return E_FAIL;

	m_pBodyTransModel = dynamic_pointer_cast<CBinaryModel>(pBody_Player->Find_Component(TEXT("Com_Model_Trans")));
	if (nullptr == m_pBodyTransModel)
		return E_FAIL;

	CWeapon::WEAPON_DESC			WeaponObjDesc{};
	WeaponObjDesc.pParentTransform = m_pTransformCom;
	WeaponObjDesc.pSocketMatrix = m_pBodyModel->Get_CombinedBoneMatrixPtr("weapon_r");

	/* For.Prototype_GameObject_Weapon */
	shared_ptr<CPartObject> pWeapon = CWeapon::Create(m_pDevice, m_pContext, &WeaponObjDesc);
	/*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, LAYER_PLAYER, TEXT("GameObject_Weapon"),
		pWeapon)))
		return E_FAIL;*/
	m_PlayerParts.emplace(TEXT("Part_Weapon"), pWeapon);

	m_pWeapon = dynamic_pointer_cast<CWeapon>(pWeapon);
	m_pWeaponCollider = dynamic_pointer_cast<CCollider>(pWeapon->Find_Component(TEXT("Com_Collider")));
	m_pWeaponCollider->SetOnCollide(false);

	//CBike::BIKE_DESC			BikeObjDesc{};
	//BikeObjDesc.pParentTransform = m_pTransformCom;
	//BikeObjDesc.pSocketMatrix = m_pBodyModel->Get_CombinedBoneMatrixPtr("root");

	///* For.Prototype_GameObject_Weapon */
	//shared_ptr<CPartObject> pBike = CBike::Create(m_pDevice, m_pContext, &BikeObjDesc);
	///*if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, LAYER_PLAYER, TEXT("GameObject_Weapon"),
	//	pWeapon)))
	//	return E_FAIL;*/
	//m_PlayerParts.emplace(TEXT("Part_Bike"), pBike);

	return S_OK;
}

HRESULT CPlayer::Add_States()
{
	shared_ptr<CState> pState = CState_Idle::Create(m_pDevice, m_pContext, STATE::STATE_IDLE, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_OwnerCollider(m_pColliderCom);

	pState = CState_Run::Create(m_pDevice, m_pContext, STATE::STATE_RUN, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_NavigationCom(m_pNavigationCom);
	pState->Set_OwnerCollider(m_pColliderCom);

	pState = CState_Attack::Create(m_pDevice, m_pContext, STATE::STATE_ATTACK, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_OwnerModel(m_pBodyModel);
	pState->Set_OwnerCollider(m_pColliderCom);
	dynamic_pointer_cast<CState_Attack>(pState)->SetOwnerAttackCollider(m_pWeaponCollider);

	pState = CState_HeavyAttack::Create(m_pDevice, m_pContext, STATE::STATE_HEAVYATTACK, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_OwnerModel(m_pBodyModel);
	pState->Set_OwnerCollider(m_pColliderCom);
	dynamic_pointer_cast<CState_HeavyAttack>(pState)->SetOwnerAttackCollider(m_pWeaponCollider);

	pState = CState_Dodge::Create(m_pDevice, m_pContext, STATE::STATE_DODGE, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_OwnerModel(m_pBodyModel);
	pState->Set_OwnerCollider(m_pColliderCom);

	pState = CState_Jump::Create(m_pDevice, m_pContext, STATE::STATE_JUMP, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_OwnerModel(m_pBodyModel);
	pState->Set_NavigationCom(m_pNavigationCom);
	pState->Set_OwnerCollider(m_pColliderCom);

	pState = CState_KillerSlash::Create(m_pDevice, m_pContext, STATE::STATE_KILLERSLASH, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_OwnerModel(m_pBodyModel);
	pState->Set_OwnerCollider(m_pColliderCom);
	dynamic_pointer_cast<CState_KillerSlash>(pState)->SetOwnerAttackCollider(m_pWeaponCollider);

	pState = CState_DeathKick::Create(m_pDevice, m_pContext, STATE::STATE_DEATHKICK, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_OwnerModel(m_pBodyModel);
	pState->Set_OwnerCollider(m_pColliderCom);
	dynamic_pointer_cast<CState_DeathKick>(pState)->SetOwnerAttackCollider(m_pWeaponCollider);

	pState = CState_DashAttack::Create(m_pDevice, m_pContext, STATE::STATE_DASHATTACK, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_OwnerModel(m_pBodyModel);
	pState->Set_OwnerCollider(m_pColliderCom);
	dynamic_pointer_cast<CState_DashAttack>(pState)->SetOwnerAttackCollider(m_pWeaponCollider);

	pState = CState_Guard::Create(m_pDevice, m_pContext, STATE::STATE_GUARD, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_OwnerModel(m_pBodyModel);
	pState->Set_OwnerCollider(m_pColliderCom);
	pState->Set_NavigationCom(m_pNavigationCom);

	pState = CState_PerfectDodge::Create(m_pDevice, m_pContext, STATE::STATE_PERFECTDODGE, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_OwnerModel(m_pBodyModel);
	pState->Set_OwnerCollider(m_pColliderCom);
	pState->Set_NavigationCom(m_pNavigationCom);

	pState = CState_Hit::Create(m_pDevice, m_pContext, STATE::STATE_HIT, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_OwnerModel(m_pBodyModel);
	pState->Set_OwnerCollider(m_pColliderCom);
	pState->Set_NavigationCom(m_pNavigationCom);

	pState = CState_Transform::Create(m_pDevice, m_pContext, STATE::STATE_TRANSFORM, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_OwnerModel(m_pBodyModel);
	pState->Set_OwnerCollider(m_pColliderCom);
	pState->Set_NavigationCom(m_pNavigationCom);

	pState = CState_Armor_Missile::Create(m_pDevice, m_pContext, STATE::STATE_ARMOR_MISSILE_IN, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_OwnerModel(m_pBodyModel);
	pState->Set_OwnerCollider(m_pColliderCom);
	dynamic_pointer_cast<CState_Armor_Missile>(pState)->Set_OwnerTransModel(m_pBodyTransModel);

	pState = CState_Throw::Create(m_pDevice, m_pContext, STATE::STATE_THROW_LEOPARDON, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_OwnerModel(m_pBodyModel);
	pState->Set_OwnerCollider(m_pColliderCom);

	pState = CState_Rising::Create(m_pDevice, m_pContext, STATE::STATE_RISING, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_OwnerModel(m_pBodyModel);

	pState = CState_Appear::Create(m_pDevice, m_pContext, STATE::STATE_APPEAR_IN, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_OwnerModel(m_pBodyModel);

	// =================================== 바이크 ==========================================

	pState = CState_Vehicle_Idle::Create(m_pDevice, m_pContext, STATE::STATE_VEHICLE_IDLE, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_OwnerModel(m_pBodyModel);
	pState->Set_NavigationCom(m_pNavigationCom);
	pState->Set_OwnerCollider(m_pColliderCom);

	pState = CState_Vehicle_Run::Create(m_pDevice, m_pContext, STATE::STATE_VEHICLE_RUN, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_OwnerModel(m_pBodyModel);
	pState->Set_NavigationCom(m_pNavigationCom);
	pState->Set_OwnerCollider(m_pColliderCom);

	pState = CState_Vehicle_Jump::Create(m_pDevice, m_pContext, STATE::STATE_VEHICLE_FALL, m_pStateMachineCom);
	if (nullptr == pState)
		return E_FAIL;
	m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	pState->Set_OwnerModel(m_pBodyModel);
	pState->Set_NavigationCom(m_pNavigationCom);
	pState->Set_OwnerCollider(m_pColliderCom);

	//pState = CState_Vehicle_Call::Create(m_pDevice, m_pContext, STATE::STATE_VEHICLE_CALL, m_pStateMachineCom);
	//if (nullptr == pState)
	//	return E_FAIL;
	//m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	//pState->Set_OwnerModel(m_pBodyModel);
	//pState->Set_OwnerCollider(m_pColliderCom);

	//pState = CState_Vehicle_Back::Create(m_pDevice, m_pContext, STATE::STATE_VEHICLE_BACK_R, m_pStateMachineCom);
	//if (nullptr == pState)
	//	return E_FAIL;
	//m_pStateMachineCom->Add_State(pState->Get_State(), pState);
	//pState->Set_OwnerModel(m_pBodyModel);
	//pState->Set_OwnerCollider(m_pColliderCom);

	m_pStateMachineCom->Set_State(STATE::STATE_IDLE);

	return S_OK;
}

void CPlayer::Key_Input(_float fTimeDelta)
{
	// 다 디버깅용임
	if (m_pGameInstance->Get_DIKeyState(DIK_MINUS) & 0x80)
		m_pStateMachineCom->Set_State(STATE::STATE_IDLE);

	if (m_pGameInstance->Get_DIKeyState(DIK_X))
	{
		m_eHitType = HIT_LIGHT;
		m_eHitDir = DIRECTION::DIR_L;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_C))
	{
		m_eHitType = HIT_DEFAULT;
		m_eHitDir = DIRECTION::DIR_F;
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_V))
	{
		m_eHitType = HIT_HEAVY;
		m_eHitDir = DIRECTION::DIR_B;
	}


	//// 이동
	//{
	//	if (GetKeyState('A') & 0x8000)
	//	{
	//		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
	//	}

	//	if (GetKeyState('D') & 0x8000)
	//	{
	//		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 1.f);
	//	}

	//	if (GetKeyState('S') & 0x8000)
	//	{
	//		m_pTransformCom->Go_Backward(fTimeDelta);
	//	}

	//	if (GetKeyState('W') & 0x8000)
	//	{
	//		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
	//		m_iState |= STATE_RUN;
	//	}
	//	else
	//	{
	//		m_iState = 0x00000000;
	//		m_iState |= STATE_IDLE;
	//	}
	//}
}

void CPlayer::Change_NaviCom(const LEVEL& eCurLevel)
{
	CNavigation::NAVI_DESC		NaviDesc{};

	NaviDesc.iStartCellIndex = 0;

	__super::Remove_Component(TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"));

	__super::Add_Component(eCurLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pNavigationCom), &NaviDesc);

	_vector vStartPos = { 0.f, 0.f, 0.f, 0.f };

	m_pNavigationCom->GetCellCenterPos(&vStartPos);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vStartPos);

	m_pBody->Change_NaviCom(m_pNavigationCom);

	if (LEVEL_VILLAGE == eCurLevel)
	{
		m_pBike->SetActive(true);
		m_pBike->SetOnMiniGame(false);
		m_pBike->Change_NaviCom(5, m_pNavigationCom);
	}

	else if (LEVEL_MINIGAME == eCurLevel)
	{
		m_pBike->SetActive(true);
		m_pBike->SetOnMiniGame(true);
		m_pBike->Change_NaviCom(0, m_pNavigationCom);
	}

	else
	{
		m_pBike->SetActive(false);

		shared_ptr<CGameObject> pHP_UI = m_pGameInstance->Find_GameObject(eCurLevel, LAYER_UI, TEXT("GameObject_UI_HP_Player"));
		CUI_Manager::GetInstance()->SetHPUI(dynamic_pointer_cast<CUI_HP_Player>(pHP_UI));
	}

	for (auto& iter : m_pStateMachineCom->Get_StateMap())
		iter.second->Set_NavigationCom(m_pNavigationCom);
}

void CPlayer::SetNextTickTurn(_fvector vAxis, _float fRadian)
{
	m_isNextTickTurn = true;
	m_vNextTickTurnAxis = vAxis;
	m_fNextTickTurnRadian = fRadian;
}

shared_ptr<CGameObject> CPlayer::SearchTarget()
{
	// 몬스터들 다 불러오기
	//shared_ptr<CLayer>	pMonsterLayer = 

	list<shared_ptr<CGameObject>>	pMonsterList = m_pGameInstance->GetInstance()->Get_Layer_Objects(m_eNextLevel, LAYER_MONSTER);
	//pMonsterLayer->Find_GameObject(m_strObjectTag);

	if (0 == pMonsterList.size())
		return nullptr;

	// 검색 기준
	// 1. 거리
	_float fDist[3] = { -1.f };

	// 2. 방향
	_float	fRadian[3] = { -1.f };
	_bool	fFront[3] = { false };

	auto iter = pMonsterList.begin();

	for (size_t i = 0; i < pMonsterList.size(); i++)
	{
		_vector vMonPos = (*iter)->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		_vector vDir = vMonPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		fDist[i] = XMVectorGetX(XMVector4Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vMonPos));
		fRadian[i] = XMVectorGetX(XMVector3Dot(m_pTransformCom->Get_State(CTransform::STATE_LOOK), vDir));
		if (0 > fRadian[i])
			fRadian[i] = -1;
		else
			fFront[i] = true;

		iter++;
	}

	_int iTargetIdx = -1;

	for (size_t i = 0; i < pMonsterList.size(); i++)
	{
		if (fFront[i])
		{
			if (-1 == iTargetIdx)
				iTargetIdx = i;

			else
			{
				if (fDist[i] <= fDist[iTargetIdx])
					iTargetIdx = i;
			}
		}
	}

	if (-1 == iTargetIdx)
	{
		iTargetIdx = 0;

		for (size_t i = 1; i < pMonsterList.size(); i++)
		{
			if (fDist[i] <= fDist[iTargetIdx])
				iTargetIdx = i;
		}
	}

	iter = pMonsterList.begin();

	for (size_t i = 0; i < iTargetIdx; i++)
	{
		iter++;
	}

	return (*iter);
}

shared_ptr<CPlayer> CPlayer::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
	shared_ptr<CPlayer> pInstance = make_shared<CPlayer>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer");
		pInstance.reset();
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();
}
