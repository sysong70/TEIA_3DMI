#include "stdafx.h"

#include "3DF.Facility.KernelSetting.h"

USING_3DF_NAMESPACE

Facility::KernelSetting::KernelSetting()
	: Base(L"Kernel")
{
}

Facility::KernelSetting::~KernelSetting()
{
}



Json::Object* Facility::KernelSetting::Get()
{
	Json::Object* pcData = new Json::Object();

	return pcData;
}

bool Facility::KernelSetting::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		return false;
	}

	return true;
}

#pragma region General

Json::Object* Facility::KernelSetting::GENERAL::Get()
{
	Json::Object* pcData = new Json::Object();

	GetObjectValue(Display);
	GetObjectValue(Transparency);
	GetObjectValue(Rendering);
	GetObjectValue(Etc);

	return pcData;
}

bool Facility::KernelSetting::GENERAL::Set(Json::Object* pcData)
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



Json::Object* Facility::KernelSetting::GENERAL::DISPLAY::Get()
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

bool Facility::KernelSetting::GENERAL::DISPLAY::Set(Json::Object* pcData)
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



Json::Object* Facility::KernelSetting::GENERAL::TRANSPARENCY::Get()
{
	Json::Object* pcData = new Json::Object();

	GetStringValue(Style);
	GetStringValue(Sorting);
	GetStringValue(DepthPeelingLayers);
	GetBooleanValue(PixelOIT);
	GetBooleanValue(DepthWriting);

	return pcData;
}

bool Facility::KernelSetting::GENERAL::TRANSPARENCY::Set(Json::Object* pcData)
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



Json::Object* Facility::KernelSetting::GENERAL::RENDERING::Get()
{
	Json::Object* pcData = new Json::Object();

	GetBooleanValue(DisplayAxisTriad);

	return pcData;
}

bool Facility::KernelSetting::GENERAL::RENDERING::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetBooleanValue(DisplayAxisTriad);

	return true;
}



Json::Object* Facility::KernelSetting::GENERAL::ETC::Get()
{
	Json::Object* pcData = new Json::Object();

	GetBooleanValue(BackplaneCulling);

	return pcData;
}

bool Facility::KernelSetting::GENERAL::ETC::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetBooleanValue(BackplaneCulling);

	return true;
}

#pragma endregion //:REGION

#pragma region Performance

Json::Object* Facility::KernelSetting::PERFORMANCE::Get()
{
	Json::Object* pcData = new Json::Object();

	GetObjectValue(Optimization);
	GetObjectValue(FramerateOptimization);

	return pcData;
}

bool Facility::KernelSetting::PERFORMANCE::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetObjectValue(Optimization);
	SetObjectValue(FramerateOptimization);

	return true;
}



Json::Object* Facility::KernelSetting::PERFORMANCE::OPTIMIZATION::Get()
{
	Json::Object* pcData = new Json::Object();

	GetEnumValue(HiddenLineMode);
	GetBooleanValue(StaticModel);
	GetBooleanValue(LMVModel);
	GetBooleanValue(OcclusionCulling);
	GetIntegerValue(OcclusionThreshold);

	return pcData;
}

bool Facility::KernelSetting::PERFORMANCE::OPTIMIZATION::Set(Json::Object* pcData)
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



Json::Object* Facility::KernelSetting::PERFORMANCE::FRAMERATE_OPTIMIZATION::Get()
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

bool Facility::KernelSetting::PERFORMANCE::FRAMERATE_OPTIMIZATION::Set(Json::Object* pcData)
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

Json::Object* Facility::KernelSetting::INTERACTION::Get()
{
	Json::Object* pcData = new Json::Object();

	GetObjectValue(Animation);
	GetObjectValue(GeometryManipulation);

	return pcData;
}

bool Facility::KernelSetting::INTERACTION::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetObjectValue(Animation);
	SetObjectValue(GeometryManipulation);

	return true;
}



Json::Object* Facility::KernelSetting::INTERACTION::GEOMETRY_MANIPULATION::Get()
{
	Json::Object* pcData = new Json::Object();

	GetBooleanValue(Spriting);
	GetBooleanValue(UpdateCutGeometry);
	GetBooleanValue(UpdateShadows);

	return pcData;
}

