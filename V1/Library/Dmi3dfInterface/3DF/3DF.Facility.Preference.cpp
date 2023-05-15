#include "stdafx.h"

#include "3DF.Facility.Preference.h"

USING_3DF_NAMESPACE

#define GetBooleanValue(name)	pcData->SetBoolean(#name, name)
#define GetEnumValue(name)		pcData->SetInteger(#name, (int)name)
#define GetIntegerValue(name)	pcData->SetInteger(#name, name)
#define GetObjectValue(name)	pcData->SetObject(#name, name.Get())
#define GetRealValue(name)		pcData->SetReal(#name, name)
#define GetStringValue(name)	pcData->SetString(#name, name)

#define SetBooleanValue(name)		name = pcData->GetBoolean(#name, name)
#define SetEnumValue(name, type)	name = (type)pcData->GetInteger(#name, (int)name)
#define SetIntegerValue(name)		name = pcData->GetInteger(#name, name)
#define SetObjectValue(name)		name.Set(&pcData->GetAt(#name))
#define SetRealValue(name)			name = pcData->GetReal(#name, name)
#define SetStringValue(name)		name = pcData->GetString(#name, name)



Facility::Preference::Preference()
	: Base(L"Preference")
{
}

Facility::Preference::~Preference()
{
}



Json::Object* Facility::Preference::Get()
{
	Json::Object* pcData = new Json::Object();

	return pcData;
}

bool Facility::Preference::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		return false;
	}

	return true;
}

#pragma region General

Json::Object* Facility::Preference::GENERAL::Get()
{
	Json::Object* pcData = new Json::Object();

	GetObjectValue(Display);
	GetObjectValue(Transparency);
	GetObjectValue(Rendering);
	GetObjectValue(Etc);

	return pcData;
}

bool Facility::Preference::GENERAL::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetObjectValue(Display);
	SetObjectValue(Transparency);
	SetObjectValue(Rendering);
	SetObjectValue(Etc);

	return true;
}



Json::Object* Facility::Preference::GENERAL::DISPLAY::Get()
{
	Json::Object* pcData = new Json::Object();

	GetStringValue(Driver);
	GetStringValue(Gpu);
	GetBooleanValue(DriverForceSoftware);
	GetBooleanValue(DriverDisplayStats);
	GetBooleanValue(DoubleBuffer);
	GetBooleanValue(StereoMode);

	return pcData;
}

bool Facility::Preference::GENERAL::DISPLAY::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetStringValue(Driver);
	SetStringValue(Gpu);
	SetBooleanValue(DriverForceSoftware);
	SetBooleanValue(DriverDisplayStats);
	SetBooleanValue(DoubleBuffer);
	SetBooleanValue(StereoMode);

	return true;
}



Json::Object* Facility::Preference::GENERAL::TRANSPARENCY::Get()
{
	Json::Object* pcData = new Json::Object();

	GetStringValue(Style);
	GetStringValue(Sorting);
	GetStringValue(DepthPeelingLayers);
	GetBooleanValue(PixelOIT);
	GetBooleanValue(DepthWriting);

	return pcData;
}

bool Facility::Preference::GENERAL::TRANSPARENCY::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetStringValue(Style);
	SetStringValue(Sorting);
	SetStringValue(DepthPeelingLayers);
	GetBooleanValue(PixelOIT);
	GetBooleanValue(DepthWriting);

	return true;
}



Json::Object* Facility::Preference::GENERAL::RENDERING::Get()
{
	Json::Object* pcData = new Json::Object();

	GetBooleanValue(DisplayAxisTriad);

	return pcData;
}

bool Facility::Preference::GENERAL::RENDERING::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetBooleanValue(DisplayAxisTriad);

	return true;
}



Json::Object* Facility::Preference::GENERAL::ETC::Get()
{
	Json::Object* pcData = new Json::Object();

	GetBooleanValue(BackplaneCulling);

	return pcData;
}

bool Facility::Preference::GENERAL::ETC::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetBooleanValue(BackplaneCulling);

	return true;
}

#pragma endregion //:REGION

#pragma region Performance

