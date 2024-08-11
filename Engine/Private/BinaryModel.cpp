#include "BinaryModel.h"

#include "BinaryBone.h"
#include "BinaryMesh.h"
#include "Shader.h"
#include "Texture.h"
#include "BinaryAnimation.h"
#include "BinaryChannel.h"

#include "Transform.h"
#include "Navigation.h"

#include "ModelParser.h"

CBinaryModel::CBinaryModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CComponent(pDevice, pContext)
{
}

CBinaryModel::CBinaryModel(const CBinaryModel& rhs)
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

_int CBinaryModel::Get_BoneIndex(const string& pBoneName) const
{
    _int        iBoneIndex = { -1 };

    auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](shared_ptr<CBinaryBone> pBone) {

        iBoneIndex++;

        // 본의 이름이 일치하면 현재 iter 반환
        if (pBoneName ==  pBone->Get_BoneName())
            return true;
        // 다음 조건으로 넘어감
        return false;
    });

    if (iter == m_Bones.end())
        return -1;

    return iBoneIndex;
}

_float4x4* CBinaryModel::Get_CombinedBoneMatrixPtr(const string& pBoneName)
{
    auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](shared_ptr<CBinaryBone> pBone)
        {
            if (pBone->Get_BoneName() == pBoneName)
                return true;

            return false;
        });

    return (*iter)->Get_CombindTransformationMatrixPtr();
}

HRESULT CBinaryModel::Initialize_Prototype(TYPE eModelType, const _char* pModelFilePath, _fmatrix PivotMatrix)
{
    m_eModelType = eModelType;

    XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

    shared_ptr<CModelParser> pModelParser = make_shared<CModelParser>();

    _tchar              szFullPath[MAX_PATH] = TEXT("");
    MultiByteToWideChar(CP_ACP, 0, pModelFilePath, strlen(pModelFilePath), szFullPath, MAX_PATH);

    Read_Parsed_File(szFullPath);

    //m_RootOffsetMatrix = m_Meshes[0]->

    //// 뼈 정보가 모델의 루트 노드에서 시작하기 때문에 RootNode부터 각 노드에서 뼈를 생성하고 저장한다.
    //if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
    //    return E_FAIL;

    ///* 모델을 구성하는 정점과 인덱스를 생성한다. */
    ///* 모델 = 메시 + 메시 + 메시 ... */
    //if (FAILED(Ready_Meshes()))
    //    return E_FAIL;

    ///* 텍스쳐를 로드해야한다. (머테리얼을 로드한다.) */
    //if (FAILED(Ready_Materials(pModelFilePath)))
    //    return E_FAIL;

    //// 애니메이션 로드
    //if (FAILED(Ready_Animations()))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CBinaryModel::Initialize(void* pArg)
{
    m_isBlockRootMotion = false;
    return S_OK;
}

HRESULT CBinaryModel::Render(_uint iMeshIndex)
{
    m_Meshes[iMeshIndex]->Bind_Buffers();
    m_Meshes[iMeshIndex]->Render();

    return S_OK;
}

HRESULT CBinaryModel::Bind_Material_ShaderResource(shared_ptr<CShader> pShader, _uint iMeshIndex, aiTextureType eMaterialType, const _char* pConstantName)
{
    _uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

    if (m_Materials[iMaterialIndex].pMtrlTextures[eMaterialType])
        return m_Materials[iMaterialIndex].pMtrlTextures[eMaterialType]->Bind_ShaderResource(pShader, pConstantName, 0);

    return S_OK;
}

HRESULT CBinaryModel::Bind_BoneMatrices(shared_ptr<CShader> pShader, const _char* pConstantName, _uint iMeshIndex)
{
	_float4x4		BoneMatrices[256];

	m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, m_Bones);

	return pShader->Bind_Matrices(pConstantName, BoneMatrices, 256);
}

HRESULT CBinaryModel::Bind_PrevBoneMatrices(shared_ptr<class CShader> pShader, const _char* pConstantName, _float4x4* BoneMatrices)
{
    return pShader->Bind_Matrices(pConstantName, BoneMatrices, 256);
}

void CBinaryModel::Play_Animation(_float fTimeDelta, _Out_ _float4x4* pRootTransform, _bool isLoop)
{

    /* 현재 애니메이션 상태에 맞게 뼈들의 TransformationMatrix행렬을 갱신한다. */
    m_Animations[m_iCurrentAnimation]->Invalidate_TransformationMatrix(fTimeDelta, m_Bones);

    /* 모든 뼈들의 CombinedTransformationMatrix를 구해준다. */
    for (auto& pBone : m_Bones)
        pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
}

