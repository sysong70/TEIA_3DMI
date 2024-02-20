#include "StdAfx.h"

#include "Operator.Attribute.h"

#include "Impl/OperatorImpl.h"

#include "Signal.Connector.h"
#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

#include "../../UiMain/Command.Resource.h"

#include <3DF.View.h>
#include <3DF/Window.h>
#include <3DF/Key.h>
#include <3DF/Selection.h>
#include <3DF/Visibility.h>
#include <3DF/Facility.AppOptions.h>

#include <Json.h>

using namespace KERNEL;

#define TheKenel TheAppOptions.Kernel
#define TheVisualEffects H3DF::Facility::KernelOption::Attribute

namespace KERNEL
{
	namespace Operator
	{
		class AttributeImpl : public OperatorImpl
		{
		public:
			AttributeImpl(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery);

			void Copy(AttributeImpl * pcInThat) {
				OperatorImpl::Copy(pcInThat);
			}

			// void Request(Json::Object & cInObject);
			// void Change(Json::Object & cInObject);
		};
	}
}

KERNEL::Operator::AttributeImpl::AttributeImpl(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery)
	: OperatorImpl(pcInView, pcInDelivery)
{
}

//== Attribute class ==============================================================================

KERNEL::Operator::Attribute::Attribute(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery)
{
	AttributeImpl * pcImpl = new AttributeImpl(pcInView, pcInDelivery);
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;
}

bool KERNEL::Operator::Attribute::ShowAll()
{
	AttributeImpl * pcImpl = (AttributeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);



	return true;
}

bool KERNEL::Operator::Attribute::Show()
{
	AttributeImpl * pcImpl = (AttributeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);


	return true;
}

bool KERNEL::Operator::Attribute::Show(H3DF::SelectionItem & cSelItem)
{
	AttributeImpl * pcImpl = (AttributeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);


	return true;
}

bool KERNEL::Operator::Attribute::Hide()
{
	AttributeImpl * pcImpl = (AttributeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);




	return true;
}

bool KERNEL::Operator::Attribute::Hide(H3DF::SelectionItem & cSelItem)
{
	AttributeImpl * pcImpl = (AttributeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	H3DF::Key cSelKey;
	cSelItem.ShowSelectedItem(cSelKey);

	return true;
}

bool KERNEL::Operator::Attribute::ShowOnly()
{
	AttributeImpl * pcImpl = (AttributeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return true;
}

bool KERNEL::Operator::Attribute::ShowToggle()
{
	AttributeImpl * pcImpl = (AttributeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return true;
}