#pragma once

#include "3DF.h"
#include "Math.h"
#include "Color.h"

#include "Facility.Base.h"

namespace H3DF
{
	namespace Facility
	{
		enum HLRMode
		{
			AnalyticHiddenLine,		// Analytic hidden line
			FastHiddenLine,			// Fast hidden line
			FakeHiddenLine			// Fake hidden line
		};

		enum FramerateMode
		{
			FramerateOff,			// unknown mode
			FramerateFixed,			// self-explanatory
			FramerateTarget			// self-explanatory
		};

		enum BloomShape
		{
			Radial,					// radial bloom
			Star					// star bloom
		};

		enum HShadowRenderingMode
		{
			SoftwareShadow,         // image driver shadow generation
			HardwareShadow,		    // opengl p-buffer shadow generation
			SoftwareOpenglShadow	// opengl software shadow generation
		};

		enum ProjMode
		{
			ProjUnknown,       // unknown mode
			ProjPerspective,   // Objects which are close to the eye appear large, and objects which are far away appear small.
			ProjOrthographic,  // The represented size of an object doesn't depend on how close or far it is---this can be handy in architecture and engineering.
			ProjStretched      // In a "stretched" view, an orthographic projection is made, and then the view is "stretched" to fit exactly into the available screen window.
		};

		enum HSelectionHighlightMode {
			HighlightDefault,	// use default conditional highlighting
			HighlightQuickmoves, // use quick move reference highlighting
			InverseTransparency,  // do nothing to the selected item, but make the rest of the model transparent.
			ColoredInverseTransparency  // color the selected item and make the rest of the model transparent.
		};

		enum HRenderMode
		{
			HRenderWireframe = 1,       // edges/lines only
			HRenderTriangulation = 2,   // edges and non-lit faces
			HRenderSilhouette = 3,      // shell/mesh silhouette edges
			HRenderHiddenLine = 4,      // hidden line removal
			HRenderHiddenLineHOOPS = 5, // currently does nothing
			HRenderHiddenLineFast = 6,  // simulate hidden line (regular hardware z-buffer, with faces set to window background color)
			HRenderFlat = 7,            // flat shading
			HRenderGouraud = 8,         // gouraud (smooth) shading
			HRenderGouraudWithEdges = 9,// gouraud (smooth) shading with edges turned on
			HRenderPhong = 10,          // phong shading
			HRenderShaded = 11,         // indicates shaded rendering; underlying code will use phong if hardware accel is avialable, otherwise gouraud will be used
			HRenderShadedWithLines = 12,// same as HReanderShaded but with lines visible
			HRenderWireframeWithSilhouette = 13, // wireframe + silhouette
			HRenderShadedWireframe = 14,// wireframe, with shaded lines
			HRenderVertices = 15,		// vertices only
			HRenderShadedVertices = 16,	// vertices only (shaded)
			HRenderGouraudWithLines = 17,// same as HReanderShaded but with lines visible
			HRenderLOD1 = 18,			// clamps to LOD1
			HRenderLOD2 = 19,			// clamps to LOD2
			HRenderBRepHiddenLine = 20, //hidden line-ish removal
			HRenderBRepHiddenLineFast = 21, //hidden line-ish removal
			HRenderBRepWireframe = 22,  //brep wireframe
			HRenderGoochShaded = 23,	//Gooch Shaded
			HRenderFakeHiddenLine = 24, //fake hidden line
			HRenderUnknown = 0
		};

		enum DisplayListType
		{
			DisplayListOff,			// No display lists
			DisplayListGeometry,	// Geometry-level display lists
			DisplayListSegment		// Segment-level display lists
		};
		class API_3DF KernelOption : public Base
		{
		public:

			KernelOption();
			~KernelOption() override;

		public:

			Json::Object * Get() override;
			bool Set(Json::Object * pData) override;

			static CString ToHexString(COLORREF nColor);
			static COLORREF ToColor(CString strHexString);

			struct GENERAL
			{
				struct DISPLAY {
					CString Driver = L"dx11";
					CString Gpu = L"Default";
					bool DriverForceSoftware = false;		// CAppSettings::bDriverForceSoftware, Disable hardware accellation
					bool DriverDisplayStats = false;		// CAppSettings::bDriverDisplayStats, Display Statistics
					bool DoubleBuffer = true;				// CAppSettings::DoubleBuffer
					bool StereoMode = false;				// CAppSettings::StereoMode

