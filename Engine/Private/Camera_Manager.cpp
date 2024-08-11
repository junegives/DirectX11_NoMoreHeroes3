#include "Camera_Manager.h"
#include "Camera.h"

CCamera_Manager::CCamera_Manager()
{
}

HRESULT CCamera_Manager::Initialize()
{
	return S_OK;
}

void CCamera_Manager::SetMainCam(CAMERA_ID eCamID)
{
	m_eMainCam = eCamID;

	for (size_t i = 0; i < CAM_END; i++)
	{
		if (!m_pCamera[i].lock())
			continue;

		if (eCamID == i)
		{
			m_pCamera[i].lock()->SetActive(true);
			continue;
		}

		m_pCamera[i].lock()->SetActive(false);
	}
}

shared_ptr<CCamera_Manager> CCamera_Manager::Create()
{
	shared_ptr<CCamera_Manager> pInstance = make_shared<CCamera_Manager>();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CCamera_Manager");
		pInstance.reset();
	}

	return pInstance;
}

void CCamera_Manager::Free()
{
}
