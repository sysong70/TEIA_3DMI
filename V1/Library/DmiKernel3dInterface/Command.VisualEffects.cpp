#include "StdAfx.h"

#include "Command.VisualEffects.h"

#include "Impl/CommandImpl.h"

#include "Signal.Connector.h"
#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

#include "../../UiMain/Command.Resource.h"

#include <Sprocket/3DF.View.h>
#include <3DF/Window.h>
#include <3DF/VisualEffects.h>
#include <3DF/Facility.AppOptions.h>

#include <Json.h>

using namespace KERNEL;

#define TheKenel TheAppOptions.Kernel
#define TheVisualEffects H3DF::Facility::KernelOption::VISUALEFFECTS

namespace KERNEL
{
	namespace Command
	{
		class VisualEffectsImpl : public CommandImpl
		{
		public:
			VisualEffectsImpl(const Session * pcInSession);

			void Copy(VisualEffectsImpl * pcInThat) {
				CommandImpl::Copy(pcInThat);
			}

			void Request(Json::Object & cInObject);

			void Change(Json::Object & cInObject);

		protected:
			bool SetSetting(Json::Object & cInObject);

			void SetShadow(TheVisualEffects::SHADOW & cInOption);

			void SetPlaneReflection(TheVisualEffects::PLANEREFLECTION & cInOption);

			void SetAmbientOcclusion(TheVisualEffects::AMBIENTOCCLUSION & cInOption);

			void SetSilhouetteEdges(TheVisualEffects::SILHOUETTEEDGES & cInOption);

			void SetBloom(TheVisualEffects::BLOOM & cInOption);

		private:
			TheVisualEffects m_cOption;
			Json::Object * m_pcDefaultSetting = nullptr; // Default Setting 값을 저장하는 Json Object
		};
	}
}

KERNEL::Command::VisualEffectsImpl::VisualEffectsImpl(const Session * pcInSession)
	: CommandImpl(pcInSession)
{
	// 변경된 전역 Visual Effect 값을 가져온다.
	m_cOption.Set(TheKenel.VisualEffects.Get());

	// 값들 중에서 checked 값들을 사용안함으로 수정한다.
	m_cOption.SetChecked(false);

	m_pcDefaultSetting = (new TheVisualEffects)->Get();
}

//== 전달된 명령어 처리 함수 ===========================================================================

// 1. UI에서 전달되는 요청사항을 처리하는 최초 함수
void KERNEL::Command::VisualEffectsImpl::Request(Json::Object & cInObject)
{
	Json::Object cCopy(*m_cOption.Get());

	CString strText;
	cCopy.Stringify(strText);

	Delivery().ViewId = cInObject.GetInteger(SKW_VIEWID);
	Delivery().taskBar.ResponseValue(HOME_3D_LST_VisualEffects, *m_cOption.Get(), *m_pcDefaultSetting);
}

void KERNEL::Command::VisualEffectsImpl::Change(Json::Object & cInObject)
{
	// cInObject에 Value값이 있는 경우 처리 (UI에서 변경된 값을 전달한 경우 처리)
	// 변경값이 있는 경우에만, 변경값에 따라서 Visual Effect 변경을 처리한다.
	Json::Object & cInSetting = cInObject.GetObject(SKW_VALUE);
	if (true == cInSetting.IsEmpty()) {
		DEBUG_RETURN;
	}

	TheVisualEffects cInOption;
	cInOption.Set(&cInSetting);

	TheKenel.VisualEffects.Set(&cInSetting);
		
	SetShadow(cInOption.Shadow);

	SetPlaneReflection(cInOption.PlaneReflection);

	SetAmbientOcclusion(cInOption.AmbientOcclusion);

	SetSilhouetteEdges(cInOption.SilhouetteEdges);

	SetBloom(cInOption.Bloom);

	m_cOption.Set(&cInSetting);

	View().Update();
}

bool KERNEL::Command::VisualEffectsImpl::SetSetting(Json::Object & cInObject)
{
	H3DF::Facility::KernelOption::VISUALEFFECTS cVisaulEffect;
	cVisaulEffect.Set(&cInObject);

	return true;
}

void KERNEL::Command::VisualEffectsImpl::SetShadow(TheVisualEffects::SHADOW & cInOption)
{
	int nResolution = cInOption.GetResolution();
	int nBlurring = cInOption.GetBlurring();

	View().GetSegmentKey().GetVisualEffectsControl().SetSimpleShadow(
		cInOption.checked, cInOption.Mode, nResolution, nBlurring, cInOption.IgnoreTransparency)
		.SetSimpleShadowColor(cInOption.GetColor());
}

void KERNEL::Command::VisualEffectsImpl::SetPlaneReflection(TheVisualEffects::PLANEREFLECTION & cInOption)
{
	float fOpacity = cInOption.GetOpacity();
	int nBlurring = cInOption.GetBlurring();
	bool bFading = cInOption.Fading;

	View().GetSegmentKey().GetVisualEffectsControl().SetSimpleReflection(cInOption.checked, fOpacity, nBlurring, bFading);
}

void KERNEL::Command::VisualEffectsImpl::SetAmbientOcclusion(TheVisualEffects::AMBIENTOCCLUSION & cInOption)
{
	float fStrength = cInOption.GetStrength();
	bool bFast = cInOption.GetQuality();

	View().GetSegmentKey().GetVisualEffectsControl().SetAmbientOcclusionEnabled(cInOption.checked, fStrength, bFast);
}

void KERNEL::Command::VisualEffectsImpl::SetSilhouetteEdges(TheVisualEffects::SILHOUETTEEDGES & cInOption)
{
	float fTolerance = cInOption.GetTolerance();
	bool bHeavyExterior = cInOption.HeavyExterior;

	View().GetSegmentKey().GetVisualEffectsControl().SetSilhouetteEdgesEnabled(cInOption.checked, fTolerance, bHeavyExterior);
}

void KERNEL::Command::VisualEffectsImpl::SetBloom(TheVisualEffects::BLOOM & cInOption)
{
	float fStrength = cInOption.GetStrength();
	int nBlurring = cInOption.GetBlurring();

	View().GetSegmentKey().GetVisualEffectsControl().SetBloomEnabled(cInOption.checked, 
		fStrength, nBlurring, cInOption.Shape);
}

//== Visual Effects class ==========================================================================

KERNEL::Command::VisualEffects::VisualEffects(const Session * pcInSession)
{
	VisualEffectsImpl * pcImpl = new VisualEffectsImpl(pcInSession);
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;
}

void KERNEL::Command::VisualEffects::Request(Json::Object & cInObject)
{
	VisualEffectsImpl * pcImpl = (VisualEffectsImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->Request(cInObject);
}

void KERNEL::Command::VisualEffects::Change(Json::Object & cInObject)
{
	VisualEffectsImpl * pcImpl = (VisualEffectsImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->Change(cInObject);
}
