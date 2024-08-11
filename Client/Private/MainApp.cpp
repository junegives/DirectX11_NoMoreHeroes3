#include "pch.h"
#include "MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "BackGround.h"

#include "Sound_Manager.h"

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT CMainApp::Initialize()
{
	// 디버깅용
	/*_matrix		matPivot = XMMatrixIdentity();

	_matrix		matText = matPivot * XMMatrixRotationX(XMConvertToRadians(20));

	matPivot *= XMMatrixRotationX(XMConvertToRadians(10));
	matPivot *= XMMatrixRotationX(XMConvertToRadians(10));*/

	srand(static_cast<unsigned int>(std::time(nullptr)));

	GRAPHIC_DESC			GraphicDesc = {};

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.isWindowed = true;
	GraphicDesc.iWinSizeX = g_iWinSizeX;
	GraphicDesc.iWinSizeY = g_iWinSizeY;

	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Gara()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_Static()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_GAMEPLAY)))
		return E_FAIL;

	CSound_Manager::GetInstance()->Initialize();
	//CSound_Manager::GetInstance()->PlaySound(L"BGM_Battle_EZ.mp3", CSound_Manager::CHANNELID::INTERACTION, 1.f);
	CSound_Manager::GetInstance()->PlayBGM(L"BGM_Village_Western.mp3", 1.f);
	/*CSound_Manager::GetInstance()->StopSound(CSound_Manager::CHANNELID::BGM);
	CSound_Manager::GetInstance()->StopAll();*/



	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Is_KeyDown(DIK_GRAVE))
	{
		m_pGameInstance->Change_ClipCursor();
	}

	if (m_pGameInstance->Is_KeyDown(DIK_F1))
	{
		m_pGameInstance->SetRenderDebug();
	}

#ifdef _DEBUG
	m_fTimeAcc += fTimeDelta;
#endif

	m_pGameInstance->Tick_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	m_pGameInstance->Clear_BackBuffer_View({ 0.0f, 0.0f, 0.4f, 1.f });
	m_pGameInstance->Clear_DepthStencil_View();

	if (FAILED(m_pGameInstance->Draw()))
		return E_FAIL;

//#ifdef _DEBUG
//	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Default"), TEXT("느려"), _float2(0.f, 0.f), XMVectorSet(1.f, 0.f, 0.f, 1.f))))
//		return E_FAIL;
//
//	if (m_pGameInstance->Get_Player())
//	{
//		_vector vPlayerPos = m_pGameInstance->Get_Player()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
//		_vector vPlayerLook = m_pGameInstance->Get_Player()->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
//
//		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Dream"),
//			L"Plr Pos. X : " + std::to_wstring(vPlayerPos.m128_f32[0])
//			+ L" Y : " + std::to_wstring(vPlayerPos.m128_f32[1])
//			+ L" Z : " + std::to_wstring(vPlayerPos.m128_f32[2]), _float2(0.f, 30.f), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
//			return E_FAIL;
//
//		/*if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Dream"),
//			L"Plr Look X : " + std::to_wstring(vPlayerLook.m128_f32[0])
//			+ L" Y : " + std::to_wstring(vPlayerLook.m128_f32[1])
//			+ L" Z : " + std::to_wstring(vPlayerLook.m128_f32[2]), _float2(0.f, 60.f), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
//			return E_FAIL;*/
//	}
//
//	if (CGame_Manager::STATE_LOADING != m_pGameInstance->Get_GameState())
//	{
//		++m_iRenderCount;
//
//		if (1.f <= m_fTimeAcc)
//		{
//			m_fTimeAcc = 0;
//			wsprintf(m_szFPS, TEXT("%d fps"), m_iRenderCount);
//
//			m_iRenderCount = 0;
//		}
//		
//		SetWindowText(g_hWnd, m_szFPS);
//	}
//#endif

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eStartLevel)
{
	if (LEVEL_LOADING == eStartLevel)
		return E_FAIL;

	return m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevel));

}