void CBinaryModel::Change_Animation(_uint iNextAnimation)
{
   /* m_iNextAnimation = iNextAnimation;
    m_Animations[m_iCurrentAnimation]->SetNextAnim_Initailize();*/
}

_bool CBinaryModel::Change_Animation_Lerp(_float fTimeDelta, _double Time, _Out_ _float4x4* pRootTransform)
{
    //if (m_Animations[m_iCurrentAnimation]->SetNextAnim_TransfomaionMatrix(fTimeDelta, Time, m_Animations[m_iNextAnimation], m_Bones, pRootTransform))
    //{
    //    m_iCurrentAnimation = m_iNextAnimation;
    //    return true;
    //}

    ///* 모든 뼈들의 CombinedTransformationMatrix를 구해준다. */
    //for (auto& pBone : m_Bones)
    //    pBone->Invalidate_CombinedTransformationMatrix(m_Bones);

    return false;
}

HRESULT CBinaryModel::Set_Animation(_uint iAnimIndex, _bool isLoop, _float fAnimSpeed, _float fChangeDuration, _uint iStartNumKeyFrames)
{
    m_isAnimChange = true;
    m_isAnimStop = false;
    m_isAnimChangeFinish = false;
    m_isAnimFirst = true;

    m_iNextAnimation = iAnimIndex;
    m_fChangeTrackPosition = 0.f;
    m_fChangeDuration = fChangeDuration;

    m_CurrentChannels = m_Animations[m_iCurrentAnimation]->GetChannels();
    m_NextChannels = m_Animations[m_iNextAnimation]->GetChannels();
    m_iNextStartKeyFrame = iStartNumKeyFrames;

    m_Animations[m_iNextAnimation]->SetLoop(isLoop);

    m_Animations[m_iNextAnimation]->SetAnimSpeed(fAnimSpeed);

    m_Animations[m_iCurrentAnimation]->Reset();

    m_PrevRootPos = { 0.f, 0.f, 0.f, 1.f };
    m_CurRootPos = { 0.f, 0.f, 0.f, 1.f };

    m_PrevRootLook = { 0.f, 0.f, 1.f ,0.f };
    m_CurRootLook = { 0.f, 0.f, 1.f ,0.f };

    return S_OK;
}

void CBinaryModel::Play_Animation(_float fTimeDelta, shared_ptr<CTransform> pTransformCom)
{
    m_isAnimChangeFinish = false;

    // 애니메이션 변경중인 경우
    if (m_isAnimChange) {
        Change_Animation(fTimeDelta, m_fChangeDuration);
    }

    // 그렇지 않은 경우
    else {
        /* 현재 애니메이션 상태에 맞게 뼈들의 TransformationMatrix행렬을 갱신한다. */
        m_isAnimStop = m_Animations[m_iCurrentAnimation]->Invalidate_TransformationMatrix(fTimeDelta, m_Bones);
    }

    /* 모든 뼈들의 CombinedTransformationMatrix를 구해준다. */
    for (auto& pBone : m_Bones)
        pBone->Invalidate_CombinedTransformationMatrix(m_Bones);

    // 루트 모션을 위해 최상위 본 가져오기
    shared_ptr<CBinaryBone> pRootBone = m_Bones[Get_BoneIndex("root")];

    m_PrevRootPos = m_CurRootPos;
    m_CurRootPos = pRootBone->Get_RootPos();

    m_PrevRootLook = m_CurRootLook;
    m_CurRootLook = pRootBone->Get_RootPos();

    m_RootOffsetMatrix = pRootBone->Get_CombinedTransformationMatrix();

    if (m_Animations[m_iCurrentAnimation]->IsLoop() && m_Animations[m_iCurrentAnimation]->IsFinish())
    {
        m_PrevRootPos = { 0.f, 0.f, 0.f, 1.f };
        m_CurRootPos = { 0.f, 0.f, 0.f, 1.f };

        m_PrevRootLook = { 0.f, 0.f, 1.f ,0.f };
        m_CurRootLook = { 0.f, 0.f, 1.f ,0.f };
    }
    if (m_isAnimStop || m_isAnimChangeFinish)
    {
        m_PrevRootPos = { 0.f, 0.f, 0.f, 1.f };
        m_CurRootPos = { 0.f, 0.f, 0.f, 1.f };

        m_PrevRootLook = { 0.f, 0.f, 1.f ,0.f };
        m_CurRootLook = { 0.f, 0.f, 1.f ,0.f };
    }
}

