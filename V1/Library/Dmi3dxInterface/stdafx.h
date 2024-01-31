#pragma once

#define WIN32_LEAN_AND_MEAN              거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

// Windows 헤더 파일
#include <windows.h>

// CString용 Header
#include <atlstr.h>

#include <iostream>
#include <vector>

#include "LogManager.h"

#ifndef INITIALIZE_A3D_API
	#include <A3DSDKIncludes.h>
#endif // INITIALIZE_A3D_API

// #ifndef A3DSDKLOADER_H
// 	#include <A3DSDKLoader.h>
// #endif // A3DSDKLOADER_H

#define BUILD_PRODUCTNAME	L"3DMI"
#define BUILD_VER_BRAND		L"1.0"