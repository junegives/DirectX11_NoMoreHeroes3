#include "Bounding.h"

CBounding::CBounding(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
}


void CBounding::Free()
{
}
