#pragma once

#include "Engine_Defines.h"

class CFile_Manager
{
public:
	CFile_Manager();
	~CFile_Manager() = default;

public:
	HRESULT Open(const wstring& strFilePath, _bool isWrite);
	HRESULT Close();

public:
	template<typename T>
	void Write(const T& data)
	{
		DWORD numOfBytes = 0;

		::WriteFile(m_hFileHandle, &data, sizeof(T), (LPDWORD)&numOfBytes, nullptr);
	}
	
	// �迭 ����
	template<typename T>
	void Write(T* data, _uint dataSize)
	{
		DWORD numOfBytes = 0;

		::WriteFile(m_hFileHandle, data, dataSize, (LPDWORD)&numOfBytes, nullptr);
	}

	// string ����
	void Write(const std::string& str) {

		DWORD numOfBytes = 0;

		// ���ڿ� ���� ����
		size_t strLength = str.size();
		Write(strLength);

		// ���ڿ� ������ ����
		::WriteFile(m_hFileHandle, str.c_str(), static_cast<DWORD>(strLength), (LPDWORD)&numOfBytes, nullptr);
	}

	template<typename T>
	HRESULT Read(T& data)
	{
		DWORD numOfBytes = 0;
		if (false == ::ReadFile(m_hFileHandle, &data, sizeof(T), (LPDWORD)&numOfBytes, nullptr))
			return E_FAIL;

		return S_OK;
	}

	template<typename T>
	HRESULT Read(T* data, _uint dataSize)
	{
		DWORD numOfBytes = 0;
		if (false == ::ReadFile(m_hFileHandle, data, dataSize, (LPDWORD)&numOfBytes, nullptr))
			return E_FAIL;

		return S_OK;
	}

	// string �б�
	HRESULT Read(string& str) {

		DWORD numOfBytes = 0;

		// ���ڿ� ���� �б�
		size_t strLength;
		Read(strLength);

		// ���ڿ� ������ �б�
		str.resize(strLength);
		if (false == ::ReadFile(m_hFileHandle, &str[0], static_cast<DWORD>(strLength), (LPDWORD)&numOfBytes, nullptr))
			return E_FAIL;

		return S_OK;
	}

private:
	HANDLE	m_hFileHandle;
};

