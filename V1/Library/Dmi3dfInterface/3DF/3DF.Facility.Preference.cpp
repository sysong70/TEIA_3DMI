#include "stdafx.h"

#include "3DF.Facility.Preference.h"

#ifdef _DEBUG
#	define new(THIS_FILE, __LINE__)
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
	Json::Object * pcData = new Json::Object();

	//pcData->SetObject("General", General.Get());

	return pcData;
}

bool Facility::Preference::Set(Json::Object* pcData)
{
	if (pcData == nullptr) {
		return false;
	}

	//SetLanguage((ELanguage)pcData->GetInteger("Language"));

	return true;
}

//== General Option ================================================================================

Json::Object * Facility::Preference::GENERAL::Get()
{
	Json::Object * pcData = new Json::Object();

	pcData->SetObject("Display", Display.Get());
	pcData->SetObject("Transparency", Transparency.Get());
	pcData->SetObject("Rendering", Rendering.Get());
	pcData->SetObject("Etc", Etc.Get());

	return pcData;
}

bool Facility::Preference::GENERAL::Set(Json::Object * pcData)
{
	Json::Object & cDisplayObject = pcData->GetObject("Display");
	Display.Set(&cDisplayObject);

	Json::Object & cTransparencyObject = pcData->GetObject("Transparency");
	Transparency.Set(&cTransparencyObject);

	Json::Object & cRenderingObject = pcData->GetObject("Rendering");
	Rendering.Set(&cRenderingObject);

	Json::Object & cEtcObject = pcData->GetObject("Etc");
	Etc.Set(&cEtcObject);

	return true;
}


// Display Option
Json::Object * Facility::Preference::GENERAL::DISPLAY::Get()
{
	Json::Object * pcData = new Json::Object();

	pcData->SetString("Driver", Driver);
	pcData->SetString("Gpu", Gpu);
	pcData->SetBoolean("DriverForceSoftware", DriverForceSoftware);
	pcData->SetBoolean("DriverDisplayStats", DriverDisplayStats);
	pcData->SetBoolean("DoubleBuffer", DoubleBuffer);
	pcData->SetBoolean("StereoMode", StereoMode);

	return pcData;
}

bool Facility::Preference::GENERAL::DISPLAY::Set(Json::Object * pcData)
{
	Driver = pcData->GetString("Driver");
	Gpu = pcData->GetString("Gpu");
	DriverForceSoftware = pcData->GetBoolean("DriverForceSoftware");
	DriverDisplayStats = pcData->GetBoolean("DriverDisplayStats");
	DoubleBuffer = pcData->GetBoolean("DoubleBuffer");
	StereoMode = pcData->GetBoolean("StereoMode");

	return true;
}

// Transparency Option
Json::Object * Facility::Preference::GENERAL::TRANSPARENCY::Get()
{
	Json::Object * pcData = new Json::Object();

	pcData->SetString("Style", Style);
	pcData->SetString("Sorting", Sorting);
	pcData->SetString("DepthPeelingLayers", DepthPeelingLayers);
	pcData->SetBoolean("PixelOIT", PixelOIT);
	pcData->SetBoolean("DepthWriting", DepthWriting);

	return pcData;
}

bool Facility::Preference::GENERAL::TRANSPARENCY::Set(Json::Object * pcData)
{
	Style = pcData->GetString("Style");
	Sorting = pcData->GetString("Sorting");
	DepthPeelingLayers = pcData->GetString("DepthPeelingLayers");
	PixelOIT = pcData->GetBoolean("PixelOIT");
	DepthWriting = pcData->GetBoolean("DepthWriting");

	return true;
}

// Rendering Option
Json::Object * Facility::Preference::GENERAL::RENDERING::Get()
{
	Json::Object * pcData = new Json::Object();

	pcData->SetBoolean("DisplayAxisTriad", DisplayAxisTriad);

	return pcData;
}

