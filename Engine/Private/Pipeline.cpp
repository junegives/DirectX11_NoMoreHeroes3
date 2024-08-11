#include "Pipeline.h"

CPipeline::CPipeline()
{
}

void CPipeline::Tick()
{
    // 매 프레임마다 뷰, 투영 행렬의 역행렬을 구해서 저장해놓는다.
    for (size_t i = 0; i < D3DTS_END; ++i)
    {
        XMStoreFloat4x4(&m_TransformMatrix_Inverse[i], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrix[i])));
    }

    // 매 프레임마다 카메라의 위치를 구해서 저장해놓는다.
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
