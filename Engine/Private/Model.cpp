#include "Model.h"

#include "Bone.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Animation.h"

CModel::CModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel& rhs)
    : CComponent(rhs)
    , m_PivotMatrix(rhs.m_PivotMatrix)
    , m_iNumMeshes(rhs.m_iNumMeshes)
    , m_Meshes(rhs.m_Meshes)
    , m_iNumMaterials(rhs.m_iNumMaterials)
    , m_Materials(rhs.m_Materials)
    , m_iNumAnimations(rhs.m_iNumAnimations)
    //, m_Bones(rhs.m_Bones)                (얕복되면 안되는 애)
    //, m_Animations(rhs.m_Animations)      (얕복되면 안되는 애)
{
    for (auto& pPrototypeAnimation : rhs.m_Animations)
        m_Animations.push_back(pPrototypeAnimation->Clone());

    for (auto& pPrototypeBone : rhs.m_Bones)
        m_Bones.push_back(pPrototypeBone->Clone());
}

_int CModel::Get_BoneIndex(const _char* pBoneName) const
{
    _int        iBoneIndex = { -1 };

    auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](shared_ptr<CBone> pBone) {

        iBoneIndex++;

        // 본의 이름이 일치하면 현재 iter 반환
        if (false == strcmp(pBoneName, pBone->Get_BoneName()))
            return true;
        // 다음 조건으로 넘어감
        return false;
    });

    if (iter == m_Bones.end())
        return -1;

    return iBoneIndex;
}

HRESULT CModel::Initialize_Prototype(TYPE eModelType, const _char* pModelFilePath, _fmatrix PivotMatrix)
{
    m_eModelType = eModelType;

    XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

    // aiProcess_ConvertToLeftHanded : 모델의 좌표계를 왼손 좌표계로 변환
    // aiProcessPreset_TargetRealtime_Fast : 모델을 실시간 렌더링에 최적화된 형태로 처리하기 위한 사전 정의 프로세스
    _uint			iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

    // 움직이지 않는 모델인 경우 미리 계산된 변환 정보를 적용하는 플래그 추가
    // aiProcess_PreTransformVertices : 모델의 각 메쉬들이 어디에 붙어있어야 하는지에 대한 정보를 갖고 있음 (알맞은 뼈에 정점을 붙임)
    // 메쉬들이 정확한 위치에 정확한 상태로 붙음 (스케일링까지)
    // 모델들의 정보를 로드할 때부터 정확한 위치에 갖다 붙임.
    // 움직이는 모델은 쓰면 안됨
    // -> 이미 변환된 뼈의 상태에 모델의 움직임으로 인해 변환하면 두 번 변환 행렬을 곱하게 된다.
    if (TYPE_NONANIM == eModelType)
        iFlag |= aiProcess_PreTransformVertices;

    // 3D 모델 파일을 로드하는 함수 (지정된 파일 경로에서 3D 모델을 읽어와서 m_pAIScene에 저장한다.)
    // (로드할 3D 모델 파일의 경로, 모델을 불러올 때 사용하는 플래그)
    m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
    if (nullptr == m_pAIScene)
        return E_FAIL;

    /* aiScene안에있는 정보들을 내가 사용하기 좋도록 정리하면된다.  */

    // 뼈 정보가 모델의 루트 노드에서 시작하기 때문에 RootNode부터 각 노드에서 뼈를 생성하고 저장한다.
    if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
        return E_FAIL;

    /* 모델을 구성하는 정점과 인덱스를 생성한다. */
    /* 모델 = 메시 + 메시 + 메시 ... */
    if (FAILED(Ready_Meshes()))
        return E_FAIL;

    /* 텍스쳐를 로드해야한다. (머테리얼을 로드한다.) */
    if (FAILED(Ready_Materials(pModelFilePath)))
        return E_FAIL;

    // 애니메이션 로드
    if (FAILED(Ready_Animations()))
        return E_FAIL;

    return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
    m_Meshes[iMeshIndex]->Bind_Buffers();
    m_Meshes[iMeshIndex]->Render();

    return S_OK;
}

