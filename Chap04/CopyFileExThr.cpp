#include "stdafx.h"
#include "windows.h"
#include "iostream"
using namespace std;

DWORD CALLBACK CopyProgressRoutine(
	LARGE_INTEGER llTotalSize, LARGE_INTEGER llTranBytes,
	LARGE_INTEGER llStrmSize, LARGE_INTEGER llStrmBytes,
	DWORD dwStrmNum, DWORD dwCbReason,
	HANDLE hSrcFile, HANDLE hDstFile, LPVOID pData
)
{
	DWORD dwNtexOpt = *((PDWORD)pData);
	int nRate = (int)(llTranBytes.QuadPart * 100LL / llTotalSize.QuadPart);
	printf(" Thr %d => File %d%% copied: %l64d bytes\n",
		GetCurrentThreadId(), nRate, llTranBytes.QuadPart);

	return dwNtexOpt;
}

DWORD WINAPI ThreadProc(PVOID pParam)
{
	PCTSTR* parFiles = (PCTSTR*)pParam;

	bool bIsOK = CopyFileEx
	(
		parFiles[0], parFiles[1], CopyProgressRoutine, (PVOID)parFiles[2], NULL, 0
	);

	return ((!bIsOK) ? GetLastError() : ERROR_SUCCESS);
}

void _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 3)
	{
		cout << "Usage: CopyFileEx sourceFile TargetFile" << endl;
		return;
	}

	DWORD dwNextOpt = PROGRESS_CONTINUE;
	PCTSTR arpArgs[3] = { argv[1], argv[2], (PCTSTR)&dwNextOpt };
	DWORD dwThrID = 0;
	HANDLE hThread = CreateThread(NULL, 0, ThreadProc, arpArgs, 0, &dwThrID);

	int ch = getchar();
	switch (ch)
	{
	case 'c': dwNextOpt = PROGRESS_CANCEL; break;
	case 'q': dwNextOpt = PROGRESS_QUIET; break;
	case 's': dwNextOpt = PROGRESS_STOP; break;
	}
	WaitForSingleObject(hThread, INFINITE);

	DWORD hr = 0;
	GetExitCodeThread(hThread, &hr);
	CloseHandle(hThread);
	if (hr != ERROR_SUCCESS)
	{
		if (hr == ERROR_REQUEST_ABORTED)
			cout << "FIle copy aborted by user..." << endl;
		else
			cout << " Error occurred in file copying, code = " << hr << endl;
	}
	else
		cout << endl << "File copy successufully completed... " << endl;
}