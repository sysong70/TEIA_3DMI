#include "stdafx.h"
#include "Facility.Preference.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



Facility::Preference::Preference()
	: Base(L"Preference")
{
}



Facility::Preference::~Preference()
{
}



Json::Object* Facility::Preference::Get()
{
	RETURN_NULL;
}



bool Facility::Preference::Set(Json::Object* pData)
{
	if (pData == nullptr) {
		return false;
	}

	SetLanguage((UiLanguage)pData->GetInteger("Language"));

	return true;
}