HRESULT CModel::Bind_Material_ShaderResource(shared_ptr<CShader> pShader, _uint iMeshIndex, aiTextureType eMaterialType, const _char* pConstantName)
{
    _uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

    if (m_Materials[iMaterialIndex].pMtrlTextures[eMaterialType])
        return m_Materials[iMaterialIndex].pMtrlTextures[eMaterialType]->Bind_ShaderResource(pShader, pConstantName, 0);

    return S_OK;
}

HRESULT CModel::Bind_BoneMatrices(shared_ptr<CShader> pShader, const _char* pConstantName, _uint iMeshIndex)
{
	_float4x4		BoneMatrices[256];

	m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, m_Bones);

	return pShader->Bind_Matrices(pConstantName, BoneMatrices, 256);
}

void CModel::Play_Animation(_float fTimeDelta, _bool isLoop)
{
    /* 현재 애니메이션 상태에 맞게 뼈들의 TransformationMatrix행렬을 갱신한다. */
    m_Animations[m_iCurrentAnimation]->Invalidate_TransformationMatrix(fTimeDelta, m_Bones, isLoop);


    /* 모든 뼈들의 CombinedTransformationMatrix를 구해준다. */
    for (auto& pBone : m_Bones)
        pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
}

HRESULT CModel::Ready_Meshes()
{
    // 메시의 개수 받아오기
    m_iNumMeshes = m_pAIScene->mNumMeshes;

    // 메시 개수만큼 반복하면서
    for (size_t i = 0; i < m_iNumMeshes; i++)
    {
        // 메시 정보를 넘기면서 Mesh 객체 생성
        shared_ptr<CMesh> pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], shared_from_this(), XMLoadFloat4x4(&m_PivotMatrix));
        if (nullptr == pMesh)
            return E_FAIL;

        // 벡터에 메시들 저장
        m_Meshes.push_back(pMesh);
    }

    return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
    // 파일 경로에서 드라이브 정보를 저장할 문자열 배열
    _char           szDrive[MAX_PATH] = "";
    // 파일 경로에서 디렉토리 정보를 저장할 문자열 배열
    _char           szDirectory[MAX_PATH] = "";

    // 모델 파일 경로에서 드라이브 및 디렉토리 정보 추출
    _splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

    // 모델의 머티리얼 개수 저장
    m_iNumMaterials = m_pAIScene->mNumMaterials;

    // 머티리얼 개수만큼 반복
    for (size_t i = 0; i < m_iNumMaterials; i++)
    {
        // 머티리얼 구조체 초기화
        MESH_MATERIAL_DESC          MaterialDesc = {};

        // 현재 인덱스에 해당하는 머티리얼 가져옴
        aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

        // 텍스처 타입에 대해 반복
        for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
        {
            // 하나의 머티리얼에 디퓨즈 텍스처가 여러개일 수도 있음 (롤 미니언 빨강 파랑) -> 삼중 루프
            // pAIMaterial->GetTextureCount(aiTexterType(j));

            // 텍스처 파일 경로를 저장할 Assimp 문자열
            aiString            strTextureFilePath;

            // 현재 텍스처 타입에 대한 텍스처 정보 가져오고, 실패 시 반복 계속 (faile 뜨면 텍스처가 없는 것이므로 continue로 넘어가기)
            // 첫 번째 인자 : 가져올 텍스처의 종류를 나타내는 열거형 (ex, aiTextureType_DIFFUSE)
            // 두 번째 인자 : 텍스처 타입에 해당하는 여러 텍스처 중 어떤 것을 가져올지 지정하는 인덱스
            // -> 0이면 디퓨즈도 한 장, 엠비언트도 한 장, 스케뮬러도 한 장
            // -> 한 텍스처가 여러개면 개수만큼 1, 2, 3...
            // 세 번째 인자 : 텍스처 파일의 경로를 나타내는 'aiString' 변수
            if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTextureFilePath)))
                continue;

            // 텍스처 파일의 이름과 확장자를 저장할 문자열 배열
            _char               szFileName[MAX_PATH] = "";
            _char               szExt[MAX_PATH] = "";

            // 텍스처 파일 경로에서 파일 이름과 확장자 추출
            // aiString.data : 해당 문자열의 첫 번째 문자에 대한 포인터 반환
            _splitpath_s(strTextureFilePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

            // 드라이브, 디렉토리, 파일 이름, 확장자를 합쳐 완전한 텍스처 파일 경로 생성
            _char               szTmp[MAX_PATH] = "";
            strcpy_s(szTmp, szDrive);
            strcat_s(szTmp, szDirectory);
            strcat_s(szTmp, szFileName);
            strcat_s(szTmp, szExt);

            // 멀티바이트 문자열을 유니코드 문자열로 변환하여 텍스처 파일의 완전한 경로 획득
            _tchar              szFullPath[MAX_PATH] = TEXT("");
            MultiByteToWideChar(CP_ACP, 0, szTmp, strlen(szTmp), szFullPath, MAX_PATH);

            // 완전한 텍스처 파일 경로를 사용하여 CTexture 객체를 생성하고, 해당 텍스처를 MaterialDesc 구조체에 할당
            MaterialDesc.pMtrlTextures[j] = CTexture::Create(m_pDevice, m_pContext, szFullPath, 1);
        }
        // 현재 머티리얼 정보를 m_Materials 벡터에 추가
        m_Materials.push_back(MaterialDesc);
    }

    return S_OK;
}

