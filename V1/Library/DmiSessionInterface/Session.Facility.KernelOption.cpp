#include "stdafx.h"

#include "Session.Facility.KernelOption.h"

using namespace SESSION;

Facility::KernelOption::KernelOption()
	: Base(L"Kernel")
{
}

Facility::KernelOption::~KernelOption()
{
}



Json::Object* Facility::KernelOption::Get()
{
	Json::Object* pData = new Json::Object();

	return pData;
}

bool Facility::KernelOption::Set(Json::Object* pData)
{
	if (pData == nullptr) {
		return false;
	}

	return true;
}

#pragma region General

Json::Object* Facility::KernelOption::GENERAL::Get()
{
	Json::Object* pData = new Json::Object();

	GetObjectValue(Display);
	GetObjectValue(Transparency);
	GetObjectValue(Rendering);
	GetObjectValue(Etc);

	return pData;
}

bool Facility::KernelOption::GENERAL::Set(Json::Object* pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	SetObjectValue(Display);
	SetObjectValue(Transparency);
	SetObjectValue(Rendering);
	SetObjectValue(Etc);

	return true;
}



Json::Object* Facility::KernelOption::GENERAL::DISPLAY::Get()
{
	Json::Object* pData = new Json::Object();

	GetStringValue(Driver);
	GetStringValue(Gpu);
	GetBooleanValue(DriverForceSoftware);
	GetBooleanValue(DriverDisplayStats);
	GetBooleanValue(DoubleBuffer);
	GetBooleanValue(StereoMode);

	return pData;
}

