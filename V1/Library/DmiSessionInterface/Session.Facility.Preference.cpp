#include "stdafx.h"
#include "Session.Facility.Preference.h"
#include "WStr.h"

using namespace SESSION;

Json::Object* Facility::Preference::Get()
{
	return nullptr;
}



bool Facility::Preference::Set(Json::Object* pData)
{
	SetObjectValue(Environment);
	SetObjectValue(Views);

	return true;
}



const char* Facility::Preference::ENVIRONMENT::GENERAL::FontName()
{
	switch (Language) {
	case ELanguage::Korean: return "malgun gothic";
	case ELanguage::English:
	default:
		return "segoe ui";
	}
}




CString Facility::Preference::ENVIRONMENT::GENERAL::Local(CString source)
{
	WStringArray result;
	if (WStr::Split(source, L'|', result)) {
		return result[(int)Language];
	}
	else {
		return source;
	}
}



bool Facility::Preference::ENVIRONMENT::Set(Json::Object* pData)
{
	SetObjectValue(General);
	SetObjectValue(Mouse);

	return true;
}

bool Facility::Preference::ENVIRONMENT::GENERAL::Set(Json::Object* pData)
{
	SetEnumValue(Language, ELanguage);

	return true;
}

bool Facility::Preference::ENVIRONMENT::MOUSE::Set(Json::Object* pData)
{
	SetBooleanValue(SwapPanAndRotate);
	SetBooleanValue(ReverseWheelDirection);

	return true;
}



bool Facility::Preference::VIEWS::Set(Json::Object* pData)
{
	SetObjectValue(Colors);
	SetObjectValue(NavCube);

	return true;
}

bool Facility::Preference::VIEWS::COLORS::Set(Json::Object* pData)
{
	SetIntegerValue(BackTop);
	SetIntegerValue(BackBottom);

	return true;
}

bool Facility::Preference::VIEWS::NAVIGATION_CUBE::Set(Json::Object* pData)
{
	SetIntegerValue(Size);
	SetBooleanValue(ShowAxis);
	SetBooleanValue(ShowCube);

	return true;
}
