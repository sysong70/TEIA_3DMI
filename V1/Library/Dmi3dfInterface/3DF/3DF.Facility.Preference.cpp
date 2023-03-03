#include "stdafx.h"

#include "3DF.Facility.Preference.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

USING_3DF_NAMESPACE

Facility::Preference::Preference()
	: Base(L"Preference")
{
}



Facility::Preference::~Preference()
{
}



Json::Object* Facility::Preference::Get()
{
	Json::Object * pData = new Json::Object();

	pData->SetObject("Transparency", Transparency.Get());

	return pData;
}

bool Facility::Preference::Set(Json::Object* pData)
{
	if (pData == nullptr) {
		return false;
	}

	//SetLanguage((ELanguage)pData->GetInteger("Language"));

	return true;
}

Json::Object * Facility::Preference::TRANSPARENCY::Get()
{
	Json::Object * pData = new Json::Object();

	pData->SetString("Style", Style);
	pData->SetString("Sorting", Sorting);
	pData->SetString("DepthPeelingLayers", DepthPeelingLayers);
	pData->SetBoolean("PixelOIT", PixelOIT);
	pData->SetBoolean("DepthWriting", DepthWriting);

	return pData;
}

bool Facility::Preference::TRANSPARENCY::Set(Json::Object * pData)
{
	Style = pData->GetString("Style");
	Sorting = pData->GetString("Sorting");
	DepthPeelingLayers = pData->GetString("DepthPeelingLayers");
	PixelOIT = pData->GetBoolean("PixelOIT");
	DepthWriting = pData->GetBoolean("DepthWriting");

	return true;
}