HRESULT CBinaryModel::Change_Animation(_float fTimeDelta, _float fChangeDuration)
{
    if (0.f == m_fChangeTrackPosition)
    {
        for (size_t i = 0; i < m_Animations[m_iNextAnimation]->GetNumChannels(); i++)
        {
            m_BeforeChangeKeyFrames[i] = m_CurrentChannels[i]->GetCurrentKeyFrame();
        }

        m_Animations[m_iNextAnimation]->SetStartKeyFrames(m_iNextStartKeyFrame, fTimeDelta, m_Bones);
    }

    m_fChangeTrackPosition += fTimeDelta;

    _vector	vScale;
    _vector vRotation;
    _vector vTranslation;

    CBinaryChannel::KEYFRAME NextKeyframe;

    for (size_t i = 0; i < m_Animations[m_iNextAnimation]->GetNumChannels(); i++)
    {
        NextKeyframe = m_NextChannels[i]->GetCurrentKeyFrame();

        while (m_fChangeTrackPosition >= fChangeDuration)
        {
            m_isAnimChange = false;
            m_isAnimChangeFinish = true;

            m_iCurrentAnimation = m_iNextAnimation;

            m_Animations[m_iCurrentAnimation]->Reset();
            m_Animations[m_iCurrentAnimation]->SetStartKeyFrames(m_iNextStartKeyFrame, fTimeDelta, m_Bones);

            return S_OK;
        }

        _float	fRatio = (m_fChangeTrackPosition - 0.f) / fChangeDuration;
        if (1.f <= fRatio)
            fRatio = 1.f;

        _vector vSourScale = XMLoadFloat3(&m_BeforeChangeKeyFrames[i].vScale);
        _vector vDestScale = XMLoadFloat3(&NextKeyframe.vScale);
        vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);

        _vector vSourRotation = XMLoadFloat4(&m_BeforeChangeKeyFrames[i].vRotation);
        _vector vDestRotation = XMLoadFloat4(&NextKeyframe.vRotation);
        vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);

        _vector vSourTranslation = XMLoadFloat3(&m_BeforeChangeKeyFrames[i].vPosition);
        _vector vDestTranslation = XMLoadFloat3(&NextKeyframe.vPosition);
        vTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, fRatio);

        _matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

        m_Bones[m_CurrentChannels[i]->GetBoneIndex()]->Set_TranslationMatrix(TransformationMatrix);
    }
    return S_OK;
}

void CBinaryModel::Apply_RootMotion(_float fTimeDelta, shared_ptr<CTransform> pTransformCom, shared_ptr<CNavigation> pNavigation, _bool isYChange)
{
    if (m_isAnimChange || m_isBlockRootMotion || (0.f == m_CurRootPos.m128_f32[0] && 0.f == m_CurRootPos.m128_f32[2]))
    {
        return;
    }

    // 애니메이션의 첫 프레임인 경우 루트모션 적용 X
    if (m_isAnimFirst) {
        m_isAnimFirst = false;
        return;
    }

    _vector vOriginPos = pTransformCom->Get_State(CTransform::STATE_POSITION);
    _vector vOriginLook = pTransformCom->Get_State(CTransform::STATE_LOOK);

    _float4 vTranslate;
    _float4 vLookChange;

    XMStoreFloat4(&vTranslate, m_CurRootPos);
    XMStoreFloat4(&vLookChange, m_CurRootLook);

    _vector vPos = vOriginPos + XMVector4Transform((XMLoadFloat4(&vTranslate) - m_PrevRootPos), pTransformCom->Get_WorldMatrix());
    _vector vLook = vOriginLook + XMVector4Transform((XMLoadFloat4(&vLookChange) - m_PrevRootLook), pTransformCom->Get_WorldMatrix());

    if (nullptr == pNavigation || true == pNavigation->isMove(vPos))
    {
        pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
        m_PrevRootMotion = XMLoadFloat4(&vTranslate);

        /*pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
        m_PrevRootMotionLook = XMLoadFloat4(&vLookChange);*/

        return;
    }
    // 움직이려는 곳에 못갈때 일단 슬라이딩 벡터로라도 가자
    else if (false == pNavigation->isMove(vPos))
    {

        /*pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
        m_PrevRootMotionLook = XMLoadFloat4(&vLookChange);*/

        _vector vSlide = pNavigation->CalcSlideVec(vPos - vOriginPos);

        _vector vSlidePos = vOriginPos + vSlide;

        // 슬라이딩 벡터로 간 곳에도 못가면?? 슬라이딩 집어치우고y값이나 바꾸자
        if (false == pNavigation->isMove(vSlidePos))
        {
            if (isYChange)
            {
                pTransformCom->Set_State(CTransform::STATE_POSITION, {vOriginPos.m128_f32[0], vPos.m128_f32[1], vOriginPos.m128_f32[2], 1.f});
                m_PrevRootMotion = XMLoadFloat4(&vTranslate);
            }
            else
            {
                m_PrevRootMotion = { 0.f, 0.f, 0.f, 0.f };
            }
            return;
        }

        // 슬라이딩 벡터로 갈 수 있으면 계속 가
        pTransformCom->Set_State(CTransform::STATE_POSITION, vSlidePos);
        m_PrevRootMotion = XMLoadFloat4(&vTranslate);
    }
    else
    {
        /*pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
        m_PrevRootMotionLook = XMLoadFloat4(&vLookChange);*/
    }
}

