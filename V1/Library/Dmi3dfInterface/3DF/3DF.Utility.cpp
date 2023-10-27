#include "StdAfx.h"

#include "3DF.Utility.h"

#include "Segment.h"

#include <HUtility.h>
#include <HTools.h>

using namespace std::chrono;

USING_3DF_NAMESPACE

bool H3DF::Utility::ChangeSubSegmentColor(SegmentKey & cInTargetSegment, MaterialMappingKit const & cInKit, bool bRecursive)
{
	SegmentKeyArray cSubSegments;

	size_t nSubCount = cInTargetSegment.ShowSubsegments(cSubSegments);

	for (size_t nIndex = 0; nIndex < nSubCount; nIndex++) {
		cSubSegments[nIndex].SetMaterialMapping(cInKit);
		if (true == bRecursive) {
			ChangeSubSegmentColor(cSubSegments[nIndex], cInKit, true);
		}
	}

	return true;
}

CString H3DF::Utility::GetTimeSpanString(milliseconds & ms)
{
	auto secs = duration_cast<seconds>(ms);
	ms -= duration_cast<milliseconds>(secs);
	auto mins = duration_cast<minutes>(secs);
	secs -= duration_cast<seconds>(mins);

	CString strSpanText;

	if (0 < mins.count()) {
		strSpanText.Format(L"%dm %d.%ds", mins.count(), secs.count(), (int)(ms.count() / 10));
	}
	else {
		strSpanText.Format(L"%d.%ds", secs.count(), (int)(ms.count() / 10));
	}

	return strSpanText;
}

void H3DF::Utility::Set3DfOptionString(char * chBuffer, char * chOption)
{
	if (0 == strlen(chOption)) {
		return;
	}

	if (0 == strlen(chBuffer)) {
		sprintf(chBuffer, "%s", chOption);
	}
	else {
		sprintf(chBuffer, "%s, %s", chBuffer, chOption);
	}
}

CString H3DF::Utility::HexStr(DWORD_PTR nValue)
{
	CString strText;
	strText.Format(L"0x%llx", nValue);
	return strText;
}

CString H3DF::Utility::GetTypeString(Type eType)
{
	CString strText;

	switch (eType)
	{
		case H3DF::Type::None:
			strText = L"None";
			break;

		case H3DF::Type::GenericMask:
			strText = L"GenericMask";
			break;

		case H3DF::Type::SelectionResults:
			strText = L"SelectionResults";
			break;

		case H3DF::Type::SelectionItem:
			strText = L"SelectionItem";
			break;

		case H3DF::Type::SelectionResultsIterator:
			strText = L"SelectionResultsIterator";
			break;

		case H3DF::Type::Kit:
			strText = L"Kit";
			break;

		case H3DF::Type::CircleKit:
			strText = L"CircleKit";
			break;

		case H3DF::Type::LineKit:
			strText = L"LineKit";
			break;

		case H3DF::Type::ShellKit:
			strText = L"ShellKit";
			break;

		case H3DF::Type::CameraKit:
			strText = L"CameraKit";
			break;

		case H3DF::Type::Key:
			strText = L"Key";
			break;

		case H3DF::Type::IncludeKey:
			strText = L"IncludeKey";
			break;

		case H3DF::Type::PortfolioKey:
			strText = L"PortfolioKey";
			break;

		case H3DF::Type::StyleKey:
			strText = L"StyleKey";
			break;

		case H3DF::Type::SegmentKey:
			strText = L"SegmentKey";
			break;

		case H3DF::Type::WindowKey:
			strText = L"WindowKey";
			break;

		case H3DF::Type::GeometryKey:
			strText = L"GeometryKey";
			break;

		case H3DF::Type::ReferenceKey:
			strText = L"ReferenceKey";
			break;

		case H3DF::Type::CircleKey:
			strText = L"CircleKey";
			break;

		case H3DF::Type::CircularArcKey:
			strText = L"CircularArcKey";
			break;

		case H3DF::Type::CircularWedgeKey:
			strText = L"CircularWedgeKey";
			break;

		case H3DF::Type::CuttingSectionKey:
			strText = L"CuttingSectionKey";
			break;

		case H3DF::Type::CylinderKey:
			strText = L"CylinderKey";
			break;

		case H3DF::Type::EllipseKey:
			strText = L"EllipseKey";
			break;

		case H3DF::Type::EllipticalArcKey:
			strText = L"EllipticalArcKey";
			break;

		case H3DF::Type::InfiniteLineKey:
			strText = L"InfiniteLineKey";
			break;

		case H3DF::Type::LineKey:
			strText = L"LineKey";
			break;

		case H3DF::Type::PolygonKey:
			strText = L"PolygonKey";
			break;

		case H3DF::Type::ShellKey:
			strText = L"ShellKey";
			break;

		case H3DF::Type::SphereKey:
			strText = L"SphereKey";
			break;

		case H3DF::Type::TextKey:
			strText = L"TextKey";
			break;

		case H3DF::Type::Control:
			strText = L"Control";
			break;

		case H3DF::Type::SelectionControl:
			strText = L"SelectionControl";
			break;

		case H3DF::Type::HighlightControl:
			strText = L"HighlightControl";
			break;

		default:
			strText.Format(L"Unknown Type: %d", (int)eType);
			break;
	}

	return strText;
}

