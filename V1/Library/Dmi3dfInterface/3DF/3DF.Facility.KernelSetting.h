#pragma once

#include "3DF.Facility.Base.h"

OPEN_3DF_NAMESPACE

namespace Facility
{
	class KernelSetting : public Base
	{
	public:

		KernelSetting();
		~KernelSetting() override;

	public:

		Json::Object* Get() override;
		bool Set(Json::Object* pcData) override;

		struct GENERAL 
		{
			struct DISPLAY {
				CString Driver = L"dx11";
				CString Gpu = L"Default";
				bool DriverForceSoftware = false;		// CAppSettings::bDriverForceSoftware, Disable hardware accellation
				bool DriverDisplayStats = false;		// CAppSettings::bDriverDisplayStats, Display Statistics
				bool DoubleBuffer = true;				// CAppSettings::DoubleBuffer
				bool StereoMode = false;				// CAppSettings::StereoMode

				Json::Object* Get();
				bool Set(Json::Object* pcData);
			}
			Display;

			struct TRANSPARENCY {
				CString Style = L"blended";
				CString Sorting = L"depth peeling";
				CString DepthPeelingLayers = L"3";
				bool PixelOIT = false;
				bool DepthWriting = false;

				Json::Object* Get();
				bool Set(Json::Object* pcData);
			}
			Transparency;

			struct RENDERING
			{
				bool DisplayAxisTriad = true;			// CAppSettings::bDisplayAxisTriad

				Json::Object* Get();
				bool Set(Json::Object* pcData);
			}
			Rendering;

			struct ETC {
				bool BackplaneCulling = false;			// CAppSettings::bBackplaneCulling

				Json::Object* Get();
				bool Set(Json::Object* pcData);
			}
			Etc;

			Json::Object* Get();
			bool Set(Json::Object* pcData);
		}
		General;

		struct PERFORMANCE 
		{
			struct OPTIMIZATION {
				HLRMode HiddenLineMode = FakeHiddenLine;	// CAppSettings::HiddenLineMode
				bool StaticModel = true;					// CAppSettings::StaticModel
				bool LMVModel = true;						// CAppSettings::LMVModel
				bool OcclusionCulling = false;				// CAppSettings::OcclusionCulling
				int OcclusionThreshold = 50;				// CAppSettings::OcclusionThreshold

				Json::Object* Get();
				bool Set(Json::Object* pcData);
			}
			Optimization;

			struct FRAMERATE_OPTIMIZATION {
				bool UseFramerate = false;					// CAppSettings::UseFramerate
				FramerateMode CurrentFramerateMode = FramerateFixed; // CAppSettings::CurrentFramerateMode
				float FramerateTime = 0.05f;				// CAppSettings::FramerateTime
				int	MaxThreshold = 100;						// CAppSettings::MaxThreshold
				int	UseLods = 0;							// CAppSettings::UseLods
				int	DetailSteps = 15;						// CAppSettings::DetailSteps
				int HardCutoff = 10;						// CAppSettings::HardCutoff
				int	CullingThresholdSet = 1;				// CAppSettings::CullingThresholdSet
				int	CullingThreshold = 10;					// CAppSettings::CullingThreshold

				Json::Object* Get();
				bool Set(Json::Object* pcData);
			}
			FramerateOptimization;

			Json::Object* Get();
			bool Set(Json::Object* pcData);
		}
		Performance;

		struct INTERACTION
		{
			struct GEOMETRY_MANIPULATION {
				bool Spriting = true;					// CAppSettings::Spriting
				bool UpdateCutGeometry = false;			// CAppSettings::UpdateCutGeometry
				bool UpdateShadows = true;				// CAppSettings::UpdateShadows

				Json::Object* Get();
				bool Set(Json::Object* pcData);
			}
			GeometryManipulation;

			struct ANIMATION {
				bool UpdateCamera = true;				// CAppSettings::UpdateCamera

				Json::Object* Get();
				bool Set(Json::Object* pcData);
			}
			Animation;

			Json::Object* Get();
			bool Set(Json::Object* pcData);
		}
		Interaction;

		struct APPEARANCE
		{
			struct ANTIALIASING {
				bool Use = true;		// CAppSettings::bAntiAliasing
				bool Line = true;		// CAppSettings::LineAntialiasing
				bool Text = true;		// CAppSettings::TextAntialiasing
				int Level = 4;			// CAppSettings::AntialiasingLevel

