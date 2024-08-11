#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class CGraphic_Device final
{
public:
	CGraphic_Device();
	~CGraphic_Device() = default;

public:

	/* �׷��� ����̽��� �ʱ�ȭ. */
	HRESULT Ready_Graphic_Device(const GRAPHIC_DESC& GraphicDesc, _Inout_ ComPtr<ID3D11Device>* ppDevice,
		_Inout_ ComPtr<ID3D11DeviceContext>* ppDeviceContextOut, HWND hWnd);
	/* ����۸� �����. */
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);

	/* ���̹��� + ���ٽǹ��۸� �����. */
	HRESULT Clear_DepthStencil_View();

	/* �ĸ� ���۸� ������۷� ��ü�Ѵ�.(����۸� ȭ�鿡 ���� �����ش�.) */
	HRESULT Present();


	//HRESULT Ready_2D_Device(HWND hWnd);
	//void	Draw2DText(const WCHAR* text, _float x, _float y);

private:

	// IDirect3DDevice9* == LPDIRECT3DDEVICE9 == ID3D11Device + ID3D11DeviceContext 

	/* �޸� �Ҵ�. (��������, �ε�������, �ؽ��ķε�) �İ�ü�� ������ ���õ� ���� */
	ComPtr<ID3D11Device> m_pDevice = nullptr;

	/* ��ɽ���.(���ε��۾�, �������۸� SetStreamSource(), SetIndices(), SetTransform(), SetTexture() */
	/* �׸���. */
	ComPtr<ID3D11DeviceContext> m_pDeviceContext = nullptr;

	/* �ĸ���ۿ� ������۸� ��ü�ذ��鼭 ȭ�鿡 �����ִ� ���� */
	ComPtr<IDXGISwapChain> m_pSwapChain = nullptr;


	/* IDirect3DTexture9 */
	/* ID3D11Texture2D : �ؽ��ĸ� ǥ���ϴ� ������ü Ÿ���̴�.
	why? �뵵�� �´� ���������� ����ϱ� ���� �ؽ��İ�ü�� ����������.  */

	/* ID3D11ShaderResourceView : ���̴��� ���޵� �� �ִ� �ؽ�ó Ÿ��. */
	/* ID3D11RenderTargetView : ����Ÿ�ٿ����� ���� �� �ִ� �ؽ�ó Ÿ��. */
	/* ID3D11DepthStencilView : ���̽��ٽ� ���۷μ� ���� �� �ִ� Ÿ��.  */
	ComPtr<ID3D11RenderTargetView> m_pBackBufferRTV = nullptr;
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView = nullptr;

	// Direct2D �� DirectWrite ���� ��� ����
	ComPtr<ID2D1Factory> m_pD2DFactory;
	ComPtr<ID2D1HwndRenderTarget> m_pD2DRenderTarget;
	ComPtr<IDWriteFactory> m_pDWriteFactory;
	ComPtr<IDWriteTextFormat> m_pTextFormat;
	ComPtr<ID2D1SolidColorBrush> m_pBlackBrush;




private:
	/* ����ü�ο��� �ʼ������� �ʿ��� �����ʹ� ����۰� �ʿ��Ͽ� ����۸� �����ϱ����� ������ �����ش�. */
	/* ����ü���� ������� == �����(�ؽ���)�� �����ȴ�. */
	HRESULT Ready_SwapChain(HWND hWnd, _bool isWindowed, _uint iWinCX, _uint iWinCY);
	HRESULT Ready_BackBufferRenderTargetView();
	HRESULT Ready_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY);

public:
	static shared_ptr<CGraphic_Device> Create(const GRAPHIC_DESC& GraphicDesc, _Out_ ComPtr<ID3D11Device>* ppDevice,
		_Out_ ComPtr<ID3D11DeviceContext>* ppDeviceContextOut, HWND hWnd);
	void Free();
};

END