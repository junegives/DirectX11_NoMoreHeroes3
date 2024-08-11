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
    //, m_Bones(rhs.m_Bones)                (�躹�Ǹ� �ȵǴ� ��)
    //, m_Animations(rhs.m_Animations)      (�躹�Ǹ� �ȵǴ� ��)
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

        // ���� �̸��� ��ġ�ϸ� ���� iter ��ȯ
        if (false == strcmp(pBoneName, pBone->Get_BoneName()))
            return true;
        // ���� �������� �Ѿ
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

    // aiProcess_ConvertToLeftHanded : ���� ��ǥ�踦 �޼� ��ǥ��� ��ȯ
    // aiProcessPreset_TargetRealtime_Fast : ���� �ǽð� �������� ����ȭ�� ���·� ó���ϱ� ���� ���� ���� ���μ���
    _uint			iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

    // �������� �ʴ� ���� ��� �̸� ���� ��ȯ ������ �����ϴ� �÷��� �߰�
    // aiProcess_PreTransformVertices : ���� �� �޽����� ��� �پ��־�� �ϴ����� ���� ������ ���� ���� (�˸��� ���� ������ ����)
    // �޽����� ��Ȯ�� ��ġ�� ��Ȯ�� ���·� ���� (�����ϸ�����)
    // �𵨵��� ������ �ε��� ������ ��Ȯ�� ��ġ�� ���� ����.
    // �����̴� ���� ���� �ȵ�
    // -> �̹� ��ȯ�� ���� ���¿� ���� ���������� ���� ��ȯ�ϸ� �� �� ��ȯ ����� ���ϰ� �ȴ�.
    if (TYPE_NONANIM == eModelType)
        iFlag |= aiProcess_PreTransformVertices;

    // 3D �� ������ �ε��ϴ� �Լ� (������ ���� ��ο��� 3D ���� �о�ͼ� m_pAIScene�� �����Ѵ�.)
    // (�ε��� 3D �� ������ ���, ���� �ҷ��� �� ����ϴ� �÷���)
    m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
    if (nullptr == m_pAIScene)
        return E_FAIL;

    /* aiScene�ȿ��ִ� �������� ���� ����ϱ� ������ �����ϸ�ȴ�.  */

    // �� ������ ���� ��Ʈ ��忡�� �����ϱ� ������ RootNode���� �� ��忡�� ���� �����ϰ� �����Ѵ�.
    if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
        return E_FAIL;

    /* ���� �����ϴ� ������ �ε����� �����Ѵ�. */
    /* �� = �޽� + �޽� + �޽� ... */
    if (FAILED(Ready_Meshes()))
        return E_FAIL;

    /* �ؽ��ĸ� �ε��ؾ��Ѵ�. (���׸����� �ε��Ѵ�.) */
    if (FAILED(Ready_Materials(pModelFilePath)))
        return E_FAIL;

    // �ִϸ��̼� �ε�
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
    /* ���� �ִϸ��̼� ���¿� �°� ������ TransformationMatrix����� �����Ѵ�. */
    m_Animations[m_iCurrentAnimation]->Invalidate_TransformationMatrix(fTimeDelta, m_Bones, isLoop);


    /* ��� ������ CombinedTransformationMatrix�� �����ش�. */
    for (auto& pBone : m_Bones)
        pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
}

