#include "stdafx.h"
#include "Facility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



#define PRESET PresetFacility

namespace PresetFacility
{
	Facility::UiLanguage Language = Facility::UiLanguage::English;
}



#pragma region Language

Facility::UiLanguage Facility::GetLanguage()
{
	return PRESET::Language;
}



void Facility::SetLanguage(UiLanguage value)
{
	PRESET::Language = value;

	BOOL success = FALSE;
	switch (value) {
	case UiLanguage::Korean:
		success = SetThreadPreferredUILanguages(MUI_LANGUAGE_NAME, L"ko-KR", NULL);
		break;

	case UiLanguage::English:
	default:
		success = SetThreadPreferredUILanguages(MUI_LANGUAGE_NAME, L"en-US", NULL);
		break;
	}

	ASSERT(success);
}



CString Facility::Local(LPCTSTR value)
{
	CString found;
	if (AfxExtractSubString(found, value, (int)PRESET::Language, '|')) {
		ASSERT(found != L"__#__"); //:TODO
		return found;
	}

	// language set not found
	RETURN(L"");
}



CString Facility::Local(CString& value)
{
	CString found;
	if (AfxExtractSubString(found, value, (int)PRESET::Language, '|')) {
		ASSERT(found != L"__#__"); //:TODO
		return found;
	}

	// language set not found
	RETURN(L"");
}

#pragma endregion //:REGION

#pragma region Resource

HBITMAP Facility::CreateBitmap(UINT id, const CSize& size)
{
	CBCGPSVGImage image;
	image.Load(id);

	return image.ExportToBitmap(size);
}



HICON Facility::CreateIcon(UINT id, const CSize& size)
{
	CBCGPSVGImage image;
	image.Load(id);

	return image.ExportToIcon(size);
}



void Facility::GetResource(UINT id, CString& title, CString& tooltip)
{
	CString resource;
	int i = resource.LoadString(id);
	ASSERT(i >= 0);

	AfxExtractSubString(tooltip, resource, 0 /* tooltip index */, '\n');
	AfxExtractSubString(title, resource, 1 /* title index */, '\n');
}



CString Facility::GetTitle(UINT id)
{
	CString resource;
	int i = resource.LoadString(id);
	ASSERT(i >= 0);

	CString result;
	AfxExtractSubString(result, resource, 1, '\n');

	return result;
}



CString Facility::GetTooltip(UINT id)
{
	CString resource;
	int i = resource.LoadString(id);
	ASSERT(i >= 0);

	CString result;
	AfxExtractSubString(result, resource, 0, '\n');

	return result;
}

#pragma endregion //:REGION

#undef PRESET