#include "stdafx.h"
#include "dllmain.h"
#include "3DF/3DF.Operator.ObjectSnap.h"

HMODULE Application::Instance = nullptr;



BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch(ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			Application::Instance = hModule;
			//TDF::Operator::ObjectSnap::LoadResource();
			break;

		case DLL_THREAD_ATTACH:
			Application::Instance = nullptr;
			break;

		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}

	return TRUE;
}

