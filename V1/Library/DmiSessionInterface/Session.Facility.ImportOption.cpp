#include "stdafx.h"
#include "Session.Facility.ImportOption.h"

using namespace SESSION;

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

	GetObjectValue(General);
	GetObjectValue(Tessellation);

	return pData;
}



bool Facility::ImportOption::Set(Json::Object* pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	SetObjectValue(General);
	SetObjectValue(Tessellation);

	return true;
}



Json::Object* Facility::ImportOption::GENERAL::Get()
{
	Json::Object* pData = new Json::Object();

	GetEnumValue(ReadingMode);
	GetBooleanValue(Solids);
	GetBooleanValue(Surfaces);
	GetBooleanValue(Wireframes);
	GetBooleanValue(Attributes);
	GetBooleanValue(HiddenObjects);
	GetBooleanValue(References);
	GetBooleanValue(ActiveFilter);
	GetBooleanValue(SewModel);
	GetRealValue(SewingTolerance);
	GetIntegerValue(ShellOrientation);
	GetEnumValue(LoadingMode);

	return pData;
}



bool Facility::ImportOption::GENERAL::Set(Json::Object* pData)
{
	SetEnumValue(ReadingMode, EReadingMode);
	SetBooleanValue(Solids);
	SetBooleanValue(Surfaces);
	SetBooleanValue(Wireframes);
	SetBooleanValue(Attributes);
	SetBooleanValue(HiddenObjects);
	SetBooleanValue(References);
	SetBooleanValue(ActiveFilter);
	SetBooleanValue(SewModel);
	SetRealValue(SewingTolerance);
	SetBooleanValue(ShellOrientation);
	SetEnumValue(LoadingMode, ELoadingMode);

	return true;
}



Json::Object* Facility::ImportOption::TESSELLATION::Get()
{
	Json::Object* pData = new Json::Object();

	GetEnumValue(TessLevel);
	GetEnumValue(ChordLimitType);
	GetIntegerValue(ChordLimit);
	GetIntegerValue(AngleTolerance);
	GetBooleanValue(PreserveUV);
	GetIntegerValue(MaxEdgeLength);
	GetBooleanValue(AccurateTess);
	GetBooleanValue(GridAlignedTess);
	GetRealValue(MaxStitchLength);
	GetBooleanValue(SurfaceCurvatures);

	return pData;
}



bool Facility::ImportOption::TESSELLATION::Set(Json::Object* pData)
{
	SetEnumValue(TessLevel, ETessLevel);
	SetEnumValue(ChordLimitType, EChordLimit);
	SetIntegerValue(ChordLimit);
	SetIntegerValue(AngleTolerance);
	SetBooleanValue(PreserveUV);
	SetIntegerValue(MaxEdgeLength);
	SetBooleanValue(AccurateTess);
	SetBooleanValue(GridAlignedTess);
	SetRealValue(MaxStitchLength);
	SetBooleanValue(SurfaceCurvatures);

	return true;
}
