#pragma once

#include "3DF.h"

#include <chrono>

namespace H3DF
{
	namespace Utility
	{
		API_3DF bool ChangeSubSegmentColor(SegmentKey & cInTargetSegment, MaterialMappingKit const & cInKit, bool bRecursive = false);
		API_3DF CString GetTimeSpanString(std::chrono::milliseconds & ms);

		void Set3DfOptionString(char * chBuffer, char * chOption);

		CString HexStr(DWORD_PTR nValue);

		CString GetTypeString(Type eType);


		API_3DF bool SetSegmentName(SegmentKey & cInSegment, CString strName);
		API_3DF bool ShowSegmentName(SegmentKey & cInSegment, CString & strName);

		// 사용후 Buffer를 삭제해야 함.
		API_3DF bool UnicodeToChar(CString strText, char *& pchBuffer);
		API_3DF bool UnicodeToChar(CString strText, char *& pchBuffer, int & nBufferSize);
		API_3DF bool CharToUnicode(char * pchText, CString & strText);

		CString GetExecuteDirectory();
	};
};