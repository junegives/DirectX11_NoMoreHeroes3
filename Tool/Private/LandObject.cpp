#include "pch.h"
#include "LandObject.h"

CLandObject::CLandObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject(pDevice, pContext)
{
}

CLandObject::CLandObject(const CLandObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CLandObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	/*if (FAILED(Add_Components()))
		return E_FAIL;*/

	return S_OK;
}

void CLandObject::Priority_Tick(_float fTimeDelta)
{
}

void CLandObject::Tick(_float fTimeDelta)
{
}

void CLandObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CLandObject::Render()
{
	return E_NOTIMPL;
}

//HRESULT CLandObject::Add_Components()
//{
//	/* Com_Navigation */
//	CNavigation::NAVI_DESC		NaviDesc{};
//
//	NaviDesc.iStartCellIndex = 0;
//
//	if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Navigation"),
//		TEXT("Com_Navigation"), reinterpret_cast<shared_ptr<CComponent>*>(&m_pNavigationCom), &NaviDesc)))
//		return E_FAIL;
//}

HRESULT CLandObject::SetUp_OnTerrain(shared_ptr<CTransform> pTargetTransform)
{
	/* 지형을 타야하는 객체의 위치정보를 얻어온다.(In WorldSpace) */
	_vector		vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);

	/* 지형객체가 그리고 있었던 지형정점들의 로컬스페이스 상에서 타겟이 존재하는 삼각형내에 높이를 계산해온다. */
	/* 지형 정점(로컬)과 타겟의 위치(월드)를 비교해야한다. */
	/* 타겟 위치(월드)를 지형 정점의 로컬스페이스 이동(지형의 로컬로가야하기때문에 지형월드행렬의 역행렬이 필요하다) 시켜서 비교해야겠다. */

	// 지형 객체의 월드 행렬의 역행렬을 사용하여, 타겟의 월드 좌표를 지형 객체의 로컬 좌표계로 변환
	vTargetPos = XMVector3TransformCoord(vTargetPos, m_pTerrainTransform->Get_WorldMatrix_Inverse());

	vTargetPos = XMVectorSetY(vTargetPos, m_pTerrainVIBuffer->Compute_Height(vTargetPos));

	pTargetTransform->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(vTargetPos, m_pTerrainTransform->Get_WorldMatrix()));

	return S_OK;
}

_float CLandObject::SetUp_OnNavi(shared_ptr<CTransform> pTargetTransform)
{
	/* 지형을 타야하는 객체의 위치정보를 얻어온다.(In WorldSpace) */
	_vector		vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);

	/* 지형객체가 그리고 있었던 지형정점들의 로컬스페이스 상에서 타겟이 존재하는 삼각형내에 높이를 계산해온다. */
	/* 지형 정점(로컬)과 타겟의 위치(월드)를 비교해야한다. */
	/* 타겟 위치(월드)를 지형 정점의 로컬스페이스 이동(지형의 로컬로가야하기때문에 지형월드행렬의 역행렬이 필요하다) 시켜서 비교해야겠다. */

	// 지형 객체의 월드 행렬의 역행렬을 사용하여, 타겟의 월드 좌표를 지형 객체의 로컬 좌표계로 변환
	//vTargetPos = XMVector3TransformCoord(vTargetPos, m_pTerrainTransform->Get_WorldMatrix_Inverse());

	vTargetPos = XMVectorSetY(vTargetPos, m_pNavigationCom->Compute_Height(vTargetPos));

	pTargetTransform->Set_State(CTransform::STATE_POSITION, vTargetPos);

	return S_OK;
}

_float CLandObject::Jump_OnNavi(shared_ptr<CTransform> pTargetTransform)
{
	/* 지형을 타야하는 객체의 위치정보를 얻어온다.(In WorldSpace) */
	_vector		vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);

	/* 지형객체가 그리고 있었던 지형정점들의 로컬스페이스 상에서 타겟이 존재하는 삼각형내에 높이를 계산해온다. */
	/* 지형 정점(로컬)과 타겟의 위치(월드)를 비교해야한다. */
	/* 타겟 위치(월드)를 지형 정점의 로컬스페이스 이동(지형의 로컬로가야하기때문에 지형월드행렬의 역행렬이 필요하다) 시켜서 비교해야겠다. */

	// 지형 객체의 월드 행렬의 역행렬을 사용하여, 타겟의 월드 좌표를 지형 객체의 로컬 좌표계로 변환
	//vTargetPos = XMVector3TransformCoord(vTargetPos, m_pTerrainTransform->Get_WorldMatrix_Inverse());

	vTargetPos = XMVectorSetY(vTargetPos, m_pNavigationCom->Compute_Height(vTargetPos));

	if (pTargetTransform->Get_State(CTransform::STATE_POSITION).m128_f32[1] <= vTargetPos.m128_f32[1])
		pTargetTransform->Set_State(CTransform::STATE_POSITION, vTargetPos);

	return S_OK;
}


_bool CLandObject::IsValidPos(_vector vPos)
{
	return m_pNavigationCom->Compute_CurrentIdx(vPos);
}

void CLandObject::Free()
{
	__super::Free();
}
