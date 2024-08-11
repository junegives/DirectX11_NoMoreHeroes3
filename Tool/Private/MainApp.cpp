#include "pch.h"
#include "MainApp.h"

#include "Level_Loading.h"
#include "ModelParser.h"

#include "BackGround.h"

CMainApp::CMainApp()
    : m_pGameInstance(CGameInstance::GetInstance())
	, m_pImGuiManager(CImGui_Manager::GetInstance())
{
}

HRESULT CMainApp::Initialize()
{
	/*shared_ptr<CModelParser> pModelParser = make_shared<CModelParser>();
	pModelParser->Read_Dir(TEXT("../../Resource/Resources/Models/NMH/"), true);*/

	GRAPHIC_DESC			GraphicDesc = {};

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.isWindowed = true;
	GraphicDesc.iWinSizeX = g_iWinSizeX;
	GraphicDesc.iWinSizeY = g_iWinSizeY;

	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_Static()))
		return E_FAIL;

	if (FAILED(m_pImGuiManager->Initialize(m_pDevice, m_pContext)))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_MAPTOOL)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	m_fTimeAcc += fTimeDelta;
#endif

	m_pGameInstance->Tick_Engine(fTimeDelta);

	if (CGameInstance::GetInstance()->Get_GameState() != CGame_Manager::STATE_LOADING)
		m_pImGuiManager->Tick(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	/*if (FAILED(m_pGameInstance->Draw(_float4(0.f, 0.f, 1.f, 1.f))))
		return E_FAIL;*/

	m_pGameInstance->Clear_BackBuffer_View({ 0.5f, 0.3f, 0.6f, 1.f });
	m_pGameInstance->Clear_DepthStencil_View();

	if (FAILED(m_pGameInstance->Draw()))
		return E_FAIL;

	if (CGameInstance::GetInstance()->Get_GameState() != CGame_Manager::STATE_LOADING)
		m_pImGuiManager->Render();

#ifdef _DEBUG
	if (CGame_Manager::STATE_LOADING != m_pGameInstance->Get_GameState())
	{
		++m_iRenderCount;

		if (1.f <= m_fTimeAcc)
		{
			m_fTimeAcc = 0;
			wsprintf(m_szFPS, TEXT("%d fps"), m_iRenderCount);

			m_iRenderCount = 0;
		}

		SetWindowText(g_hWnd, m_szFPS);
	}
#endif

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component_Static()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
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


	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eStartLevel)
{
	if (LEVEL_LOADING == eStartLevel)
		return E_FAIL;

	return m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevel));

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
	m_pImGuiManager->Free();
}
