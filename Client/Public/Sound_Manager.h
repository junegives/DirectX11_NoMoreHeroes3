#pragma once

#include "Client_Defines.h"
#include <fmod.h>

class CSound_Manager
{
	DECLARE_SINGLETON_SMART(CSound_Manager)

public:
	enum CHANNELID
	{
		BGM, PLAYER, PLAYER_STEP, PLAYER_WEAPON, PLAYER_ATTACK,
		CAT, CAT_STEP, PROP1, PROP2, PROP3, PROP4, PROP5,
		MONSTER1, MONSTER1_ATTACK, MONSTER1_ATTACK2, MONSTER2, MONSTER2_ATTACK, MONSTER2_ATTACK2, MONSTER3, MONSTER3_ATTACK, MONSTER3_ATTACK2,
		ENUM_END
	};

public:
	CSound_Manager();
	~CSound_Manager();

public:
	void Initialize();
	void Release();

public:
	void PlaySound(const TCHAR* _pSoundKey, CHANNELID eID, _float _fVolume);
	void PlayBGM(const TCHAR* _pSoundKey, _float _fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();

private:
	void LoadSoundFile();

private:

	map<TCHAR*, FMOD_SOUND*> m_mapSound;
	// FMOD_CHANNEL : ����ϰ� �ִ� ���带 ������ ��ü 
	FMOD_CHANNEL* m_pChannelArr[(unsigned long long)CHANNELID::ENUM_END];
	// ���� ,ä�� ��ü �� ��ġ�� �����ϴ� ��ü 
	FMOD_SYSTEM* m_pSystem;
};