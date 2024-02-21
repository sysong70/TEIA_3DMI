#include "StdAfx.h"

#include "Operator.Attribute.h"

#include "Impl/OperatorImpl.h"

#include "Kernel.DocView.h"
#include "Impl/Kernel.DocViewImpl.h"

#include "Signal.Connector.h"
#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

#include "../../UiMain/Command.Resource.h"

#include <3DF.View.h>
#include <3DF/Window.h>
#include <3DF/Key.h>
#include <3DF/Selection.h>
#include <3DF/Visibility.h>
#include <3DF/Condition.h>
#include <3DF/Facility.AppOptions.h>
#include <3DF/3DF.Utility.h>

#include <Json.h>

using namespace KERNEL;
using namespace H3DF;

#define TheKenel TheAppOptions.Kernel
#define TheVisualEffects H3DF::Facility::KernelOption::Attribute

namespace KERNEL
{
	namespace Operator
	{
		class AttributeImpl : public OperatorImpl
		{
		public:
			AttributeImpl(const DocView * pcInDocView);

			void Copy(AttributeImpl * pcInThat) {
				OperatorImpl::Copy(pcInThat);
			}

			// void Request(Json::Object & cInObject);
			// void Change(Json::Object & cInObject);
		};
	}
}

KERNEL::Operator::AttributeImpl::AttributeImpl(const DocView * pcInDocView)
	: OperatorImpl(pcInDocView)
{
}

//== Attribute class ==============================================================================

KERNEL::Operator::Attribute::Attribute(const DocView * pcInDocView)
{
	AttributeImpl * pcImpl = new AttributeImpl(pcInDocView);
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

	DocViewImpl * pcDocViewImpl = dynamic_cast<DocViewImpl *>(pcImpl->GetDocView().GetImpl());
	DEBUG_VALID(pcDocViewImpl);

	size_t nCount = pcDocViewImpl->Select().Results().GetCount();
	if (0 == nCount) {
		return false;
	}

	SelectionResults cResult = pcDocViewImpl->Select().Results();

	SelectionResultsIterator cIter = cResult.GetIterator();

	while (true == cIter.IsValid()) {
		SelectionItem cItem = cIter.GetItem();

		KeyPath cPath;
		if (true == cItem.ShowPath(cPath)) {
			Key cKey = cPath.At(1);
			H3DF::Type eType = Utility::GetType(cKey.KeyValue());

			if (H3DF::Type::IncludeKey == eType) {
				IncludeKey cIncludeKey(cKey.KeyValue());
				ConditionalExpression cCondExp("noshow");
				cIncludeKey.SetConditionalExpression(cCondExp);
			}
		}

		cIter.Next();
	}

	//H3DF::ConditionalExpression cCondExp(;

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