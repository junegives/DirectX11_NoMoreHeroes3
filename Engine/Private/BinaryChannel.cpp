#include "BinaryChannel.h"

#include "BinaryModel.h"
#include "BinaryBone.h"

CBinaryChannel::CBinaryChannel()
{
}

HRESULT CBinaryChannel::Initialize(shared_ptr<CFile_Manager> pFile, shared_ptr<class CBinaryModel> pModel)
{
	// mNodeName.data : ���� �̸� (�� Ŭ������ �����ص� �Ͱ� ����)
	pFile->Read(m_strName);
	pFile->Read(m_iNumKeyFrames);

	m_iBoneIndex = pModel->Get_BoneIndex(m_strName);

	_float3         vScale;
	_float4         vRotation;
	_float3         vPosition;

	_double			fTime;

	// �ִ� Ű������ ����ŭ �ݺ��ϸ鼭
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
	// TrackPosition�� 0���� ���� ���� �� �� ����� ������ ����� �Ǵ� ���ۿ� ����.
	if (0.0 == TrackPosition)
		*pCurrentKeyFrame = 0;

	// ������ Ű������ ����
	KEYFRAME        LastKeyFrame = m_KeyFrames.back();

	// ��ȯ ���
	_matrix         TransformationMatrix;

	// TrackPosition�� ������ Ű�������� �ð����� ũ�ų� ������
	if (TrackPosition >= LastKeyFrame.Time)
	{
		// ������ Ű������ ���� ���
		m_vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		m_vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		m_vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vPosition), 1.f);
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
		m_vScale = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrame].vScale), XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrame + 1].vScale), Ratio);
		m_vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrame].vRotation), XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrame + 1].vRotation), Ratio);
		m_vTranslation = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrame].vPosition), XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrame + 1].vPosition), Ratio), 1.f);
	}

	XMStoreFloat3(&m_CurrentKeyFrame.vScale, m_vScale);
	XMStoreFloat4(&m_CurrentKeyFrame.vRotation, m_vRotation);
	XMStoreFloat3(&m_CurrentKeyFrame.vPosition, m_vTranslation);

	// XMMatrixAffineTransformation : ũ�� , ȸ��, �̵� �������� ����� ���� ��ȯ ��Ʈ���� ��ȯ
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
