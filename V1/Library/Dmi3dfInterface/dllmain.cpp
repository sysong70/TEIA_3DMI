#include "stdafx.h"
#include "dllmain.h"
#include "3DF/Operator.ObjectSnap.h"

// #define _CRTDBG_MAP_ALLOC
// #include <stdlib.h>
// #include <crtdbg.h>

HMODULE Application::Instance = nullptr;


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch(ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			Application::Instance = hModule;
			//H3DF::Operator::ObjectSnap::LoadResource();
			break;

		case DLL_THREAD_ATTACH:
			Application::Instance = nullptr;
			break;

		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			int i = 0;
// 			_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
// 			_CrtDumpMemoryLeaks();
			break;
	}

	return TRUE;
}

