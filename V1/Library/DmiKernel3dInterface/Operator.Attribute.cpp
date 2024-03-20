#include "StdAfx.h"

#include "Operator.Attribute.h"

#include "Impl/OperatorImpl.h"

#include "Kernel.DocView.h"
#include "Impl/Kernel.DocViewImpl.h"

#include "Signal.Connector.h"
#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

#include "../../UiMain/Command.Resource.h"


#include <3DF/Window.h>
#include <3DF/Key.h>
#include <3DF/Selection.h>
#include <3DF/Visibility.h>
#include <3DF/Condition.h>
#include <3DF/Facility.AppOptions.h>
#include <3DF/3DF.Utility.h>

#include <Sprocket/3DF.View.h>
#include <Sprocket/Impl/CADModelImpl.h>
#include <Sprocket/Impl/ModelImpl.h>
#include <Sprocket/Impl/ViewImpl.h>

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

			void ShowStyleToNoShowStyle();
			void NoShowStyleToShowStyle();

			void SetShowComponent(H3DF::Component & cInComponent, bool bShowFlag, bool bRecursive = true);
		};
	}
}

KERNEL::Operator::AttributeImpl::AttributeImpl(const DocView * pcInDocView)
	: OperatorImpl(pcInDocView)
{
}

void KERNEL::Operator::AttributeImpl::ShowStyleToNoShowStyle()
{
	DocViewImpl * pcDocImpl = (DocViewImpl *)GetDocView().GetImpl();
	DEBUG_VALID(pcDocImpl);

	pcDocImpl->GetBaseView()->InvalidateSceneBounding();

	pcDocImpl->Select().ResetSnapItems(false);

	ModelImpl * pcModelImpl = (ModelImpl *)pcDocImpl->GetModel().GetImpl();
	DEBUG_VALID(pcModelImpl);

	pcModelImpl->ShowStyleSegment().GetVisibilityControl().SetFaces(false).SetLines(false);
	pcModelImpl->ShowVertexStyleSegment().GetVisibilityControl().SetVertices(false);

	pcModelImpl->NoShowStyleSegment().GetVisibilityControl().SetFaces(true).SetLines(true);
	pcModelImpl->NoShowVertexStyleSegment().GetVisibilityControl().SetVertices(true);
}

void KERNEL::Operator::AttributeImpl::NoShowStyleToShowStyle()
{
	DocViewImpl * pcDocImpl = (DocViewImpl *)GetDocView().GetImpl();
	DEBUG_VALID(pcDocImpl);

	pcDocImpl->Select().ResetSnapItems(false);

	ModelImpl * pcModelImpl = (ModelImpl *)pcDocImpl->GetModel().GetImpl();
	DEBUG_VALID(pcModelImpl);

	pcModelImpl->ShowStyleSegment().GetVisibilityControl().SetFaces(true).SetLines(true);
	pcModelImpl->ShowVertexStyleSegment().GetVisibilityControl().SetVertices(true);

	pcModelImpl->NoShowStyleSegment().GetVisibilityControl().SetFaces(false).SetLines(false);
	pcModelImpl->NoShowVertexStyleSegment().GetVisibilityControl().SetVertices(false);
}

// 입력된 Component및 하위 Component들을 검색해서, RepresentationItem이 나올때까지 하부 검색을 해서 NoShow로 변경
void KERNEL::Operator::AttributeImpl::SetShowComponent(H3DF::Component & cInComponent, bool bShowFlag, bool bRecursive)
{
	ModelImpl & cModelImpl = GetModelImpl();

	H3DF::Component::Type eType = cInComponent.GetType();

	// PartDefinition Component가 다중으로 Include되어 있으면, PartDefinition을 Clone을 만들도록 한다. 
	// Clone은 Part부터 하부 RI Segment까지의 모든 Segment를 복사하고, 최하부의 Geometry는 Reference로 변경한다.
	// 이렇게 해서, 다중으로 영향을 미치는 것을 최소하 하도록 하다.
	Component cPartDefComponent;
	if (H3DF::ComponentImpl::FindParentPartDefinition(cInComponent, cPartDefComponent)) {
		SegmentKey cPartSegment(cPartDefComponent.GetSegmentKey());

		DWORD nCount = 0;
		if (true == H3DF::UserData::ShowIncludedCount(cPartSegment, nCount)) {
			nCount = nCount;
		}
	}

	if (true == cInComponent.GetSubComponents().empty()) {
		SegmentKey cSegment(cInComponent.GetSegmentKey());

		if (true == bShowFlag) {
			if (H3DF::Component::NoShow & cInComponent.GetStatus()) {
				// Show Style이 있으면 삭제하고 No Show Style로 변경
				if (H3DF::Component::Type::ExchangeRIPointSet == eType) {
					cSegment.GetStyleControl().Flush(cModelImpl.NoShowVertexStyleSegment());
					cSegment.GetStyleControl().PushSegment(cModelImpl.ShowVertexStyleSegment());
				}
				else {
					cSegment.GetStyleControl().Flush(cModelImpl.NoShowStyleSegment());
					cSegment.GetStyleControl().PushSegment(cModelImpl.ShowStyleSegment());
				}

				cInComponent.RemoveStatus(H3DF::Component::NoShow);
			}
		}
		else {
			if (!(H3DF::Component::NoShow & cInComponent.GetStatus())) {
				// Show Style이 있으면 삭제하고 No Show Style로 변경
				if (H3DF::Component::Type::ExchangeRIPointSet == eType) {
					cSegment.GetStyleControl().Flush(cModelImpl.ShowVertexStyleSegment());
					cSegment.GetStyleControl().PushSegment(cModelImpl.NoShowVertexStyleSegment());
				}
				else {
					cSegment.GetStyleControl().Flush(cModelImpl.ShowStyleSegment());
					cSegment.GetStyleControl().PushSegment(cModelImpl.NoShowStyleSegment());
				}

				cInComponent.AddStatus(H3DF::Component::NoShow);
			}
		}
	}

	for (H3DF::Component * pcSubComponent : cInComponent.GetSubComponents()) {
		// Sub Component가 없으면 NoShow로 변경
	
		if (true == bRecursive) {
			SetShowComponent(*pcSubComponent, bShowFlag, bRecursive);
		}
	}
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

	pcImpl->PrepareUpdate();


	pcImpl->Updated();

	return true;
}

