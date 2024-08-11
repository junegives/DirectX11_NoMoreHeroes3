#include "Pipeline.h"

CPipeline::CPipeline()
{
}

void CPipeline::Tick()
{
    // �� �����Ӹ��� ��, ���� ����� ������� ���ؼ� �����س��´�.
    for (size_t i = 0; i < D3DTS_END; ++i)
    {
        XMStoreFloat4x4(&m_TransformMatrix_Inverse[i], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrix[i])));
    }

    // �� �����Ӹ��� ī�޶��� ��ġ�� ���ؼ� �����س��´�.
    memcpy(&m_vCamPosition, &m_TransformMatrix_Inverse[D3DTS_VIEW].m[3][0], sizeof(_float4));
    memcpy(&m_vCamLook, &m_TransformMatrix_Inverse[D3DTS_VIEW].m[2][0], sizeof(_float4));
}

shared_ptr<CPipeline> CPipeline::Create()
{
    return make_shared<CPipeline>();
}

void CPipeline::Free()
{
}
