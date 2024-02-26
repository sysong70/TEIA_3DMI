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

#include <Impl/ModelImpl.h>
#include <Impl/ViewImpl.h>

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

				m_bToogled = pcInThat->m_bToogled;
			}

			bool m_bToogled = false;
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

bool KERNEL::Operator::Attribute::Show(H3DF::Key & cKey)
{
	H3DF::Type eType = H3DF::Utility::GetType(cKey);

	SegmentKey cSegment;
	if (H3DF::Type::IncludeKey == eType) {
		IncludeKey cIncludeKey(cKey.KeyValue());
		cSegment = cIncludeKey.GetTarget();
	}
	else {
		cSegment = SegmentKey(cKey.KeyValue());
	}

	AttributeImpl * pcImpl = (AttributeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	DocViewImpl * pcDocImpl = (DocViewImpl *)pcImpl->GetDocView().GetImpl();
	DEBUG_VALID(pcDocImpl);

	ModelImpl * pcModelImpl = (ModelImpl *)pcDocImpl->GetModel().GetImpl();
	DEBUG_VALID(pcModelImpl);

	StyleTypeArray cTypes;
	SegmentKeyArray cSegmentSources;
	AStringArray astrStyleNames;
	ConditionalExpressionArray acOutConditions;

	if (true == cSegment.GetStyleControl().Show(cTypes, cSegmentSources, astrStyleNames, acOutConditions)) {
		for (size_t nIndex = 0; nIndex < cSegmentSources.size(); nIndex++) {
			if ("noshow_style" == astrStyleNames[nIndex]) {
				cSegment.GetStyleControl().Flush(cSegmentSources[nIndex]);
			}
		}
	}

	// cSegment.GetStyleControl().PushSegment(pcModelImpl->NoShowStyleSegment());

	pcDocImpl->GetCanvas().GetFrontView().Update();

	return true;
}


bool KERNEL::Operator::Attribute::Show(H3DF::SelectionItem & cSelItem)
{
	AttributeImpl * pcImpl = (AttributeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);


	return true;
}

bool KERNEL::Operator::Attribute::NoShow()
{
	AttributeImpl * pcImpl = (AttributeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	DocViewImpl * pcDocViewImpl = dynamic_cast<DocViewImpl *>(pcImpl->GetDocView().GetImpl());
	DEBUG_VALID(pcDocViewImpl);

	H3DF::ModelImpl * pcModelImpl = dynamic_cast<ModelImpl *>(pcDocViewImpl->GetModel().GetImpl());
	DEBUG_VALID(pcModelImpl);

	size_t nCount = pcDocViewImpl->Select().Results().GetCount();
	if (0 == nCount) {
		return false;
	}

	SelectionResults cResult = pcDocViewImpl->Select().Results();

	SelectionResultsIterator cIter = cResult.GetIterator();

	//ConditionalExpression cNoShowCond("noshow");

	while (true == cIter.IsValid()) {
		SelectionItem cItem = cIter.GetItem();

		KeyPath cPath;
		if (true == cItem.ShowPath(cPath)) {
			Key cKey1 = cPath.At(0);
			SegmentKey cSegment(cKey1.KeyValue());
			//cSegment.GetStyleControl().PushSegment(pcModelImpl->NoShowStyleSegment(), cNoShowCond);
			//cSegment.SetCondition("noshow");

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

	pcDocViewImpl->GetCanvas().GetFrontView().Update();


	//H3DF::ConditionalExpression cCondExp(;

	return true;
}

bool KERNEL::Operator::Attribute::NoShow(H3DF::Key & cKey)
{
	H3DF::Type eType = H3DF::Utility::GetType(cKey);

	SegmentKey cSegment;
	if (H3DF::Type::IncludeKey == eType) {
		IncludeKey cIncludeKey(cKey.KeyValue());
		cSegment = cIncludeKey.GetTarget();
	}
	else {
		cSegment = SegmentKey(cKey.KeyValue());
	}

	AttributeImpl * pcImpl = (AttributeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	DocViewImpl * pcDocImpl = (DocViewImpl *)pcImpl->GetDocView().GetImpl();
	DEBUG_VALID(pcDocImpl);

	ModelImpl * pcModelImpl = (ModelImpl *)pcDocImpl->GetModel().GetImpl();
	DEBUG_VALID(pcModelImpl);

	cSegment.GetStyleControl().PushSegment(pcModelImpl->NoShowStyleSegment());

	pcDocImpl->GetCanvas().GetFrontView().Update();

	return true;
}

bool KERNEL::Operator::Attribute::NoShow(H3DF::SelectionItem & cSelItem)
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

	DocViewImpl * pcDocImpl = (DocViewImpl *)pcImpl->GetDocView().GetImpl();
	DEBUG_VALID(pcDocImpl);

	ModelImpl * pcModelImpl = (ModelImpl *)pcDocImpl->GetModel().GetImpl();
	DEBUG_VALID(pcModelImpl);

	if(false == pcImpl->m_bToogled) {
		pcModelImpl->ShowStyleSegment().GetVisibilityControl().SetFaces(false).SetLines(false);
		pcModelImpl->ShowVertexStyleSegment().GetVisibilityControl().SetVertices(false);
		pcModelImpl->NoShowStyleSegment().GetVisibilityControl().SetFaces(true).SetLines(true);
		pcModelImpl->NoShowVertexStyleSegment().GetVisibilityControl().SetVertices(true);
		pcImpl->m_bToogled = true;
	}
	else {
		pcModelImpl->ShowStyleSegment().GetVisibilityControl().SetFaces(true).SetLines(true);
		pcModelImpl->ShowVertexStyleSegment().GetVisibilityControl().SetVertices(true);
		pcModelImpl->NoShowStyleSegment().GetVisibilityControl().SetFaces(false).SetLines(false);
		pcModelImpl->NoShowVertexStyleSegment().GetVisibilityControl().SetVertices(false);
		pcImpl->m_bToogled = false;
	}

	pcDocImpl->GetBaseView()->InvalidateSceneBounding();

	pcDocImpl->GetBaseView()->FitWorld();
	pcDocImpl->GetBaseView()->CameraPositionChanged();

	pcDocImpl->GetBaseView()->SetZoomLimit();

	pcDocImpl->GetCanvas().GetFrontView().Update();

	return true;
}