bool Facility::KernelSetting::INTERACTION::GEOMETRY_MANIPULATION::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetBooleanValue(Spriting);
	SetBooleanValue(UpdateCutGeometry);
	SetBooleanValue(UpdateShadows);

	return true;
}



Json::Object* Facility::KernelSetting::INTERACTION::ANIMATION::Get()
{
	Json::Object* pcData = new Json::Object();

	GetBooleanValue(UpdateCamera);

	return pcData;
}

bool Facility::KernelSetting::INTERACTION::ANIMATION::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetBooleanValue(UpdateCamera);

	return true;
}

#pragma endregion //:REGION

#pragma region Appearance

Json::Object* Facility::KernelSetting::APPEARANCE::Get()
{
	Json::Object* pcData = new Json::Object();

	GetObjectValue(AntiAliasing);
	GetObjectValue(BackgroundColor);

	return pcData;
}

bool Facility::KernelSetting::APPEARANCE::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetObjectValue(AntiAliasing);
	SetObjectValue(BackgroundColor);

	return true;
}



Json::Object* Facility::KernelSetting::APPEARANCE::ANTIALIASING::Get()
{
	Json::Object* pcData = new Json::Object();

	GetBooleanValue(Use);
	GetBooleanValue(Line);
	GetBooleanValue(Text);
	GetIntegerValue(Level);

	return pcData;
}

bool Facility::KernelSetting::APPEARANCE::ANTIALIASING::Set(Json::Object* pcData)
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



Json::Object* Facility::KernelSetting::APPEARANCE::BACKGROUND_COLOR::Get()
{
	Json::Object* pcData = new Json::Object();

	GetIntegerValue(Top);
	GetIntegerValue(Bottom);

	return pcData;
}

bool Facility::KernelSetting::APPEARANCE::BACKGROUND_COLOR::Set(Json::Object* pcData)
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

Json::Object* Facility::KernelSetting::SELECTIION::Get()
{
	Json::Object* pcData = new Json::Object();

	GetObjectValue(Behavior);
	GetObjectValue(Highlight);

	return pcData;
}

bool Facility::KernelSetting::SELECTIION::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetObjectValue(Behavior);
	SetObjectValue(Highlight);

	return true;
}



Json::Object* Facility::KernelSetting::SELECTIION::BEHAVIOR::Get()
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

bool Facility::KernelSetting::SELECTIION::BEHAVIOR::Set(Json::Object* pcData)
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



Json::Object* Facility::KernelSetting::SELECTIION::HIGHLIGHT::Get()
{
	Json::Object* pcData = new Json::Object();

	GetStringValue(QuickMovesType);

	return pcData;
}

bool Facility::KernelSetting::SELECTIION::HIGHLIGHT::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetStringValue(QuickMovesType);

	return true;
}

#pragma endregion //:REGION

#pragma region Lighting

Json::Object* Facility::KernelSetting::LIGHTING::Get()
{
	Json::Object* pcData = new Json::Object();

	GetObjectValue(Bloom);
	GetObjectValue(Light);

	return pcData;
}

bool Facility::KernelSetting::LIGHTING::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		RETURN_FALSE;
	}

	SetObjectValue(Bloom);
	SetObjectValue(Light);

	return true;
}



Json::Object* Facility::KernelSetting::LIGHTING::BLOOM::Get()
{
	Json::Object* pcData = new Json::Object();

	GetBooleanValue(Use);
	GetRealValue(Strength);
	GetIntegerValue(Blur);
	GetEnumValue(Shape);

	return pcData;
}

bool Facility::KernelSetting::LIGHTING::BLOOM::Set(Json::Object* pcData)
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



Json::Object* Facility::KernelSetting::LIGHTING::LIGHT::Get()
{
	Json::Object* pcData = new Json::Object();

	GetBooleanValue(FollowsCamera);
	GetBooleanValue(Scaling);
	GetIntegerValue(ScaleFactor);

	return pcData;
}

bool Facility::KernelSetting::LIGHTING::LIGHT::Set(Json::Object* pcData)
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
