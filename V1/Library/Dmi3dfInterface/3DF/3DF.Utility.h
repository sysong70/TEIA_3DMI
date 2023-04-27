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
};

CLOSE_3DF_NAMESPACE