bool H3DF::Utility::SetSegmentName(SegmentKey & cInSegment, CString strName)
{
	char * pchName = nullptr;
	int nSize = 0;

	if (false == UnicodeToChar(strName, pchName, nSize)) {
		return false;
	}

	cInSegment.SetUserData((intptr_t)UserDataIndex::Name, nSize, (BYTE *)pchName);

	delete [] pchName;

	return true;
}

bool H3DF::Utility::ShowSegmentName(SegmentKey & cInSegment, CString & strName)
{
	ByteArray aUserData;
	if (false == cInSegment.ShowUserData((intptr_t)UserDataIndex::Name, aUserData)) {
		return false;
	}

	if (false == CharToUnicode((char *)aUserData.data(), strName)) {
		return false;
	}

	return true;
}

bool H3DF::Utility::UnicodeToChar(CString strText, char *& pchBuffer)
{
	int nBufferSize = 0;
	return UnicodeToChar(strText, pchBuffer, nBufferSize);
}

bool H3DF::Utility::UnicodeToChar(CString strText, char *& pchBuffer, int & nBufferSize)
{
	if (true == strText.IsEmpty()) {
		return false;
	}

	int nSize = WideCharToMultiByte(CP_ACP, 0, strText, -1, NULL, 0, NULL, NULL);
	pchBuffer = new char[nSize];
	if (nullptr == pchBuffer) {
		ASSERT(FALSE);
		return false;
	}

	nBufferSize = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)strText, -1, pchBuffer, nSize, NULL, NULL);

	if (0 == nBufferSize) {
		ASSERT(FALSE);
		delete [] pchBuffer;
		return false;
	}

	return true;
}

CStringA H3DF::Utility::ToChar(CString strText)
{
	char * pchBuffer = nullptr;
	if (false == UnicodeToChar(strText, pchBuffer)) {
		return CStringA();
	}

	CStringA strResult(pchBuffer);

	delete [] pchBuffer;

	return strResult;
}

bool H3DF::Utility::CharToUnicode(char * pchText, CString & strText)
{
	if (nullptr == pchText) {
		return false;
	}

	size_t nSize = strlen(pchText);
	if (0 == nSize) {
		return false;
	}

	int nBufferSize = MultiByteToWideChar(CP_ACP, 0, pchText, -1, NULL, NULL);
	if (0 == nBufferSize) {
		return false;
	}

	WCHAR * pchBuffer = new WCHAR[nBufferSize + 1];
	if (nullptr == pchBuffer) {
		ASSERT(FALSE);
		return false;
	}

	int nConvertSize = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pchText, strlen(pchText), pchBuffer, nBufferSize);
	if (0 == nConvertSize) {
		ASSERT(FALSE);
		delete [] pchBuffer;
		return false;
	}

	pchBuffer[nConvertSize] = L'\0';

	strText = pchBuffer;

	delete [] pchBuffer;

	return true;
}

CString H3DF::Utility::GetExecuteDirectory()
{
	TCHAR szBuffer[MAX_PATH];
	TCHAR Drive[_MAX_DRIVE];
	TCHAR Path[_MAX_PATH];
	TCHAR Filename[_MAX_FNAME];
	TCHAR Ext[_MAX_EXT];

	GetModuleFileName(NULL, szBuffer, sizeof(szBuffer)); // get process file name
	_wsplitpath_s(szBuffer, Drive, _MAX_DRIVE, Path, _MAX_PATH, Filename, _MAX_FNAME, Ext, _MAX_EXT); // get drive, path, file, ext name

	CString strFilePath;
	strFilePath.Format(L"%s%s", Drive, Path);

	return strFilePath;
}

//== String 관련 함수 ===============================================================================
bool H3DF::Utility::CopyString(const char * pchSoruce, char *& pchDestination)
{
	// 입력 문자열의 크기 계산
	size_t nSourceSize = strlen(pchSoruce) + 1; // 널 종료 문자('\0')를 포함해서 크기 계산

	// 대상 문자열에 충분한 메모리 할당
	pchDestination = new char[nSourceSize * sizeof(char)];
	if (nullptr == pchDestination) {
		// 메모리 할당 실패 처리
		return false;
	}

	// 문자열 복사
	strcpy(pchDestination, pchSoruce);
/*
	size_t nBufferSize = wcslen(pchBuffer) + 1; // 널 종료 문자('\0')를 포함해서 크기 계산

	// 대상 문자열에 충분한 메모리 할당
	wchar_t * pchCopyBuffer = new wchar_t[nBufferSize];
	if (nullptr == pchCopyBuffer) {
		wprintf(L"메모리 할당 실패\n");
		return;
	}

	// 문자열 복사
	wcscpy(pchCopyBuffer, pchBuffer);
*/

	return true;
}