bool KERNEL::Operator::Attribute::Show(H3DF::Component * pcInComponent)
{
	AttributeImpl * pcImpl = (AttributeImpl *)m_pcImpl;

	pcImpl->SetShowComponent(*pcInComponent, true);

	return true;
}


bool KERNEL::Operator::Attribute::Show(H3DF::SelectionItem & cSelItem)
{
	AttributeImpl * pcImpl = (AttributeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return true;
}

bool KERNEL::Operator::Attribute::NoShow(H3DF::Component * pcInComponent)
{
	AttributeImpl * pcImpl = (AttributeImpl *)m_pcImpl;

	pcImpl->SetShowComponent(*pcInComponent, false, true);

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

bool KERNEL::Operator::Attribute::HideOnly()
{
	AttributeImpl * pcImpl = (AttributeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	DocViewImpl * pcDocViewImpl = dynamic_cast<DocViewImpl *>(pcImpl->GetDocView().GetImpl());
	DEBUG_VALID(pcDocViewImpl);

	H3DF::SelectionResults cResults = pcDocViewImpl->Select().Results();
	if (0 == cResults.GetCount()) {
		return false;
	}

	pcImpl->PrepareUpdate();

	H3DF::ModelImpl cModelImpl = pcImpl->GetModelImpl();

	SelectionResultsIterator cIter = cResults.GetIterator();

	while (true == cIter.IsValid()) {
		SelectionItem cItem = cIter.GetItem();

		Key cSelectionKey;
		if (true == cItem.ShowSelectedItem(cSelectionKey)) {

			H3DF::Type eType = H3DF::Utility::GetType(cSelectionKey);

			SegmentKey cSegment;
			if (H3DF::Type::IncludeKey == eType) {
				IncludeKey cIncludeKey(cSelectionKey.KeyValue());
				cSegment = cIncludeKey.GetTarget();
			}
			else {
				cSegment = SegmentKey(cSelectionKey.KeyValue());
			}
						
			cSegment.GetStyleControl().Flush(cModelImpl.ShowStyleSegment());
			cSegment.GetStyleControl().PushSegment(cModelImpl.NoShowStyleSegment());
		}

		cIter.Next();
	}

	pcDocViewImpl->Camera().FitWorldOnly();

	pcImpl->Updated();

	return true;
}

// KERNEL::DocViewImpl::SetVisibility에서 호출함.
bool KERNEL::Operator::Attribute::ShowOnly()
{
	AttributeImpl * pcImpl = (AttributeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	H3DF::SelectionResults cResults = pcImpl->GetDocView().Select().Results();
	if (0 == cResults.GetCount()) {
		return false;
	}

	pcImpl->PrepareUpdate();

	// 입력된 CADModel 및 하위 Component들을 NoShow로 변경
	pcImpl->SetShowComponent(pcImpl->GetDocView().CADModel(), false);

	SelectionResultsIterator cIter = cResults.GetIterator();

	while (true == cIter.IsValid()) {
		SelectionItem cItem = cIter.GetItem();

		Component * pcComponent = pcImpl->GetDocView().CADModel().GetComponent(cItem);
		if (nullptr == pcComponent) {
			DEBUG_STOP;
			cIter.Next();
			continue;
		}

		SegmentKey cSegment(pcComponent->GetSegmentKey());

		// 기존에 있을지 모르는 NoShowStyle을 삭제하고 ShowStyle로 변경
		cSegment.GetStyleControl().Flush(pcImpl->GetModelImpl().NoShowStyleSegment());
		cSegment.GetStyleControl().PushSegment(pcImpl->GetModelImpl().ShowStyleSegment());

		pcComponent->RemoveStatus(H3DF::Component::NoShow);

		cIter.Next();
	}

	// UI ModelPanel에 Redraw를 하지 않기 위해서 false로 설정
	pcImpl->Delivery().modelPanel.RedrawTree(false);

	pcImpl->GetDocView().ModelPanel().CheckedUpdate(pcImpl->GetDocView().CADModel());

	pcImpl->Delivery().modelPanel.RedrawTree(true);

	// Update후에 Fit을 해야 Smooth Transition이 효과가 나타난다.
	pcImpl->Updated();

	pcImpl->GetDocView().Camera().FitWorldOnly();

	return true;
}

bool KERNEL::Operator::Attribute::ShowToggle()
{
	AttributeImpl * pcImpl = (AttributeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->PrepareUpdate();

	if(false == pcImpl->m_bToogled) {
		pcImpl->ShowStyleToNoShowStyle();
		pcImpl->m_bToogled = true;
	}
	else {
		pcImpl->NoShowStyleToShowStyle();
		pcImpl->m_bToogled = false;
	}

	pcImpl->Delivery().modelPanel.InverseCheckedStatus();

	// Zoom Fit 명령어 호출
	DocViewImpl * pcDocViewImpl = dynamic_cast<DocViewImpl *>(pcImpl->GetDocView().GetImpl());
	DEBUG_VALID(pcDocViewImpl);

	pcDocViewImpl->Camera().FitWorldOnly();

	pcImpl->Updated();

	return true;
}