bool Facility::Preference::GENERAL::RENDERING::Set(Json::Object * pcData)
{
	DisplayAxisTriad = pcData->GetBoolean("DisplayAxisTriad");

	return true;
}

// ETC Option
Json::Object * Facility::Preference::GENERAL::ETC::Get()
{
	Json::Object * pcData = new Json::Object();

	pcData->SetBoolean("BackplaneCulling", BackplaneCulling);

	return pcData;
}

bool Facility::Preference::GENERAL::ETC::Set(Json::Object * pcData)
{
	BackplaneCulling = pcData->GetBoolean("BackplaneCulling");

	return true;
}

//== Perfromance Option ============================================================================
Json::Object * Facility::Preference::PERFROMANCE::Get()
{
	Json::Object * pcData = new Json::Object();

	pcData->SetObject("Optimization", Optimization.Get());
	pcData->SetObject("FramerateOptimization", FramerateOptimization.Get());

	return pcData;
}

bool Facility::Preference::PERFROMANCE::Set(Json::Object * pcData)
{
	Json::Object & cOptimizationObject = pcData->GetObject("Optimization");
	Optimization.Set(&cOptimizationObject);

	Json::Object & cFramerateOptimizationObject = pcData->GetObject("FramerateOptimization");
	FramerateOptimization.Set(&cFramerateOptimizationObject);

	return true;
}

// Optimization Option
Json::Object * Facility::Preference::PERFROMANCE::OPTIMIZATION::Get()
{
	Json::Object * pcData = new Json::Object();

	pcData->SetInteger("HiddenLineMode", (int)HiddenLineMode);
	pcData->SetBoolean("OcclusionCulling", OcclusionCulling);
	pcData->SetInteger("OcclusionThreshold", OcclusionThreshold);

	return pcData;
}

bool Facility::Preference::PERFROMANCE::OPTIMIZATION::Set(Json::Object * pcData)
{
	HiddenLineMode = (HLRMode)pcData->GetInteger("HiddenLineMode");
	OcclusionCulling = pcData->GetBoolean("OcclusionCulling");
	OcclusionThreshold = pcData->GetInteger("OcclusionThreshold");

	return true;
}

// Framerate Optimization Option
Json::Object * Facility::Preference::PERFROMANCE::FRAMERATE_OPTIMIZATION::Get()
{
	Json::Object * pcData = new Json::Object();

	pcData->SetBoolean("UseFramerate", UseFramerate);
	pcData->SetInteger("CurrentFramerateMode", (int)CurrentFramerateMode);
	pcData->SetReal("FramerateTime", FramerateTime);
	pcData->SetInteger("MaxThreshold", MaxThreshold);
	pcData->SetInteger("UseLods", UseLods);
	pcData->SetInteger("DetailSteps", DetailSteps);
	pcData->SetInteger("HardCutoff", HardCutoff);
	pcData->SetInteger("CullingThresholdSet", CullingThresholdSet);
	pcData->SetInteger("CullingThreshold", CullingThreshold);

	return pcData;
}

bool Facility::Preference::PERFROMANCE::FRAMERATE_OPTIMIZATION::Set(Json::Object * pcData)
{
	UseFramerate = pcData->GetBoolean("UseFramerate");
	CurrentFramerateMode = (FramerateMode)pcData->GetReal("CurrentFramerateMode");
	MaxThreshold = pcData->GetInteger("MaxThreshold");
	UseLods = pcData->GetInteger("UseLods");
	DetailSteps = pcData->GetInteger("DetailSteps");
	HardCutoff = pcData->GetInteger("HardCutoff");
	CullingThresholdSet = pcData->GetInteger("CullingThresholdSet");
	CullingThreshold = pcData->GetInteger("CullingThreshold");

	return true;
}

//== Interaction Option ============================================================================
Json::Object * Facility::Preference::INTERACTION::Get()
{
	Json::Object * pcData = new Json::Object();

	pcData->SetObject("Animation", Animation.Get());
	pcData->SetObject("GeometryManipulation", GeometryManipulation.Get());

	return pcData;
}

