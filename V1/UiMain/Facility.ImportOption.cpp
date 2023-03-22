#include "stdafx.h"
#include "Facility.ImportOption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



Facility::ImportOption::ImportOption(CString name)
	: Base(name)
{
}



Facility::ImportOption::~ImportOption()
{
}



Json::Object* Facility::ImportOption::Get()
{
	Json::Object* pData = new Json::Object();

	pData->SetObject("General", General.Get());
	pData->SetObject("Tessellation", Tessellation.Get());

	return pData;
}



bool Facility::ImportOption::Set(Json::Object* pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	bool bSuccess = true;
	bSuccess &= General.Set(pData->GetValue("General").ToObject());
	bSuccess &= Tessellation.Set(pData->GetValue("Tessellation").ToObject());

	return bSuccess;
}



Json::Object* Facility::ImportOption::GENERAL::Get()
{
	Json::Object* pData = new Json::Object();

	pData->SetInteger("ReadingMode", (int)ReadingMode);
	pData->SetBoolean("Solids", Solids);
	pData->SetBoolean("Surfaces", Surfaces);
	pData->SetBoolean("Wireframes", Wireframes);
	pData->SetBoolean("Attributes", Attributes);
	pData->SetBoolean("HiddenObjects", HiddenObjects);
	pData->SetBoolean("References", References);
	pData->SetBoolean("ActiveFilter", ActiveFilter);
	pData->SetBoolean("SewModel", SewModel);
	pData->SetReal("SewingTolerance", SewingTolerance);
	pData->SetInteger("ShellOrientation", ShellOrientation);
	pData->SetBoolean("LoadingMode", (int)LoadingMode);

	return pData;
}



bool Facility::ImportOption::GENERAL::Set(Json::Object* pData)
{
	ReadingMode = (EReadingMode)pData->GetInteger("ReadingMode");
	Solids = pData->GetBoolean("Solids");
	Surfaces = pData->GetBoolean("Surfaces");
	Wireframes = pData->GetBoolean("Wireframes");
	Attributes = pData->GetBoolean("Attributes");
	HiddenObjects = pData->GetBoolean("HiddenObjects");
	References = pData->GetBoolean("References");
	ActiveFilter = pData->GetBoolean("ActiveFilter");
	SewModel = pData->GetBoolean("SewModel");
	SewingTolerance = pData->GetReal("SewingTolerance");
	ShellOrientation = pData->GetBoolean("ShellOrientation");
	LoadingMode = (ELoadingMode)pData->GetBoolean("LoadingMode");

	return true;
}



Json::Object* Facility::ImportOption::TESSELLATION::Get()
{
	Json::Object* pData = new Json::Object();

	pData->SetInteger("TessLevel", (int)TessLevel);
	pData->SetInteger("ChordLimitType", (int)ChordLimitType);
	pData->SetInteger("ChordLimit", ChordLimit);
	pData->SetInteger("AngleTolerance", AngleTolerance);
	pData->SetBoolean("PreserveUV", PreserveUV);
	pData->SetInteger("MaxEdgeLength", MaxEdgeLength);
	pData->SetBoolean("AccurateTess", AccurateTess);
	pData->SetBoolean("GridAlignedTess", GridAlignedTess);
	pData->SetReal("MaxStitchLength", MaxStitchLength);
	pData->SetBoolean("SurfaceCurvatures", SurfaceCurvatures);

	return pData;
}



bool Facility::ImportOption::TESSELLATION::Set(Json::Object* pData)
{
	TessLevel = (ETessLevel)pData->GetInteger("TessLevel");
	ChordLimitType = (EChordLimit)pData->GetInteger("ChordLimitType");
	ChordLimit = pData->GetInteger("ChordLimit");
	AngleTolerance = pData->GetInteger("AngleTolerance");
	PreserveUV = pData->GetBoolean("PreserveUV");
	MaxEdgeLength = pData->GetInteger("MaxEdgeLength");
	AccurateTess = pData->GetBoolean("AccurateTess");
	GridAlignedTess = pData->GetBoolean("GridAlignedTess");
	MaxStitchLength = pData->GetReal("MaxStitchLength");
	SurfaceCurvatures = pData->GetBoolean("SurfaceCurvatures");

	return true;
}