HRESULT CModel::Ready_Bones(aiNode* pNode, _int iParentBoneIndex)
{
    // node를 기반으로 하는 뼈 객체 생성
    // iParentBoneIndex를 한 번 저장 후에 후위증가연산자로, 다음 인덱스로 넘겨주기
    shared_ptr<CBone> pBone = CBone::Create(pNode, iParentBoneIndex, XMLoadFloat4x4(&m_PivotMatrix));
    if (nullptr == pBone)
        return E_FAIL;

    m_Bones.push_back(pBone);

    _uint	iParentIndex = m_Bones.size() - 1;

    for (size_t i = 0; i < pNode->mNumChildren; i++)
    {
        // 현재 노드의 자식 노드들에 대해 재귀적으로 Ready_Bones 호출
        // 자식 노드들에 대한 부모 뼈의 인덱스 함께 넘김
        Ready_Bones(pNode->mChildren[i], iParentIndex);
    }

    return S_OK;
}

HRESULT CModel::Ready_Animations()
{
    m_iNumAnimations = m_pAIScene->mNumAnimations;

    for (size_t i = 0; i < m_iNumAnimations; i++)
    {
        shared_ptr<CAnimation>  pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], shared_from_this());
        if (nullptr == pAnimation)
            return E_FAIL;

        m_Animations.push_back(pAnimation);
    }

    return S_OK;
}


_bool CModel::Picking(_long iMouseX, _long iMouseY, _matrix matWorld, _float3* vPickPoint, _float* fMinDist)
{
    for (size_t i = 0; i < m_iNumMeshes; i++)
    {
        if (m_Meshes[i]->Picking(iMouseX, iMouseY, matWorld, vPickPoint, fMinDist))
            return true;
    }

    return false;
}

shared_ptr<CModel> CModel::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, TYPE eModelType, const _char* pModelFilePath, _fmatrix PivotMatrix)
{
    shared_ptr<CModel> pInstance = make_shared<CModel>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(eModelType, pModelFilePath, PivotMatrix)))
    {
        MSG_BOX("Failed to Creatd : CModel");
        pInstance.reset();
    }

    return pInstance;
}

shared_ptr<CComponent> CModel::Clone(void* pArg)
{
    shared_ptr<CModel> pInstance = make_shared<CModel>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Creatd : CModel");
        pInstance.reset();
    }

    return pInstance;
}

void CModel::Free()
{
    __super::Free();

    // 모델이 알아서 지워진다.
    m_Importer.FreeScene();
}