					Json::Object * Get();
					bool Set(Json::Object * pData);
				}
				Display;

				struct TRANSPARENCY {
					CString Style = L"blended";
					CString Sorting = L"depth peeling";
					CString DepthPeelingLayers = L"3";
					bool PixelOIT = false;
					bool DepthWriting = false;

					Json::Object * Get();
					bool Set(Json::Object * pData);
				}
				Transparency;

				struct RENDERING
				{
					bool DisplayAxisTriad = true;			// CAppSettings::bDisplayAxisTriad

					Json::Object * Get();
					bool Set(Json::Object * pData);
				}
				Rendering;

				struct ETC {
					bool BackplaneCulling = false;			// CAppSettings::bBackplaneCulling

					Json::Object * Get();
					bool Set(Json::Object * pData);
				}
				Etc;

				Json::Object * Get();
				bool Set(Json::Object * pData);
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

					Json::Object * Get();
					bool Set(Json::Object * pData);
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

					Json::Object * Get();
					bool Set(Json::Object * pData);
				}
				FramerateOptimization;

				Json::Object * Get();
				bool Set(Json::Object * pData);
			}
			Performance;

			struct INTERACTION
			{
				struct GEOMETRY_MANIPULATION {
					bool Spriting = true;					// CAppSettings::Spriting
					bool UpdateCutGeometry = false;			// CAppSettings::UpdateCutGeometry
					bool UpdateShadows = true;				// CAppSettings::UpdateShadows

					Json::Object * Get();
					bool Set(Json::Object * pData);
				}
				GeometryManipulation;

				struct ANIMATION {
					bool UpdateCamera = true;				// CAppSettings::UpdateCamera

					Json::Object * Get();
					bool Set(Json::Object * pData);
				}
				Animation;

				Json::Object * Get();
				bool Set(Json::Object * pData);
			}
			Interaction;

			struct APPEARANCE
			{
				struct ANTIALIASING {
					bool Use = true;		// CAppSettings::bAntiAliasing
					bool Line = true;		// CAppSettings::LineAntialiasing
					bool Text = true;		// CAppSettings::TextAntialiasing
					int Level = 4;			// CAppSettings::AntialiasingLevel

					Json::Object * Get();
					bool Set(Json::Object * pData);
				}
				AntiAliasing;

				struct BACKGROUND_COLOR
				{
					COLORREF Top = RGB(0x30, 0x30, 0x30);
					COLORREF Bottom = RGB(0x30, 0x30, 0x30);

					Json::Object * Get();
					bool Set(Json::Object * pData);
				}
				BackgroundColor;

				Json::Object * Get();
				bool Set(Json::Object * pData);
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

					Json::Object * Get();
					bool Set(Json::Object * pData);
				}
				Behavior;

				struct HIGHLIGHT {
					CString QuickMovesType = L"Default";		// CAppSettings::csQuickMovesType, Quick Moves Preference

					Json::Object * Get();
					bool Set(Json::Object * pData);
				}
				Highlight;

				Json::Object * Get();
				bool Set(Json::Object * pData);
			}
			Selection;

			struct LIGHTING
			{
				struct BLOOM {
					bool Use = false;					// CAppSettings::UseBloom
					float Strength = 1.0;				// CAppSettings::BloomStrength
					int Blur = 5;						// CAppSettings::BloomBlur
					BloomShape Shape = Radial;			// CAppSettings::BloomShape

					Json::Object * Get();
					bool Set(Json::Object * pData);
				}
				Bloom;

				struct LIGHT {
					bool FollowsCamera = true;			// CAppSettings::LightFollowsCamera
					bool Scaling = true;				// CAppSettings::bLightScaling
					int ScaleFactor = 100000;			// CAppSettings::LightScaleFactor

					Json::Object * Get();
					bool Set(Json::Object * pData);
				}
				Light;

				Json::Object * Get();
				bool Set(Json::Object * pData);
			}
			Lighting;

			struct EFFECTS
			{
				struct SIMPLE_SHADOW {
					HShadowRenderingMode ShadowRenderingMode = SoftwareOpenglShadow;	// CAppSettings::ShadowRenderingMode

