#include "stdafx.h"
#include "windows.h"

PCTSTR c_pszActions[] =
{
	_T("ADDED"),
	_T("REMOVE"),
	_T("MODIFIED"),
	_T("RENAMED_OLD_NAME"),
	_T("RENAMED_NEW_NAME")
};
#define DIR_NOTI_FILTER FILE_NOTIFY_CHANGE_FILE_NAME | \
						FILE_NOTIFY_CHANGE_DIR_NAME | \
						FILE_NOTIFY_CHANGE_SIZE | \
						FILE_NOTIFY_CHANGE_LAST_WRITE

void PrintDirModEntries(PBYTE plter)
{
	while (true)
	{
		PFILE_NOTIFY_INFORMATION pFNI = (PFILE_NOTIFY_INFORMATION)plter;
		TCHAR szFileName[MAX_PATH];
		memcpy(szFileName, pFNI->FileName, pFNI->FileNameLength * sizeof(TCHAR));
		_tprintf(_T("File %s %s\n"), szFileName, c_pszActions[pFNI->Action - 1]);

		if (pFNI->NextEntryOffset == 0)
			break;
		plter += pFNI->NextEntryOffset;
	}
}

void _tmain()
{
	HANDLE hDir = INVALID_HANDLE_VALUE;
	try
	{
		hDir = CreateFile
		(
			_T("C:\\Temp"),
			GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS,
			NULL
		);
		if (hDir == ERROR_SUCCESS)
			throw HRESULT_FROM_WIN32(GetLastError());

		while (true)
		{
			DWORD dwNotiBytes = 0;
			BYTE arBuff[65536];
			BOOL blsOK = ReadDirectoryChangesW
			(
				hDir, arBuff, sizeof(arBuff), FALSE, DIR_NOTI_FILTER, &dwNotiBytes, NULL, NULL
			);
			if (!blsOK)
				throw HRESULT_FROM_WIN32(GetLastError());

			PrintDirModEntries(arBuff);
		}
	}
	catch (HRESULT ex)
	{
		printf("Error occurred, code = 0x%08x\n", ex);
	}
	if (hDir != INVALID_HANDLE_VALUE)
		CloseHandle(hDir);
}