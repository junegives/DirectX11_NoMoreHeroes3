#include "Channel.h"

#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* pAIChannel, shared_ptr<class CModel> pModel)
{
    // mNodeName.data : ���� �̸� (�� Ŭ������ �����ص� �Ͱ� ����)
    strcpy_s(m_szName, pAIChannel->mNodeName.data);

    m_iBoneIndex = pModel->Get_BoneIndex(m_szName);
    
    // Ű�������� ���� �����ϸ�Ű, �����̼�Ű, ������Ű �� ���� ���� Ű������ ���� ��ü Ű������ ���̴�.
    // ���� �����ϸ�Ű 3��, ȸ�� 4��, ������ 2���̸� ȸ�� 4�� �ٲ� ���� �������� ���� ���¸� �����Ѵ�.
    m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
    m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

    _float3         vScale;
    _float4         vRotation;
    _float3         vPosition;

    // �ִ� Ű������ ����ŭ �ݺ��ϸ鼭
    for (size_t i = 0; i < m_iNumKeyFrames; i++)
    {
        KEYFRAME    KeyFrame = {};

        // �����ϸ�Ű�� ���� ��ä��������
        if (pAIChannel->mNumScalingKeys > i)
        {
            memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
            KeyFrame.Time = pAIChannel->mScalingKeys[i].mTime;
        }

        // �����̼�Ű�� ���� ��ä��������
        if (pAIChannel->mNumRotationKeys > i)
        {
            // mRotationKeys[i].mValue�� ������ ���ʹϾ��̶� w, x, y, z�̱� ������ �ٷ� �������ִ� �� ���� �Ӽ����� ����
            // memcpy(&vRotation, &pAIChannel->mRotationKeys[i].mValue, sizeof(_float4));
            vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
            vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
            vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
            vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
            KeyFrame.Time = pAIChannel->mRotationKeys[i].mTime;
        }

        // ������Ű�� ���� ��ä��������
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
    // TrackPosition�� 0���� ���� ���� �� �� ����� ������ ����� �Ǵ� ���ۿ� ����.
    if (0.0 == TrackPosition)
        *pCurrentKeyFrame = 0;

    // ������ Ű������ ����
    KEYFRAME        LastKeyFrame = m_KeyFrames.back();

    // ��ȯ ���
    _matrix         TransformationMatrix;
    _vector         vScale, vRotation, vTranslation;

    // TrackPosition�� ������ Ű�������� �ð����� ũ�ų� ������
    if (TrackPosition >= LastKeyFrame.Time)
    {
        // ������ Ű������ ���� ���
        vScale = XMLoadFloat3(&LastKeyFrame.vScale);
        vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
        vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vPosition), 1.f);
    }
    // �׷��� ������, ���� Ű�����Ӱ� ���� Ű������ ���̿��� ���� ����
    else
    {
        // TrackPosition�� ���� Ű������ + 1(���� Ű������)�� �ð����� ũ�ų� ���� ����
        // while�� ���� ���� : �����ӵ�� ������ TrackPosition�� �� ���� ���� Ű�������� �ǳʶ� ���� �ֱ� ����
        while (TrackPosition >= m_KeyFrames[*pCurrentKeyFrame + 1].Time)
            // ���� Ű������ ����
            ++(*pCurrentKeyFrame);

        // TrackPosition - m_KeyFrames[m_iCurrentKeyFrame].Time : ���� Ű�����ӿ��� �� ���ư� �ð�
        // m_KeyFrames[m_iCurrentKeyFrame + 1].Time - m_KeyFrames[m_iCurrentKeyFrame].Time : ���� Ű�����Ӱ� ���� Ű�������� �ð� ����
        // Ratio : ���� Ű�����ӿ��� ���� Ű�����ӿ� �����ϱ���� ������ ����
        _double     Ratio = (TrackPosition - m_KeyFrames[*pCurrentKeyFrame].Time) /
            (m_KeyFrames[*pCurrentKeyFrame + 1].Time - m_KeyFrames[*pCurrentKeyFrame].Time);

        // ���� Ű�����Ӱ� ���� Ű������ ������ ������ ����Ͽ� vScale, vRotation, vTranslation�� ���� (���� ����)
        vScale = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrame].vScale), XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrame + 1].vScale), Ratio);
        vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrame].vRotation), XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrame + 1].vRotation), Ratio);
        vTranslation = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrame].vPosition), XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrame + 1].vPosition), Ratio), 1.f);
    }

    // XMMatrixAffineTransformation : ũ�� , ȸ��, �̵� �������� ����� ���� ��ȯ ��Ʈ���� ��ȯ
    TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

    // ���� ������ ��Ŀ� ���� ��ȯ
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
