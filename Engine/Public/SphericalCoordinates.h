#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CSphericalCoordinates final
{
private:
    _float      m_fRadius;          // ������
    _float      m_fAzimuth;         // ������
    _float      m_fElevation;       // ����

    _float      minAzimuth_Deg;     // �ּ� ������ (��)
    _float      minAzimuth_Rad;     // �ּ� ������ (����)
    
    _float      maxAzimuth_Deg;     // �ִ� ������ (��)
    _float      maxAzimuth_Rad;     // �ִ� ������ (����)

    _float      minElevation_Deg;   // �ּ� ���� (��)
    _float      minElevation_Rad;   // �ּ� ���� (����)

    _float      maxElevation_Deg;   // �ִ� ���� (��)
    _float      maxElevation_Rad;   // �ִ� ���� (����)

public:
    // �ʱ�ȭ �� ������ �������� ��ȯ
    CSphericalCoordinates(_float3 _camCoordinate, _float _radius);
    // ���� ��ǥ�� ���� ��ǥ�� ��ȯ
    _float3 toCartesian();
    // ȸ�� ����
    CSphericalCoordinates Rotate(float newAzimuth, float newElevation);
    // ������, ������, ���� ��ȯ
    void    ChangeCoordinates(_vector _vCoordinate, _float _radius);

    // ������, ���� ����
    void    LimitRad();
};

END