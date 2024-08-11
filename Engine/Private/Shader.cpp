#include "Shader.h"

CShader::CShader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CComponent(pDevice, pContext)
{
}

CShader::CShader(const CShader& rhs)
    : CComponent(rhs)
    , m_pEffect(rhs.m_pEffect)
    , m_InputLayouts(rhs.m_InputLayouts)
{
}

HRESULT CShader::Initialize_Prototype(const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	_uint		iHlslFlag = { 0 };

#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	/* ��η� �޾ƿ� ���̴������� �������Ͽ� �� ��üȭ�Ѵ�. */
	if (FAILED(D3DX11CompileEffectFromFile(strShaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice.Get(), &m_pEffect, nullptr)))
		return E_FAIL;

	D3DX11_EFFECT_DESC		EffectDesc = {};
	m_pEffect->GetDesc(&EffectDesc);

	for (size_t i = 0; i < EffectDesc.Techniques; i++)
	{
		ComPtr<ID3DX11EffectTechnique> pTechnique = m_pEffect->GetTechniqueByIndex(i);
		if (nullptr == pTechnique)
			return E_FAIL;

		D3DX11_TECHNIQUE_DESC		TechniqueDesc = {};
		pTechnique->GetDesc(&TechniqueDesc);

		for (size_t j = 0; j < TechniqueDesc.Passes; j++)
		{
			ComPtr<ID3D11InputLayout> pInputLayout = { nullptr };

			ComPtr<ID3DX11EffectPass> pPass = pTechnique->GetPassByIndex(j);
			if (nullptr == pPass)
				return E_FAIL;

			D3DX11_PASS_DESC		PassDesc = {};
			pPass->GetDesc(&PassDesc);

			if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
				return E_FAIL;

			m_InputLayouts.push_back(pInputLayout);
		}
	}
	return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CShader::Begin(_uint iPassIndex, _uint iTechniqueIndex)
{
	ComPtr<ID3DX11EffectTechnique> pTechnique = m_pEffect->GetTechniqueByIndex(iTechniqueIndex);
	if (nullptr == pTechnique)
		return E_FAIL;

	ComPtr<ID3DX11EffectPass> pPass = pTechnique->GetPassByIndex(iPassIndex);
	if (nullptr == pPass)
		return E_FAIL;

	pPass->Apply(0, m_pContext.Get());

	m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex].Get());

	return S_OK;
}

HRESULT CShader::Bind_RawValue(const _char* pConstantName, const void* pData, _uint iLength)
{
	// texture, bool(AsBool() ����ϱ�)�� ��������.
	// ����� ���� �� ������ ��� ������ �Լ��� ���� ����. (row�� column�� ��������.)

	// ������ �̸��� ���� ���̴� ������ ���̴����� ã�ƿ´�.
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	// �ش� ������ ���̴� ������ ���ε��Ѵ�.
	return pVariable->SetRawValue(pData, 0, iLength);
}

HRESULT CShader::Bind_SRV(const _char* pConstantName, ComPtr<ID3D11ShaderResourceView> pSRV)
{
	ComPtr<ID3DX11EffectVariable> pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ComPtr<ID3DX11EffectShaderResourceVariable> pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
		return E_FAIL;

	return pSRVariable->SetResource(pSRV.Get());
}

HRESULT CShader::Bind_SRVs(const _char* pConstantName, ID3D11ShaderResourceView** ppSRV, _uint iNumTextures)
{
	// ������ �̸��� ���� ���̴� ������ ���̴����� ã�ƿ´�.
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	// ���� ���̴� ������ SRV�� ����� �� �ֵ��� �� ��ȯ�Ѵ�.
	ID3DX11EffectShaderResourceVariable* pSRVarible = pVariable->AsShaderResource();

	//// �� �ؽ�ó�� ���������� ���ε��Ѵ�.
	//for (_uint i = 0; i < iNumTextures; ++i)
	//{
	//	// �ش� �ؽ�ó�� ���̴� ������ ���ε��Ѵ�.
	//	HRESULT hr = pSRVarible->SetResource(ppSRV[i].Get());
	//	if (FAILED(hr))
	//		return hr;
	//}

	//return S_OK;

	// ���������� ���̴� ������ ��� SRV�� ����ȯ�ߴٸ�, �ش� SRV�� ���̴� ������ ���ε��Ѵ�.
	return pSRVarible->SetResourceArray(ppSRV, 0, iNumTextures);
}

HRESULT CShader::Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix)
{
	ComPtr<ID3DX11EffectVariable> pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ComPtr<ID3DX11EffectMatrixVariable> pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	return pMatrixVariable->SetMatrix((_float*)pMatrix);
}

HRESULT CShader::Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrices, _uint iNumMatrices)
{
	ComPtr<ID3DX11EffectVariable> pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ComPtr<ID3DX11EffectMatrixVariable> pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	return pMatrixVariable->SetMatrixArray((_float*)pMatrices, 0, iNumMatrices);
}

shared_ptr<CShader> CShader::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	shared_ptr<CShader> pInstance = make_shared<CShader>(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strShaderFilePath, pElements, iNumElements)))
	{
		MSG_BOX("Failed to Created : CShader");
        pInstance.reset();
	}

	return pInstance;
}

shared_ptr<CComponent> CShader::Clone(void* pArg)
{
	shared_ptr<CShader> pInstance = make_shared<CShader>(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CShader");
		pInstance.reset();
	}

	return pInstance;
}

void CShader::Free()
{
	__super::Free();

	for (auto& pInputLayout : m_InputLayouts)
		pInputLayout.Reset();

	m_pEffect.Reset();
}
