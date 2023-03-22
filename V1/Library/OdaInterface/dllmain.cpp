#include "stdafx.h"
#include "Application.h"



BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		Application::Instance = hModule;
		break;

	case DLL_PROCESS_DETACH:
		Application::Instance = nullptr;
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;

	default:
		break;
	}

	return TRUE;
}