					// 				Json::Object* Get();
					// 				bool Set(Json::Object* pData);
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
					// 				bool Set(Json::Object* pData);
				}
				FrameBuffer;

				// 			Json::Object* Get();
				// 			bool Set(Json::Object* pData);
			}
			Effects;

			struct API_3DF VISUALEFFECTS
			{
				struct API_3DF SHADOW
				{
					bool checked = false;
					H3DF::VisualEffects::ShadowMode Mode = H3DF::VisualEffects::ShadowMode::Soft;
					int	Resolution = 3;
					int	Blurring = 1;
					bool IgnoreTransparency = false;
					CString Color = Json::Helper::ToString(RGB(80, 80, 80));
					int Opacity = 10;

					int GetResolution();
					int GetBlurring();
					H3DF::RGBAColor GetColor();
					float GetOpacity();

					Json::Object * Get();
					bool Set(Json::Object * pData);
				}
				Shadow;

				struct API_3DF PLANEREFLECTION
				{
					bool checked = false;
					int Opacity = 5;
					int Blurring = 1;
					bool Fading = false;

					float GetOpacity();
					int GetBlurring();

					Json::Object * Get();
					bool Set(Json::Object * pData);
				}
				PlaneReflection;

				struct API_3DF AMBIENTOCCLUSION
				{
					bool checked = false;
					int Strength = 1;
					int Quality = 0;	// Fast, Nicest

					float GetStrength();
					bool GetQuality();

					Json::Object * Get();
					bool Set(Json::Object * pData);
				}
				AmbientOcclusion;

				struct API_3DF SILHOUETTEEDGES
				{
					bool checked = false;
					int Tolerance = 1;
					bool HeavyExterior = true;

					float GetTolerance();

					Json::Object * Get();
					bool Set(Json::Object * pData);
				}
				SilhouetteEdges;

				struct API_3DF BLOOM
				{
					bool checked = false;
					int Strength = 2;
					int Blurring = 7;
					int Shape = 0;

					float GetStrength();
					int GetBlurring();

					Json::Object * Get();
					bool Set(Json::Object * pData);
				}
				Bloom;

				void SetChecked(bool bChecked);

				Json::Object * Get();
				bool Set(Json::Object * pData);
			}
			VisualEffects;
		};

		class API_3DF KernelPreset : public Base
		{
		public:

			KernelPreset() {}
			~KernelPreset() override {}

		public:

			Json::Object * Get() override {
				return nullptr;
			}
			bool Set(Json::Object * pData) override {
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
			
			int	MarkupWeight = 400;	// Markup line weight * 100 (to store float as int)
			bool CiByValue = true;// color interpolation, by value
			bool CiByColormapIndex = true;	// color interpolation, by colormap index
			bool CiIsolines = false;	// color interpolation, isolines only
			HSelectionHighlightMode HighlightMode = HighlightQuickmoves;
			float TransparencyLevel = 0.9f;
			CString RefSelType = "Spriting";
			HRenderMode RenderMode = HRenderGouraud;
			bool ShowCollisions = false;

			bool ShadowMap = false;
			int HardCutoff = 10;
			bool SplatRendering = false;	//use splat rendering
			bool FastMarkers = true;
			int SMResolution = 2048;
			int SMSamples = 4;
			bool Jitter = false;

/*
			m_pcBaseView->SetReflectionPlane(ThePreset.ReflectionPlane, ThePreset.ReflectionOpacity,
				ThePreset.ReflectionFading, ThePreset.ReflectionUseAttenuation,
				ThePreset.ReflectionHither, ThePreset.ReflectionYon,
				ThePreset.ReflectionUseBlur, ThePreset.ReflectionBlur);
*/


			bool UseLightVector = false;
			Point LightVector = Point(0.0f, 0.0f, 1.0f);
			
			bool StaticModel = true;
			bool LMVModel = true;
			bool RestoreAnnotations = false;
			DisplayListType DisplayList = DisplayListSegment;
			int	DynamicAdjustment = 1;

			

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
			Point AmbientUpVector = Point(0.0f, 1.0f, 0.0f);
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
}

extern H3DF::Facility::KernelPreset ThePreset;
