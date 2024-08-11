#pragma once

#include "Component.h"

/* 1. 정점과 인덱스 버퍼를 생성하는 기능. */
/* 2. 정점과 인덱스 버퍼를 보관한다. */

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual HRESULT Render();
public:
	HRESULT			Bind_Buffers();
	virtual _bool	Picking(_long iMouseX, _long iMouseY, _matrix matWorld, _float3* vPickPoint, _float* fMinDist);

protected:
	ComPtr<ID3D11Buffer> m_pVB = { nullptr };
	ComPtr<ID3D11Buffer> m_pIB = { nullptr };

protected:
	D3D11_BUFFER_DESC			m_BufferDesc = {};
	D3D11_SUBRESOURCE_DATA		m_InitialData = {};
	_uint						m_iVertexStride = { 0 };
	_uint						m_iNumVertices = { 0 };
	_uint						m_iIndexStride = { 0 };
	_uint						m_iNumIndices = { 0 };
	_uint						m_iNumVertexBuffers = { 0 };
	DXGI_FORMAT					m_eIndexFormat = {};
	D3D11_PRIMITIVE_TOPOLOGY	m_ePrimitiveTopology = {};
	_float3*					m_pVerticesPos = { nullptr };

protected:
	shared_ptr<class CPicking> m_pPicking = { nullptr };

protected:
	HRESULT Create_Buffer(_Out_ ComPtr<ID3D11Buffer>* ppBuffer);

public:
	virtual shared_ptr<CComponent> Clone(void* pArg) = 0;
	virtual void Free() override;
};

END