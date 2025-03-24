#pragma once

#include "stdafx.h"

using SendSignalFunc = void (*)(const wchar_t* content);

//--------------------------------------------------------------------------------------------------

extern "C"
{
	// Pipe for renderer to ui
	INTERFACE_API void AssignSendSignalFunc(SendSignalFunc fp);
	// Pipe for ui to renderer, pContent: Json object
	INTERFACE_API bool ReceiveSignal(const wchar_t* pContent);

	// WARNING - not thread call
	INTERFACE_API void SetLanguage(int value);
	// WARNING - not thread call
	INTERFACE_API double* GetCoordinate(int viewId, int x, int y);
}
