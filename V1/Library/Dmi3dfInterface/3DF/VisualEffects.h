#pragma once

#include "3DF.h"

#include "Kit.h"
#include "Control.h"
#include "Color.h"
#include "Math.h"

namespace H3DF
{
	class API_3DF VisualEffectsControl : public Control
	{
	public:
		VisualEffectsControl(SegmentKey & cInSegmentKey);
		VisualEffectsControl(VisualEffectsControl const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::VisualEffectsControl;
		H3DF::Type ObjectType() const { return staticType; };

		void Set(VisualEffectsControl const & cInThat);
		VisualEffectsControl & operator = (VisualEffectsControl const & cInThat);

		VisualEffectsControl & SetPostProcessEffectsEnabled(bool bInState);

		VisualEffectsControl & SetAmbientOcclusionEnabled(bool bInState, float fStrength = 5.0f, bool bFast = true);
		VisualEffectsControl & SetSilhouetteEdgesEnabled(bool bInState, float fTolerance = 1.0f, bool bHeavyExterior = true);
		VisualEffectsControl & SetBloomEnabled(bool bInState, float fStrength = 1.f, int Blurring = 5, int nShape = 0);

		VisualEffectsControl & SetAntiAliasing(bool bInState);
		VisualEffectsControl & SetTextAntiAliasing(bool bInState);
		VisualEffectsControl & SetLineAntiAliasing(bool bInState);

		VisualEffectsControl & SetSimpleShadow(bool bInState, H3DF::VisualEffects::ShadowMode eMode = H3DF::VisualEffects::ShadowMode::Soft, 
			UINT nInResolution = 256, UINT nInBlurring = 1, bool bInIgnoreTransparency = false);

	/*
		// Manipulates the state of simple shadows, projected onto a plane.  The state is implicitly on.
		// param: in_resolution The width and height of the simple shadow. Valid range is [32, 1024].
		// param: in_blurring The level of blurring (softening) that is applied to the shadow.  Valid range is [1,31].
		// param: in_ignore_transparency Whether any segment-level transparency setting should be ignored when rendering the simple shadow.
		VisualEffectsControl & SetSimpleShadow(unsigned int in_resolution, unsigned int in_blurring, bool in_ignore_transparency);

		// Manipulates the location and orientation of the plane that simple shadows are projected onto.
		// param: in_projected_onto The plane that simple shadows should be projected onto.
		VisualEffectsControl & SetSimpleShadowPlane(Plane const & in_projected_onto = Plane(0.0f, 1.0f, 0.0f, 1.0f));

		// Sets the direction that the light source for a simple shadow is coming from.
		// param: in_direction The direction that the light source for a simple shadow is coming from.
		VisualEffectsControl & SetSimpleShadowLightDirection(Vector const & in_direction = Vector(0.0f, 1.0f, 0.0f));
	*/

		VisualEffectsControl & SetSimpleShadowColor(RGBAColor const & cInColor = RGBAColor(0.0f, 0.0f, 0.0f, 1.0f));

		VisualEffectsControl & SetSimpleReflection(bool bInState, float fInOpacity = 0.5f, unsigned int nInBlurring = 1, bool bInFading = false, 
			float fInAttenuationNearDistance = -1.0f, float fInAttenuationFarDistance = -1.0f);

		//== Unset Functions =======================================================================
		VisualEffectsControl & UnsetPostProcessEffectsEnabled();
		VisualEffectsControl & UnsetAmbientOcclusionEnabled();
		VisualEffectsControl & UnsetSilhouetteEdgesEnabled();
		VisualEffectsControl & UnsetBloomEnabled();

		VisualEffectsControl & UnsetAntiAliasing();

		VisualEffectsControl & UnsetSimpleShadow();

		//== Show Functions ========================================================================
		bool ShowPostProcessEffectsEnabled(bool & bOutState) const;
		bool ShowAmbientOcclusionEnabled(bool & bOutState) const;
		bool ShowSilhouetteEdgesEnabled(bool & bOutState) const;
		bool ShowBloomEnabled(bool & bOutState) const;

		bool ShowAntiAliasing(bool & bOutState) const;
		bool ShowTextAntiAliasing(bool & bOutState) const;
		bool ShowLineAntiAliasing(bool & bOutState) const;

		bool ShowSimpleShadowColor(H3DF::RGBAColor & cOutColor) const;
	};
}

