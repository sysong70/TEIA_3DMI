#pragma once

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>

#include <assert.h>

#include <sstream>
#include <iostream>

// CString용 Header
#include <atlstr.h>

// HPS headers must come after Windows headers
#include <hps.h>
#include <sprk.h>
#include <sprk_ops.h>

#include <sprk_exchange.h>

#define CHECK_POINTER(pcValue) if(0 == pcValue) { assert(FALSE);  return false; } 
#define CHECK_DWORD_PTR(nValue) if(0 == nValue) { assert(FALSE);  return false; } 

// 이름을 바꾸면 선택이 않됨. (절대 바꾸면 않됨)
#define HIGHLIGHT_STYLE_NAME	"highlight_style"

#define HPS_USER_DATA_MBITEM_INDEX		0
#define HPS_USER_DATA_NAME_INDEX		1