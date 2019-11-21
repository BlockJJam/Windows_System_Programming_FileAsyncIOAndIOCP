#include "stdafx.h"
#include "windows.h"

HANDLE g_hevExit = NULL;

BOOL CtrlHandler(DWORD fdwCtrlType)
{
	bool bCtrlRet = false;
	switch (fdwCtrlType)
	{
		// Handle the ctrl -c signal
	case CTRL_C_EVENT:
		printf("CTRL-C event\n\n");
		bCtrlRet = true;
		break;

		// CTRL-Close : confirm that the user wants to exit
	case CTRL_CLOSE_EVENT:
		printf("Ctrl-Close event \n\n");
		bCtrlRet = true;
		break;

	case CTRL_BREAK_EVENT:
		printf("CTRL-Break event\n\n");
		break;

	case CTRL_LOGOFF_EVENT:
		printf("Ctrl-Logoff event\n\n");
		break;

	case CTRL_SHUTDOWN_EVENT:
		printf("Ctrl-Shutdown event\n\n");
		break;
	}
	SetEvent(g_hevExit);
	return bCtrlRet;
}

void _tmain()
{
	g_hevExit = CreateEvent(NULL, true, false, NULL);

	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE))
	{
		printf("\nERROR: Could not set control handelr");
		return;
	}
	printf("\n The Control Handler is installed . \n\n");

	while (true)
	{
		DWORD dwWaitRet = WaitForSingleObject(g_hevExit, INFINITE);
		if (dwWaitRet == WAIT_OBJECT_0)
			break;
	}
	CloseHandle(g_hevExit);
	printf("\nProgram terminated... \n\n");
}