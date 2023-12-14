#include "stdafx.h"

#include "Facility.KernelOption.h"

USING_3DF_NAMESPACE

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

	GetObjectValue(General);
	GetObjectValue(Performance);
	GetObjectValue(Interaction);
	GetObjectValue(Appearance);
	GetObjectValue(Selection);
	GetObjectValue(Lighting);
	GetObjectValue(VisualEffects);

	return pData;
}

bool Facility::KernelOption::Set(Json::Object* pData)
{
	if (pData == nullptr) {
		return false;
	}

	SetObjectValue(General);
	SetObjectValue(Performance);
	SetObjectValue(Interaction);
	SetObjectValue(Appearance);
	SetObjectValue(Selection);
	SetObjectValue(Lighting);
	SetObjectValue(VisualEffects);

	return true;
}

CString Facility::KernelOption::ToHexString(COLORREF nColor)
{
	CString strHexString;
	strHexString.Format(_T("0x%06X"), nColor & 0x00FFFFFF);
	return strHexString;

// 	CString strColor;
// 	strColor.Format(L"0x%02X%02X%02X", GetRValue(nColor), GetGValue(nColor), GetBValue(nColor));
// 	return strColor;
}

COLORREF Facility::KernelOption::ToColor(CString strHexString)
{
	COLORREF nColor;
	_stscanf_s(strHexString, _T("0x%x"), &nColor);
	return nColor;

// 	strHexString.Replace(L"0x", L"");
// 	int nRed = _wtoi(strHexString.Mid(0, 2));
// 	int nGreen = _wtoi(strHexString.Mid(2, 2));
// 	int nBlue = _wtoi(strHexString.Mid(4, 2));
// 	return RGB(nRed, nGreen, nBlue);
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
	SetEnumValue(Shape, BloomShape);

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

#pragma region VisualEffects

//== Visual effects 관련 함수 ========================================================================

// 1-1. Visual Effects를 Json Object로 반환
Json::Object * Facility::KernelOption::VISUALEFFECTS::Get()
{
	Json::Object * pData = new Json::Object();

	GetObjectValue(Shadow);
	GetObjectValue(PlaneReflection);
	GetObjectValue(AmbientOcclusion);
	GetObjectValue(SilhouetteEdges);
	GetObjectValue(Bloom);

	return pData;
}

// 1-2. Visual Effects를 저장
bool Facility::KernelOption::VISUALEFFECTS::Set(Json::Object * pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	SetObjectValue(Shadow);
	SetObjectValue(PlaneReflection);
	SetObjectValue(AmbientOcclusion);
	SetObjectValue(SilhouetteEdges);
	SetObjectValue(Bloom);

	return true;
}

// 2-1. Shadow를 Json Object로 반환
Json::Object * Facility::KernelOption::VISUALEFFECTS::SHADOW::Get()
{
	Json::Object * pData = new Json::Object();

	GetBooleanValue(checked);
	GetEnumValue(Mode);
	GetIntegerValue(Resolution);
	GetIntegerValue(Blurring);
	GetBooleanValue(IgnoreTransparency);
	GetStringValue(Color);
	GetIntegerValue(Opacity);

	return pData;
}

// 2-2. Shadow를 저장
bool Facility::KernelOption::VISUALEFFECTS::SHADOW::Set(Json::Object * pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	SetBooleanValue(checked);
	SetEnumValue(Mode, ShadowMode);
	SetIntegerValue(Resolution);
	SetIntegerValue(Blurring);
	SetBooleanValue(IgnoreTransparency);
	SetStringValue(Color);
	SetIntegerValue(Opacity);

	return true;
}

// 2-3. 실제 적용가능한 Resolution 값 반환
int Facility::KernelOption::VISUALEFFECTS::SHADOW::GetResolution()
{
	// Type: int (32~1024), default value: 256
	// UI: default value: 3, range: 1 ~ 10
	float fStep = (1024 - 32) / 10.0f;
	int nValue = (int)(Resolution * fStep);
	return nValue;
}

// 2-4. 실제 적용가능한 Blurring 값 반환
int Facility::KernelOption::VISUALEFFECTS::SHADOW::GetBlurring()
{
	// Type: int (1~31), default value: 1
	// UI: default value: 1, range: 1 ~ 10
	float fStep = (31 - 1) / 10.0f;
	int nValue = (int) (Blurring * fStep);
	return nValue;
}

// 2-5. 실제 적용가능한 Opacity 값 반환
float Facility::KernelOption::VISUALEFFECTS::SHADOW::GetOpacity()
{
	// Type: Type: float (0~1), default value: 1.0f
	// UI: default value: 10, range: 1 ~ 10
	float fStep = (1 - 0) / 10.0f;
	int nValue = (int) (Opacity * fStep);
	return nValue;
}

COLORREF Facility::KernelOption::VISUALEFFECTS::SHADOW::GetColor()
{
	COLORREF nColor = Json::Helper::ToColor(Color);
	return nColor;
}

// 3-1. Plane Reflection을 Json Object로 반환
Json::Object * Facility::KernelOption::VISUALEFFECTS::PLANEREFLECTION::Get()
{
	Json::Object * pData = new Json::Object();

	GetBooleanValue(checked);
	GetIntegerValue(Opacity);
	GetIntegerValue(Blurring);
	GetBooleanValue(Fading);

	return pData;
}

// 3-2. Plane Reflection을 저장
bool Facility::KernelOption::VISUALEFFECTS::PLANEREFLECTION::Set(Json::Object * pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	SetBooleanValue(checked);
	SetIntegerValue(Opacity);
	SetIntegerValue(Blurring);
	SetBooleanValue(Fading);

	return true;
}

// 3-3. 실제 적용가능한 Opacity 값 반환
float Facility::KernelOption::VISUALEFFECTS::PLANEREFLECTION::GetOpacity()
{
	// Type: Type: float (0~1), default value: 0.5f
	// UI: default value: 5, range: 1 ~ 10
	float fStep = (1 - 0) / 10.0f;
	int nValue = (int) (Opacity * fStep);
	return nValue;
}

// 3-4. 실제 적용가능한 Blurring 값 반환
int Facility::KernelOption::VISUALEFFECTS::PLANEREFLECTION::GetBlurring()
{
	// Type: int (1~31), default value: 1
	// UI: default value: 1, range: 1 ~ 10
	float fStep = (31 - 1) / 10.0f;
	int nValue = (int) (Blurring * fStep);
	return nValue;
}

// 4-1. Ambient Occlusion을 Json Object로 반환
Json::Object * Facility::KernelOption::VISUALEFFECTS::AMBIENTOCCLUSION::Get()
{
	Json::Object * pData = new Json::Object();

	GetBooleanValue(checked);
	GetIntegerValue(Strength);
	GetIntegerValue(Quality);

	return pData;
}

// 4-2. Ambient Occlusion을 저장
bool Facility::KernelOption::VISUALEFFECTS::AMBIENTOCCLUSION::Set(Json::Object * pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	SetBooleanValue(checked);
	SetIntegerValue(Strength);
	SetIntegerValue(Quality);

	return true;
}

// 4-3. 실제 적용가능한 Strength 값 반환
float Facility::KernelOption::VISUALEFFECTS::AMBIENTOCCLUSION::GetStrength()
{
	// Type: float(1 - 100), default value : 1.0f
	// UI: default value: 20, range: 1 ~ 20
	float fStep = (100 - 1) / 20.0f;
	int nValue = (int) (Strength * fStep);
	return nValue;
}

// 4-4. 실제 적용가능한 Quality 값 반환
CStringA Facility::KernelOption::VISUALEFFECTS::AMBIENTOCCLUSION::GetQuality()
{
	CStringA strQuality = (Quality == 0) ? "Fast" : "Nicest";
	return strQuality;
}

// 5-1. Silhouette Edges을 Json Object로 반환
Json::Object * Facility::KernelOption::VISUALEFFECTS::SILHOUETTEEDGES::Get()
{
	Json::Object * pData = new Json::Object();

	GetBooleanValue(checked);
	GetIntegerValue(Tolerance);
	GetBooleanValue(HeavyExterior);

	return pData;
}

// 5-2. Silhouette Edges을 저장
bool Facility::KernelOption::VISUALEFFECTS::SILHOUETTEEDGES::Set(Json::Object * pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	SetBooleanValue(checked);
	SetIntegerValue(Tolerance);
	SetBooleanValue(HeavyExterior);

	return true;
}

// 5-3. 실제 적용가능한 Tolerance 값 반환
float Facility::KernelOption::VISUALEFFECTS::SILHOUETTEEDGES::GetTolerance()
{
	// Type: float(1 - 100), default value : 1.0f
	// UI: default value: 20, range: 1 ~ 20
	float fStep = (100 - 1) / 20.0f;
	int nValue = (int) (Tolerance * fStep);
	return nValue;
}

// 6-1. Bloom을 Json Object로 반환
Json::Object * Facility::KernelOption::VISUALEFFECTS::BLOOM::Get()
{
	Json::Object * pData = new Json::Object();

	GetBooleanValue(checked);
	GetRealValue(Strength);
	GetIntegerValue(Blurring);
	GetEnumValue(Shape);

	return pData;
}

// 6-2. Bloom을 저장
bool Facility::KernelOption::VISUALEFFECTS::BLOOM::Set(Json::Object * pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	SetBooleanValue(checked);
	SetRealValue(Strength);
	SetIntegerValue(Blurring);
	SetEnumValue(Shape, BloomShape);

	return true;
}

// 6-3. 실제 적용가능한 Strength 값 반환
float Facility::KernelOption::VISUALEFFECTS::BLOOM::GetStrength()
{
	// Type: float(1 - 100), default value : 1.0f
	// UI: default value: 20, range: 1 ~ 20
	float fStep = (100 - 1) / 20.0f;
	int nValue = (int) (Strength * fStep);
	return nValue;
}

// 6-4. 실제 적용가능한 Blurring 값 반환
int Facility::KernelOption::VISUALEFFECTS::BLOOM::GetBlurring()
{
	// Type: int (1~31), default value: 1
	// UI: default value: 1, range: 1 ~ 10
	float fStep = (31 - 1) / 10.0f;
	int nValue = (int) (Blurring * fStep);
	return nValue;
}

// 6-5. 실제 적용가능한 Shape 값 반환
CStringA Facility::KernelOption::VISUALEFFECTS::BLOOM::GetShape()
{
	CStringA strShape = (Shape == 0) ? "Radial" : "Star";
	return strShape;
}

#pragma endregion //:REGION (VisualEffects)


