#include "stdafx.h"
#include "windows.h"
#include "iostream"
using namespace std;

#define BUFF_SIZE 65536

void _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 3)
	{
		cout << "Usage : FileCopyNoBuffer SourceFile TargetFile " << endl;
		return;
	}

	TCHAR* pszSrcFile = argv[1];
	TCHAR* pszDstFile = argv[2];

	HANDLE hSrcFile = CreateFile
	(
		argv[1],
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_NO_BUFFERING,
		NULL
	);
	
	if (hSrcFile == INVALID_HANDLE_VALUE)
	{
		cout << "CreateFile Failed , code : " << GetLastError() << endl;
		return;
	}

	LARGE_INTEGER lISrcSize;
	GetFileSizeEx(hSrcFile, &lISrcSize);

	LARGE_INTEGER lIDstSize = lISrcSize;
	DWORD dwPad = (lIDstSize.QuadPart % BUFF_SIZE);
	if (dwPad > 0)
		lIDstSize.QuadPart = lIDstSize.QuadPart + (BUFF_SIZE - dwPad);

	HANDLE hDstFile = CreateFile
	(
		argv[2],
		GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_FLAG_NO_BUFFERING,
		NULL
	);

	if (hDstFile == INVALID_HANDLE_VALUE)
	{
		cout << "CreateFile failed, code : " << GetLastError() << endl;
		return;
	}
	SetFilePointerEx(hDstFile, lIDstSize, NULL, FILE_BEGIN);
	SetEndOfFile(hDstFile);

	SetFilePointer(hDstFile, 0, NULL, FILE_BEGIN);

	PBYTE pBuff = (PBYTE)VirtualAlloc(NULL, BUFF_SIZE, MEM_COMMIT, PAGE_READWRITE);
	DWORD dwErrCode = 0;
	LONGLONG lICopiedSize = 0;
	while (lICopiedSize < lIDstSize.QuadPart)
	{
		DWORD dwReadBytes = 0;
		BOOL bIsOK = ReadFile
		(
			hSrcFile, pBuff, BUFF_SIZE,
			&dwReadBytes, NULL
		);
		if (!bIsOK)
		{
			dwErrCode = GetLastError();
			break;
		}

		DWORD dwWroteBytes = 0;
		bIsOK = WriteFile
		(
			hDstFile, pBuff, BUFF_SIZE,
			&dwWroteBytes, NULL
		);
		if (!bIsOK)
		{
			dwErrCode = GetLastError();
			break;
		}

		lICopiedSize += dwWroteBytes;
		printf("Copy bytes : %I64d, %d%%\n", lICopiedSize, (100 * lICopiedSize) / lIDstSize.QuadPart);
	}
	
	VirtualFree(pBuff, 0, MEM_RELEASE);
	CloseHandle(hSrcFile);

	if (dwErrCode == ERROR_SUCCESS)
	{
		hDstFile = ReOpenFile(hDstFile, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0);
		SetFilePointerEx(hDstFile, lISrcSize, NULL, FILE_BEGIN);
		SetEndOfFile(hDstFile);
	}

	else
		cout << endl << "Error occurred in file Copying, code = " << dwErrCode << endl;
	CloseHandle(hDstFile);
}