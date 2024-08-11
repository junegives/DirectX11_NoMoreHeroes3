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
	/* ������ Ÿ���ϴ� ��ü�� ��ġ������ ���´�.(In WorldSpace) */
	_vector		vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);

	/* ������ü�� �׸��� �־��� ������������ ���ý����̽� �󿡼� Ÿ���� �����ϴ� �ﰢ������ ���̸� ����ؿ´�. */
	/* ���� ����(����)�� Ÿ���� ��ġ(����)�� ���ؾ��Ѵ�. */
	/* Ÿ�� ��ġ(����)�� ���� ������ ���ý����̽� �̵�(������ ���÷ΰ����ϱ⶧���� ������������� ������� �ʿ��ϴ�) ���Ѽ� ���ؾ߰ڴ�. */

	// ���� ��ü�� ���� ����� ������� ����Ͽ�, Ÿ���� ���� ��ǥ�� ���� ��ü�� ���� ��ǥ��� ��ȯ
	vTargetPos = XMVector3TransformCoord(vTargetPos, m_pTerrainTransform->Get_WorldMatrix_Inverse());

	vTargetPos = XMVectorSetY(vTargetPos, m_pTerrainVIBuffer->Compute_Height(vTargetPos));

	pTargetTransform->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(vTargetPos, m_pTerrainTransform->Get_WorldMatrix()));

	return S_OK;
}

_float CLandObject::SetUp_OnNavi(shared_ptr<CTransform> pTargetTransform)
{
	/* ������ Ÿ���ϴ� ��ü�� ��ġ������ ���´�.(In WorldSpace) */
	_vector		vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);

	/* ������ü�� �׸��� �־��� ������������ ���ý����̽� �󿡼� Ÿ���� �����ϴ� �ﰢ������ ���̸� ����ؿ´�. */
	/* ���� ����(����)�� Ÿ���� ��ġ(����)�� ���ؾ��Ѵ�. */
	/* Ÿ�� ��ġ(����)�� ���� ������ ���ý����̽� �̵�(������ ���÷ΰ����ϱ⶧���� ������������� ������� �ʿ��ϴ�) ���Ѽ� ���ؾ߰ڴ�. */

	// ���� ��ü�� ���� ����� ������� ����Ͽ�, Ÿ���� ���� ��ǥ�� ���� ��ü�� ���� ��ǥ��� ��ȯ
	//vTargetPos = XMVector3TransformCoord(vTargetPos, m_pTerrainTransform->Get_WorldMatrix_Inverse());

	vTargetPos = XMVectorSetY(vTargetPos, m_pNavigationCom->Compute_Height(vTargetPos));

	pTargetTransform->Set_State(CTransform::STATE_POSITION, vTargetPos);

	return S_OK;
}

_float CLandObject::Jump_OnNavi(shared_ptr<CTransform> pTargetTransform)
{
	/* ������ Ÿ���ϴ� ��ü�� ��ġ������ ���´�.(In WorldSpace) */
	_vector		vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);

	/* ������ü�� �׸��� �־��� ������������ ���ý����̽� �󿡼� Ÿ���� �����ϴ� �ﰢ������ ���̸� ����ؿ´�. */
	/* ���� ����(����)�� Ÿ���� ��ġ(����)�� ���ؾ��Ѵ�. */
	/* Ÿ�� ��ġ(����)�� ���� ������ ���ý����̽� �̵�(������ ���÷ΰ����ϱ⶧���� ������������� ������� �ʿ��ϴ�) ���Ѽ� ���ؾ߰ڴ�. */

	// ���� ��ü�� ���� ����� ������� ����Ͽ�, Ÿ���� ���� ��ǥ�� ���� ��ü�� ���� ��ǥ��� ��ȯ
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