Json::Object* Facility::Preference::PERFORMANCE::Get()
{
	Json::Object* pcData = new Json::Object();

	GetObjectValue(Optimization);
	GetObjectValue(FramerateOptimization);

	return pcData;
}

bool Facility::Preference::PERFORMANCE::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetObjectValue(Optimization);
	SetObjectValue(FramerateOptimization);

	return true;
}



Json::Object* Facility::Preference::PERFORMANCE::OPTIMIZATION::Get()
{
	Json::Object* pcData = new Json::Object();

	GetEnumValue(HiddenLineMode);
	GetBooleanValue(StaticModel);
	GetBooleanValue(LMVModel);
	GetBooleanValue(OcclusionCulling);
	GetIntegerValue(OcclusionThreshold);

	return pcData;
}

bool Facility::Preference::PERFORMANCE::OPTIMIZATION::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetEnumValue(HiddenLineMode, HLRMode);
	SetBooleanValue(StaticModel);
	SetBooleanValue(LMVModel);
	SetBooleanValue(OcclusionCulling);
	SetIntegerValue(OcclusionThreshold);

	return true;
}



Json::Object* Facility::Preference::PERFORMANCE::FRAMERATE_OPTIMIZATION::Get()
{
	Json::Object* pcData = new Json::Object();

	GetBooleanValue(UseFramerate);
	GetEnumValue(CurrentFramerateMode);
	GetIntegerValue(MaxThreshold);
	GetIntegerValue(UseLods);
	GetIntegerValue(DetailSteps);
	GetIntegerValue(HardCutoff);
	GetIntegerValue(CullingThresholdSet);
	GetIntegerValue(CullingThreshold);

	return pcData;
}

bool Facility::Preference::PERFORMANCE::FRAMERATE_OPTIMIZATION::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetBooleanValue(UseFramerate);
	SetEnumValue(CurrentFramerateMode, FramerateMode);
	SetIntegerValue(MaxThreshold);
	SetIntegerValue(UseLods);
	SetIntegerValue(DetailSteps);
	SetIntegerValue(HardCutoff);
	SetIntegerValue(CullingThresholdSet);
	SetIntegerValue(CullingThreshold);

	return true;
}

#pragma endregion //:REGION

#pragma region Interaction

Json::Object* Facility::Preference::INTERACTION::Get()
{
	Json::Object* pcData = new Json::Object();

	GetObjectValue(Animation);
	GetObjectValue(GeometryManipulation);

	return pcData;
}

bool Facility::Preference::INTERACTION::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetObjectValue(Animation);
	SetObjectValue(GeometryManipulation);

	return true;
}



Json::Object* Facility::Preference::INTERACTION::GEOMETRY_MANIPULATION::Get()
{
	Json::Object* pcData = new Json::Object();

	GetBooleanValue(Spriting);
	GetBooleanValue(UpdateCutGeometry);
	GetBooleanValue(UpdateShadows);

	return pcData;
}

bool Facility::Preference::INTERACTION::GEOMETRY_MANIPULATION::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetBooleanValue(Spriting);
	SetBooleanValue(UpdateCutGeometry);
	SetBooleanValue(UpdateShadows);

	return true;
}



Json::Object* Facility::Preference::INTERACTION::ANIMATION::Get()
{
	Json::Object* pcData = new Json::Object();

	GetBooleanValue(UpdateCamera);

	return pcData;
}

bool Facility::Preference::INTERACTION::ANIMATION::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetBooleanValue(UpdateCamera);

	return true;
}

#pragma endregion //:REGION

#pragma region Appearance

Json::Object* Facility::Preference::APPEARANCE::Get()
{
	Json::Object* pcData = new Json::Object();

	GetObjectValue(AntiAliasing);
	GetObjectValue(BackgroundColor);

	return pcData;
}

bool Facility::Preference::APPEARANCE::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetObjectValue(AntiAliasing);
	SetObjectValue(BackgroundColor);

	return true;
}



Json::Object* Facility::Preference::APPEARANCE::ANTIALIASING::Get()
{
	Json::Object* pcData = new Json::Object();

	GetBooleanValue(Use);
	GetBooleanValue(Line);
	GetBooleanValue(Text);
	GetIntegerValue(Level);

	return pcData;
}