bool Facility::KernelOption::GENERAL::DISPLAY::Set(Json::Object* pData)
{
	if (pData == nullptr) {
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



Json::Object* Facility::KernelOption::GENERAL::TRANSPARENCY::Get()
{
	Json::Object* pData = new Json::Object();

	GetStringValue(Style);
	GetStringValue(Sorting);
	GetStringValue(DepthPeelingLayers);
	GetBooleanValue(PixelOIT);
	GetBooleanValue(DepthWriting);

	return pData;
}

bool Facility::KernelOption::GENERAL::TRANSPARENCY::Set(Json::Object* pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	SetStringValue(Style);
	SetStringValue(Sorting);
	SetStringValue(DepthPeelingLayers);
	GetBooleanValue(PixelOIT);
	GetBooleanValue(DepthWriting);

	return true;
}



Json::Object* Facility::KernelOption::GENERAL::RENDERING::Get()
{
	Json::Object* pData = new Json::Object();

	GetBooleanValue(DisplayAxisTriad);

	return pData;
}

bool Facility::KernelOption::GENERAL::RENDERING::Set(Json::Object* pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	SetBooleanValue(DisplayAxisTriad);

	return true;
}



Json::Object* Facility::KernelOption::GENERAL::ETC::Get()
{
	Json::Object* pData = new Json::Object();

	GetBooleanValue(BackplaneCulling);

	return pData;
}

bool Facility::KernelOption::GENERAL::ETC::Set(Json::Object* pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	SetBooleanValue(BackplaneCulling);

	return true;
}

#pragma endregion //:REGION

#pragma region Performance

Json::Object* Facility::KernelOption::PERFORMANCE::Get()
{
	Json::Object* pData = new Json::Object();

	GetObjectValue(Optimization);
	GetObjectValue(FramerateOptimization);

	return pData;
}

bool Facility::KernelOption::PERFORMANCE::Set(Json::Object* pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	SetObjectValue(Optimization);
	SetObjectValue(FramerateOptimization);

	return true;
}



Json::Object* Facility::KernelOption::PERFORMANCE::OPTIMIZATION::Get()
{
	Json::Object* pData = new Json::Object();

	GetEnumValue(HiddenLineMode);
	GetBooleanValue(StaticModel);
	GetBooleanValue(LMVModel);
	GetBooleanValue(OcclusionCulling);
	GetIntegerValue(OcclusionThreshold);

	return pData;
}

bool Facility::KernelOption::PERFORMANCE::OPTIMIZATION::Set(Json::Object* pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	SetEnumValue(HiddenLineMode, HLRMode);
	SetBooleanValue(StaticModel);
	SetBooleanValue(LMVModel);
	SetBooleanValue(OcclusionCulling);
	SetIntegerValue(OcclusionThreshold);

	return true;
}



Json::Object* Facility::KernelOption::PERFORMANCE::FRAMERATE_OPTIMIZATION::Get()
{
	Json::Object* pData = new Json::Object();

	GetBooleanValue(UseFramerate);
	GetEnumValue(CurrentFramerateMode);
	GetIntegerValue(MaxThreshold);
	GetIntegerValue(UseLods);
	GetIntegerValue(DetailSteps);
	GetIntegerValue(HardCutoff);
	GetIntegerValue(CullingThresholdSet);
	GetIntegerValue(CullingThreshold);

	return pData;
}

bool Facility::KernelOption::PERFORMANCE::FRAMERATE_OPTIMIZATION::Set(Json::Object* pData)
{
	if (pData == nullptr) {
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

Json::Object* Facility::KernelOption::INTERACTION::Get()
{
	Json::Object* pData = new Json::Object();

	GetObjectValue(Animation);
	GetObjectValue(GeometryManipulation);

	return pData;
}

bool Facility::KernelOption::INTERACTION::Set(Json::Object* pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	SetObjectValue(Animation);
	SetObjectValue(GeometryManipulation);

	return true;
}



Json::Object* Facility::KernelOption::INTERACTION::GEOMETRY_MANIPULATION::Get()
{
	Json::Object* pData = new Json::Object();

	GetBooleanValue(Spriting);
	GetBooleanValue(UpdateCutGeometry);
	GetBooleanValue(UpdateShadows);

	return pData;
}

bool Facility::KernelOption::INTERACTION::GEOMETRY_MANIPULATION::Set(Json::Object* pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	SetBooleanValue(Spriting);
	SetBooleanValue(UpdateCutGeometry);
	SetBooleanValue(UpdateShadows);

	return true;
}



Json::Object* Facility::KernelOption::INTERACTION::ANIMATION::Get()
{
	Json::Object* pData = new Json::Object();

	GetBooleanValue(UpdateCamera);

	return pData;
}

bool Facility::KernelOption::INTERACTION::ANIMATION::Set(Json::Object* pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	SetBooleanValue(UpdateCamera);

	return true;
}

#pragma endregion //:REGION

#pragma region Appearance

Json::Object* Facility::KernelOption::APPEARANCE::Get()
{
	Json::Object* pData = new Json::Object();

	GetObjectValue(AntiAliasing);
	GetObjectValue(BackgroundColor);

	return pData;
}

bool Facility::KernelOption::APPEARANCE::Set(Json::Object* pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	SetObjectValue(AntiAliasing);
	SetObjectValue(BackgroundColor);

	return true;
}



Json::Object* Facility::KernelOption::APPEARANCE::ANTIALIASING::Get()
{
	Json::Object* pData = new Json::Object();

	GetBooleanValue(Use);
	GetBooleanValue(Line);
	GetBooleanValue(Text);
	GetIntegerValue(Level);

	return pData;
}

bool Facility::KernelOption::APPEARANCE::ANTIALIASING::Set(Json::Object* pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	SetBooleanValue(Use);
	SetBooleanValue(Line);
	SetBooleanValue(Text);
	SetIntegerValue(Level);

	return true;
}



Json::Object* Facility::KernelOption::APPEARANCE::BACKGROUND_COLOR::Get()
{
	Json::Object* pData = new Json::Object();

	GetIntegerValue(Top);
	GetIntegerValue(Bottom);

	return pData;
}

bool Facility::KernelOption::APPEARANCE::BACKGROUND_COLOR::Set(Json::Object* pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	SetIntegerValue(Top);
	SetIntegerValue(Bottom);

	return true;
}

#pragma endregion //:REGION

#pragma region Selection

Json::Object* Facility::KernelOption::SELECTIION::Get()
{
	Json::Object* pData = new Json::Object();

	GetObjectValue(Behavior);
	GetObjectValue(Highlight);

	return pData;
}

bool Facility::KernelOption::SELECTIION::Set(Json::Object* pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	SetObjectValue(Behavior);
	SetObjectValue(Highlight);

	return true;
}



Json::Object* Facility::KernelOption::SELECTIION::BEHAVIOR::Get()
{
	Json::Object* pData = new Json::Object();

	GetBooleanValue(VisibilitySelection);
	GetBooleanValue(DynamicHighlighting);
	GetBooleanValue(DetailSelection);
	GetIntegerValue(RelatedSelectionLimit);
	GetBooleanValue(UseSelectBox);
	GetBooleanValue(RespectCulling);

	return pData;
}

bool Facility::KernelOption::SELECTIION::BEHAVIOR::Set(Json::Object* pData)
{
	if (pData == nullptr) {
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



Json::Object* Facility::KernelOption::SELECTIION::HIGHLIGHT::Get()
{
	Json::Object* pData = new Json::Object();

	GetStringValue(QuickMovesType);

	return pData;
}

bool Facility::KernelOption::SELECTIION::HIGHLIGHT::Set(Json::Object* pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	SetStringValue(QuickMovesType);

	return true;
}

#pragma endregion //:REGION

#pragma region Lighting

Json::Object* Facility::KernelOption::LIGHTING::Get()
{
	Json::Object* pData = new Json::Object();

	GetObjectValue(Bloom);
	GetObjectValue(Light);

	return pData;
}

bool Facility::KernelOption::LIGHTING::Set(Json::Object* pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	SetObjectValue(Bloom);
	SetObjectValue(Light);

	return true;
}



Json::Object* Facility::KernelOption::LIGHTING::BLOOM::Get()
{
	Json::Object* pData = new Json::Object();

	GetBooleanValue(Use);
	GetRealValue(Strength);
	GetIntegerValue(Blur);
	GetEnumValue(Shape);

	return pData;
}

bool Facility::KernelOption::LIGHTING::BLOOM::Set(Json::Object* pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	SetBooleanValue(Use);
	SetRealValue(Strength);
	SetIntegerValue(Blur);
	SetEnumValue(Shape, HBloomShape);

	return true;
}



Json::Object* Facility::KernelOption::LIGHTING::LIGHT::Get()
{
	Json::Object* pData = new Json::Object();

	GetBooleanValue(FollowsCamera);
	GetBooleanValue(Scaling);
	GetIntegerValue(ScaleFactor);

	return pData;
}

bool Facility::KernelOption::LIGHTING::LIGHT::Set(Json::Object* pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	SetBooleanValue(FollowsCamera);
	SetBooleanValue(Scaling);
	SetIntegerValue(ScaleFactor);

	return true;
}

#pragma endregion //:REGION
