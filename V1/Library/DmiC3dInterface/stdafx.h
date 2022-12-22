#pragma once

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>

#include <assert.h>

#include <sstream>
#include <iostream>

// CString용 Header
#include <atlstr.h>

#define CHECK_POINTER(pcValue) if(0 == pcValue) { assert(FALSE);  return false; } 
#define CHECK_DWORD_PTR(nValue) if(0 == nValue) { assert(FALSE);  return false; } 
