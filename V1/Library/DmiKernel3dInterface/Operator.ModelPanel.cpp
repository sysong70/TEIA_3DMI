#include "StdAfx.h"

#include "Operator.ModelPanel.h"

#include "Impl/OperatorImpl.h"

#include "Signal.Connector.h"

#include <3DF/Facility.AppOptions.h>

#include <3DF.View.h>

#include <3DF/3DF.Utility.h>

#include <Path.h>

#define MODELS_GROUP_KEY				1
#define MEASUREMENTS_GROUP_KEY			2
#define MARKUPS_GROUP_KEY				3

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

	return;

	nParentKey = cItem.Key;

	cTreeItems.clear();

	// Model Item 생성, 이하에 CAD Model data를 저장한다.
	cItem.Title = L"Models";
	cItem.HasChildren = true;
	cItem.Key = MODELS_GROUP_KEY;
	cTreeItems.push_back(cItem);

	nModelParentKey = cItem.Key;

	// Measure Item 생성, 이하에 CAD Model data를 저장한다.
	cItem.Title = L"Measurements";
	cItem.HasChildren = false;
	cItem.Key = MEASUREMENTS_GROUP_KEY;
	cTreeItems.push_back(cItem);

	// Measure Item 생성, 이하에 CAD Model data를 저장한다.
	cItem.Title = L"Markups";
	cItem.HasChildren = false;
	cItem.Key = MARKUPS_GROUP_KEY;
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
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	int nViewId = cInObject.GetInteger(SKW_VIEWID);
	int nAction = cInObject.GetInteger(SKW_ACTION);

	/*pcImpl->Delivery().modelPanel.ViewId = nViewId;*/

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
			ItemExpandedSignal(cInObject);
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

// 2. Item Expanded 처리
void KERNEL::Operator::ModelPanel::ItemExpandedSignal(Json::Object & cInObject)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

// 	CString strText;
// 	cInObject.Stringify(strText);

	// 대소문자 구별함. 주의할 것.
	HC_KEY nInKey = (HC_KEY)cInObject.GetDwordPtr("Key");

	HC_KEY nModelKey = pcImpl->View().GetModelOverrideSegmentKey().KeyValue();

	if (nInKey == nModelKey) {
		ModelItemExpanded(nModelKey);
	}
	else if (MODELS_GROUP_KEY == nInKey) {
		ModelGroupItemExpanded();
	}
	else if (MEASUREMENTS_GROUP_KEY == nInKey) {
		MeasurementsGroupItemExpanded();
	}
	else if (MARKUPS_GROUP_KEY == nInKey) {
		MarkupsGroupItemExpanded();
	}
	else {
		ItemExpanded(nInKey);
	}
}

// 2.1 Model Item Expanded 처리
void KERNEL::Operator::ModelPanel::ModelItemExpanded(HC_KEY nModelKey)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);
	
	Signal::TreeItems cTreeItems;
	Signal::TreeItem cItem;

	cItem.Title = L"Models";
	cItem.HasChildren = true;
	cItem.Key = MODELS_GROUP_KEY;
	cTreeItems.push_back(cItem);

	// Measure Item 생성, 이하에 CAD Model data를 저장한다.
	cItem.Title = L"Measurements";
	cItem.HasChildren = false;
	cItem.Key = MEASUREMENTS_GROUP_KEY;
	cTreeItems.push_back(cItem);

	// Measure Item 생성, 이하에 CAD Model data를 저장한다.
	cItem.Title = L"Markups";
	cItem.HasChildren = false;
	cItem.Key = MARKUPS_GROUP_KEY;
	cTreeItems.push_back(cItem);

	pcImpl->Delivery().modelPanel.AddChildren(nModelKey, cTreeItems);
}

// 2.1.1 Model Group Item Expanded 처리
void KERNEL::Operator::ModelPanel::ModelGroupItemExpanded()
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	Signal::TreeItems cTreeItems;
	Signal::TreeItem cItem;

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

	pcImpl->Delivery().modelPanel.AddChildren(MODELS_GROUP_KEY, cTreeItems);
}

// 2.1.2 Measurements Group Item Expanded 처리
void KERNEL::Operator::ModelPanel::MeasurementsGroupItemExpanded()
{
}

// 2.1.3 Markups Group Item Expanded 처리
void KERNEL::Operator::ModelPanel::MarkupsGroupItemExpanded()
{
}

// 2.2 일반 Item Expanded 처리
void KERNEL::Operator::ModelPanel::ItemExpanded(HC_KEY nItemKey)
{
}