bool Facility::Preference::INTERACTION::Set(Json::Object * pcData)
{
	Json::Object & cAnimationObject = pcData->GetObject("Animation");
	Animation.Set(&cAnimationObject);

	Json::Object & cGeometryManipulationObject = pcData->GetObject("GeometryManipulation");
	GeometryManipulation.Set(&cGeometryManipulationObject);

	return true;
}

// Geometry Manipulation Option
Json::Object * Facility::Preference::INTERACTION::GEOMETRY_MANIPULATION::Get()
{
	Json::Object * pcData = new Json::Object();

	pcData->SetBoolean("Spriting", Spriting);
	pcData->SetBoolean("UpdateCutGeometry", UpdateCutGeometry);
	pcData->SetBoolean("UpdateShadows", UpdateShadows);

	return pcData;
}

bool Facility::Preference::INTERACTION::GEOMETRY_MANIPULATION::Set(Json::Object * pcData)
{
	Spriting = pcData->GetBoolean("Spriting");
	UpdateCutGeometry = pcData->GetBoolean("UpdateCutGeometry");
	UpdateShadows = pcData->GetBoolean("UpdateShadows");

	return true;
}

// Animation Option
Json::Object * Facility::Preference::INTERACTION::ANIMATION::Get()
{
	Json::Object * pcData = new Json::Object();

	pcData->SetBoolean("UpdateCamera", UpdateCamera);

	return pcData;
}

bool Facility::Preference::INTERACTION::ANIMATION::Set(Json::Object * pcData)
{
	UpdateCamera = pcData->GetBoolean("UpdateCamera");

	return true;
}

//== Appearance Option ===============================================================================

Json::Object * Facility::Preference::APPEARANCE::Get()
{
	Json::Object * pcData = new Json::Object();

	pcData->SetObject("AntiAliasing", AntiAliasing.Get());
	pcData->SetObject("BackgroundColor", AntiAliasing.Get());

	return pcData;
}

bool Facility::Preference::APPEARANCE::Set(Json::Object * pcData)
{
	Json::Object & cAntiAliasingObject = pcData->GetObject("AntiAliasing");
	AntiAliasing.Set(&cAntiAliasingObject);

	Json::Object & cBackgroundColorObject = pcData->GetObject("BackgroundColor");
	BackgroundColor.Set(&cBackgroundColorObject);

	return true;
}

// AntiAliasing Option
Json::Object * Facility::Preference::APPEARANCE::ANTIALIASING::Get()
{
	Json::Object * pcData = new Json::Object();

	pcData->SetBoolean("Use", Use);
	pcData->SetBoolean("Line", Line);
	pcData->SetBoolean("Text", Text);
	pcData->SetInteger("Level", Level);

	return pcData;
}

bool Facility::Preference::APPEARANCE::ANTIALIASING::Set(Json::Object * pcData)
{
	Use = pcData->GetBoolean("Use");
	Line = pcData->GetBoolean("Line");
	Text = pcData->GetBoolean("Text");
	Level = pcData->GetInteger("Level");

	return true;
}

// Background Color Option
Json::Object * Facility::Preference::APPEARANCE::BACKGROUND_COLOR::Get()
{
	Json::Object * pcData = new Json::Object();

	pcData->SetInteger("Top", Top);
	pcData->SetInteger("Bottom", Bottom);

	return pcData;
}

bool Facility::Preference::APPEARANCE::BACKGROUND_COLOR::Set(Json::Object * pcData)
{
	Top = pcData->GetInteger("Top");
	Bottom = pcData->GetInteger("Bottom");

	return true;
}

//== Selectiion Option ============================================================================
Json::Object * Facility::Preference::SELECTIION::Get()
{
	Json::Object * pcData = new Json::Object();

	pcData->SetObject("Behavior", Behavior.Get());
	pcData->SetObject("Highlight", Highlight.Get());

	return pcData;
}

