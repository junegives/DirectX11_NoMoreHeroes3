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
    // 애니메이션 이름
    pFile->Read(m_strName);

    /* 내 애니메이션의 전체 길이 */
    pFile->Read(m_Duration);

    /* 초당 재생 속도 */
    pFile->Read(m_TickPerSecond);

    /* 이 애니메이션을 구동하는데 필요한 뼈의 개수 */
    pFile->Read(m_iNumChannels);

	m_iCurrentKeyFrames.resize(m_iNumChannels);

    /* 이 애니메이션을 구동하는데 필요한 뼈들의 정보를 생성한다. */
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
    // 애니메이션 종료 전 멈춘 경우
    if (m_isStop)
        return m_isStop;

    else
    {
        if (true == m_isFinished)
            m_isFinished = false;
    }

    // 초당 프레임 수 * 시간 간격
    m_TrackPosition += m_TickPerSecond * m_fAnimSpeed * fTimeDelta;
     
    // 애니메이션 한 바퀴 다 돌았을 때
    if (m_TrackPosition >= m_Duration)
    {
        // 반복 재생이라면
        if (m_isLoop)
        {
            // 트랙포지션 처음으로 옮겨서 다시 재생
            m_TrackPosition = 0.f;
            m_isFinished = true;
        }
        else
        {
            // 종료
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
