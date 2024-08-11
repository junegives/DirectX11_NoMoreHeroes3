#include "BinaryChannel.h"

#include "BinaryModel.h"
#include "BinaryBone.h"

CBinaryChannel::CBinaryChannel()
{
}

HRESULT CBinaryChannel::Initialize(shared_ptr<CFile_Manager> pFile, shared_ptr<class CBinaryModel> pModel)
{
	// mNodeName.data : 뼈의 이름 (모델 클래스에 저장해둔 것과 같음)
	pFile->Read(m_strName);
	pFile->Read(m_iNumKeyFrames);

	m_iBoneIndex = pModel->Get_BoneIndex(m_strName);

	_float3         vScale;
	_float4         vRotation;
	_float3         vPosition;

	_double			fTime;

	// 최대 키프레임 수만큼 반복하면서
	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME    KeyFrame = {};

		pFile->Read(vScale);
		pFile->Read(vRotation);
		pFile->Read(vPosition);
		pFile->Read(fTime);

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vPosition = vPosition;
		KeyFrame.Time = fTime;

		m_KeyFrames.push_back(KeyFrame);
	}

	ResetCurKeyFrame();

	return S_OK;
}

void CBinaryChannel::Invalidate_TransformationMatrix(_double TrackPosition, _uint* pCurrentKeyFrame, const vector<shared_ptr<class CBinaryBone>>& Bones)
{
	// TrackPosition이 0으로 들어가는 경우는 한 번 재생이 끝나고 재시작 되는 경우밖에 없다.
	if (0.0 == TrackPosition)
		*pCurrentKeyFrame = 0;

	// 마지막 키프레임 저장
	KEYFRAME        LastKeyFrame = m_KeyFrames.back();

	// 변환 행렬
	_matrix         TransformationMatrix;

	// TrackPosition이 마지막 키프레임의 시간보다 크거나 같으면
	if (TrackPosition >= LastKeyFrame.Time)
	{
		// 마지막 키프레임 정보 사용
		m_vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		m_vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		m_vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vPosition), 1.f);
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
		m_vScale = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrame].vScale), XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrame + 1].vScale), Ratio);
		m_vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrame].vRotation), XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrame + 1].vRotation), Ratio);
		m_vTranslation = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrame].vPosition), XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrame + 1].vPosition), Ratio), 1.f);
	}

	XMStoreFloat3(&m_CurrentKeyFrame.vScale, m_vScale);
	XMStoreFloat4(&m_CurrentKeyFrame.vRotation, m_vRotation);
	XMStoreFloat3(&m_CurrentKeyFrame.vPosition, m_vTranslation);

	// XMMatrixAffineTransformation : 크기 , 회전, 이동 정보에서 빌드된 아핀 변환 매트릭스 반환
	TransformationMatrix = XMMatrixAffineTransformation(m_vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), m_vRotation, m_vTranslation);

	Bones[m_iBoneIndex]->Set_TranslationMatrix(TransformationMatrix);
}

shared_ptr<CBinaryChannel> CBinaryChannel::Create(shared_ptr<CFile_Manager> pFile, shared_ptr<class CBinaryModel> pModel)
{
	shared_ptr<CBinaryChannel> pInstance = make_shared<CBinaryChannel>();

	if (FAILED(pInstance->Initialize(pFile, pModel)))
	{
		MSG_BOX("Failed to Created : CChannel");
		pInstance.reset();
	}

	return pInstance;
}

void CBinaryChannel::Free()
{
}
