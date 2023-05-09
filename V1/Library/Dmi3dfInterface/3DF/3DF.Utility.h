#pragma once

#include "3DF.h"

#include <chrono>

OPEN_3DF_NAMESPACE

namespace Utility
{
	API_3DF bool ChangeSubSegmentColor(SegmentKey & cInTargetSegment, MaterialMappingKit const & cInKit, bool bRecursive = false);
	API_3DF CString GetTimeSpanString(std::chrono::milliseconds & ms);

	void Set3DfOptionString(char * chBuffer, char * chOption);

	CString HexStr(DWORD_PTR nValue);

	CString GetTypeString(Type eType);


	API_3DF bool SetSegmentName(SegmentKey & cInSegment, CString strName);

	// 사용후 Buffer를 삭제해야 함.
	bool UnicodeToChar(CString strText, char *& pchBuffer);
	bool UnicodeToChar(CString strText, char *& pchBuffer, int & nBufferSize);
};

CLOSE_3DF_NAMESPACE