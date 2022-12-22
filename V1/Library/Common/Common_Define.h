#pragma once

#include <atlstr.h>

void Trace(const wchar_t* pFormat, ...);

#ifndef ASSERT
#include <cassert>
#define ASSERT	assert

#define TRACE	Trace
#endif

//--------------------------------------------------------------------------------------------------

#ifdef _DEBUG

#define DEBUG_TRACE					Trace

#define DEBUG_RETURN				{ ASSERT(FALSE); return; }
#define DEBUG_STOP					ASSERT(FALSE);
#define DEBUG_CHECK					{ TRACE(L"%s(%d) : [CHECK]\n", __FILE__, __LINE__); }
#define DEBUG_VALID(x)				ASSERT(x != nullptr);
#define DEBUG_INVALID(x)			ASSERT(x == nullptr);
#define TRACE_INFO(x)				{ TRACE(L"%s(%d) : ", __FILE__, __LINE__); TRACE(x); TRACE(L"\n"); }

#define RETURN(x)					{ ASSERT(FALSE); return x; }
#define RETURN_NULL					{ ASSERT(FALSE); return nullptr; }
#define RETURN_FALSE				{ ASSERT(FALSE); return false; }
#define RETURN_TRUE					{ ASSERT(FALSE); return true; }

#else

#define DEBUG_TRACE

#define DEBUG_RETURN				return;
#define DEBUG_STOP
#define DEBUG_CHECK
#define DEBUG_VALID(x)
#define DEBUG_INVALID(x)
#define TRACE_INFO(x)

#define RETURN(x)					{ return x; }
#define RETURN_NULL					{ return nullptr; }
#define RETURN_FALSE				{ return false; }
#define RETURN_TRUE					{ return true; }

#endif	// _DEBUG

#define REMOVE_POINTER(x)			{ delete x; x = nullptr; }
#define REMOVE_ARRAY(x)				{ delete [] x; x = nullptr; }

//--------------------------------------------------------------------------------------------------

#include <vector>

typedef std::vector<CStringA> AStringArray;
typedef std::vector<char> ACharArray;
typedef std::vector<CString> WStringArray;
typedef std::vector<wchar_t> WCharArray;