_float CBinaryModel::Get_KeyFrameRatio(_float fTrackPos)
{
    return m_Animations[m_iCurrentAnimation]->GetTrackPosRatio(fTrackPos);
}

_float CBinaryModel::Get_CurKeyFrameRatio()
{
    return m_Animations[m_iCurrentAnimation]->GetCurTrackPosRatio();
}

_float CBinaryModel::Get_CurKeyFrameTime()
{
    return m_Animations[m_iCurrentAnimation]->GetCurTrackPos();
}

HRESULT CBinaryModel::Copy_BoneMatrix(_uint iMeshIndex, _float4x4* pBoneMatrices)
{
    _float4x4 BoneMatrices[256];

    m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, m_Bones);

    memcpy(pBoneMatrices, BoneMatrices, sizeof(_float4x4) * 256);

    return S_OK;
}

HRESULT CBinaryModel::Read_Parsed_File(const wstring& strFilePath)
{
    shared_ptr<CFile_Manager> pFile = make_shared<CFile_Manager>();
    pFile->Open(strFilePath, false);

    ZeroMemory(&m_Model, sizeof(psMODEL));

    pFile->Read(m_Model.isStatic);
    pFile->Read(m_Model.iNumMeshes);
    pFile->Read(m_Model.iNumMaterials);

    m_iNumMeshes = m_Model.iNumMeshes;
    m_iNumMaterials = m_Model.iNumMaterials;

    if (m_Model.isStatic)
    {
        if (FAILED(Read_Static_Model(pFile)))
            return E_FAIL;
    }
    else
    {
        pFile->Read(m_iNumAnimations);

        if (FAILED(Read_Dynamic_Model(pFile)))
            return E_FAIL;
    }

    pFile->Close();

    return S_OK;
}

HRESULT CBinaryModel::Read_Static_Model(shared_ptr<CFile_Manager> pFile)
{
    if (FAILED(Read_Static_Meshes(pFile)))
        return E_FAIL;

    if (FAILED(Read_Materials(pFile)))
        return E_FAIL;
}

HRESULT CBinaryModel::Read_Dynamic_Model(shared_ptr<CFile_Manager> pFile)
{
    if (FAILED(Read_Bones(pFile)))
        return E_FAIL;

    if (FAILED(Read_Dynamic_Meshes(pFile)))
        return E_FAIL;

    if (FAILED(Read_Materials(pFile)))
        return E_FAIL;

    if (FAILED(Read_Animations(pFile)))
        return E_FAIL;
}

HRESULT CBinaryModel::Read_Static_Meshes(shared_ptr<CFile_Manager> pFile)
{
    // 메시 개수만큼 반복하면서
    for (size_t i = 0; i < m_iNumMeshes; i++)
    {
        // 메시 정보를 넘기면서 Mesh 객체 생성
        shared_ptr<CBinaryMesh> pMesh = CBinaryMesh::Create(m_pDevice, m_pContext, m_eModelType, pFile, shared_from_this(), XMLoadFloat4x4(&m_PivotMatrix));
        if (nullptr == pMesh)
            return E_FAIL;

        // 벡터에 메시들 저장
        m_Meshes.push_back(pMesh);
    }

    return S_OK;
}

HRESULT CBinaryModel::Read_Dynamic_Meshes(shared_ptr<CFile_Manager> pFile)
{
    // 메시 개수만큼 반복하면서
    for (size_t i = 0; i < m_iNumMeshes; i++)
    {
        // 메시 정보를 넘기면서 Mesh 객체 생성
        shared_ptr<CBinaryMesh> pMesh = CBinaryMesh::Create(m_pDevice, m_pContext, m_eModelType, pFile, shared_from_this(), XMLoadFloat4x4(&m_PivotMatrix));
        if (nullptr == pMesh)
            return E_FAIL;

        // 벡터에 메시들 저장
        m_Meshes.push_back(pMesh);
    }

    return S_OK;
}

