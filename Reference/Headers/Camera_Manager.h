#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CCamera_Manager final
{
public:
	enum CAMERA_ID { CAM_FREE, CAM_ATTACH, CAM_EVENT, CAM_END };

public:
	CCamera_Manager();
	~CCamera_Manager() = default;

public:
	HRESULT Initialize();

public:
	void							AddCamera(CAMERA_ID eCamType, shared_ptr<class CGameObject> pCamera) { m_pCamera[eCamType] = pCamera; }
	shared_ptr<class CGameObject>	GetCamera(CAMERA_ID eCamType) { return m_pCamera[eCamType].lock(); }

	void							SetMainCam(CAMERA_ID eCamID);

private:
	weak_ptr<class CGameObject>		m_pCamera[CAM_END];
	CAMERA_ID						m_eMainCam = CAM_END;

public:
	static	shared_ptr<CCamera_Manager> Create();
	void	Free();
};

END