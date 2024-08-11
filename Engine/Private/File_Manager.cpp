#include "File_Manager.h"

CFile_Manager::CFile_Manager()
	: m_hFileHandle(INVALID_HANDLE_VALUE)
{
}

HRESULT CFile_Manager::Open(const wstring& strFilePath, _bool isWrite)
{
	/*assert(m_hFileHandle != INVALID_HANDLE_VALUE);*/

	DWORD dwDesiredAccess = isWrite ? GENERIC_WRITE : GENERIC_READ;
	DWORD dwCreationDisposition = isWrite ? CREATE_ALWAYS : OPEN_EXISTING;

	m_hFileHandle = CreateFile(
		strFilePath.c_str(),
		dwDesiredAccess,
		0,
		nullptr,
		dwCreationDisposition,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	return S_OK;
}

HRESULT CFile_Manager::Close()
{
	if (m_hFileHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFileHandle);
		m_hFileHandle = INVALID_HANDLE_VALUE;
	}

	return S_OK;
}
