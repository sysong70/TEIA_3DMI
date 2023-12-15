#include "StdAfx.h"

#include "OPERATOR.VisualEffects.h"

#include "Private/OperatorPrivate.h"

#include "Signal.Connector.h"
#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

#include "../../UiMain/Command.Resource.h"

#include <3DF/Window.h>
#include <3DF/VisualEffects.h>
#include <3DF/Facility.AppOptions.h>

#include <Json.h>

using namespace KERNEL;

#define TheKenel TheAppOptions.Kernel

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

			Json::Object & Setting() { return *m_pcSetting; }
			Json::Object & DefaultSetting() { return *m_pcDefaultSetting; }

		private:
			Json::Object * m_pcSetting = nullptr; // Current Setting 값을 저장하는 Json Object
			Json::Object * m_pcDefaultSetting = nullptr; // Default Setting 값을 저장하는 Json Object
		};
	}
}

KERNEL::Operator::VisualEffectsPrivate::VisualEffectsPrivate(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery)
	: OperatorPrivate(pcInView, pcInDelivery)
{
	m_pcSetting = TheKenel.VisualEffects.Get();
	m_pcDefaultSetting = TheKenel.VisualEffects.Get();
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

	// cInObject에 Value값이 있는 경우 처리 (UI에서 변경된 값을 전달한 경우 처리)
	Json::Object & cInValue = cInObject.GetObject(SKW_VALUE);
	if (false == cInValue.IsEmpty()) {
		CString strText1;
		cInValue.Stringify(strText1);
	}

	Json::Object * pcValue = new Json::Object(pcImpl->Setting());

	pcImpl->Delivery().command.ResponseValue(HOME_3D_LST_VisualEffects, *pcValue, pcImpl->DefaultSetting());
}