HRESULT CBinaryModel::Read_Materials(shared_ptr<CFile_Manager> pFile)
{
    size_t iType;
    _bool isExist;
    string strTexPath;

    // 머티리얼 개수만큼 반복
    for (size_t i = 0; i < m_iNumMaterials; i++)
    {
        // 머티리얼 구조체 초기화
        MESH_MATERIAL_DESC          MaterialDesc = {};

        string strMaterialName;

        pFile->Read(strMaterialName);

        for (size_t j = 0; j < TEXTYPE_MAX; j++)
        {

            pFile->Read(iType);
            pFile->Read(isExist);

            if (!isExist)
                continue;

            pFile->Read(strTexPath);

            // 완전한 텍스처 파일 경로를 사용하여 CTexture 객체를 생성하고, 해당 텍스처를 MaterialDesc 구조체에 할당
            MaterialDesc.pMtrlTextures[j] = CTexture::Create(m_pDevice, m_pContext, wstring().assign(strTexPath.begin(), strTexPath.end()), 1);
        }

        // 현재 머티리얼 정보를 m_Materials 벡터에 추가
        m_Materials.push_back(MaterialDesc);
    }

    return S_OK;
}

HRESULT CBinaryModel::Read_Bones(shared_ptr<CFile_Manager> pFile)
{
    size_t iNumBones;

    pFile->Read(iNumBones);

    string      strBoneName;
    _int        iParentBoneIndex;
    _int        iBoneIndex;
    _float4x4   TransformMatrix;

    for (size_t i = 0; i < iNumBones; i++)
    {
        pFile->Read(strBoneName);
        pFile->Read(iParentBoneIndex);
        pFile->Read(iBoneIndex);
        pFile->Read(TransformMatrix);

        if (2 == iNumBones)
            m_RootOffsetMatrix = TransformMatrix;

        shared_ptr<CBinaryBone> pBone = CBinaryBone::Create(strBoneName, iParentBoneIndex, iBoneIndex, XMLoadFloat4x4(&TransformMatrix), XMLoadFloat4x4(&m_PivotMatrix));
        m_Bones.push_back(pBone);
    }

    return S_OK;
}

HRESULT CBinaryModel::Read_Animations(shared_ptr<CFile_Manager> pFile)
{
    for (size_t i = 0; i < m_iNumAnimations; i++)
    {
        shared_ptr<CBinaryAnimation> pAnimation = CBinaryAnimation::Create(pFile, shared_from_this());
        if (nullptr == pAnimation)
            return E_FAIL;

        m_Animations.push_back(pAnimation);
    }

    return S_OK;
}




_bool CBinaryModel::Picking(_long iMouseX, _long iMouseY, _matrix matWorld, _float3* vPickPoint, _float* fMinDist)
{
    *fMinDist = -1.f;
    _float  fCheckDist = -1.f;
    _float3 vCheckPickPoint = { 0.f, 0.f, 0.f };

    for (size_t i = 0; i < m_iNumMeshes; i++)
    {
        if (m_Meshes[i]->Picking(iMouseX, iMouseY, matWorld, &vCheckPickPoint, &fCheckDist))
        {
            if (-1.f == *fMinDist)
            {
                *fMinDist = fCheckDist;
                vPickPoint->x = vCheckPickPoint.x;
                vPickPoint->y = vCheckPickPoint.y;
                vPickPoint->z = vCheckPickPoint.z;
            }
            else if (*fMinDist > fCheckDist)
            {
                *fMinDist = fCheckDist;
                vPickPoint->x = vCheckPickPoint.x;
                vPickPoint->y = vCheckPickPoint.y;
                vPickPoint->z = vCheckPickPoint.z;
            }
        }
    }

    if (-1.f == *fMinDist)
        return false;

    else
        return true;
}

shared_ptr<CBinaryModel> CBinaryModel::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, TYPE eModelType, const _char* pModelFilePath, _fmatrix PivotMatrix)
{
    shared_ptr<CBinaryModel> pInstance = make_shared<CBinaryModel>(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(eModelType, pModelFilePath, PivotMatrix)))
    {
        MSG_BOX("Failed to Creatd : CModel");
        pInstance.reset();
    }

    return pInstance;
}

shared_ptr<CComponent> CBinaryModel::Clone(void* pArg)
{
    shared_ptr<CBinaryModel> pInstance = make_shared<CBinaryModel>(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Creatd : CModel");
        pInstance.reset();
    }

    return pInstance;
}

void CBinaryModel::Free()
{
    __super::Free();
}
