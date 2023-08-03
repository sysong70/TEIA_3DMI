#pragma once

#include "Json.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

	_declspec (dllexport) void ExecuteCommand(const wchar_t * pchCommandText);
	_declspec (dllexport) void SendCommand(void (*lpfnCallback)(const wchar_t *));

#ifdef __cplusplus
}
#endif