				Json::Object* Get();
				bool Set(Json::Object* pcData);
			}
			AntiAliasing;

			struct BACKGROUND_COLOR
			{
				COLORREF Top = RGB(0x30, 0x30, 0x30);
				COLORREF Bottom = RGB(0x30, 0x30, 0x30);

				Json::Object* Get();
				bool Set(Json::Object* pcData);
			}
			BackgroundColor;

			Json::Object* Get();
			bool Set(Json::Object* pcData);
		}
		Appearance;

		struct SELECTIION
		{
			struct BEHAVIOR {
				bool VisibilitySelection = false;		// CAppSettings::VisibilitySelection
				bool DynamicHighlighting = false;		// CAppSettings::DynamicHighlighting
				bool DetailSelection = false;			// CAppSettings::DetailSelection, Honor Line/Edge Weight/Pattern
				int RelatedSelectionLimit = 0;			// CAppSettings::RelatedSelectionLimit
				bool UseSelectBox = true;				// CAppSettings::bUseSelectBox, Transparent select box, show a transparent box when selecting areas
				bool RespectCulling = false;			// CAppSettings::SelectionRespectCulling, Respect Culling during selection.

				Json::Object* Get();
				bool Set(Json::Object* pcData);
			}
			Behavior;

			struct HIGHLIGHT {
				CString QuickMovesType = L"Default";		// CAppSettings::csQuickMovesType, Quick Moves Preference

				Json::Object* Get();
				bool Set(Json::Object* pcData);
			}
			Highlight;

			Json::Object* Get();
			bool Set(Json::Object* pcData);
		}
		Selection;

		struct LIGHTING
		{
			struct BLOOM {
				bool Use = false;					// CAppSettings::UseBloom
				float Strength = 1.0;				// CAppSettings::BloomStrength
				int Blur = 5;						// CAppSettings::BloomBlur
				HBloomShape Shape = RadialBloom;	// CAppSettings::BloomShape

				Json::Object* Get();
				bool Set(Json::Object* pcData);
			}
			Bloom;

			struct LIGHT {
				bool FollowsCamera = true;			// CAppSettings::LightFollowsCamera
				bool Scaling = true;				// CAppSettings::bLightScaling
				int ScaleFactor = 100000;			// CAppSettings::LightScaleFactor

				Json::Object* Get();
				bool Set(Json::Object* pcData);
			}
			Light;

			Json::Object* Get();
			bool Set(Json::Object* pcData);
		}
		Lighting;

		struct EFFECTS
		{
			struct SIMPLE_SHADOW {
				HShadowRenderingMode ShadowRenderingMode = SoftwareOpenglShadow;	// CAppSettings::ShadowRenderingMode

// 				Json::Object* Get();
// 				bool Set(Json::Object* pcData);
			}
			SimpleShadow;

			struct FRAMEBUFFER {
				bool UseAmbient = false;				// CAppSettings::UseFastAmbient, Ambient Occlusion
				float AmbientStrength = 1.0f;			// CAppSettings::FastAmbientStrength
				bool HighQualityAmbient = false;		// CAppSettings::HQAmbientOcclusion
				bool UseFastSilhouette = false;			// CAppSettings::UseFastSilhouette
				float FastSilhouetteTolerance = 1.0f;	// CAppSettings::FastSilhouetteTolerance
				bool HeavyExteriorSilhouette = false;	// CAppSettings::HeavyExteriorSilhouette

// 				Json::Object* Get();
// 				bool Set(Json::Object* pcData);
			}
			FrameBuffer;

// 			Json::Object* Get();
// 			bool Set(Json::Object* pcData);
		}
		Effects;
	};



	class AppSetting : public Base
	{
	public:

		AppSetting() {}
		~AppSetting() override {}

	public:

		Json::Object* Get() override {
			return nullptr;
		}
		bool Set(Json::Object* pcData) override {
			return false;
		}

	public:

