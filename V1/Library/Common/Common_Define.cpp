#include "stdafx.h"
#include "Common_Define.h"

#include <locale>
#include <memory>
#include <string>

#ifdef _DEBUG
#ifdef _AFXDLL
#define new DEBUG_NEW
#endif

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

void Trace(const wchar_t* pFormat, ...)
{
	static TCHAR buffer[2048] = { TEXT('\0') };

	va_list args;
	va_start(args, pFormat);
	int size = ::vswprintf(nullptr, 0, pFormat, args) + 1;
	// CHECK
	::vswprintf(buffer, size, pFormat, args);
	va_end(args);

	// KEN - 20240206
	buffer[size - 1] = L'\r';
	buffer[size] = L'\n';
	buffer[size + 1] = L'\0';

	OutputDebugString(buffer);
	ZeroMemory(buffer, sizeof(buffer));
}
