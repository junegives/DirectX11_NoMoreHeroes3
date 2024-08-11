#include "pch.h"
//#include "Camera_Manager.h"
//
//#include "Camera.h"
//
//IMPLEMENT_SINGLETON_SMART(CCamera_Manager)
//
//CCamera_Manager::CCamera_Manager()
//	: m_pGameInstance(CGameInstance::GetInstance())
//{
//	m_pCameraList.reserve((size_t)CAMERA_END);
//}
//
//HRESULT CCamera_Manager::Initialize()
//{
//	return S_OK;
//}
//
//void CCamera_Manager::AddCamera(CAMERA_ID eCamID, shared_ptr<CCamera> pCamera)
//{
//	m_pCameraList[eCamID] = pCamera;
//}
//
//shared_ptr<CCamera> CCamera_Manager::GetCamera(CAMERA_ID eCamID)
//{
//	return m_pCameraList[eCamID];
//}
//
//void CCamera_Manager::ChangeMainCam(CAMERA_ID eCamID)
//{
//	for (size_t i = 0; i < m_pCameraList.size(); i++)
//	{
//		if (eCamID == i)
//		{
//			m_pCameraList[i]->SetActive(true);
//			m_pMainCamera = m_pCameraList[i];
//
//			continue;
//		}
//
//		m_pCameraList[i]->SetActive(false);
//	}
//}