HRESULT CModel::Ready_Meshes()
{
    // �޽��� ���� �޾ƿ���
    m_iNumMeshes = m_pAIScene->mNumMeshes;

    // �޽� ������ŭ �ݺ��ϸ鼭
    for (size_t i = 0; i < m_iNumMeshes; i++)
    {
        // �޽� ������ �ѱ�鼭 Mesh ��ü ����
        shared_ptr<CMesh> pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], shared_from_this(), XMLoadFloat4x4(&m_PivotMatrix));
        if (nullptr == pMesh)
            return E_FAIL;

        // ���Ϳ� �޽õ� ����
        m_Meshes.push_back(pMesh);
    }

    return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
    // ���� ��ο��� ����̺� ������ ������ ���ڿ� �迭
    _char           szDrive[MAX_PATH] = "";
    // ���� ��ο��� ���丮 ������ ������ ���ڿ� �迭
    _char           szDirectory[MAX_PATH] = "";

    // �� ���� ��ο��� ����̺� �� ���丮 ���� ����
    _splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

    // ���� ��Ƽ���� ���� ����
    m_iNumMaterials = m_pAIScene->mNumMaterials;

    // ��Ƽ���� ������ŭ �ݺ�
    for (size_t i = 0; i < m_iNumMaterials; i++)
    {
        // ��Ƽ���� ����ü �ʱ�ȭ
        MESH_MATERIAL_DESC          MaterialDesc = {};

        // ���� �ε����� �ش��ϴ� ��Ƽ���� ������
        aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

        // �ؽ�ó Ÿ�Կ� ���� �ݺ�
        for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
        {
            // �ϳ��� ��Ƽ���� ��ǻ�� �ؽ�ó�� �������� ���� ���� (�� �̴Ͼ� ���� �Ķ�) -> ���� ����
            // pAIMaterial->GetTextureCount(aiTexterType(j));

            // �ؽ�ó ���� ��θ� ������ Assimp ���ڿ�
            aiString            strTextureFilePath;

            // ���� �ؽ�ó Ÿ�Կ� ���� �ؽ�ó ���� ��������, ���� �� �ݺ� ��� (faile �߸� �ؽ�ó�� ���� ���̹Ƿ� continue�� �Ѿ��)
            // ù ��° ���� : ������ �ؽ�ó�� ������ ��Ÿ���� ������ (ex, aiTextureType_DIFFUSE)
            // �� ��° ���� : �ؽ�ó Ÿ�Կ� �ش��ϴ� ���� �ؽ�ó �� � ���� �������� �����ϴ� �ε���
            // -> 0�̸� ��ǻ� �� ��, �����Ʈ�� �� ��, ���ɹķ��� �� ��
            // -> �� �ؽ�ó�� �������� ������ŭ 1, 2, 3...
            // �� ��° ���� : �ؽ�ó ������ ��θ� ��Ÿ���� 'aiString' ����
            if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTextureFilePath)))
                continue;

            // �ؽ�ó ������ �̸��� Ȯ���ڸ� ������ ���ڿ� �迭
            _char               szFileName[MAX_PATH] = "";
            _char               szExt[MAX_PATH] = "";

            // �ؽ�ó ���� ��ο��� ���� �̸��� Ȯ���� ����
            // aiString.data : �ش� ���ڿ��� ù ��° ���ڿ� ���� ������ ��ȯ
            _splitpath_s(strTextureFilePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

            // ����̺�, ���丮, ���� �̸�, Ȯ���ڸ� ���� ������ �ؽ�ó ���� ��� ����
            _char               szTmp[MAX_PATH] = "";
            strcpy_s(szTmp, szDrive);
            strcat_s(szTmp, szDirectory);
            strcat_s(szTmp, szFileName);
            strcat_s(szTmp, szExt);

            // ��Ƽ����Ʈ ���ڿ��� �����ڵ� ���ڿ��� ��ȯ�Ͽ� �ؽ�ó ������ ������ ��� ȹ��
            _tchar              szFullPath[MAX_PATH] = TEXT("");
            MultiByteToWideChar(CP_ACP, 0, szTmp, strlen(szTmp), szFullPath, MAX_PATH);

            // ������ �ؽ�ó ���� ��θ� ����Ͽ� CTexture ��ü�� �����ϰ�, �ش� �ؽ�ó�� MaterialDesc ����ü�� �Ҵ�
            MaterialDesc.pMtrlTextures[j] = CTexture::Create(m_pDevice, m_pContext, szFullPath, 1);
        }
        // ���� ��Ƽ���� ������ m_Materials ���Ϳ� �߰�
        m_Materials.push_back(MaterialDesc);
    }

    return S_OK;
}

HRESULT CModel::Ready_Bones(aiNode* pNode, _int iParentBoneIndex)
{
    // node�� ������� �ϴ� �� ��ü ����
    // iParentBoneIndex�� �� �� ���� �Ŀ� �������������ڷ�, ���� �ε����� �Ѱ��ֱ�
    shared_ptr<CBone> pBone = CBone::Create(pNode, iParentBoneIndex, XMLoadFloat4x4(&m_PivotMatrix));
    if (nullptr == pBone)
        return E_FAIL;

    m_Bones.push_back(pBone);

    _uint	iParentIndex = m_Bones.size() - 1;

    for (size_t i = 0; i < pNode->mNumChildren; i++)
    {
        // ���� ����� �ڽ� ���鿡 ���� ��������� Ready_Bones ȣ��
        // �ڽ� ���鿡 ���� �θ� ���� �ε��� �Բ� �ѱ�
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

    // ���� �˾Ƽ� ��������.
    m_Importer.FreeScene();
}