bool Facility::Preference::APPEARANCE::ANTIALIASING::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetBooleanValue(Use);
	SetBooleanValue(Line);
	SetBooleanValue(Text);
	SetIntegerValue(Level);

	return true;
}



Json::Object* Facility::Preference::APPEARANCE::BACKGROUND_COLOR::Get()
{
	Json::Object* pcData = new Json::Object();

	GetIntegerValue(Top);
	GetIntegerValue(Bottom);

	return pcData;
}

bool Facility::Preference::APPEARANCE::BACKGROUND_COLOR::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetIntegerValue(Top);
	SetIntegerValue(Bottom);

	return true;
}

#pragma endregion //:REGION

#pragma region Selection

Json::Object* Facility::Preference::SELECTIION::Get()
{
	Json::Object* pcData = new Json::Object();

	GetObjectValue(Behavior);
	GetObjectValue(Highlight);

	return pcData;
}

bool Facility::Preference::SELECTIION::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetObjectValue(Behavior);
	SetObjectValue(Highlight);

	return true;
}



Json::Object* Facility::Preference::SELECTIION::BEHAVIOR::Get()
{
	Json::Object* pcData = new Json::Object();

	GetBooleanValue(VisibilitySelection);
	GetBooleanValue(DynamicHighlighting);
	GetBooleanValue(DetailSelection);
	GetIntegerValue(RelatedSelectionLimit);
	GetBooleanValue(UseSelectBox);
	GetBooleanValue(RespectCulling);

	return pcData;
}

bool Facility::Preference::SELECTIION::BEHAVIOR::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetBooleanValue(VisibilitySelection);
	SetBooleanValue(DynamicHighlighting);
	SetBooleanValue(DetailSelection);
	SetIntegerValue(RelatedSelectionLimit);
	SetBooleanValue(UseSelectBox);
	SetBooleanValue(RespectCulling);

	return true;
}



Json::Object* Facility::Preference::SELECTIION::HIGHLIGHT::Get()
{
	Json::Object* pcData = new Json::Object();

	GetStringValue(QuickMovesType);

	return pcData;
}

bool Facility::Preference::SELECTIION::HIGHLIGHT::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetStringValue(QuickMovesType);

	return true;
}

#pragma endregion //:REGION

#pragma region Lighting

Json::Object* Facility::Preference::LIGHTING::Get()
{
	Json::Object* pcData = new Json::Object();

	GetObjectValue(Bloom);
	GetObjectValue(Light);

	return pcData;
}

bool Facility::Preference::LIGHTING::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetObjectValue(Bloom);
	SetObjectValue(Light);

	return true;
}



Json::Object* Facility::Preference::LIGHTING::BLOOM::Get()
{
	Json::Object* pcData = new Json::Object();

	GetBooleanValue(Use);
	GetRealValue(Strength);
	GetIntegerValue(Blur);
	GetEnumValue(Shape);

	return pcData;
}

bool Facility::Preference::LIGHTING::BLOOM::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetBooleanValue(Use);
	SetRealValue(Strength);
	SetIntegerValue(Blur);
	SetEnumValue(Shape, HBloomShape);

	return true;
}



Json::Object* Facility::Preference::LIGHTING::LIGHT::Get()
{
	Json::Object* pcData = new Json::Object();

	GetBooleanValue(FollowsCamera);
	GetBooleanValue(Scaling);
	GetIntegerValue(ScaleFactor);

	return pcData;
}

bool Facility::Preference::LIGHTING::LIGHT::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetBooleanValue(FollowsCamera);
	SetBooleanValue(Scaling);
	SetIntegerValue(ScaleFactor);

	return true;
}

#pragma endregion //:REGION

#undef GetBooleanValue
#undef GetEnumValue
#undef GetIntegerValue
#undef GetObjectValue
#undef GetRealValue
#undef GetStringValue

#undef SetBooleanValue
#undef SetEnumValue
#undef SetIntegerValue
#undef SetObjectValue
#undef SetRealValue
#undef SetStringValue
