#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CSphericalCoordinates final
{
private:
    _float      m_fRadius;          // 반지름
    _float      m_fAzimuth;         // 방위각
    _float      m_fElevation;       // 고도각

    _float      minAzimuth_Deg;     // 최소 방위각 (도)
    _float      minAzimuth_Rad;     // 최소 방위각 (라디안)
    
    _float      maxAzimuth_Deg;     // 최대 방위각 (도)
    _float      maxAzimuth_Rad;     // 최대 방위각 (라디안)

    _float      minElevation_Deg;   // 최소 고도각 (도)
    _float      minElevation_Rad;   // 최소 고도각 (라디안)

    _float      maxElevation_Deg;   // 최대 고도각 (도)
    _float      maxElevation_Rad;   // 최대 고도각 (라디안)

public:
    // 초기화 및 각도를 라디안으로 변환
    CSphericalCoordinates(_float3 _camCoordinate, _float _radius);
    // 구면 좌표를 직교 좌표로 변환
    _float3 toCartesian();
    // 회전 적용
    CSphericalCoordinates Rotate(float newAzimuth, float newElevation);
    // 반지름, 방위각, 고도각 변환
    void    ChangeCoordinates(_vector _vCoordinate, _float _radius);

    // 방위각, 고도각 제한
    void    LimitRad();
};

END