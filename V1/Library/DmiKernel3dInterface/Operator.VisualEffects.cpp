#include "StdAfx.h"

#include "OPERATOR.VisualEffects.h"

#include "Private/OperatorPrivate.h"

#include "Signal.Connector.h"
#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

#include "../../UiMain/Command.Resource.h"

#include <3DF.View.h>
#include <3DF/Window.h>
#include <3DF/VisualEffects.h>
#include <3DF/Facility.AppOptions.h>

#include <Json.h>

using namespace KERNEL;

#define TheKenel TheAppOptions.Kernel
#define TheVisualEffects H3DF::Facility::KernelOption::VISUALEFFECTS

namespace KERNEL
{
	namespace Operator
	{
		class VisualEffectsPrivate : public OperatorPrivate
		{
		public:
			VisualEffectsPrivate(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery);

			void Copy(VisualEffectsPrivate * pcInThat) {
				OperatorPrivate::Copy(pcInThat);
			}

			void Request(Json::Object & cInObject);
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

KERNEL::Operator::VisualEffectsPrivate::VisualEffectsPrivate(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery)
	: OperatorPrivate(pcInView, pcInDelivery)
{
	m_cOption.Set(TheKenel.VisualEffects.Get());
	m_pcDefaultSetting = (new TheVisualEffects)->Get();
}

//== 전달된 명령어 처리 함수 ===========================================================================

// 1. UI에서 전달되는 요청사항을 처리하는 최초 함수
void KERNEL::Operator::VisualEffectsPrivate::Request(Json::Object & cInObject)
{
	// cInObject에 Value값이 있는 경우 처리 (UI에서 변경된 값을 전달한 경우 처리)
	// 변경값이 있는 경우에만, 변경값에 따라서 Visual Effect 변경을 처리한다.
	Json::Object & cInSetting = cInObject.GetObject(SKW_VALUE);
	if (false == cInSetting.IsEmpty()) {
		// 현재 설정값과 같은지 여부를 확인.
	// 	if (*m_pcSetting == cInObject) {
	// 	   DEBUG_VAILD(false);
	// 		return false;
	// 	}

		TheVisualEffects cInOption;
		cInOption.Set(&cInSetting);

		TheKenel.VisualEffects.Set(&cInSetting);
		
		SetShadow(cInOption.Shadow);

		SetPlaneReflection(cInOption.PlaneReflection);

		SetAmbientOcclusion(cInOption.AmbientOcclusion);

		SetSilhouetteEdges(cInOption.SilhouetteEdges);

		SetBloom(cInOption.Bloom);

		m_cOption.Set(&cInSetting);

// 		CString strText1;
// 		cInSetting.Stringify(strText1);
		//View(). GetSegmentKey().GetVisualEffectsControl().SetVisualEffects(m_cOption);

		//View().SetSimpleShadow(true);

		//View().SetSimpleReflection(true);

		View().Update();
	}

	Delivery().taskBar.ResponseValue(HOME_3D_LST_VisualEffects, *m_cOption.Get(), *m_pcDefaultSetting);
}

bool KERNEL::Operator::VisualEffectsPrivate::SetSetting(Json::Object & cInObject)
{
	H3DF::Facility::KernelOption::VISUALEFFECTS cVisaulEffect;
	cVisaulEffect.Set(&cInObject);

	return true;
}

void KERNEL::Operator::VisualEffectsPrivate::SetShadow(TheVisualEffects::SHADOW & cInOption)
{
	if (m_cOption.Shadow.checked == cInOption.checked) {
		return;
	}

	int nResolution = cInOption.GetResolution();
	int nBlurring = cInOption.GetBlurring();

	View().GetSegmentKey().GetVisualEffectsControl().SetSimpleShadow(
		cInOption.checked, nResolution, nBlurring, cInOption.IgnoreTransparency)
		.SetSimpleShadowColor(cInOption.GetColor());
}

void KERNEL::Operator::VisualEffectsPrivate::SetPlaneReflection(TheVisualEffects::PLANEREFLECTION & cInOption)
{
	if (m_cOption.PlaneReflection.checked == cInOption.checked) {
		return;
	}

	float fOpacity = cInOption.GetOpacity();
	int nBlurring = cInOption.GetBlurring();
	bool bFading = cInOption.Fading;

	View().GetSegmentKey().GetVisualEffectsControl().SetSimpleReflection(cInOption.checked, fOpacity, nBlurring, bFading);
}

void KERNEL::Operator::VisualEffectsPrivate::SetAmbientOcclusion(TheVisualEffects::AMBIENTOCCLUSION & cInOption)
{
	if (m_cOption.AmbientOcclusion.checked == cInOption.checked) {
		return;
	}

	float fStrength = cInOption.GetStrength();
	bool bFast = cInOption.GetQuality();

	View().GetSegmentKey().GetVisualEffectsControl().SetAmbientOcclusionEnabled(cInOption.checked, fStrength, bFast);
}

void KERNEL::Operator::VisualEffectsPrivate::SetSilhouetteEdges(TheVisualEffects::SILHOUETTEEDGES & cInOption)
{
	if (m_cOption.SilhouetteEdges.checked == cInOption.checked) {
		return;
	}

	float fTolerance = cInOption.GetTolerance();
	bool bHeavyExterior = cInOption.HeavyExterior;

	View().GetSegmentKey().GetVisualEffectsControl().SetSilhouetteEdgesEnabled(cInOption.checked, fTolerance, bHeavyExterior);
}

void KERNEL::Operator::VisualEffectsPrivate::SetBloom(TheVisualEffects::BLOOM & cInOption)
{
	if (m_cOption.Bloom.checked == cInOption.checked) {
		return;
	}

	float fStrength = cInOption.GetStrength();
	int nBlurring = cInOption.GetBlurring();
	int nShape = cInOption.Shape;

	View().GetSegmentKey().GetVisualEffectsControl().SetBloomEnabled(cInOption.checked, fStrength, nBlurring, nShape);
}

//== Visual Effects class ==========================================================================

KERNEL::Operator::VisualEffects::VisualEffects(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery)
{
	VisualEffectsPrivate * pcImpl = new VisualEffectsPrivate(pcInView, pcInDelivery);
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;
}

void KERNEL::Operator::VisualEffects::Request(Json::Object & cInObject)
{
	VisualEffectsPrivate * pcImpl = (VisualEffectsPrivate *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->Request(cInObject);
}
