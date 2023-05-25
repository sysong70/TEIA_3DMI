#include "stdafx.h"
#include "Facility.GuiSetting.h"
#include "WStr.h"



const char* Facility::GuiSetting::GENERAL::FontName()
{
	switch (Language) {
	case ELanguage::Korean: return "malgun gothic";
	case ELanguage::English:
	default:
		return "segoe ui";
	}
}



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
