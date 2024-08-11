#include "Game_Manager.h"

CGame_Manager::CGame_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
}

HRESULT CGame_Manager::Initialize(HWND hWnd)
{
	GetClientRect(hWnd, &m_ClientRect);
	m_hWnd = hWnd;

	// 클라이언트 크기를 좌표로 변환
	m_p1.x = m_ClientRect.left;
	m_p1.y = m_ClientRect.top;
	m_p2.x = m_ClientRect.right;
	m_p2.y = m_ClientRect.bottom;

	// 클라이언트 크기를 스크린 크기로 변환
	ClientToScreen(hWnd, &m_p1);
	ClientToScreen(hWnd, &m_p2);

	m_ClientRect.left = m_p1.x;
	m_ClientRect.top = m_p1.y;
	m_ClientRect.right = m_p2.x;
	m_ClientRect.bottom = m_p2.y;

	//해당 좌표를 기준으로 커서를 고정
	ClipCursor(&m_ClientRect);

	return S_OK;
}

void CGame_Manager::Change_ClipCursor()
{
	m_isClipCursor = !m_isClipCursor;

	if (m_isClipCursor)
	{
		GetClientRect(m_hWnd, &m_ClientRect);

		// 클라이언트 크기를 좌표로 변환
		m_p1.x = m_ClientRect.left;
		m_p1.y = m_ClientRect.top;
		m_p2.x = m_ClientRect.right;
		m_p2.y = m_ClientRect.bottom;

		// 클라이언트 크기를 스크린 크기로 변환
		ClientToScreen(m_hWnd, &m_p1);
		ClientToScreen(m_hWnd, &m_p2);

		m_ClientRect.left = m_p1.x;
		m_ClientRect.top = m_p1.y;
		m_ClientRect.right = m_p2.x;
		m_ClientRect.bottom = m_p2.y;

		//해당 좌표를 기준으로 커서를 고정
		ClipCursor(&m_ClientRect);
	}

	else
	{
		ClipCursor(nullptr);
	}
}

shared_ptr<CGame_Manager> CGame_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, HWND hWnd)
{
	shared_ptr<CGame_Manager>		pInstance = make_shared<CGame_Manager>(pDevice, pContext);

	if (FAILED(pInstance->Initialize(hWnd)))
	{
		MSG_BOX("Failed to Created : CGame_Manager");
		pInstance.reset();
	}

	return pInstance;
}
