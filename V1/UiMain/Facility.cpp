#include "stdafx.h"
#include "Facility.h"
#include "Control.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetFacility

namespace PresetFacility
{
	Facility::ELanguage Language = Facility::ELanguage::English;
	Json::Value DummyValue;
}



#pragma region Language

Facility::ELanguage Facility::GetLanguage()
{
	return PRESET::Language;
}



void Facility::SetLanguage(ELanguage value)
{
	PRESET::Language = value;

	BOOL success = FALSE;
	switch (value) {
	case ELanguage::Korean:
		success = ::SetThreadPreferredUILanguages(MUI_LANGUAGE_NAME, L"ko-KR", NULL);
		break;

	case ELanguage::English:
	default:
		success = ::SetThreadPreferredUILanguages(MUI_LANGUAGE_NAME, L"en-US", NULL);
		break;
	}

	ASSERT(success);
}



CString Facility::Local(LPCTSTR value)
{
	CString found;
	if (AfxExtractSubString(found, value, (int)PRESET::Language, '|')) {
		//:TODO
		//ASSERT(found != L"__#__");
		return found;
	}

	// language set not found
	RETURN(L"");
}



CString Facility::Local(CString& value)
{
	CString found;
	if (AfxExtractSubString(found, value, (int)PRESET::Language, '|')) {
		//:TODO
		//ASSERT(found != L"__#__");
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



bool Facility::LoadTextResource(UINT id, CString& result)
{
	HRSRC hRes = ::FindResource(NULL, MAKEINTRESOURCE(id), L"TEXT");
	DWORD dwResourceSize = ::SizeofResource(NULL, hRes);
	HGLOBAL hGlobal = ::LoadResource(NULL, hRes);
	LPVOID pData = ::LockResource(hGlobal);

	char* pChar = new char[dwResourceSize + 1];
	memcpy_s(pChar, dwResourceSize, pData, dwResourceSize);
	pChar[dwResourceSize] = _T('\0');

	int nSize = ::MultiByteToWideChar(CP_UTF8, 0, pChar, -1, NULL, 0);
	if (nSize == 0) {
		RETURN_FALSE;
	}

	wchar_t* pWide = new wchar_t[nSize];
	DEBUG_VALID(pWide);

	::MultiByteToWideChar(CP_UTF8, 0, pChar, -1, pWide, nSize);
	// UTF-8 BOM or UTF-8
	result = (pWide[0] == 0xFEFF ? pWide + 1 : pWide);

	REMOVE_ARRAY(pChar);
	REMOVE_ARRAY(pWide);
	::FreeResource(hGlobal);

	return (result.IsEmpty() == false);
}

#pragma endregion //:REGION

#pragma region Json

#include "Facility.AppResources.h"

CString Facility::GetDescription(Json::Object& source)
{
	Json::Value* pValue = source.FindValue("desc");
	if (pValue == nullptr) {
		return L"";
	}

	if (pValue->IsInteger()) {
		CStringA code = (CStringA)pValue->ToString();
		CString desc = TheAppResources.GetDescription().GetString(code);
		desc.Replace(L"<br>", L"\n");

		return Local(desc);
	}
	else {
		return Local(pValue->AsString());
	}
}



UINT Facility::GetId(Json::Object& source)
{
	return source.GetInteger("id");
}



Json::Array* Facility::GetItems(Json::Object& source)
{
	if (source.FindValue("items")) {
		return &source.GetValue("items").AsArray();
	}

	return nullptr;
}



void Facility::GetItems(Json::Object& source, std::vector<CString>& list)
{
	Json::Value& value = source.GetValue("items");
	if (value.IsArray() == false) {
		return;
	}

	for (Json::Value* pValue : value.AsArray().GetBuffer()) {
		list.push_back(Facility::Local(pValue->AsString()));
	}
}



CString Facility::GetName(Json::Object& source)
{
	return source.GetString("name");
}



int Facility::GetPivot(Json::Object& source)
{
	return source.GetInteger("pivot");
}



CSize Facility::GetSize(Json::Object& source)
{
	return CSize(source.GetInteger("cx"), source.GetInteger("cy"));
}



CRect Facility::GetRect(Json::Object& source)
{
	CRect rect;
	Json::Value* pValue = source.FindValue("rect");

	if (pValue != nullptr && pValue->IsArray()) {
		Json::Array& rectValue = pValue->AsArray();

		rect.left = rectValue[0]->ToInteger();
		rect.top = rectValue[1]->ToInteger();
		rect.right = rectValue[2]->ToInteger();
		rect.bottom = rectValue[3]->ToInteger();
	}

	return rect;
}



CString Facility::GetTitle(Json::Object& source)
{
	return Local(source.GetString("title"));
}



CString Facility::GetType(Json::Object& source)
{
	return source.GetString("type");
}



Json::Object& Facility::SetData(Json::Object& target, UINT id, const CString& title, int ePivot, const CRect& rect)
{
	if (target.FindValue("id") == nullptr && id > 0) {
		target.SetInteger("id", id);
	}
	if (target.FindValue("title") == nullptr && title.IsEmpty() == false) {
		target.SetString("title", title);
	}
	if (target.FindValue("pivot") == nullptr && (Control::EPivot)ePivot != Control::EPivot::Unknown) {
		target.SetInteger("pivot", (int)ePivot);
	}
	if (target.FindValue("rect") == nullptr && (rect.Width() > 0 || rect.Height() > 0)) {
		Json::Array* pRect = new Json::Array();
		pRect->AddInteger(rect.left);
		pRect->AddInteger(rect.top);
		pRect->AddInteger(rect.right);
		pRect->AddInteger(rect.bottom);

		target.SetArray("rect", pRect);
	}

	return target;
}

#include <WStr.h>

void Facility::SetValue(Json::Value& target, CBCGPProp& source)
{
	if (source.GetOptionCount() > 0) {
		target.SetInteger(source.GetSelectedOption());
	}
	else if (source.IsGroupWithCheckBox()) {
		ASSERT(target.GetType() == Json::EValueType::Object);
		target.AsObject().SetBoolean("checked", (bool)source.IsGroupChecked());
	}
	else if (dynamic_cast<CBCGPColorProp*>(&source) != nullptr) {
		//:WARNING - not GetValue()
		COLORREF color = ((CBCGPColorProp*)&source)->GetColor();
		target.SetString(Json::Helper::ToString(color));
	}
	else if (dynamic_cast<CBCGPFileProp*>(&source) != nullptr) {
		CString value = source.GetValue();
		value.Replace(L"\\", L"/");
		target.SetString(value);
	}
	else if (dynamic_cast<CBCGPFontProp*>(&source) != nullptr) {
		CString value = source.GetValue();
		WStr::RemoveFrom(value, L'(');
		target.SetString(value);
	}
	else {
		switch (target.GetType()) {
		case Json::EValueType::Boolean:	target.SetBoolean(source.GetValue());	break;
		case Json::EValueType::Int:		target.SetInteger(source.GetValue());	break;
		case Json::EValueType::Uint:	target.SetInteger(source.GetValue());	break;
		case Json::EValueType::Real:	target.SetReal(source.GetValue());		break;
		case Json::EValueType::String:	target.SetString(source.GetValue());	break;

		default:
			DEBUG_STOP;
		}
	}
}

void Facility::SetValue(CBCGPProp& target, Json::Value& source)
{
	if (target.GetOptionCount() > 0) {
		target.SelectOption(source.ToInteger());
	}
	else if (dynamic_cast<CBCGPColorProp*>(&target) != nullptr) {
		COLORREF color = Json::Helper::ToColor(source.ToString());
		//:WARNING - not SetValue()
		((CBCGPColorProp*)&target)->SetColor(color);
	}
	else if (dynamic_cast<CBCGPFileProp*>(&target) != nullptr) {
		CString value = source.ToString();
		value.Replace(L"/", L"\\");
		target.SetValue((LPCTSTR)value);
	}
	else {
		switch (source.GetType()) {
		case Json::EValueType::Boolean:	target.SetValue(source.ToBoolean());			break;
		case Json::EValueType::Int:		target.SetValue(source.ToInteger());			break;
		case Json::EValueType::Uint:	target.SetValue(source.ToInteger());			break;
		case Json::EValueType::Real:	target.SetValue(source.ToReal());				break;
		case Json::EValueType::String:	target.SetValue((LPCTSTR)source.ToString());	break;

		default:
			DEBUG_STOP;
		}
	}
}



void Facility::SetValueByPath(Json::Object& object, CString& path, CBCGPProp& value)
{
	Json::Value* pFound = Json::Helper::FindValueByPath(object, (CStringA)path);
	if (pFound != nullptr) {
		SetValue(*pFound, value);
	}
	else {
		DEBUG_STOP;
	}
}

#pragma endregion //:REGION

#undef PRESET
