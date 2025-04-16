#include "StdAfx.h"

#include "VisualEffects.h"
#include "./Impl/ControlImpl.h"

#include "Segment.h"
#include "./Impl/SegmentImpl.h"

#include "./Impl/WindowImpl.h"

#include <Common_Define.h>

#include <HUtility.h>
#include <HTools.h>

using namespace H3DF;

class VisualEffectsControlImpl : public ControlImpl
{
public:
	VisualEffectsControlImpl() { m_eType = H3DF::Type::VisualEffectsControl; }

	void Copy(VisualEffectsControlImpl * pcInThat) {
		ControlImpl::Copy(pcInThat);
	}

	BaseView * GetBaseView();
};

BaseView * VisualEffectsControlImpl::GetBaseView()
{
	SegmentKeyImpl * pcKeyImpl = static_cast<SegmentKeyImpl *>(m_cOverrideKey.GetImpl());
	DEBUG_VALID(pcKeyImpl);

	return pcKeyImpl->GetBaseView();
}


H3DF::VisualEffectsControl::VisualEffectsControl(SegmentKey & cInSegmentKey)
{
	VisualEffectsControlImpl * pcImpl = new VisualEffectsControlImpl();
	pcImpl->m_cOverrideKey = cInSegmentKey;

	m_pcImpl = pcImpl;
}

H3DF::VisualEffectsControl::VisualEffectsControl(VisualEffectsControl const & cInThat)
{
	m_pcImpl = new VisualEffectsControlImpl();
	Set(cInThat);
}