		bool OcclusionCulling = false;
		int OcclusionThreshold = 50;
		COLORREF FakeHLRColor = RGB(255, 255, 255);
		ProjMode ProjectionMode = ProjOrthographic;
		bool SmoothTransition = true;
		bool DisableEditing = true;
		bool WorldHandedness = true;
		bool GrayScaleSelection = false;	// no gray scale selection
		bool UseDefinedHighlighting = false;
		bool InvisibleSelection = false;
		bool DisplaceSelection = false;	// no gray scale selection
		int SelectionColorTransparency = 0;
		COLORREF PolygonSelectionColor = RGB(255, 128, 0);
		COLORREF LineSelectionColor = RGB(255, 128, 0);
		COLORREF MarkerSelectionColor = RGB(255, 128, 0);
		COLORREF MarkupColor = RGB(255, 0, 0);
		COLORREF ShadowColor = RGB(45, 45, 45);
		int	MarkupWeight = 400;	// Markup line weight * 100 (to store float as int)
		int	ShadowRes = 64;
		int	ShadowBlur = 4;
		bool CiByValue = true;// color interpolation, by value
		bool CiByColormapIndex = true;	// color interpolation, by colormap index
		bool CiIsolines = false;	// color interpolation, isolines only
		HSelectionHighlightMode HighlightMode = HighlightQuickmoves;
		float TransparencyLevel = 0.9f;
		CString RefSelType = "Spriting";
		HRenderMode RenderMode = HRenderGouraud;
		bool ShowCollisions = false;
		HShadowMode ShadowMode = HShadowNone;
		bool ShadowMap = false;
		int HardCutoff = 10;
		bool SplatRendering = false;	//use splat rendering
		bool FastMarkers = true;
		int SMResolution = 2048;
		int SMSamples = 4;
		bool Jitter = false;

		bool ReflectionPlane = false;
		float ReflectionOpacity = 0.5;
		bool ReflectionFading = true;
		bool ReflectionUseAttenuation = false;
		float ReflectionHither = 0.f;
		float ReflectionYon = 1.f;
		bool ReflectionUseBlur = false;
		int ReflectionBlur = 1;
		bool UseLightVector = false;
		HPoint LightVector = HPoint(0.0f, 0.0f, 1.0f);
		bool IgnoreTransparency = false;
		bool StaticModel = true;
		bool LMVModel = true;
		bool RestoreAnnotations = false;
		DisplayListType DisplayList = DisplayListSegment;
		int	DynamicAdjustment = 1;

		float ShadowOpacity = 1.f;

		CString FontName = "sans serif";
		CString FontSize = "14";
		CString FontUnits = "pts";

		bool HideOverlappedText = false;	//hide overlapped text?
		bool ViewDependentShadowMap = true;

		CString LineWeight = "1 pixels";
		bool StereoMode = false;
		int StereoSeparation = 30000;

		CString GoochColorMap = "blue, red";

		COLORREF GoochColor1 = RGB(255.0, 105.0, 180.0);
		COLORREF GoochColor2 = RGB(60.0, 179.0, 113.0);
		COLORREF GoochColor3 = RGB(255.0, 255.0, 255.0);
		COLORREF GoochColor4 = RGB(255.0, 255.0, 255.0);
		COLORREF GoochColor5 = RGB(255.0, 255.0, 255.0);
		COLORREF GoochColor6 = RGB(255.0, 255.0, 255.0);

		float GoochWeight = 0.5f;
		float GoochColorHigh = 2.0f;

		int NearCameraLimit = 0;
		int Budget = 512;
		int ContinuedBudget = 0;
		int MaxDeviation = 0;
		int MaxAngle = 200000;
		int MaxLength = 1000;
		bool ViewIndependent = true;

		bool HemisphericAmbient = false;
		COLORREF AmbientTopColor = RGB(1.0000 * 255, 1.0000 * 255, 1.0000 * 255);
		COLORREF AmbientBottomColor = RGB(1.0000 * 255, 1.0000 * 255, 1.0000 * 255);
		bool UseAmbientUpVector = false;
		HPoint AmbientUpVector = HPoint(0.0f, 1.0f, 0.0f);
		bool UseGreeking = false;	//use text greeking
		CString GreekingMode = "Box";
		CString GreekingUnits = "px";
		int	GreekingLimit = 6000;	// Greeking limit * 1000 (to store float as int)

		int LightCount = 1;

		CString TransparencyStyle = "blended";
		CString TransparencySorting = "depth peeling";
		CString TransparencyDepthPeelingLayers = "3";
		bool PixelOIT = false;
		bool DepthWriting = false;
	};
}

CLOSE_3DF_NAMESPACE

extern TDF::Facility::AppSetting TheAppSetting;
