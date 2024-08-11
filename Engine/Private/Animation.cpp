#include "Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
    : m_iNumAnimations(rhs.m_iNumAnimations)
    , m_Duration(rhs.m_Duration)
    , m_TickPerSecond(rhs.m_TickPerSecond)
    , m_TrackPosition(rhs.m_TrackPosition)
    , m_iNumChannels(rhs.m_iNumChannels)
    , m_Channels(rhs.m_Channels)
    , m_iCurrentKeyFrames(rhs.m_iCurrentKeyFrames)
    , m_isFinished(rhs.m_isFinished)
{
    strcpy_s(m_szName, rhs.m_szName);
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, shared_ptr<class CModel> pModel)
{
    // �ִϸ��̼� �̸�
    strcpy_s(m_szName, pAIAnimation->mName.data);

    /* �� �ִϸ��̼��� ��ü ���� */
    m_Duration = pAIAnimation->mDuration;

    /* �ʴ� ��� �ӵ� */
    m_TickPerSecond = pAIAnimation->mTicksPerSecond;

    /* �� �ִϸ��̼��� �����ϴµ� �ʿ��� ���� ���� */
    m_iNumChannels = pAIAnimation->mNumChannels;

	m_iCurrentKeyFrames.resize(m_iNumChannels);

    /* �� �ִϸ��̼��� �����ϴµ� �ʿ��� ������ ������ �����Ѵ�. */
    for (size_t i = 0; i < m_iNumChannels; i++)
    {
        shared_ptr<CChannel> pChannel = CChannel::Create(pAIAnimation->mChannels[i], pModel);
        if (nullptr == pChannel)
            return E_FAIL;

        m_Channels.push_back(pChannel);
    }

    return S_OK;
}

void CAnimation::Invalidate_TransformationMatrix(_float fTimeDelta, const vector<shared_ptr<class CBone>>& Bones, _bool isLoop)
{
    // �ʴ� ������ �� * �ð� ����
    m_TrackPosition += m_TickPerSecond * fTimeDelta;

    if (m_TrackPosition >= m_Duration)
    {
        if (false == isLoop)
        {
            m_isFinished = true;
            return;
        }
        else
        {
            m_TrackPosition = 0.0;
        }
    }


    for (size_t i = 0; i < m_iNumChannels; i++)
    {
        m_Channels[i]->Invalidate_TransformationMatrix(m_TrackPosition, &m_iCurrentKeyFrames[i], Bones);
    }
}

shared_ptr<CAnimation> CAnimation::Create(const aiAnimation* pAIAnimation, shared_ptr<class CModel> pModel)
{
    shared_ptr<CAnimation> pInstance = make_shared<CAnimation>();

    if (FAILED(pInstance->Initialize(pAIAnimation, pModel)))
    {
        MSG_BOX("Failed to Created : CAnimation");
        pInstance.reset();
    }

    return pInstance;
}

shared_ptr<CAnimation> CAnimation::Clone()
{
    return make_shared<CAnimation>(*this);
}

void CAnimation::Free()
{
}
