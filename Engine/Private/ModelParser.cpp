#include "ModelParser.h"
#include "Model.h"
#include "Mesh.h"
#include "Bone.h"
#include "Animation.h"

namespace fs = std::experimental::filesystem;

CModelParser::CModelParser()
{
}

HRESULT CModelParser::Read_Dir(const wstring& strDirectoryPath, _bool isStatic)
{
	ZeroMemory(&m_Model, sizeof(psMODEL));

	for (const auto& entry : fs::directory_iterator(strDirectoryPath))
	{
		if (fs::is_directory(entry))
		{
			if (entry.path().filename() == "." || entry.path().filename() == "..")
				continue;

			Read_Dir(entry.path(), isStatic);
		}
		else if (fs::is_regular_file(entry))
		{
			if (".fbx" == entry.path().extension().string())
			{
				Read_Origin_File(entry.path(), isStatic);
			}
		}
	}

	return S_OK;
}

HRESULT CModelParser::Read_Origin_File(const wstring& strFilePath, _bool isStatic)
{
	string ModelFilePath;
	ModelFilePath.assign(strFilePath.begin(), strFilePath.end());

	_char		szDrive[MAX_PATH] = "";
	_char		szDirectory[MAX_PATH] = "";
	_char		szFileName[MAX_PATH] = "";

	_splitpath_s(ModelFilePath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);

	_char		szTmp[MAX_PATH] = "";
	strcpy_s(szTmp, szDrive);
	strcat_s(szTmp, szDirectory);
	strcat_s(szTmp, szFileName);
	strcat_s(szTmp, ".model");

	_tchar		szExportFileName[MAX_PATH] = TEXT("");
	MultiByteToWideChar(CP_ACP, 0, szTmp, strlen(szTmp), szExportFileName, MAX_PATH);

	shared_ptr<CFile_Manager> pFile = make_shared<CFile_Manager>();
	pFile->Open(szExportFileName, true);

	if (isStatic)
	{
		if (FAILED(Write_Static_Model(ModelFilePath, pFile)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(Write_Dynamic_Model(ModelFilePath, pFile)))
			return E_FAIL;
	}

	pFile->Close();

	return S_OK;
}

HRESULT CModelParser::Write_Static_Model(const string& strModelFilePath, shared_ptr<class CFile_Manager> pFile)
{
	Assimp::Importer Import;

	_uint			iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast | aiProcess_PreTransformVertices;
	const aiScene* pAIScene = Import.ReadFile(strModelFilePath.c_str(), iFlag);

	m_Model.isStatic = true;
	pFile->Write(true);

	m_Model.iNumMeshes = pAIScene->mNumMeshes;
	m_Model.iNumMaterials = pAIScene->mNumMaterials;

	pFile->Write(pAIScene->mNumMeshes);
	pFile->Write(pAIScene->mNumMaterials);

	if (FAILED(Write_Static_Meshes(pAIScene, pFile)))
		return E_FAIL;

	if (FAILED(Write_Materials(pAIScene, strModelFilePath, pFile)))
		return E_FAIL;

	pFile->Close();

	return S_OK;
}

HRESULT CModelParser::Write_Dynamic_Model(const string& strModelFilePath, shared_ptr<class CFile_Manager> pFile)
{
	Assimp::Importer Import;

	_uint			iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	const aiScene* pAIScene = Import.ReadFile(strModelFilePath.c_str(), iFlag);

	m_Model.isStatic = false;
	pFile->Write(false);

	m_Model.iNumMeshes = pAIScene->mNumMeshes;
	m_Model.iNumMaterials = pAIScene->mNumMaterials;
	m_Model.iNumAnimations = pAIScene->mNumAnimations;

	pFile->Write(pAIScene->mNumMeshes);
	pFile->Write(pAIScene->mNumMaterials);
	pFile->Write(pAIScene->mNumAnimations);

	if (FAILED(Write_Bones(pAIScene, pFile)))
		return E_FAIL;

	if (FAILED(Write_Dynamic_Meshes(pAIScene, pFile)))
		return E_FAIL;

	if (FAILED(Write_Materials(pAIScene, strModelFilePath, pFile)))
		return E_FAIL;

	if (FAILED(Write_Animations(pAIScene, pFile)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModelParser::Write_Static_Meshes(const aiScene* pAIScene, shared_ptr<CFile_Manager> pFile)
{
	/*pFile->Write(m_Model.iNumMeshes);
	pFile->Write(m_Model.iNumMaterials);*/

	for (size_t i = 0; i < m_Model.iNumMeshes; i++)
	{
		aiMesh* pMesh = pAIScene->mMeshes[i];

		/*psMESH_STATIC MeshStatic;
		ZeroMemory(&MeshStatic, sizeof(psMESH_STATIC));

		MeshStatic.strMeshName = pMesh->mName.C_Str();
		MeshStatic.iMaterialIndex = pMesh->mMaterialIndex;
		MeshStatic.iNumVertices = pMesh->mNumVertices;
		MeshStatic.iNumFaces = pMesh->mNumFaces;*/

		string strMeshName = pMesh->mName.C_Str();

		pFile->Write(strMeshName);
		pFile->Write(pMesh->mMaterialIndex);
		pFile->Write(pMesh->mNumVertices);
		pFile->Write(pMesh->mNumFaces);

		//MeshStatic.Vertices = new VTXMESH[pMesh->mNumVertices];

		_float3 vPosition;
		_float3 vNormal;
		_float2 vTexcoord;
		_float3 vTangent;

		for (size_t j = 0; j < pMesh->mNumVertices; j++)
		{
			/*memcpy(&MeshStatic.Vertices[j].vPosition, &pMesh->mVertices[j], sizeof(_float3));
			memcpy(&MeshStatic.Vertices[j].vNormal, &pMesh->mNormals[j], sizeof(_float3));
			memcpy(&MeshStatic.Vertices[j].vTexcoord, &pMesh->mTextureCoords[0][j], sizeof(_float2));
			memcpy(&MeshStatic.Vertices[j].vTangent, &pMesh->mTangents[j], sizeof(_float3));*/

			memcpy(&vPosition, &pMesh->mVertices[j], sizeof(vPosition));
			pFile->Write(vPosition);

			memcpy(&vNormal, &pMesh->mNormals[j], sizeof(vNormal));
			pFile->Write(vNormal);

			memcpy(&vTexcoord, &pMesh->mTextureCoords[0][j], sizeof(vTexcoord));
			pFile->Write(vTexcoord);

			memcpy(&vTangent, &pMesh->mTangents[j], sizeof(vTangent));
			pFile->Write(vTangent);
		}

		//MeshStatic.vIndices = new _uint[pMesh->mNumFaces * 3];

		_uint iNumIndices = { 0 };

		for (size_t k = 0; k < pMesh->mNumFaces; k++)
		{
			/*MeshStatic.vIndices[iNumIndices++] = pMesh->mFaces[k].mIndices[0];
			MeshStatic.vIndices[iNumIndices++] = pMesh->mFaces[k].mIndices[1];
			MeshStatic.vIndices[iNumIndices++] = pMesh->mFaces[k].mIndices[2];*/

			pFile->Write(pMesh->mFaces[k].mIndices[0]);
			pFile->Write(pMesh->mFaces[k].mIndices[1]);
			pFile->Write(pMesh->mFaces[k].mIndices[2]);
		}

		//pFile->Write(MeshStatic.vIndices, pMesh->mNumFaces * 3 * sizeof(_uint));
	}

	return S_OK;
}

HRESULT CModelParser::Write_Materials(const aiScene* pAIScene, const string& strModelFilePath, shared_ptr<CFile_Manager> pFile)
{
	for (size_t i = 0; i < m_Model.iNumMaterials; i++)
	{
		/*psMATERIAL Material;
		ZeroMemory(&Material, sizeof(psMATERIAL));*/

		aiMaterial* pMaterial = pAIScene->mMaterials[i];
		//Material.strMaterialName = pMaterial->GetName().C_Str();

		string strMaterialName = pMaterial->GetName().C_Str();

		pFile->Write(strMaterialName);
		//pFile->Write(Material.strMaterialName);

		for (size_t j = 0; j < TEXTYPE_MAX; j++)
		{
			//Material.MaterialTextures[j].TexType = TEXTYPE(j);
			pFile->Write(j);

			aiString	strAITexturePath;

			if (FAILED(pMaterial->GetTexture(aiTextureType(j), 0, &strAITexturePath)))
			{
				string      strTexturePath = "";
				//pFile->Write<string>(strTexturePath);

				//Material.MaterialTextures[j].strTexPath = "";
				pFile->Write(false);
			}
			else
			{
				string			strTexturePath = strAITexturePath.data;

				char			szDrive[MAX_PATH] = "";
				char			szDirectory[MAX_PATH] = "";
				_splitpath_s(strModelFilePath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

				char			szFileName[MAX_PATH] = "";
				char			szExt[MAX_PATH] = ".png";
				_splitpath_s(strTexturePath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

				char			szFullPath[MAX_PATH] = "";
				strcpy_s(szFullPath, szDrive);
				strcat_s(szFullPath, szDirectory);
				strcat_s(szFullPath, szFileName);
				strcat_s(szFullPath, szExt);

				pFile->Write(true);
				string strFullPath = szFullPath;
				pFile->Write(strFullPath);
				//Material.MaterialTextures[j].strTexPath = szFullPath;
			}

			//pFile->Write(Material.MaterialTextures[j].TexType);
			//pFile->Write(Material.MaterialTextures[j].strTexPath);
		}
	}

	return S_OK;
}

HRESULT CModelParser::Write_Dynamic_Meshes(const aiScene* pAIScene, shared_ptr<CFile_Manager> pFile)
{
	/*pFile->Write(m_Model.iNumMeshes);
	pFile->Write(m_Model.iNumMaterials);
	pFile->Write(m_Model.iNumAnimations);*/

	for (size_t i = 0; i < m_Model.iNumMeshes; i++)
	{
		aiMesh* pMesh = pAIScene->mMeshes[i];

		/*psMESH_DYNAMIC MeshDynamic;
		ZeroMemory(&MeshDynamic, sizeof(psMESH_DYNAMIC));*/

		/*MeshDynamic.strMeshName = pMesh->mName.data;
		MeshDynamic.iMaterialIndex = pMesh->mMaterialIndex;
		MeshDynamic.iNumVertices = pMesh->mNumVertices;
		MeshDynamic.iNumFaces = pMesh->mNumFaces;

		pFile->Write(MeshDynamic.strMeshName);
		pFile->Write(MeshDynamic.iMaterialIndex);
		pFile->Write(MeshDynamic.iNumVertices);
		pFile->Write(MeshDynamic.iNumFaces);*/

		string strMeshName = pMesh->mName.C_Str();

		pFile->Write(strMeshName);
		pFile->Write(pMesh->mMaterialIndex);
		pFile->Write(pMesh->mNumVertices);
		pFile->Write(pMesh->mNumFaces);

		_float3 vPosition;
		_float3 vNormal;
		_float2 vTexcoord;
		_float3 vTangent;

		//MeshDynamic.Vertices = new VTXANIMMESH[pMesh->mNumVertices];

		for (size_t j = 0; j < pMesh->mNumVertices; j++)
		{

			memcpy(&vPosition, &pMesh->mVertices[j], sizeof(vPosition));
			pFile->Write(vPosition);

			memcpy(&vNormal, &pMesh->mNormals[j], sizeof(vNormal));
			pFile->Write(vNormal);

			memcpy(&vTexcoord, &pMesh->mTextureCoords[0][j], sizeof(vTexcoord));
			pFile->Write(vTexcoord);

			memcpy(&vTangent, &pMesh->mTangents[j], sizeof(vTangent));
			pFile->Write(vTangent);
		}

		//pFile->Write(MeshDynamic.vIndices, pMesh->mNumFaces * 3 * sizeof(_uint));

		_bool isNoBone = false;
		_uint iNumBones = pMesh->mNumBones;

		//MeshDynamic.iNumBones = pMesh->mNumBones;

		if (0 == iNumBones)
		{
			iNumBones = 1;
			isNoBone = true;
		}

		pFile->Write(iNumBones);

		/*MeshDynamic.iBoneIndices = new _uint[MeshDynamic.iNumBones];
		MeshDynamic.OffsetMatrices = new _float4x4[MeshDynamic.iNumBones];*/

		VTXANIMMESH* Vertices = new VTXANIMMESH[pMesh->mNumVertices];
		ZeroMemory(Vertices, sizeof(VTXANIMMESH) * pMesh->mNumVertices);	// 정점 데이터 배열을 0으로 초기화

		// 메쉬에 본이 없는 경우 본 1개로 설정
		if (isNoBone)
		{
			_int iBoneIndex;

			iBoneIndex =
				find_if(m_vecBone.begin(), m_vecBone.end(), [strMeshName](const psBONE& bone) {
				return bone.strBoneName == strMeshName;
					})->iBoneIndex;

			_float4x4 OffsetMatrix;

			XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

			pFile->Write(iBoneIndex);
			pFile->Write(OffsetMatrix);
		}

		else
		{
			for (size_t l = 0; l < pMesh->mNumBones; l++)
			{
				const aiBone* pBone = pMesh->mBones[l];

				_float4x4 OffsetMatrix;
				memcpy(&OffsetMatrix, &pBone->mOffsetMatrix, sizeof(_float4x4));
				XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

				string searchBoneName = pBone->mName.C_Str();

				_int iBoneIndex;
				// 본 이름으로 m_Model의 본 벡터에서 인덱스만 가져와서 저장하기
				iBoneIndex =
					find_if(m_vecBone.begin(), m_vecBone.end(), [searchBoneName](const psBONE& bone) {
					return bone.strBoneName == searchBoneName;
						})->iBoneIndex;

				pFile->Write(iBoneIndex);
				pFile->Write(OffsetMatrix);

				for (size_t m = 0; m < pBone->mNumWeights; m++)
				{
					if (0.f == Vertices[pBone->mWeights[m].mVertexId].vBlendWeights.x)
					{
						Vertices[pBone->mWeights[m].mVertexId].vBlendIndices.x = l;
						Vertices[pBone->mWeights[m].mVertexId].vBlendWeights.x = pBone->mWeights[m].mWeight;
					}

					else if (0.f == Vertices[pBone->mWeights[m].mVertexId].vBlendWeights.y)
					{
						Vertices[pBone->mWeights[m].mVertexId].vBlendIndices.y = l;
						Vertices[pBone->mWeights[m].mVertexId].vBlendWeights.y = pBone->mWeights[m].mWeight;
					}

					else if (0.f == Vertices[pBone->mWeights[m].mVertexId].vBlendWeights.z)
					{
						Vertices[pBone->mWeights[m].mVertexId].vBlendIndices.z = l;
						Vertices[pBone->mWeights[m].mVertexId].vBlendWeights.z = pBone->mWeights[m].mWeight;
					}

					else if (0.f == Vertices[pBone->mWeights[m].mVertexId].vBlendWeights.w)
					{
						Vertices[pBone->mWeights[m].mVertexId].vBlendIndices.w = l;
						Vertices[pBone->mWeights[m].mVertexId].vBlendWeights.w = pBone->mWeights[m].mWeight;
					}
				}
			}
		}

		//pFile->Write(MeshDynamic.Vertices, pMesh->mNumVertices * sizeof(VTXANIMMESH));

		//pFile->Write(MeshDynamic.iNumBones);
		//pFile->Write(MeshDynamic.iBoneIndices, MeshDynamic.iNumBones * sizeof(_uint));
		//pFile->Write(MeshDynamic.OffsetMatrices, MeshDynamic.iNumBones * sizeof(_float4x4));

		for (size_t z = 0; z < pMesh->mNumVertices; z++)
		{
			pFile->Write(Vertices[z].vBlendIndices);
			pFile->Write(Vertices[z].vBlendWeights);
		}

		_uint iNumIndices = { 0 };

		for (size_t k = 0; k < pMesh->mNumFaces; k++)
		{
			/*MeshDynamic.vIndices[iNumIndices++] = pMesh->mFaces[k].mIndices[0];
			MeshDynamic.vIndices[iNumIndices++] = pMesh->mFaces[k].mIndices[1];
			MeshDynamic.vIndices[iNumIndices++] = pMesh->mFaces[k].mIndices[2];*/

			pFile->Write(pMesh->mFaces[k].mIndices[0]);
			pFile->Write(pMesh->mFaces[k].mIndices[1]);
			pFile->Write(pMesh->mFaces[k].mIndices[2]);
		}

		Safe_Delete_Array(Vertices);
	}

	return S_OK;
}

HRESULT CModelParser::Write_Bones(const aiScene* pAIScene, shared_ptr<CFile_Manager> pFile)
{
	Write_BoneNode(pAIScene->mRootNode, -1, pFile);

	pFile->Write(m_vecBone.size());

	string strBoneName;

	for (auto iter : m_vecBone)
	{
		strBoneName = iter.strBoneName;

		pFile->Write(strBoneName);
		pFile->Write(iter.iParentBoneIndex);
		pFile->Write(iter.iBoneIndex);
		pFile->Write(iter.TransformMatrix);
	}

	return S_OK;
}

// 모델이 가진 전체 본
// 본 이름, 부모 인덱스, 본인 인덱스, 변환 행렬 가짐
HRESULT CModelParser::Write_BoneNode(const aiNode* pAINode, _int iParentBoneIndex, shared_ptr<CFile_Manager> pFile)
{
	psBONE Bone;
	Bone.strBoneName = pAINode->mName.data;
	Bone.iParentBoneIndex = iParentBoneIndex;
	Bone.iBoneIndex = m_vecBone.size();

	memcpy(&Bone.TransformMatrix, &pAINode->mTransformation, sizeof(_float4x4));

	m_vecBone.push_back(Bone);

	_uint iParentIndex = Bone.iBoneIndex;

	for (size_t i = 0; i < pAINode->mNumChildren; i++)
	{
		Write_BoneNode(pAINode->mChildren[i], iParentIndex, pFile);
	}

	return S_OK;
}

HRESULT CModelParser::Write_Animations(const aiScene* pAIScene, shared_ptr<CFile_Manager> pFile)
{
	string strAnimName;
	string strNodeName;

	_double			fTime;

	_float3         vScale;
	_float4         vRotation;
	_float3         vPosition;

	for (size_t i = 0; i < m_Model.iNumAnimations; i++)
	{
		/*psANIMATION Animation;
		ZeroMemory(&Animation, sizeof(psANIMATION));*/

		aiAnimation* pAnimation = pAIScene->mAnimations[i];

		/*Animation.strAnimName = pAnimation->mName.data;
		Animation.fDuration = pAnimation->mDuration;
		Animation.fTickPerSecond = pAnimation->mTicksPerSecond;


		Animation.iNumChannels = pAnimation->mNumChannels;*/

		/*pFile->Write(Animation.strAnimName);
		pFile->Write(Animation.fDuration);
		pFile->Write(Animation.fTickPerSecond);
		pFile->Write(Animation.iNumChannels);*/
		strAnimName = pAnimation->mName.data;

		pFile->Write(strAnimName);
		pFile->Write(pAnimation->mDuration);
		pFile->Write(pAnimation->mTicksPerSecond);
		pFile->Write(pAnimation->mNumChannels);

		/*Animation.Channels = new psCHANNEL[pAnimation->mNumChannels];
		ZeroMemory(Animation.Channels, sizeof(psCHANNEL) * pAnimation->mNumChannels);*/

		for (size_t j = 0; j < pAnimation->mNumChannels; j++)
		{
			/*psCHANNEL Channel;
			ZeroMemory(&Channel, sizeof(psCHANNEL));*/

			aiNodeAnim* pAIChannel = pAnimation->mChannels[j];

			_uint iNumKeyFrame;

			iNumKeyFrame = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
			iNumKeyFrame = max(iNumKeyFrame, pAIChannel->mNumPositionKeys);

			//Channel.strChannelName = Animation.Channels[j].strChannelName;

			/*pFile->Write(Channel.strChannelName);
			pFile->Write(Channel.iNumKeyFrame);*/

			strNodeName = pAIChannel->mNodeName.data;

			pFile->Write(strNodeName);
			pFile->Write(iNumKeyFrame);

			/*Channel.KeyFrames = new psKEYFRAME[Channel.iNumKeyFrame];
			ZeroMemory(Channel.KeyFrames, sizeof(psKEYFRAME) * Channel.iNumKeyFrame);*/

			for (size_t k = 0; k < iNumKeyFrame; k++)
			{
				if (pAIChannel->mNumScalingKeys > k)
				{
					memcpy(&vScale, &pAIChannel->mScalingKeys[k].mValue, sizeof(_float3));
					fTime = pAIChannel->mScalingKeys[k].mTime;
				}

				// 로테이션키가 아직 덜채워졌으면
				if (pAIChannel->mNumRotationKeys > k)
				{
					// mRotationKeys[i].mValue의 순서가 쿼터니언이라 w, x, y, z이기 때문에 바로 복사해주는 것 말고 속성별로 대입
					// memcpy(&vRotation, &pAIChannel->mRotationKeys[i].mValue, sizeof(_float4));
					vRotation.x = pAIChannel->mRotationKeys[k].mValue.x;
					vRotation.y = pAIChannel->mRotationKeys[k].mValue.y;
					vRotation.z = pAIChannel->mRotationKeys[k].mValue.z;
					vRotation.w = pAIChannel->mRotationKeys[k].mValue.w;
					fTime = pAIChannel->mRotationKeys[k].mTime;
				}

				// 포지션키가 아직 덜채워졌으면
				if (pAIChannel->mNumPositionKeys > k)
				{
					memcpy(&vPosition, &pAIChannel->mPositionKeys[k].mValue, sizeof(_float3));
					fTime = pAIChannel->mPositionKeys[k].mTime;
				}

				pFile->Write(vScale);
				pFile->Write(vRotation);
				pFile->Write(vPosition);
				pFile->Write(fTime);
			}

			//Animation.Channels[j] = Channel;
		}
	}

	return S_OK;
}

HRESULT CModelParser::Read_Parsed_File(const wstring& strFilePath)
{
	shared_ptr<CFile_Manager> pFile = make_shared<CFile_Manager>();
	pFile->Open(strFilePath, false);

	ZeroMemory(&m_Model, sizeof(psMODEL));

	pFile->Read(m_Model.isStatic);
	pFile->Read(m_Model.iNumMeshes);
	pFile->Read(m_Model.iNumMaterials);

	if (m_Model.isStatic)
	{
		if (FAILED(Read_Static_Model(pFile)))
			return E_FAIL;
	}
	else
	{
		pFile->Read(m_Model.iNumAnimations);

		if (FAILED(Read_Dynamic_Model(pFile)))
			return E_FAIL;
	}

	pFile->Close();

	return S_OK;
}

HRESULT CModelParser::Read_Static_Model(shared_ptr<CFile_Manager> pFile)
{
	if (FAILED(Read_Static_Meshes(pFile)))
		return E_FAIL;

	if (FAILED(Read_Materials(pFile)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModelParser::Read_Dynamic_Model(shared_ptr<CFile_Manager> pFile)
{
	if (FAILED(Read_Bones(pFile)))
		return E_FAIL;

	if (FAILED(Read_Static_Meshes(pFile)))
		return E_FAIL;

	if (FAILED(Read_Materials(pFile)))
		return E_FAIL;

	if (FAILED(Read_Animations(pFile)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModelParser::Read_Static_Meshes(shared_ptr<CFile_Manager> pFile)
{
	string	strMeshName;
	_uint	iMaterialIndex;
	_uint	iNumVertices;
	_uint	iNumFaces;

	_float3 vPosition;
	_float3 vNormal;
	_float2 vTexcoord;
	_float3 vTangent;

	_uint	iIndex1;
	_uint	iIndex2;
	_uint	iIndex3;

	for (size_t i = 0; i < m_Model.iNumMeshes; i++)
	{
		//psMESH_STATIC MeshStatic;

		pFile->Read(strMeshName);
		pFile->Read(iMaterialIndex);
		pFile->Read(iNumVertices);
		pFile->Read(iNumFaces);

		/*MeshStatic.vIndices = new _uint[MeshStatic.iNumFaces * 3];
		MeshStatic.Vertices = new VTXMESH[MeshStatic.iNumVertices];*/

		for (size_t k = 0; k < iNumVertices; k++)
		{
			pFile->Read(vPosition);
			pFile->Read(vNormal);
			pFile->Read(vTexcoord);
			pFile->Read(vTangent);
		}

		for (size_t k = 0; k < iNumFaces; k++)
		{
			pFile->Read(iIndex1);
			pFile->Read(iIndex2);
			pFile->Read(iIndex3);
		}

		/*pFile->Read(MeshStatic.vIndices, MeshStatic.iNumFaces * 3 * sizeof(_uint));
		pFile->Read(MeshStatic.Vertices, MeshStatic.iNumVertices * sizeof(VTXMESH));*/
	}

	return S_OK;
}

HRESULT CModelParser::Read_Dynamic_Meshes(shared_ptr<CFile_Manager> pFile)
{
	return E_NOTIMPL;
}

HRESULT CModelParser::Read_Materials(shared_ptr<CFile_Manager> pFile)
{
	for (size_t i = 0; i < m_Model.iNumMaterials; i++)
	{
		//psMATERIAL Material;

		string strMaterialName;

		pFile->Read(strMaterialName);

		for (size_t j = 0; j < TEXTYPE_MAX; j++)
		{
			size_t iType;
			_bool isExist;
			string strTexPath;

			pFile->Read(iType);
			pFile->Read(isExist);

			if(isExist)
				pFile->Read(strTexPath);
		}
	}

	return S_OK;
}

HRESULT CModelParser::Read_Bones(shared_ptr<CFile_Manager> pFile)
{
	return E_NOTIMPL;
}

HRESULT CModelParser::Read_Animations(shared_ptr<CFile_Manager> pFile)
{
	return E_NOTIMPL;
}