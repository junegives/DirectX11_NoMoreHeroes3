#include "Channel.h"

#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* pAIChannel, shared_ptr<class CModel> pModel)
{
    // mNodeName.data : 뼈의 이름 (모델 클래스에 저장해둔 것과 같음)
    strcpy_s(m_szName, pAIChannel->mNodeName.data);

    m_iBoneIndex = pModel->Get_BoneIndex(m_szName);
    
    // 키프레임의 수는 스케일링키, 로테이션키, 포지션키 중 가장 많은 키프레임 수가 전체 키프레임 수이다.
    // 만약 스케일링키 3개, 회전 4개, 포지션 2개이면 회전 4번 바뀔 동안 나머지는 이전 상태를 유지한다.
    m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
    m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

    _float3         vScale;
    _float4         vRotation;
    _float3         vPosition;

    // 최대 키프레임 수만큼 반복하면서
    for (size_t i = 0; i < m_iNumKeyFrames; i++)
    {
        KEYFRAME    KeyFrame = {};

        // 스케일링키가 아직 덜채워졌으면
        if (pAIChannel->mNumScalingKeys > i)
        {
            memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
            KeyFrame.Time = pAIChannel->mScalingKeys[i].mTime;
        }

        // 로테이션키가 아직 덜채워졌으면
        if (pAIChannel->mNumRotationKeys > i)
        {
            // mRotationKeys[i].mValue의 순서가 쿼터니언이라 w, x, y, z이기 때문에 바로 복사해주는 것 말고 속성별로 대입
            // memcpy(&vRotation, &pAIChannel->mRotationKeys[i].mValue, sizeof(_float4));
            vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
            vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
            vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
            vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
            KeyFrame.Time = pAIChannel->mRotationKeys[i].mTime;
        }

        // 포지션키가 아직 덜채워졌으면
        if (pAIChannel->mNumPositionKeys > i)
        {
            memcpy(&vPosition, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
            KeyFrame.Time = pAIChannel->mPositionKeys[i].mTime;
        }

        KeyFrame.vScale = vScale;
        KeyFrame.vRotation = vRotation;
        KeyFrame.vPosition = vPosition;

        m_KeyFrames.push_back(KeyFrame);
    }

    return S_OK;
}

void CChannel::Invalidate_TransformationMatrix(_double TrackPosition, _uint* pCurrentKeyFrame, const vector<shared_ptr<class CBone>>& Bones)
{
    // TrackPosition이 0으로 들어가는 경우는 한 번 재생이 끝나고 재시작 되는 경우밖에 없다.
    if (0.0 == TrackPosition)
        *pCurrentKeyFrame = 0;

    // 마지막 키프레임 저장
    KEYFRAME        LastKeyFrame = m_KeyFrames.back();

    // 변환 행렬
    _matrix         TransformationMatrix;
    _vector         vScale, vRotation, vTranslation;

    // TrackPosition이 마지막 키프레임의 시간보다 크거나 같으면
    if (TrackPosition >= LastKeyFrame.Time)
    {
        // 마지막 키프레임 정보 사용
        vScale = XMLoadFloat3(&LastKeyFrame.vScale);
        vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
        vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vPosition), 1.f);
    }
    // 그렇지 않으면, 현재 키프레임과 다음 키프레임 사이에서 선형 보간
    else
    {
        // TrackPosition이 현재 키프레임 + 1(다음 키프레임)의 시간보다 크거나 같은 동안
        // while을 쓰는 이유 : 프레임드랍 때문에 TrackPosition이 한 번에 여러 키프레임을 건너뛸 수도 있기 때문
        while (TrackPosition >= m_KeyFrames[*pCurrentKeyFrame + 1].Time)
            // 현재 키프레임 증가
            ++(*pCurrentKeyFrame);

        // TrackPosition - m_KeyFrames[m_iCurrentKeyFrame].Time : 현재 키프레임에서 더 나아간 시간
        // m_KeyFrames[m_iCurrentKeyFrame + 1].Time - m_KeyFrames[m_iCurrentKeyFrame].Time : 현재 키프레임과 다음 키프레임의 시간 차이
        // Ratio : 현재 키프레임에서 다음 키프레임에 도달하기까지 진행한 정도
        _double     Ratio = (TrackPosition - m_KeyFrames[*pCurrentKeyFrame].Time) /
            (m_KeyFrames[*pCurrentKeyFrame + 1].Time - m_KeyFrames[*pCurrentKeyFrame].Time);

        // 현재 키프레임과 다음 키프레임 사이의 비율을 계산하여 vScale, vRotation, vTranslation에 저장 (선형 보간)
        vScale = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrame].vScale), XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrame + 1].vScale), Ratio);
        vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrame].vRotation), XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrame + 1].vRotation), Ratio);
        vTranslation = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrame].vPosition), XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrame + 1].vPosition), Ratio), 1.f);
    }

    // XMMatrixAffineTransformation : 크기 , 회전, 이동 정보에서 빌드된 아핀 변환 매트릭스 반환
    TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

    // 본을 생성한 행렬에 따라 변환
    Bones[m_iBoneIndex]->Set_TranslationMatrix(TransformationMatrix);
}

shared_ptr<CChannel> CChannel::Create(const aiNodeAnim* pAIChannel, shared_ptr<class CModel> pModel)
{
    shared_ptr<CChannel> pInstance = make_shared<CChannel>();

    if (FAILED(pInstance->Initialize(pAIChannel, pModel)))
    {
        MSG_BOX("Failed to Created : CChannel");
        pInstance.reset();
    }

    return pInstance;
}

void CChannel::Free()
{
}
