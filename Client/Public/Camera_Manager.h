#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Manager final
{
//	DECLARE_SINGLETON_SMART(CCamera_Manager)
//
//public:
//	enum CAMERA_ID { CAMERA_FREE, CAMERA_ATTACH, CAMERA_CUTSCENE, CAMERA_END };
//	
//public:
//	CCamera_Manager();
//	~CCamera_Manager() = default;
//
//public:
//	void						AddCamera(CAMERA_ID eCamID, shared_ptr<class CCamera> pCamera);
//	shared_ptr<class CCamera>	GetCamera(CAMERA_ID eCamID);
//
//	void						ChangeMainCam(CAMERA_ID eCamID);
//
//private:
//	shared_ptr<class CGameInstance>			m_pGameInstance = { nullptr };
//	shared_ptr<class CCamera>				m_pMainCamera = { nullptr };
//	vector<shared_ptr<class CCamera>>		m_pCameraList = { nullptr };
};

END