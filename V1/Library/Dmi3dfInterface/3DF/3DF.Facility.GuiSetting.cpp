#include "stdafx.h"

#include "3DF.Facility.GuiSetting.h"
#include "WStr.h"

USING_3DF_NAMESPACE



const char* Facility::GuiSetting::GENERAL::FontName()
{
	switch (Language) {
	case ELanguage::Korean: return "malgun gothic";
	case ELanguage::English:
	default:
		return "segoe ui";
	}
}

USING_3DF_NAMESPACE



CString Facility::GuiSetting::GENERAL::Local(CString source)
{
	WStringArray result;
	if (WStr::Split(source, L'|', result)) {
		return result[(int)Language];
	}
	else {
		return source;
	}
}
