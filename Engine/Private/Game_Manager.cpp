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

	// Ŭ���̾�Ʈ ũ�⸦ ��ǥ�� ��ȯ
	m_p1.x = m_ClientRect.left;
	m_p1.y = m_ClientRect.top;
	m_p2.x = m_ClientRect.right;
	m_p2.y = m_ClientRect.bottom;

	// Ŭ���̾�Ʈ ũ�⸦ ��ũ�� ũ��� ��ȯ
	ClientToScreen(hWnd, &m_p1);
	ClientToScreen(hWnd, &m_p2);

	m_ClientRect.left = m_p1.x;
	m_ClientRect.top = m_p1.y;
	m_ClientRect.right = m_p2.x;
	m_ClientRect.bottom = m_p2.y;

	//�ش� ��ǥ�� �������� Ŀ���� ����
	ClipCursor(&m_ClientRect);

	return S_OK;
}

void CGame_Manager::Change_ClipCursor()
{
	m_isClipCursor = !m_isClipCursor;

	if (m_isClipCursor)
	{
		GetClientRect(m_hWnd, &m_ClientRect);

		// Ŭ���̾�Ʈ ũ�⸦ ��ǥ�� ��ȯ
		m_p1.x = m_ClientRect.left;
		m_p1.y = m_ClientRect.top;
		m_p2.x = m_ClientRect.right;
		m_p2.y = m_ClientRect.bottom;

		// Ŭ���̾�Ʈ ũ�⸦ ��ũ�� ũ��� ��ȯ
		ClientToScreen(m_hWnd, &m_p1);
		ClientToScreen(m_hWnd, &m_p2);

		m_ClientRect.left = m_p1.x;
		m_ClientRect.top = m_p1.y;
		m_ClientRect.right = m_p2.x;
		m_ClientRect.bottom = m_p2.y;

		//�ش� ��ǥ�� �������� Ŀ���� ����
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
