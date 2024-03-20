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

		API_3DF CString GetTypeString(Type eType);
		API_3DF Type GetType(Key & cKey);
		API_3DF Type GetType(HC_KEY nInKey);
		API_3DF CStringA GetName(HC_KEY nInKey);
		API_3DF CStringA GetName(Key & cInKey);

		//== String 관련 함수 ========================================================================
		// 사용후 Buffer를 삭제해야 함.
		API_3DF bool UnicodeToChar(CString strText, char *& pchBuffer);
		API_3DF bool UnicodeToChar(CString strText, char *& pchBuffer, int & nBufferSize);
		API_3DF CStringA ToChar(CString strText);

		API_3DF bool CharToUnicode(char * pchText, CString & strText);
		API_3DF CString ToString(char * pchText);

		API_3DF bool CopyString(const char * pchSoruce, char *& pchDestination);

		//== Raw Data와 숫자 변환 함수 ================================================================
		CString DoubleToRawString(double dValue);
		CStringA DoubleToRawStringA(double dValue);
		double RawStringToDouble(CString strText);
		double RawStringToDouble(CStringA strText);

		CString IntToRawString(int nValue);
		CStringA IntToRawStringA(int nValue);
		int RawStringToInt(CString strText);
		int RawStringToInt(CStringA strText);

		int RawByteToInt(BYTE * pbData);

		CString ShortToRawString(short nValue);
		CStringA ShortToRawStringA(short nValue);
		short RawStringToShort(CString strText);
		short RawStringToShort(CStringA strText);

		short RawByteToShort(BYTE * pbData);

		//== File 관련 함수 ==========================================================================
		CString GetExecuteDirectory();

		//== Segment 관련 함수 =======================================================================
		API_3DF bool ShowSubSegment(SegmentKey & cInTargetSegment, CStringA strInSegmentName, SegmentKey & cOutSegment);
	};

	class API_3DF UserData
	{
	public:
		//== Segment User Data 관련 함수 ============================================================= 
		static bool SetSegmentName(SegmentKey & cInSegment, CString strName);
		static bool ShowSegmentName(SegmentKey & cInSegment, CString & strName);
		static bool ShowSegmentName(HC_KEY nInKey, CString & strOutName);

		static bool SetComponentType(SegmentKey & cInSegment, DWORD nInType);
		static bool ShowComponentType(SegmentKey & cInSegment, DWORD & eOutType);

		static bool AddComponentStatus(SegmentKey & cInSegment, DWORD nInStatus);
		static bool RemoveComponentStatus(SegmentKey & cInSegment, DWORD nInStatus);
		static bool ShowComponentStatus(SegmentKey & cInSegment, DWORD & nOutStatus);

		static bool SetIncludedCount(SegmentKey & cInSegment, DWORD nInCount);
		static bool ShowIncludedCount(SegmentKey & cInSegment, DWORD & nOutCount);

		//== Geomety User Data 관련 함수 ============================================================= 
		static bool SetComponentType(GeometryKey & cInGeometry, DWORD nInType);
		static bool AddTopologyType(GeometryKey & cInGeometry, DWORD nInType);
		static bool ShowComponentType(GeometryKey & cInGeometry, DWORD & eOutType);
	};
};