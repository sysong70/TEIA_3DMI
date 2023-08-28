#pragma once

#include "Json.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
	namespace SESSION
	{
		_declspec (dllexport) void ExecuteCommand(const wchar_t * pchCommandText);
		_declspec (dllexport) void AssignSendSignalFunc(void (*lpfnCallback)(const wchar_t *));
	}
#ifdef __cplusplus
}
#endif
