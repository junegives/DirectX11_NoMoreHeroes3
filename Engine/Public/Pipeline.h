#pragma once

#include "Engine_Defines.h"

// ��, ���� ��� ����
// ��, ���� ��� ����
// ��, ���� ����� ������� ���ϰ� ����
// ī�޶� ���� ��ġ ���Ͽ� ����

BEGIN(Engine)

class CPipeline
{
public:
	enum D3DTRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };

public:
	CPipeline();
	~CPipeline() = default;

public:
	// �� or ���� ����� �����ϴ� �Լ�
	void Set_Transform(D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
	{
		XMStoreFloat4x4(&m_TransformMatrix[eState], TransformMatrix);
	}

	// �� or ���� ����� �����ϴ� �Լ�
	// Float4x4 Ÿ�� : ���̴��� �����ų� �����
	_float4x4 Get_Transform_Float4x4(D3DTRANSFORMSTATE eState)
	{
		return m_TransformMatrix[eState];
	}

	// Matrix Ÿ�� : �����
	_matrix Get_Transform_Matrix(D3DTRANSFORMSTATE eState)
	{
		return XMLoadFloat4x4(&m_TransformMatrix[eState]);
	}

	_float4x4 Get_Transform_Float4x4_Inverse(D3DTRANSFORMSTATE eState)
	{
		return m_TransformMatrix_Inverse[eState];
	}

	_matrix Get_Transform_Matrix_Inverse(D3DTRANSFORMSTATE eState)
	{
		return XMLoadFloat4x4(&m_TransformMatrix_Inverse[eState]);
	}

	_float4 Get_CamPosition()
	{
		return m_vCamPosition;
	}

	_float4 Get_CamLook()
	{
		return m_vCamLook;
	}

public:
	void Tick();

private:
	// ��, ���� ����� �����ϴ� ����
	_float4x4		m_TransformMatrix[D3DTS_END] = {};
		
	// ��, ���� ����� ������� �����ϴ� �Լ�
	_float4x4		m_TransformMatrix_Inverse[D3DTS_END] = {};

	// ī�޶� ���� ��ġ�� Look�� �����ϴ� ����
	_float4			m_vCamPosition = {};
	_float4			m_vCamLook = {};

public:
	static shared_ptr<CPipeline> Create();
	void Free();

};

END