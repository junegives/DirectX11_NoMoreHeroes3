#pragma once

#include "Engine_Defines.h"

// 뷰, 투영 행렬 보관
// 뷰, 투영 행렬 리턴
// 뷰, 투영 행렬의 역행렬을 구하고 보관
// 카메라 월드 위치 구하여 보관

BEGIN(Engine)

class CPipeline
{
public:
	enum D3DTRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };

public:
	CPipeline();
	~CPipeline() = default;

public:
	// 뷰 or 투영 행렬을 세팅하는 함수
	void Set_Transform(D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
	{
		XMStoreFloat4x4(&m_TransformMatrix[eState], TransformMatrix);
	}

	// 뷰 or 투영 행렬을 리턴하는 함수
	// Float4x4 타입 : 셰이더에 던지거나 저장용
	_float4x4 Get_Transform_Float4x4(D3DTRANSFORMSTATE eState)
	{
		return m_TransformMatrix[eState];
	}

	// Matrix 타입 : 연산용
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
	// 뷰, 투영 행렬을 보관하는 변수
	_float4x4		m_TransformMatrix[D3DTS_END] = {};
		
	// 뷰, 투영 행렬의 역행렬을 보관하는 함수
	_float4x4		m_TransformMatrix_Inverse[D3DTS_END] = {};

	// 카메라 월드 위치와 Look을 보관하는 변수
	_float4			m_vCamPosition = {};
	_float4			m_vCamLook = {};

public:
	static shared_ptr<CPipeline> Create();
	void Free();

};

END