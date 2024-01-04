#include "StdAfx.h"

#include "Operator.Visibility.h"

#include "Impl/OperatorImpl.h"

#include "Signal.Connector.h"
#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

#include "../../UiMain/Command.Resource.h"

#include <3DF.View.h>
#include <3DF/Window.h>
#include <3DF/Visibility.h>
#include <3DF/Facility.AppOptions.h>

#include <Json.h>

using namespace KERNEL;

#define TheKenel TheAppOptions.Kernel
#define TheVisualEffects H3DF::Facility::KernelOption::Visibility

namespace KERNEL
{
	namespace Operator
	{
		class VisibilityImpl : public OperatorImpl
		{
		public:
			VisibilityImpl(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery);

			void Copy(VisibilityImpl * pcInThat) {
				OperatorImpl::Copy(pcInThat);
			}

			// void Request(Json::Object & cInObject);
			// void Change(Json::Object & cInObject);
		};
	}
}

KERNEL::Operator::VisibilityImpl::VisibilityImpl(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery)
	: OperatorImpl(pcInView, pcInDelivery)
{
}

//== Visibility class ==============================================================================

KERNEL::Operator::Visibility::Visibility(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery)
{
	VisibilityImpl * pcImpl = new VisibilityImpl(pcInView, pcInDelivery);
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;
}

bool KERNEL::Operator::Visibility::ShowAll()
{
	VisibilityImpl * pcImpl = (VisibilityImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return true;
}

bool KERNEL::Operator::Visibility::Hide()
{
	VisibilityImpl * pcImpl = (VisibilityImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return true;
}

bool KERNEL::Operator::Visibility::ShowOnly()
{
	VisibilityImpl * pcImpl = (VisibilityImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return true;
}

bool KERNEL::Operator::Visibility::Toggle()
{
	VisibilityImpl * pcImpl = (VisibilityImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return true;
}