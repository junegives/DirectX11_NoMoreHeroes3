#include "Input_Device.h"

CInput_Device::CInput_Device(void)
{
}

_bool CInput_Device::Is_KeyDown(_ubyte byKeyID)
{
    return ((m_byKeyState[byKeyID] & 0x80) && !(m_byPrevKeyState[byKeyID] & 0x80));
}

_bool CInput_Device::Is_KeyUp(_ubyte byKeyID)
{
    return (!(m_byKeyState[byKeyID] & 0x80) && (m_byPrevKeyState[byKeyID] & 0x80));
}

_bool CInput_Device::Is_KeyPressing(_ubyte byKeyID)
{
    return ((m_byKeyState[byKeyID] & 0x80) && (m_byPrevKeyState[byKeyID] & 0x80));
}

_bool CInput_Device::Is_MouseDown(MOUSEKEYSTATE eMouse)
{
    return (m_tMouseState.rgbButtons[eMouse] && !m_tPrevMouseState.rgbButtons[eMouse]);
}

_bool CInput_Device::Is_MouseUp(MOUSEKEYSTATE eMouse)
{
    return (!m_tMouseState.rgbButtons[eMouse] && m_tPrevMouseState.rgbButtons[eMouse]);
}

_bool CInput_Device::Is_MousePressing(MOUSEKEYSTATE eMouse)
{
    return (m_tMouseState.rgbButtons[eMouse] && m_tPrevMouseState.rgbButtons[eMouse]);
}

HRESULT CInput_Device::Ready_InputDev(HINSTANCE hInst, HWND hWnd)
{
    // DInput 컴객체를 생성하는 함수
    if (FAILED(DirectInput8Create(hInst,
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        (void**)&m_pInputSDK,
        nullptr)))
        return E_FAIL;

    // 키보드 객체 생성
    if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
        return E_FAIL;

    // 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
    m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

    // 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
    m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

    // 장치에 대한 access 버전을 받아오는 함수
    m_pKeyBoard->Acquire();


    // 마우스 객체 생성
    if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
        return E_FAIL;

    // 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
    m_pMouse->SetDataFormat(&c_dfDIMouse);

    // 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
    m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

    // 장치에 대한 access 버전을 받아오는 함수
    m_pMouse->Acquire();

    return S_OK;
}

void CInput_Device::Tick(void)
{
    for (size_t i = 0; i < 256; i++)
        m_byPrevKeyState[i] = m_byKeyState[i];

    for (size_t i = 0; i < DIM_END; i++)
        m_tPrevMouseState.rgbButtons[i] = m_tMouseState.rgbButtons[i];

    m_pKeyBoard->GetDeviceState(256, m_byKeyState);
    m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);
}

shared_ptr<CInput_Device> CInput_Device::Create(HINSTANCE hInst, HWND hWnd)
{
    shared_ptr<CInput_Device> pInstance = make_shared<CInput_Device>();

    if (FAILED(pInstance->Ready_InputDev(hInst, hWnd)))
    {
        MSG_BOX("Failed to Created : CInput_Device");
        pInstance.reset();
    }

    return pInstance;
}

void CInput_Device::Free()
{
    Safe_Release(m_pKeyBoard);
    Safe_Release(m_pMouse);
    Safe_Release(m_pInputSDK);
}
