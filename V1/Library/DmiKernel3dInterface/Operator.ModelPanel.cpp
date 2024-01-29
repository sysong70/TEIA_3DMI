#include "StdAfx.h"

#include "Operator.ModelPanel.h"

#include "Impl/OperatorImpl.h"

#include "Signal.Connector.h"

#include <3DF/Facility.AppOptions.h>

#include <3DF.View.h>

#include <3DF/3DF.Utility.h>

#include <Path.h>

using namespace KERNEL;
using namespace H3DF;

namespace KERNEL
{
	namespace Operator
	{
		class ModelTreeItem
		{
		public:
			H3DF::SegmentKey cParent;
			H3DF::SegmentKey cKey;
			H3DF::SegmentKeyArray caChildren;
			int nStatus = 0;
		};
	}
}

//== ModelPanelImpl 관련 함수 ========================================================================

namespace KERNEL
{
	namespace Operator
	{
		class ModelPanelImpl : public OperatorImpl
		{
		public:
			ModelPanelImpl(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery);

			void Copy(ModelPanelImpl * pcInThat) {
				OperatorImpl::Copy(pcInThat);
			}
		};
	}
}

KERNEL::Operator::ModelPanelImpl::ModelPanelImpl(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery) :
	OperatorImpl(pcInView, pcInDelivery)
{

}

//== ModelPanel 관련 함수 ============================================================================
KERNEL::Operator::ModelPanel::ModelPanel(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery)
{
	auto pcImpl = new ModelPanelImpl(pcInView, pcInDelivery);
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;
}

// 1. 초기화 함수
void KERNEL::Operator::ModelPanel::Initialize(CString strFilePathName)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	CString strFileTitle = Path::GetFileTitle(strFilePathName);
	CString strFileName = Path::GetFileName(strFilePathName);

	Signal::TreeItems cTreeItems;

	DWORD_PTR nParentKey = 0;
	DWORD_PTR nModelParentKey = 0;

	// Root Item 생성
	Signal::TreeItem cItem;
	cItem.Title = strFileTitle;
	cItem.HasChildren = true;
	cItem.Key = (DWORD_PTR)pcImpl->View().GetModelOverrideSegmentKey().KeyValue();
	cTreeItems.push_back(cItem);
	pcImpl->Delivery().modelPanel.AddItems(cTreeItems);

	nParentKey = cItem.Key;

	cTreeItems.clear();

	// Model Item 생성, 이하에 CAD Model data를 저장한다.
	cItem.Title = L"Models";
	cItem.HasChildren = true;
	cItem.Key = 1;
	cTreeItems.push_back(cItem);

	nModelParentKey = cItem.Key;

	// Measure Item 생성, 이하에 CAD Model data를 저장한다.
	cItem.Title = L"Measurments";
	cItem.HasChildren = false;
	cItem.Key = 2;
	cTreeItems.push_back(cItem);

	// Measure Item 생성, 이하에 CAD Model data를 저장한다.
	cItem.Title = L"Markups";
	cItem.HasChildren = false;
	cItem.Key = 3;
	cTreeItems.push_back(cItem);

	pcImpl->Delivery().modelPanel.AddChildren(nParentKey, cTreeItems);

	H3DF::IncludeKeyArray cChildren;
	pcImpl->View().GetModelOverrideSegmentKey().ShowIncluders(cChildren);

	for (auto cInclude : cChildren) {
		SegmentKey cSegment = cInclude.GetTarget();

		CString strName;
		if (false == H3DF::UserData::ShowSegmentName(cSegment, strName)) {
			strName = cSegment.Name();
		}

		size_t nCount = cSegment.ShowSubsegments();

		cItem.Title = strName;
		cItem.Key = cInclude.KeyValue();
		cItem.HasChildren = (0 < nCount) ? true : false;
		cTreeItems.push_back(cItem);
	}

	pcImpl->Delivery().modelPanel.AddChildren(nModelParentKey, cTreeItems);

}

//== Signal 처리 관련 함수 ============================================================================

// 1. Signal 처리
void KERNEL::Operator::ModelPanel::Signal(Json::Object & cInObject)
{
	int nAction = cInObject.GetInteger(SKW_ACTION);

	CString strText;
	cInObject.Stringify(strText);

	switch ((Signal::ModelPanel::Action)nAction)
	{
		case Signal::ModelPanel::Action::OnBeginDrag:
			break;

		case Signal::ModelPanel::Action::OnBeginLabelEdit:
			break;

		case Signal::ModelPanel::Action::OnClick:
			break;

		case Signal::ModelPanel::Action::OnDblClick:
			break;

		case Signal::ModelPanel::Action::OnDeleteItem:
			break;

		case Signal::ModelPanel::Action::OnEndLabelEdit:
			break;

		case Signal::ModelPanel::Action::OnItemExpanded:
			break;

		case Signal::ModelPanel::Action::OnItemExpanding:
			break;

		case Signal::ModelPanel::Action::OnRClick:
			break;

		case Signal::ModelPanel::Action::OnRDbClick:
			break;

		case Signal::ModelPanel::Action::OnSelChanged:
			break;

		case Signal::ModelPanel::Action::OnSelChanging:
			break;

		case Signal::ModelPanel::Action::OnSetFocus:
			break;

		case Signal::ModelPanel::Action::AddItems:
			break;

		case Signal::ModelPanel::Action::AddChildren:
			break;

		default:
			assert(false);
			break;
	}
}