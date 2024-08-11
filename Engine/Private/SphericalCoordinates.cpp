#pragma once

#include "Engine_Defines.h"
#include "SphericalCoordinates.h"

CSphericalCoordinates::CSphericalCoordinates(_float3 _camCoordinate, _float _radius)
{
	minAzimuth_Deg = 0.0f;
	maxAzimuth_Deg = 360.0f;
	minElevation_Deg = -40.0f;
	maxElevation_Deg = 40.0f;

	minAzimuth_Rad = XMConvertToRadians(minAzimuth_Deg);
	maxAzimuth_Rad = XMConvertToRadians(maxAzimuth_Deg);
	minElevation_Rad = XMConvertToRadians(minElevation_Deg);
	maxElevation_Rad = XMConvertToRadians(maxElevation_Deg);

	m_fRadius = _radius;
	m_fAzimuth = atan2(_camCoordinate.z, _camCoordinate.x);
	m_fElevation = asin(_camCoordinate.y / m_fRadius);

	//LimitRad();
}

// 구면 좌표를 직교 좌표로 변환
_float3 CSphericalCoordinates::toCartesian()
{
	_float t = m_fRadius * cos(m_fElevation);

	//LimitRad();
	return _float3(t * cos(m_fAzimuth),
		m_fRadius * sin(m_fElevation),
		t * sin(m_fAzimuth));
}

// 회전 적용
CSphericalCoordinates CSphericalCoordinates::Rotate(float newAzimuth, float newElevation)
{
	m_fAzimuth += newAzimuth;
	m_fElevation += newElevation;

	LimitRad();
	return *this;
}

void CSphericalCoordinates::ChangeCoordinates(_vector _vCoordinate, _float _radius)
{
	m_fRadius = _radius;
	m_fAzimuth = atan2(_vCoordinate.m128_f32[2], _vCoordinate.m128_f32[0]);
	m_fElevation = asin(_vCoordinate.m128_f32[1] / m_fRadius);
}

void CSphericalCoordinates::LimitRad()
{
	if (m_fElevation > maxElevation_Rad)
		m_fElevation = maxElevation_Rad;

	if (m_fElevation < minElevation_Rad)
		m_fElevation = minElevation_Rad;
}
