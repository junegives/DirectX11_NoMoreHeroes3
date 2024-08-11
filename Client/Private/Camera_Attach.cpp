#include "pch.h"
#include "Camera_Attach.h"

#include "SphericalCoordinates.h"

#include "UI_Target.h"
#include "UI_Manager.h"

CCamera_Attach::CCamera_Attach(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CCamera(pDevice, pContext)
{
}

CCamera_Attach::CCamera_Attach(const CCamera_Attach& rhs)
    : CCamera(rhs)
{
}

HRESULT CCamera_Attach::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

	m_pTargetTransform = m_pGameInstance->GetInstance()->Get_Player()->Get_TransformCom();

    CAMERA_ATTACH_DESC* pCameraDesc = (CAMERA_ATTACH_DESC*)pArg;

    m_fMouseSensor = pCameraDesc->fMouseSensor;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

	m_vCamPos = _float3(0, 1.5f, -4);

	//카메라 위치 계산을 위해 x, y, z좌표와 반지름 r값을 넘겨준다.
	m_pSphereicalCoord = make_shared<CSphericalCoordinates>(m_vCamPos, abs(m_vCamPos.z));

	_vector	vPos;
	_float3 vCartesian = m_pSphereicalCoord->toCartesian();
	vPos = XMLoadFloat3(&vCartesian) + m_pTargetTransform.lock()->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	CUI::UIDESC		UIDesc = {};

	UIDesc.fCX = 3; UIDesc.fCY = 3;
	UIDesc.fX = 0.f;
	UIDesc.fY = 0.f;

	m_pTargetUI = CUI_Target::Create(m_pDevice, m_pContext, &UIDesc);

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_STATIC, LAYER_UI, TEXT("GameObject_UI_Target"), m_pTargetUI)))
		return E_FAIL;

	CUI_Manager::GetInstance()->AddUI(TEXT("GameObject_UI_Target"), m_pTargetUI);

    return S_OK;
}

void CCamera_Attach::Priority_Tick(_float fTimeDelta)
{
	if (!m_isLocked)
	{
		POINT ptMouse;

		GetCursorPos(&ptMouse);

		// 마우스 이동량
		_float horizontal = m_pGameInstance->Get_DIMouseMove(DIMS_X) * m_fMouseSensor * -1;
		//m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * horizontal);

		_float vertical = m_pGameInstance->Get_DIMouseMove(DIMS_Y) * m_fMouseSensor;
		//m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * vertical);

		// 타겟의 위치를 기반으로 Look 벡터 업데이트
		//플레이어 위치에서 조금더 위쪽으로 자리잡게 만든다.
		XMStoreFloat3(&m_vLookPos, m_pTargetTransform.lock()->Get_State(CTransform::STATE_POSITION));

		m_vLookPos.y += m_vCamPos.y;

		//플레이어 중심으로 구한 구면좌표를 카메라 위치에 적용
		_float3 vSphereCoord = m_pSphereicalCoord->Rotate(horizontal * fTimeDelta, vertical * fTimeDelta).toCartesian();

		vSphereCoord.x += m_vLookPos.x;
		vSphereCoord.y += m_vLookPos.y;
		vSphereCoord.z += m_vLookPos.z;

		// Lock On 했다가 다시 제자리로 돌아가는 시간 (Lerp)
		if (0.f < m_fUnLockTime)
		{

			if (1.7 >= m_fUnLockTime)
				m_pTargetUI->SetActive(false);

			else
			{
				m_fTargetScale -= fTimeDelta * 10.f;

				if (0.f >= m_fTargetScale)
					m_pTargetUI->SetActive(false);

				m_pTargetUI->Get_TransformCom()->Set_Scale(m_fTargetScale, m_fTargetScale, 1.f);
				m_pTargetUI->WorldToScreenPos(m_vTargetObjectPos + XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.f);
			}

			m_fUnLockTime -= fTimeDelta;

			m_vTargetPos = XMVectorLerp(m_vTargetPos, XMVectorSet(vSphereCoord.x, vSphereCoord.y, vSphereCoord.z, 1.f), 1 - m_fUnLockTime / 2.f);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vTargetPos);

			m_vTargetDir = XMVectorLerp(m_vTargetDir, XMVectorSet(m_vLookPos.x, m_vLookPos.y, m_vLookPos.z, 1.f), 1 - m_fUnLockTime / 2.f);
			m_pTransformCom->LookAt(m_vTargetDir);
		}

		else
		{
			m_pTargetUI->SetActive(false);

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(vSphereCoord.x, vSphereCoord.y, vSphereCoord.z, 1.f));

			m_pTransformCom->LookAt(XMVectorSet(m_vLookPos.x, m_vLookPos.y, m_vLookPos.z, 1.f));
		}


		//목표지점으로 카메라를 보게함
		//m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&vLookPos));

		//_vector vTargetPos = m_pTargetTransform.lock()->Get_State(CTransform::STATE_POSITION);
	}

	else
	{
		// Position 보간
		m_vTargetPos	= XMVectorLerp(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_vTargetPos + m_pTransformCom->Get_State(CTransform::STATE_RIGHT), m_fLerpRatio);
		m_vTargetPos	+= XMVectorLerp({ 0.f, 0.f, 0.f, 0.f }, { 0.f, m_vLookPos.y / 3.f, 0.f, 0.f }, m_fLerpRatio);
		
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vTargetPos);

		// Look 보간
		m_vTargetDir = XMVectorLerp(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_vTargetDir, m_fLerpRatio);

		m_pTransformCom->SetLook(m_vTargetDir);

		// Dir을 LookAt에서도 적용하기 위해서 현재 위치 기준 Dir로 바꿔줌
		m_vTargetDir = m_vTargetPos + m_vTargetDir;

		m_pSphereicalCoord->ChangeCoordinates(m_vTargetPos - m_pTargetTransform.lock()->Get_State(CTransform::STATE_POSITION), XMVectorGetZ(XMVector4Length(m_vTargetPos - m_pTargetTransform.lock()->Get_State(CTransform::STATE_POSITION))));

		m_pTargetUI->SetActive(true);
		m_pTargetUI->Get_TransformCom()->Set_Scale(m_fTargetScale, m_fTargetScale, 1.f);
		m_pTargetUI->WorldToScreenPos(m_vTargetObjectPos + XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.f);
	}

	if (FAILED(SetUp_TransformMatrices()))
		return;
}

void CCamera_Attach::Tick(_float fTimeDelta)
{
}

void CCamera_Attach::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCamera_Attach::Render()
{
    return S_OK;
}

void CCamera_Attach::ChangeTargetCamLerp(_vector vTargetDir, _float fLerpRatio, _vector vTargetPos)
{
	m_vTargetDir = vTargetDir;
	m_fLerpRatio = fLerpRatio;

	m_vTargetPos = m_pTargetTransform.lock()->Get_State(CTransform::STATE_POSITION) - vTargetDir * 3.f;
	//m_vTargetPos.m128_f32[1] += m_vCamPos.y;

	m_vTargetObjectPos = vTargetPos;

	m_isLocked = true;
	m_fUnLockTime = 2.f;
	m_fTargetScale = 3.f;
}

shared_ptr<CCamera_Attach> CCamera_Attach::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
	shared_ptr<CCamera_Attach> pInstance = make_shared<CCamera_Attach>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CCamera_Attach");
		pInstance.reset();
	}

	return pInstance;
}

void CCamera_Attach::Free()
{
}
