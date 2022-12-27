#include "stdafx.h"
#include "Facility.Preference.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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

	SetLanguage((ELanguage)pData->GetInteger("Language"));

	return true;
}