void H3DF::VisualEffectsControl::Set(VisualEffectsControl const & cInThat)
{
	VisualEffectsControlImpl * pcImpl = (VisualEffectsControlImpl *) m_pcImpl;
	VisualEffectsControlImpl * pcInThatImpl = (VisualEffectsControlImpl *) cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

VisualEffectsControl & H3DF::VisualEffectsControl::operator = (VisualEffectsControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

//== Set Functions =================================================================================

VisualEffectsControl & H3DF::VisualEffectsControl::SetPostProcessEffectsEnabled(bool bInState)
{
	VisualEffectsControlImpl * pcImpl = static_cast<VisualEffectsControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		CStringA strOption;
		strOption.Format("frame buffer effects = %s", (bInState ? "on" : "off"));
		HC_Set_Rendering_Options(strOption);

	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

// Allows ambient occlusion to be enabled or disabled on a per segment basis.
// param: in_state Whether ambient occlusion should be used.
VisualEffectsControl & H3DF::VisualEffectsControl::SetAmbientOcclusionEnabled(bool bInState, float fStrength, bool bFast)
{
	VisualEffectsControlImpl * pcImpl = static_cast<VisualEffectsControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	BaseView * pcBaseView = pcImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	if (nullptr != pcBaseView) {
		HC_Open_Segment_By_Key(pcBaseView->GetViewKey()); {
			CStringA strOption;
			strOption.Format("ambient occlusion = (%s, strength = %f, quality = %s)",
				(bInState ? "on" : "off"), fStrength, bFast ? "fast" : "nicest");
			HC_Set_Driver_Options(strOption);
		} HC_Close_Segment();
	}

	return *this;
}

VisualEffectsControl & H3DF::VisualEffectsControl::SetSilhouetteEdgesEnabled(bool bInState, float fTolerance, bool bHeavyExterior)
{
	VisualEffectsControlImpl * pcImpl = static_cast<VisualEffectsControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	BaseView * pcBaseView = pcImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	if (nullptr != pcBaseView) {
		bool bHeavyExteriorSilhouette = true;

		HC_Open_Segment_By_Key(pcBaseView->GetViewKey()); {
			CStringA strOption;
			strOption.Format("fast silhouette edges = (%s, tolerance = %f, %s heavy exterior)",
				(bInState ? "on" : "off"),
				fTolerance,
				(bHeavyExterior ? "" : "no"));
			HC_Set_Driver_Options(strOption);
		} HC_Close_Segment();
	}

	return *this;
}

VisualEffectsControl & H3DF::VisualEffectsControl::SetBloomEnabled(bool bInState, float fStrength, int nBlurring, int nShape)
{
	VisualEffectsControlImpl * pcImpl = static_cast<VisualEffectsControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	BaseView * pcBaseView = pcImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	if (nullptr != pcBaseView) {
		HC_Open_Segment_By_Key(pcBaseView->GetViewKey()); {
			CStringA strOption;
			strOption.Format("bloom = (%s, strength = %f, blur = %d, shape = %s)",
				(bInState ? "on" : "off"),
				fStrength,
				nBlurring,
				(nShape == (int)RadialBloom ? "radial" : "star"));
			HC_Set_Driver_Options(strOption);
		} HC_Close_Segment();
	}

	return *this;
}

// Manipulates the state of anti - aliasing(text and screen).
VisualEffectsControl & H3DF::VisualEffectsControl::SetAntiAliasing(bool bInState)
{
	VisualEffectsControlImpl * pcImpl = (VisualEffectsControlImpl *) m_pcImpl;

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		CStringA strOption;

		if (true == bInState) {
			strOption = "anti-alias = (screen = on)";
		}
		else {
			strOption = "anti-alias = (screen = off)";
		}

		HC_Set_Rendering_Options(strOption);

	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

VisualEffectsControl & H3DF::VisualEffectsControl::SetTextAntiAliasing(bool bInState)
{
	VisualEffectsControlImpl * pcImpl = (VisualEffectsControlImpl *) m_pcImpl;

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		CString strOption;

		if (true == bInState) {
			strOption = L"anti-alias = (text = on)";
		}
		else {
			strOption = L"anti-alias = (text = off)";
		}

		HC_Set_Rendering_Options(Utility::ToChar(strOption));

	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

VisualEffectsControl & H3DF::VisualEffectsControl::SetLineAntiAliasing(bool bInState)
{
	VisualEffectsControlImpl * pcImpl = (VisualEffectsControlImpl *) m_pcImpl;

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		CString strOption;

		if (true == bInState) {
			strOption = L"anti-alias = (lines = on)";
		}
		else {
			strOption = L"anti-alias = (lines = off)";
		}

		HC_Set_Rendering_Options(Utility::ToChar(strOption));

	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

//	Manipulates the state of simple shadows, projected onto a plane.
//	param: in_state Whether simple shadows should be used.
//	param: in_resolution The width and height of the simple shadow.  Valid range is [32, 1024].
//	param: in_blurring The level of blurring (softening) that is applied to the shadow.  Valid range is [1,31].
//	param: in_ignore_transparency Whether any segment-level transparency setting should be ignored when rendering the simple shadow.
VisualEffectsControl & H3DF::VisualEffectsControl::SetSimpleShadow(bool bInState, H3DF::VisualEffects::ShadowMode eMode, UINT nInResolution, UINT nInBlurring, bool bInIgnoreTransparency)
{
	VisualEffectsControlImpl * pcImpl = static_cast<VisualEffectsControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	BaseView * pcBaseView = pcImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	if (nullptr != pcBaseView) {
		if (true == bInState) {
			if (H3DF::VisualEffects::ShadowMode::Soft == eMode) {
				// HShadowHard 모드에서 생성된 Segment를 Flush 하기 위해서 HShadowNone을 실행시켜야 함.
				pcBaseView->SetShadowMode(HShadowMode::HShadowNone);

				pcBaseView->SetShadowMode(HShadowMode::HShadowSoft);
			}
			else if(H3DF::VisualEffects::ShadowMode::Hard == eMode) {
				pcBaseView->SetShadowMode(HShadowMode::HShadowHard);
			}
			else {	
				pcBaseView->SetShadowMode(HShadowMode::HShadowNone);
			}
		}
		else {
			pcBaseView->SetShadowMode(HShadowMode::HShadowNone);
		}

		if (32 <= nInResolution && nInResolution <= 1024) {
			pcBaseView->SetShadowResolution(nInResolution);
		}

		if (1 <= nInBlurring && nInBlurring <= 31) {
			pcBaseView->SetShadowBlurring(nInBlurring);
		}

		pcBaseView->SetShadowIgnoresTransparency(bInIgnoreTransparency);
	}

	return *this;
}

// Sets the color that simple shadows will be rendered in.
// param: in_color The color of simple shadows.
VisualEffectsControl & H3DF::VisualEffectsControl::SetSimpleShadowColor(RGBAColor const & cInColor)
{
	VisualEffectsControlImpl * pcImpl = static_cast<VisualEffectsControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	BaseView * pcBaseView = pcImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	if (nullptr != pcBaseView) {
		pcBaseView->SetShadowColor(HPoint(cInColor.red, cInColor.green, cInColor.blue));

		char chOption[MVO_BUFFER_SIZE];
		HC_Open_Segment_By_Key(pcBaseView->GetSceneKey()); {
			sprintf(chOption, "simple shadow = (opacity = %f)", cInColor.alpha);
			HC_Set_Rendering_Options(chOption);
		} HC_Close_Segment();
	}

	return *this;
}

// Manipulates the state of simple reflections, projected onto a plane.
// param: bInState Whether simple shadows should be used.
// param: fInOpacity An alpha value that sets the transparency level of the reflection plane.  Valid range is [0,1] with a value of zero making the plane completely transparent.
// param: nInBlurring The level of blurring (softening) that is applied to the shadow.  Valid range is [1,31].
// param: bInFading Whether the reflection plane should fade as it moves away from the camera.
// param: in_attenuation_near_distance The orthogonal distances (in world space) from the reflection plane that the model begins to fade.
//        Attenuation begins at in_attenuation_near_distance and increases linearly such that the model is not visible in the reflection beyond in_attenuation_far_distance.
// param: in_attenuation_far_distance The orthogonal distances (in world space) from the reflection plane that the model is completely faded.
//        Attenuation begins at in_attenuation_near_distance and increases linearly such that the model is not visible in the reflection beyond in_attenuation_far_distance.

VisualEffectsControl & H3DF::VisualEffectsControl::SetSimpleReflection(bool bInState, float fInOpacity, unsigned int nInBlurring, bool bInFading, float fInAttenuationNearDistance, float fInAttenuationFarDistance)
{
	VisualEffectsControlImpl * pcImpl = static_cast<VisualEffectsControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	BaseView * pcBaseView = pcImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	if (nullptr != pcBaseView) {
		bool bBlurring = false;
		if (1 <= nInBlurring && nInBlurring <= 31) {
			bBlurring = true;
		}

		bool bAttenuate = true;

		if (true == Float::IsInfinite(fInAttenuationFarDistance)) {
			bAttenuate = false;
		}

		pcBaseView->SetReflectionPlane(bInState, fInOpacity, bInFading, bAttenuate, fInAttenuationNearDistance, fInAttenuationFarDistance, bBlurring, nInBlurring);
	}

	return *this;
}

//== Unset Functions ===============================================================================

VisualEffectsControl & H3DF::VisualEffectsControl::UnsetPostProcessEffectsEnabled()
{
	VisualEffectsControlImpl * pcImpl = static_cast<VisualEffectsControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_UnSet_One_Rendering_Option("frame buffer effects");
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}


VisualEffectsControl & H3DF::VisualEffectsControl::UnsetAmbientOcclusionEnabled()
{
	VisualEffectsControlImpl * pcImpl = static_cast<VisualEffectsControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	BaseView * pcBaseView = pcImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	if (nullptr != pcBaseView) {
		HC_Open_Segment_By_Key(pcBaseView->GetViewKey()); {
			HC_UnSet_One_Driver_Option("ambient occlusion");
		} HC_Close_Segment();
	}

	return *this;
}

VisualEffectsControl & H3DF::VisualEffectsControl::UnsetSilhouetteEdgesEnabled()
{
	VisualEffectsControlImpl * pcImpl = static_cast<VisualEffectsControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	BaseView * pcBaseView = pcImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	if (nullptr != pcBaseView) {
		HC_Open_Segment_By_Key(pcBaseView->GetViewKey()); {
			HC_UnSet_One_Driver_Option("fast silhouette edges");
		} HC_Close_Segment();
	}

	return *this;
}

VisualEffectsControl & H3DF::VisualEffectsControl::UnsetBloomEnabled()
{
	VisualEffectsControlImpl * pcImpl = static_cast<VisualEffectsControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	BaseView * pcBaseView = pcImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	if (nullptr != pcBaseView) {
		HC_Open_Segment_By_Key(pcBaseView->GetViewKey()); {
			HC_UnSet_One_Driver_Option("bloom");
		} HC_Close_Segment();
	}

	return *this;
}

VisualEffectsControl & H3DF::VisualEffectsControl::UnsetAntiAliasing()
{
	VisualEffectsControlImpl * pcImpl = static_cast<VisualEffectsControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_UnSet_One_Rendering_Option("anti-alias");
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

VisualEffectsControl & H3DF::VisualEffectsControl::UnsetSimpleShadow()
{
	VisualEffectsControlImpl * pcImpl = static_cast<VisualEffectsControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	BaseView * pcBaseView = pcImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	if (nullptr != pcBaseView) {
		HC_Open_Segment_By_Key(pcBaseView->GetSceneKey()); {
			HC_UnSet_One_Rendering_Option("simple shadow");
		}HC_Close_Segment();
	}

	return *this;
}

//== Show Functions ================================================================================
bool H3DF::VisualEffectsControl::ShowPostProcessEffectsEnabled(bool & bOutState) const
{
	VisualEffectsControlImpl * pcImpl = static_cast<VisualEffectsControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	bool bResult = false;

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		char chValue[MVO_BUFFER_SIZE];
		HC_Show_One_Rendering_Option("frame buffer effects", chValue);

		if (0 == strlen(chValue)) {
			bResult = false;
		}
		else if (nullptr != strstr(chValue, "on")) {
			bOutState = true;
			bResult = true;
		}
		else if (nullptr != strstr(chValue, "off")) {
			bOutState = false;
			bResult = true;
		}
		else {
			bOutState = false;
			bResult = true;
			DEBUG_STOP;
		}

	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return bResult;
}

bool H3DF::VisualEffectsControl::ShowAmbientOcclusionEnabled(bool & bOutState) const
{
	VisualEffectsControlImpl * pcImpl = static_cast<VisualEffectsControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	BaseView * pcBaseView = pcImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	if (nullptr != pcBaseView) {
		HC_Open_Segment_By_Key(pcBaseView->GetViewKey()); {
			char chValue[MVO_BUFFER_SIZE];
			HC_Show_One_Driver_Option("ambient occlusion", chValue);
			if (nullptr != strstr(chValue, "on")) {
				bOutState = true;
			}
			else {
				bOutState = false;
			}
		} HC_Close_Segment();
	}
	else {
		return false;
	}

	return true;
}

bool H3DF::VisualEffectsControl::ShowSilhouetteEdgesEnabled(bool & bOutState) const
{
	VisualEffectsControlImpl * pcImpl = static_cast<VisualEffectsControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	BaseView * pcBaseView = pcImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	bool bStatus = false;

	if (nullptr != pcBaseView) {
		HC_Open_Segment_By_Key(pcBaseView->GetViewKey()); {
			char chValue[MVO_BUFFER_SIZE];
			HC_Show_One_Driver_Option("fast silhouette edges", chValue);

			if (0 < strlen(chValue)) {
				bStatus = true;
			}

			if (nullptr != strstr(chValue, "on")) {
				bOutState = true;
			}
			else {
				bOutState = false;
			}
		} HC_Close_Segment();
	}

	return bStatus;
}

bool H3DF::VisualEffectsControl::ShowBloomEnabled(bool & bOutState) const
{
	VisualEffectsControlImpl * pcImpl = static_cast<VisualEffectsControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	BaseView * pcBaseView = pcImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	bool bStatus = false;

	if (nullptr != pcBaseView) {
		HC_Open_Segment_By_Key(pcBaseView->GetViewKey()); {
			char chValue[MVO_BUFFER_SIZE];
			HC_Show_One_Driver_Option("bloom", chValue);

			if (0 < strlen(chValue)) {
				bStatus = true;
			}

			if (nullptr != strstr(chValue, "on")) {
				bOutState = true;
			}
			else {
				bOutState = false;
			}
		} HC_Close_Segment();
	}

	return bStatus;
}
bool H3DF::VisualEffectsControl::ShowAntiAliasing(bool & bOutState) const
{
	VisualEffectsControlImpl * pcImpl = (VisualEffectsControlImpl *) m_pcImpl;

	bool bResult = false;

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		char chValue[MVO_BUFFER_SIZE];
		HC_Show_One_Rendering_Option("anti-alias", chValue);

		if (0 == strlen(chValue)) {
			bResult = false;
		}
		else if (NULL != strstr(chValue, "no screen")) {
			bOutState = false;
			bResult = true;
		}
		else if (NULL != strstr(chValue, "screen")) {
			bOutState = true;
			bResult = true;
		}
		else {
			bOutState = false;
			bResult = true;
			DEBUG_STOP;
		}

	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return bResult;
}

bool H3DF::VisualEffectsControl::ShowTextAntiAliasing(bool & bOutState) const
{
	VisualEffectsControlImpl * pcImpl = (VisualEffectsControlImpl *) m_pcImpl;

	bool bResult = false;

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		char chValue[MVO_BUFFER_SIZE];
		HC_Show_One_Rendering_Option("anti-alias = text", chValue);

		if (0 == strlen(chValue)) {
			bResult = false;
		}
		// chValue값에 no text가 포함되어 있으면 false
		else if (NULL != strstr(chValue, "no text")) {
			bOutState = false;
			bResult = true;
		}
		else if (NULL != strstr(chValue, "text")) {
			bOutState = true;
			bResult = true;
		}
		else if (NULL != strstr(chValue, "screen")) {
			bOutState = true;
			bResult = true;
		}
		else {
			bOutState = false;
			bResult = true;
			DEBUG_STOP;
		}
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return bResult;
}

bool H3DF::VisualEffectsControl::ShowLineAntiAliasing(bool & bOutState) const
{
	VisualEffectsControlImpl * pcImpl = (VisualEffectsControlImpl *) m_pcImpl;

	bool bResult = false;

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		char chValue[MVO_BUFFER_SIZE];
		HC_Show_One_Rendering_Option("anti-alias", chValue);

		if (0 == strlen(chValue)) {
			bResult = false;
		}
		// chValue값에 no line이 포함되어 있으면 false
		else if (NULL != strstr(chValue, "no lines")) {
			bOutState = false;
			bResult = true;
		}
		else if (NULL != strstr(chValue, "lines")) {
			bOutState = true;
			bResult = true;
		}
		else if (NULL != strstr(chValue, "screen")) {
			bOutState = true;
			bResult = true;
		}
		else {
			bOutState = false;
			bResult = true;
			DEBUG_STOP;
		}
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return bResult;
}

bool H3DF::VisualEffectsControl::ShowSimpleShadowColor(RGBAColor & cOutColor) const
{
	VisualEffectsControlImpl * pcImpl = (VisualEffectsControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	BaseView * pcBaseView = pcImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	bool bResult = false;

	if (nullptr != pcBaseView) {
		char chColorSpace[4] = { "" };

		HC_Open_Segment_By_Key(pcBaseView->GetSceneKey()); {
			HC_Open_Segment("shadows"); {
				if (TRUE == HC_Show_Existence("color")) {
					HC_Show_One_Color_By_Value("faces", chColorSpace, &cOutColor.red, &cOutColor.green, &cOutColor.blue);
					bResult = true;
				}
			} HC_Close_Segment();
		} HC_Close_Segment();
	}

	return bResult;
}