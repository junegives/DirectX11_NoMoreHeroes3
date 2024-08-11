#pragma once

#include "Engine_Defines.h"
#include "File_Manager.h"
#include "Model.h"

BEGIN(Engine)

class ENGINE_DLL CModelParser final
{
public:
	CModelParser();
	~CModelParser() = default;

public:
	/* 경로 상의 .fbx 파일을 읽어서 write */
	HRESULT	Read_Dir(const wstring& strDirectoryPath, _bool isStatic);

	HRESULT Read_Origin_File(const wstring& strFilePath, _bool isStatic);

	HRESULT Write_Static_Model(const string& strModelFilePath, shared_ptr<CFile_Manager> pFile);
	HRESULT Write_Dynamic_Model(const string& strModelFilePath, shared_ptr<CFile_Manager> pFile);

	HRESULT Write_Static_Meshes(const aiScene* pAIScene, shared_ptr<CFile_Manager> pFile);
	HRESULT Write_Materials(const aiScene* pAIScene, const string& strModelFilePath, shared_ptr<CFile_Manager> pFile);

	HRESULT Write_Bones(const aiScene* pAIScene, shared_ptr<CFile_Manager> pFile);
	HRESULT Write_BoneNode(const aiNode* pAINode, _int iParentBoneIndex, shared_ptr<CFile_Manager> pFile);
	HRESULT Write_Dynamic_Meshes(const aiScene* pAIScene, shared_ptr<CFile_Manager> pFile);
	HRESULT Write_Animations(const aiScene* pAIScene, shared_ptr<CFile_Manager> pFile);

	HRESULT Read_Parsed_File(const wstring& strFilePath);
	HRESULT Read_Static_Model(shared_ptr<CFile_Manager> pFile);
	HRESULT Read_Dynamic_Model(shared_ptr<CFile_Manager> pFile);
	HRESULT Read_Static_Meshes(shared_ptr<CFile_Manager> pFile);
	HRESULT Read_Materials(shared_ptr<CFile_Manager> pFile);
	HRESULT Read_Dynamic_Meshes(shared_ptr<CFile_Manager> pFile);
	HRESULT Read_Bones(shared_ptr<CFile_Manager> pFile);
	HRESULT Read_Animations(shared_ptr<CFile_Manager> pFile);

private:
	psMODEL			m_Model;
	vector<psBONE>	m_vecBone;
};

END