HRESULT CMainApp::Ready_Gara()
{
#ifdef _DEBUG
	// Kor
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Default"), TEXT("../../Resource/Resources/Fonts/140.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Nexon"), TEXT("../../Resource/Resources/Fonts/Default.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_NexonSmall"), TEXT("../../Resource/Resources/Fonts/NexonSmall.spritefont"))))
		return E_FAIL;

	// Eng
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Dream"), TEXT("../../Resource/Resources/Fonts/Dream.spritefont"))))
		return E_FAIL;
#endif

	// 네비게이션 셀 생성
	{
		_ulong		dwByte = { 0 };
		HANDLE		hFile = CreateFile(TEXT("../Bin/DataFiles/Navigation.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return E_FAIL;

		_float3			vPoints[3];

		//vPoints[0] = _float3(0.0f, 0.f, 0.f);
		//vPoints[1] = _float3(0.0f, 0.f, 10.f);
		//vPoints[2] = _float3(10.0f, 0.f, 0.f);
		//WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

		//vPoints[0] = _float3(0.0f, 0.f, 10.f);
		//vPoints[1] = _float3(10.0f, 0.f, 10.f);
		//vPoints[2] = _float3(10.0f, 0.f, 0.f);
		//WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

		//vPoints[0] = _float3(0.0f, 0.f, 20.f);
		//vPoints[1] = _float3(10.0f, 0.f, 10.f);
		//vPoints[2] = _float3(0.0f, 0.f, 10.f);
		//WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

		//vPoints[0] = _float3(10.0f, 0.f, 10.f);
		//vPoints[1] = _float3(20.0f, 0.f, 0.f);
		//vPoints[2] = _float3(10.0f, 0.f, 0.f);
		//WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

		_float fNaviY = 33.1f;
		_float fNaviX = 50.f;
		_float fNaviZ = 220.f;

		vPoints[0] = _float3(50.0f, fNaviY, 220.f);
		vPoints[1] = _float3(50.0f, fNaviY, 240.f);
		vPoints[2] = _float3(70.0f, fNaviY, 220.f);
		WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

		vPoints[0] = _float3(50.0f, fNaviY, 240.f);
		vPoints[1] = _float3(70.0f, fNaviY, 240.f);
		vPoints[2] = _float3(70.0f, fNaviY, 220.f);
		WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

		vPoints[0] = _float3(50.0f, fNaviY, 260.f);
		vPoints[1] = _float3(70.0f, fNaviY, 240.f);
		vPoints[2] = _float3(50.0f, fNaviY, 240.f);
		WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

		vPoints[0] = _float3(70.0f, fNaviY, 240.f);
		vPoints[1] = _float3(90.0f, fNaviY, 220.f);
		vPoints[2] = _float3(70.0f, fNaviY, 220.f);
		WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

		vPoints[0] = _float3(50.0f, fNaviY, 260.f);
		vPoints[1] = _float3(70.0f, fNaviY, 260.f);
		vPoints[2] = _float3(70.0f, fNaviY, 240.f);
		WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

		vPoints[0] = _float3(70.0f, fNaviY, 240.f);
		vPoints[1] = _float3(90.0f, fNaviY, 240.f);
		vPoints[2] = _float3(90.0f, fNaviY, 220.f);
		WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

		vPoints[0] = _float3(70.0f, fNaviY, 240.f);
		vPoints[1] = _float3(70.0f, fNaviY, 260.f);
		vPoints[2] = _float3(90.0f, fNaviY, 240.f);
		WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

		vPoints[0] = _float3(70.0f, fNaviY, 260.f);
		vPoints[1] = _float3(90.0f, fNaviY, 260.f);
		vPoints[2] = _float3(90.0f, fNaviY, 240.f);
		WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

		CloseHandle(hFile);
	}

	// 텍스처 생성&저장
	{
		// 텍스처를 생성하고 디스크에 저장하는 과정
		// Grahphic_Device에서 DepthStencilRenderTargetView를 만드는 과정과 비슷

		// 텍스처를 가리킬 변수
		ID3D11Texture2D* pTexture2D = { nullptr };

		// 텍스처를 생성할 때 필요한 설정들을 담을 구조체
		D3D11_TEXTURE2D_DESC		TextureDesc;
		ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

		TextureDesc.Width = 256;
		TextureDesc.Height = 256;
		// 마스크 텍스처는 그리기용이 아니기 때문에 mipmap 필요 없음
		TextureDesc.MipLevels = 1;
		TextureDesc.ArraySize = 1;
		TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 텍스처 만들 때 r, g, b, a 각각 8비트씩 부여(원하는대로)

		TextureDesc.SampleDesc.Quality = 0;
		TextureDesc.SampleDesc.Count = 1;

		//CPU에서 동적으로 업데이트 가능하도록 (텍스처 데이터 변경 가능)
		TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
		// 텍스처를 바인딩할 때 특정 용도 설정 (셰이더 리소스용)
		TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		// CPU에서 텍스처에 쓰기 권한 필요 (텍스처를 업데이트하려면 CPU에서 해당 텍스처의 메모리에 쓰기 작업을 수행할 수 있다.)
		TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		TextureDesc.MiscFlags = 0;

#pragma region 텍스처 초기값
		_ulong* pPixel = new _ulong[TextureDesc.Width * TextureDesc.Height];

		for (size_t i = 0; i < TextureDesc.Height; ++i)
		{
			for (size_t j = 0; j < TextureDesc.Width; ++j)
			{
				_uint		iIndex = (_uint)i * TextureDesc.Width + (_uint)j;

				pPixel[iIndex] = D3DCOLOR_ARGB(255, 255, 0, 0);
			}
		}

		// 텍스처의 초기 데이터를 설정하는 구조체
		D3D11_SUBRESOURCE_DATA		InitialData = {};

		// 텍스처에 할당된 픽셀 데이터 배열을 지정
		InitialData.pSysMem = pPixel;
		// 이미지에 값을 채울 때는 가로 한 줄이 몇 바이트인지 알려줘야함
		InitialData.SysMemPitch = TextureDesc.Width * 4;

		// 텍스처 생성
		// (텍스처를 생성할 때 필요한 설정들을 담은 구조체, 초기 데이터, 생성된 텍스처를 저장할 포인터)
		// 두 번째 인자 D3D11_SUBRESOURCE_DATA : 어떤 공간을 사용할 때 최초 어떤 값으로 채워 넣을지
		if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &InitialData, &pTexture2D)))
			return E_FAIL;

#pragma endregion

#pragma region 텍스처 수정

		// 0~128번째 열은 흰색, 그 이상은 검은색
		for (_uint i = 0; i < TextureDesc.Height; i++)
		{
			for (_uint j = 0; j < TextureDesc.Width; j++)
			{
				_uint	iIndex = i * TextureDesc.Width + j;

				if (j < 128)
					pPixel[iIndex] = D3DCOLOR_ARGB(255, 255, 255, 255);
				else
					pPixel[iIndex] = D3DCOLOR_ARGB(255, 0, 0, 0);
			}
		}

		D3D11_MAPPED_SUBRESOURCE	SubResource = {};

		/*
		DirectX9의 Lock()과 같은 역할
		ex) m_pVB->Lock();
		다른 점은, Context로 모두 처리한다.
		*/
		// 텍스처의 메모리를 매핑한다.
		// (매핑할 텍스처, 매핑할 서브리소스의 인덱스(밉맵)(보통 0), 메모리 전체 쓰기 작업, 매핑할 영역 지정(보통 0), 매핑된 데이터에 대한 정보를 담을 구조체)
		// D3D11_MAP_WRITE_DISCARD : 주소를 얻어오긴 하지만 들어가있던 값은 모두 버린다. (텍스처에 있는 값들을 모두 다시 갱신할 때 사용)
		// D3D11_MAP_NO_OVERWRITE : 기존의 값들을 다 살려둔다. (텍스처의 경우 사용하지 못함)
		// -> Map() 동작이 매우 느리기 때문에 클라이언트에서는 잘 쓰지 않고, 툴 같은 곳에서 쓴다.
		// -> 모든 데이터를 가지고 있는 배열을 미리 갖고 있으면 상관없음.
		m_pContext->Map(pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

		// 매핑된 메모리에 데이터 복사
		//SubResource.pData = pPixel; // 주소를 바꿔버리는 코드이기 때문에 사용하면 안된다. (값을 복사해줘야 한다.) 
		memcpy(SubResource.pData, pPixel, sizeof(_ulong) * TextureDesc.Width * TextureDesc.Height);

		// 텍스처의 메모리 매핑 해제
		m_pContext->Unmap(pTexture2D, 0);

#pragma endregion

		// 생성된 텍스처를 DDS 파일 형식으로 디스크에 저장
		if (FAILED(DirectX::SaveDDSTextureToFile(m_pContext.Get(), pTexture2D, TEXT("../Bin/Resources/Textures/Terrain/GaraMask.dds"))))
			return E_FAIL;

		Safe_Delete_Array(pPixel);
		Safe_Release(pTexture2D);
	}

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component_Static()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Particle_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle_Rect"),
		CVIBuffer_Particle_Rect::Create(m_pDevice, m_pContext, 300))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Particle_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle_Point"),
		CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext, 2000))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point"),
		CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext, 1))))
		return E_FAIL;
	
	/* For.Prototype_Component_VIBuffer_UI_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_UI_Point_58"),
		CVIBuffer_Particle_Point_UI::Create(m_pDevice, m_pContext, 58))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_UI_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_UI_Point_16"),
		CVIBuffer_Particle_Point_UI::Create(m_pDevice, m_pContext, 16))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_UI_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_UI_Point_20"),
		CVIBuffer_Particle_Point_UI::Create(m_pDevice, m_pContext, 25))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_UI_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_UI_Point_150"),
		CVIBuffer_Particle_Point_UI::Create(m_pDevice, m_pContext, 150))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Trail */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"),
		CVIBuffer_Trail::Create(m_pDevice, m_pContext, 10000))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_SwordTrail */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_SwordTrail"),
		CVIBuffer_SwordTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_RushTrail */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_RushTrail"),
		CVIBuffer_RushTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTexPoint"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTexPoint.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Effect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_EffectTexture"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_EffectTexture.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_EffectMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_EffectMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Background_Loading */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Background_Loading"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Resources/Models/NMH/forPS4_TitleImage.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_LOADING, LAYER_BACKGROUND, TEXT("GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext, TEXT("Prototype_Component_Texture_Background_Loading")))))
		return E_FAIL;

	//  For. Prototype_GameObject_Shader_Terrain
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxRectInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTX_RECT_INSTANCE::Elements, VTX_RECT_INSTANCE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTX_POINT_INSTANCE::Elements, VTX_POINT_INSTANCE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxUIPointInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxUIPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxUIPointInstance.hlsl"), VTX_POINT_INSTANCE::Elements, VTX_POINT_INSTANCE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Trail */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Trail"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Trail.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	return S_OK;
}

shared_ptr<CMainApp> CMainApp::Create()
{
	shared_ptr<CMainApp> pInstance = make_shared<CMainApp>();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		pInstance = nullptr;
	}

	return pInstance;
}

void CMainApp::Free()
{
	m_pGameInstance->Free();
}
