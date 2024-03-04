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

			void ShowToNoShow();
			void NoShowToShow();

			void ZoomFit();
		};
	}
}

KERNEL::Operator::AttributeImpl::AttributeImpl(const DocView * pcInDocView)
	: OperatorImpl(pcInDocView)
{
}

void KERNEL::Operator::AttributeImpl::ShowToNoShow()
{
	DocViewImpl * pcDocImpl = (DocViewImpl *)GetDocView().GetImpl();
	DEBUG_VALID(pcDocImpl);

	ModelImpl * pcModelImpl = (ModelImpl *)pcDocImpl->GetModel().GetImpl();
	DEBUG_VALID(pcModelImpl);

	pcModelImpl->ShowStyleSegment().GetVisibilityControl().SetFaces(false).SetLines(false);
	pcModelImpl->ShowVertexStyleSegment().GetVisibilityControl().SetVertices(false);

	pcModelImpl->NoShowStyleSegment().GetVisibilityControl().SetFaces(true).SetLines(true);
	pcModelImpl->NoShowVertexStyleSegment().GetVisibilityControl().SetVertices(true);
}

void KERNEL::Operator::AttributeImpl::NoShowToShow()
{
	DocViewImpl * pcDocImpl = (DocViewImpl *)GetDocView().GetImpl();
	DEBUG_VALID(pcDocImpl);

	ModelImpl * pcModelImpl = (ModelImpl *)pcDocImpl->GetModel().GetImpl();
	DEBUG_VALID(pcModelImpl);

	pcModelImpl->ShowStyleSegment().GetVisibilityControl().SetFaces(true).SetLines(true);
	pcModelImpl->ShowVertexStyleSegment().GetVisibilityControl().SetVertices(true);

	pcModelImpl->NoShowStyleSegment().GetVisibilityControl().SetFaces(false).SetLines(false);
	pcModelImpl->NoShowVertexStyleSegment().GetVisibilityControl().SetVertices(false);
}

void KERNEL::Operator::AttributeImpl::ZoomFit()
{
	DocViewImpl * pcDocImpl = (DocViewImpl *)GetDocView().GetImpl();
	DEBUG_VALID(pcDocImpl);

	pcDocImpl->GetBaseView()->InvalidateSceneBounding();

	pcDocImpl->GetBaseView()->FitWorld();
	pcDocImpl->GetBaseView()->CameraPositionChanged();

	pcDocImpl->GetBaseView()->SetZoomLimit();

	pcDocImpl->GetCanvas().GetFrontView().Update();
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

bool KERNEL::Operator::Attribute::ShowOnly(H3DF::Key & cKey)
{
	AttributeImpl * pcImpl = (AttributeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	H3DF::Type eType = H3DF::Utility::GetType(cKey);

	SegmentKey cSegment;
	if (H3DF::Type::IncludeKey == eType) {
		IncludeKey cIncludeKey(cKey.KeyValue());
		cSegment = cIncludeKey.GetTarget();
	}
	else {
		cSegment = SegmentKey(cKey.KeyValue());
	}

	DocViewImpl * pcDocImpl = (DocViewImpl *)pcImpl->GetDocView().GetImpl();
	DEBUG_VALID(pcDocImpl);

	ModelImpl * pcModelImpl = (ModelImpl *)pcDocImpl->GetModel().GetImpl();
	DEBUG_VALID(pcModelImpl);

	// 보이는 부분들을 모두 NoShow로 변경
	pcImpl->ShowToNoShow();

	cSegment.GetStyleControl().PushSegment(pcModelImpl->ShowOnlyStyleSegment());

	pcImpl->ZoomFit();

	return true;
}

bool KERNEL::Operator::Attribute::ShowOnly(H3DF::SelectionItem & cSelItem)
{
	AttributeImpl * pcImpl = (AttributeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return true;
}

bool KERNEL::Operator::Attribute::ShowToggle()
{
	AttributeImpl * pcImpl = (AttributeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	if(false == pcImpl->m_bToogled) {
		pcImpl->ShowToNoShow();
		pcImpl->m_bToogled = true;
	}
	else {
		pcImpl->NoShowToShow();
		pcImpl->m_bToogled = false;
	}

	pcImpl->ZoomFit();

	return true;
}
