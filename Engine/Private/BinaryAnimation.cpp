#include "BinaryAnimation.h"
#include "BinaryChannel.h"

CBinaryAnimation::CBinaryAnimation()
{
}

CBinaryAnimation::CBinaryAnimation(const CBinaryAnimation& rhs)
    : m_iNumAnimations(rhs.m_iNumAnimations)
    , m_strName(rhs.m_strName)
    , m_Duration(rhs.m_Duration)
    , m_TickPerSecond(rhs.m_TickPerSecond)
    , m_TrackPosition(rhs.m_TrackPosition)
    , m_iNumChannels(rhs.m_iNumChannels)
    , m_Channels(rhs.m_Channels)
    , m_iCurrentKeyFrames(rhs.m_iCurrentKeyFrames)
{
}

HRESULT CBinaryAnimation::Initialize(shared_ptr<CFile_Manager> pFile, shared_ptr<CBinaryModel> pModel)
{
    // �ִϸ��̼� �̸�
    pFile->Read(m_strName);

    /* �� �ִϸ��̼��� ��ü ���� */
    pFile->Read(m_Duration);

    /* �ʴ� ��� �ӵ� */
    pFile->Read(m_TickPerSecond);

    /* �� �ִϸ��̼��� �����ϴµ� �ʿ��� ���� ���� */
    pFile->Read(m_iNumChannels);

	m_iCurrentKeyFrames.resize(m_iNumChannels);

    /* �� �ִϸ��̼��� �����ϴµ� �ʿ��� ������ ������ �����Ѵ�. */
    for (size_t i = 0; i < m_iNumChannels; i++)
    {
        shared_ptr<CBinaryChannel> pChannel = CBinaryChannel::Create(pFile, pModel);
        if (nullptr == pChannel)
            return E_FAIL;

        m_Channels.push_back(pChannel);
    }

    m_isFinished = false;

    return S_OK;
}

_bool CBinaryAnimation::Invalidate_TransformationMatrix(_float fTimeDelta, const vector<shared_ptr<class CBinaryBone>>& Bones)
{
    // �ִϸ��̼� ���� �� ���� ���
    if (m_isStop)
        return m_isStop;

    else
    {
        if (true == m_isFinished)
            m_isFinished = false;
    }

    // �ʴ� ������ �� * �ð� ����
    m_TrackPosition += m_TickPerSecond * m_fAnimSpeed * fTimeDelta;
     
    // �ִϸ��̼� �� ���� �� ������ ��
    if (m_TrackPosition >= m_Duration)
    {
        // �ݺ� ����̶��
        if (m_isLoop)
        {
            // Ʈ�������� ó������ �Űܼ� �ٽ� ���
            m_TrackPosition = 0.f;
            m_isFinished = true;
        }
        else
        {
            // ����
            m_isStop = true;
            m_isFinished = true;
            return m_isStop;
        }
    }

    for (size_t i = 0; i < m_iNumChannels; i++)
    {
        m_Channels[i]->Invalidate_TransformationMatrix(m_TrackPosition, &m_iCurrentKeyFrames[i], Bones);
    }

    return m_isStop;
}


void CBinaryAnimation::SetStartKeyFrames(_uint iNumKeyFrame, _float fTimeDelta, vector<shared_ptr<CBinaryBone>>& Bones)
{
    while (true)
    {
        if (iNumKeyFrame == 0)
        {
            for (auto& pChannel : m_Channels)
            {
                pChannel->ResetCurKeyFrame();
            }
            return;
        }
        else if (iNumKeyFrame <= m_TrackPosition)
            return;

        m_TrackPosition += m_TickPerSecond * m_fAnimSpeed * fTimeDelta;

        _uint	iNumChannel = 0;

        for (size_t i = 0; i < m_iNumChannels; i++)
        {
            m_Channels[i]->Invalidate_TransformationMatrix(m_TrackPosition, &m_iCurrentKeyFrames[i], Bones);
        }
    }
}

void CBinaryAnimation::Reset()
{
    m_TrackPosition = 0.f;
    m_isFinished = false;
    m_isStop = false;
    m_ChangeStartTime = 0.0;

    for (auto& iCurrentKeyFrame : m_iCurrentKeyFrames)
    {
        iCurrentKeyFrame = 0;
    }
}

shared_ptr<CBinaryAnimation> CBinaryAnimation::Create(shared_ptr<CFile_Manager> pFile, shared_ptr<class CBinaryModel> pModel)
{
    shared_ptr<CBinaryAnimation> pInstance = make_shared<CBinaryAnimation>();

    if (FAILED(pInstance->Initialize(pFile, pModel)))
    {
        MSG_BOX("Failed to Created : CAnimation");
        pInstance.reset();
    }

    return pInstance;
}

shared_ptr<CBinaryAnimation> CBinaryAnimation::Clone()
{
    return make_shared<CBinaryAnimation>(*this);
}

void CBinaryAnimation::Free()
{
}
