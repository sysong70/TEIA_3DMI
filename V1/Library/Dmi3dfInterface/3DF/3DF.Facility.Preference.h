#pragma once

#include "Json.h"

#include "3DF.Facility.Base.h"

#include <HBaseView.h>

OPEN_3DF_NAMESPACE

namespace Facility
{
	class Preference : public Base
	{
	public:
		Preference();
		~Preference() override;

	public:
		Json::Object * Get() override;
		bool Set(Json::Object * pcData) override;

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
				bool Set(Json::Object * pcData);
			}
			Display;

			struct TRANSPARENCY {
				CString Style = L"blended";
				CString Sorting = L"depth peeling";
				CString DepthPeelingLayers = L"3";
				bool PixelOIT = false;
				bool DepthWriting = false;

				Json::Object * Get();
				bool Set(Json::Object * pcData);
			}
			Transparency;

			struct RENDERING
			{
				bool DisplayAxisTriad = true;			// CAppSettings::bDisplayAxisTriad

				Json::Object * Get();
				bool Set(Json::Object * pcData);
			}
			Rendering;

			struct ETC {
				bool BackplaneCulling = false;			// CAppSettings::bBackplaneCulling

				Json::Object * Get();
				bool Set(Json::Object * pcData);
			}
			Etc;

			Json::Object * Get();
			bool Set(Json::Object * pcData);
		}
		General;

		struct PERFROMANCE 
		{
			struct OPTIMIZATION {
				HLRMode HiddenLineMode = FakeHiddenLine;	// CAppSettings::HiddenLineMode
				bool OcclusionCulling = false;				// CAppSettings::OcclusionCulling
				int OcclusionThreshold = 50;				// CAppSettings::OcclusionThreshold

				Json::Object * Get();
				bool Set(Json::Object * pcData);
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
				bool Set(Json::Object * pcData);
			}
			FramerateOptimization;

			Json::Object * Get();
			bool Set(Json::Object * pcData);
		}
		Perfromance;

		struct INTERACTION
		{
			struct GEOMETRY_MANIPULATION {
				bool Spriting = true;					// CAppSettings::Spriting
				bool UpdateCutGeometry = false;			// CAppSettings::UpdateCutGeometry
				bool UpdateShadows = true;				// CAppSettings::UpdateShadows

				Json::Object * Get();
				bool Set(Json::Object * pcData);
			}
			GeometryManipulation;

			struct ANIMATION {
				bool UpdateCamera = true;				// CAppSettings::UpdateCamera

				Json::Object * Get();
				bool Set(Json::Object * pcData);
			}
			Animation;

			Json::Object * Get();
			bool Set(Json::Object * pcData);
		}
		Interaction;

		struct APPEARANCE
		{
			struct ANTIALIASING {
				bool Use = true;		// CAppSettings::bAntiAliasing
				bool Line = false;		// CAppSettings::LineAntialiasing
				bool Text = false;		// CAppSettings::TextAntialiasing
				int Level = 4;			// CAppSettings::AntialiasingLevel

				Json::Object * Get();
				bool Set(Json::Object * pcData);
			}
			AntiAliasing;

			struct BACKGROUND_COLOR
			{
				COLORREF Top = RGB(59, 68, 83);
				COLORREF Bottom = RGB(59, 68, 83);

				Json::Object * Get();
				bool Set(Json::Object * pcData);
			}
			BackgroundColor;

			Json::Object * Get();
			bool Set(Json::Object * pcData);
		}
		Appearance;

		struct SELECTIION
		{
			struct BEHAVIOR {
				bool VisibilitySelection = false;		// CAppSettings::VisibilitySelection
				bool DynamicHighlighting = true;		// CAppSettings::DynamicHighlighting
				bool DetailSelection = false;			// CAppSettings::DetailSelection, Honor Line/Edge Weight/Pattern
				int RelatedSelectionLimit = 0;			// CAppSettings::RelatedSelectionLimit
				bool UseSelectBox = true;				// CAppSettings::bUseSelectBox, Transparent select box, show a transparent box when selecting areas
				bool RespectCulling = false;			// CAppSettings::SelectionRespectCulling, Respect Culling during selection.

				Json::Object * Get();
				bool Set(Json::Object * pcData);
			}
			Behavior;

			struct HIGHLIGHT {
				CString QuickMovesType = L"Default";		// CAppSettings::csQuickMovesType, Quick Moves Preference

				Json::Object * Get();
				bool Set(Json::Object * pcData);
			}
			Highlight;

			Json::Object * Get();
			bool Set(Json::Object * pcData);
		}
		Selection;

		struct LIGHTING
		{
			struct BLOOM {
				bool Use = false;					// CAppSettings::UseBloom
				float Strength = 1.0;				// CAppSettings::BloomStrength
				int Blur = 5;						// CAppSettings::BloomBlur
				HBloomShape Shape = RadialBloom;	// CAppSettings::BloomShape

				Json::Object * Get();
				bool Set(Json::Object * pcData);
			}
			Bloom;

			struct LIGHT {
				bool FollowsCamera = true;			// CAppSettings::LightFollowsCamera
				bool Scaling = true;				// CAppSettings::bLightScaling
				int ScaleFactor = 100000;			// CAppSettings::LightScaleFactor

				Json::Object * Get();
				bool Set(Json::Object * pcData);
			}
			Light;

			Json::Object * Get();
			bool Set(Json::Object * pcData);
		}
		Lighting;

		struct EFFECTS
		{
			struct SIMPLE_SHADOW {
				HShadowRenderingMode ShadowRenderingMode = SoftwareOpenglShadow;	// CAppSettings::ShadowRenderingMode

// 				Json::Object * Get();
// 				bool Set(Json::Object * pcData);
			}
			SimpleShadow;

			struct FRAMEBUFFER {
				bool UseAmbient = false;				// CAppSettings::UseFastAmbient, Ambient Occlusion
				float AmbientStrength = 1.0f;			// CAppSettings::FastAmbientStrength
				bool HighQualityAmbient = false;		// CAppSettings::HQAmbientOcclusion
				bool UseFastSilhouette = false;			// CAppSettings::UseFastSilhouette
				float FastSilhouetteTolerance = 1.0f;	// CAppSettings::FastSilhouetteTolerance
				bool HeavyExteriorSilhouette = false;	// CAppSettings::HeavyExteriorSilhouette

// 				Json::Object * Get();
// 				bool Set(Json::Object * pcData);
			}
			FrameBuffer;

// 			Json::Object * Get();
// 			bool Set(Json::Object * pcData);
		}
		Effects;
	};
}

CLOSE_3DF_NAMESPACE