bool Facility::Preference::SELECTIION::Set(Json::Object * pcData)
{
	Json::Object & cBehaviorObject = pcData->GetObject("Behavior");
	Behavior.Set(&cBehaviorObject);

	Json::Object & cHighlightObject = pcData->GetObject("Highlight");
	Highlight.Set(&cHighlightObject);

	return true;
}

// Behavior Option
Json::Object * Facility::Preference::SELECTIION::BEHAVIOR::Get()
{
	Json::Object * pcData = new Json::Object();

	pcData->SetBoolean("VisibilitySelection", VisibilitySelection);
	pcData->SetBoolean("DynamicHighlighting", DynamicHighlighting);
	pcData->SetBoolean("DetailSelection", DetailSelection);
	pcData->SetInteger("RelatedSelectionLimit", RelatedSelectionLimit);
	pcData->SetBoolean("UseSelectBox", UseSelectBox);
	pcData->SetBoolean("RespectCulling", RespectCulling);

	return pcData;
}

bool Facility::Preference::SELECTIION::BEHAVIOR::Set(Json::Object * pcData)
{
	VisibilitySelection = pcData->GetBoolean("VisibilitySelection");
	DynamicHighlighting = pcData->GetBoolean("DynamicHighlighting");
	DetailSelection = pcData->GetBoolean("DetailSelection");
	RelatedSelectionLimit = pcData->GetInteger("RelatedSelectionLimit");
	UseSelectBox = pcData->GetBoolean("UseSelectBox");
	RespectCulling = pcData->GetBoolean("RespectCulling");

	return true;
}

// Highlight Option
Json::Object * Facility::Preference::SELECTIION::HIGHLIGHT::Get()
{
	Json::Object * pcData = new Json::Object();

	pcData->SetString("QuickMovesType", QuickMovesType);

	return pcData;
}

bool Facility::Preference::SELECTIION::HIGHLIGHT::Set(Json::Object * pcData)
{
	QuickMovesType = pcData->GetString("QuickMovesType");

	return true;
}

//== Lighting Option ===============================================================================

Json::Object * Facility::Preference::LIGHTING::Get()
{
	Json::Object * pcData = new Json::Object();

	pcData->SetObject("Bloom", Bloom.Get());
	pcData->SetObject("Light", Light.Get());

	return pcData;
}

bool Facility::Preference::LIGHTING::Set(Json::Object * pcData)
{
	Json::Object & cBloomObject = pcData->GetObject("Bloom");
	Bloom.Set(&cBloomObject);

	Json::Object & cLightObject = pcData->GetObject("Light");
	Light.Set(&cLightObject);

	return true;
}

// Bloom Option
Json::Object * Facility::Preference::LIGHTING::BLOOM::Get()
{
	Json::Object * pcData = new Json::Object();

	pcData->SetBoolean("Use", Use);
	pcData->SetReal("Strength", Strength);
	pcData->SetInteger("Blur", Blur);
	pcData->SetInteger("Shape", (int)Shape);

	return pcData;
}

bool Facility::Preference::LIGHTING::BLOOM::Set(Json::Object * pcData)
{
	Use = pcData->GetBoolean("Use");
	Strength = pcData->GetReal("Strength");
	Blur = pcData->GetInteger("Blur");
	Shape = (HBloomShape)pcData->GetInteger("Shape");

	return true;
}

// Light Option
Json::Object * Facility::Preference::LIGHTING::LIGHT::Get()
{
	Json::Object * pcData = new Json::Object();

	pcData->SetBoolean("FollowsCamera", FollowsCamera);
	pcData->SetBoolean("Scaling", FollowsCamera);
	pcData->SetInteger("ScaleFactor", ScaleFactor);

	return pcData;
}

bool Facility::Preference::LIGHTING::LIGHT::Set(Json::Object * pcData)
{
	FollowsCamera = pcData->GetBoolean("FollowsCamera");
	Scaling = pcData->GetBoolean("Scaling");
	ScaleFactor = pcData->GetInteger("ScaleFactor");

	return true;
}

