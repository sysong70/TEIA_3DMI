#include "StdAfx.h"

#include "Operator.ModelPanel.h"

#include "Impl/OperatorImpl.h"

#include "Operator.Attribute.h"
#include "Operator.Select.h"

#include "Kernel.DocView.h"
#include "Impl/Kernel.DocViewImpl.h"

#include "Signal.Connector.h"

#include <3DF/KeyPath.h>
#include <3DF/Selection.h>
#include <3DF/Impl/SelectionImpl.h>
#include <3DF/Facility.AppOptions.h>
#include <3DF/3DF.Utility.h>

#include <3DF/Impl/SegmentImpl.h>

#include <3DF.View.h>

#include <Entity.ModelTree.h>

#include <Path.h>

#include <ranges>
#include <unordered_map>

#define MODELS_GROUP_KEY				1
#define MEASUREMENTS_GROUP_KEY			2
#define MARKUPS_GROUP_KEY				3

using namespace KERNEL;
using namespace H3DF;
using namespace H3DF::Entity;

//== ModelPanelImpl 관련 함수 ========================================================================

namespace KERNEL
{
	namespace Operator
	{
		class ModelPanelImpl : public OperatorImpl
		{
		public:
			ModelPanelImpl(const DocView * pcInDocView);

			void Copy(ModelPanelImpl * pcInThat) {
				OperatorImpl::Copy(pcInThat);
			}

			H3DF::Entity::ModelTree & ModelTree() { return m_cModelTree; }
			H3DF::Entity::ModelTree m_cModelTree;

			KERNEL::Operator::Select & Select();
			KERNEL::Operator::Attribute & Attribute();

			void UserInterfaceItemExpanded(ModelTreeItem * pcInItem, bool bRecursiveExpand = false);

			void Show(ModelTreeItem * pcInItem);
			void NoShow(ModelTreeItem * pcInItem);
		};
	}
}

KERNEL::Operator::ModelPanelImpl::ModelPanelImpl(const DocView * pcInDocView) :
	OperatorImpl(pcInDocView)
{

}

KERNEL::Operator::Select & KERNEL::Operator::ModelPanelImpl::Select()
{ 
	DocViewImpl * pcImpl = (DocViewImpl *)GetDocView().GetImpl();
	DEBUG_VALID(pcImpl);

	return pcImpl->Select();
}

KERNEL::Operator::Attribute & KERNEL::Operator::ModelPanelImpl::Attribute()
{
	DocViewImpl * pcImpl = (DocViewImpl *)GetDocView().GetImpl();
	DEBUG_VALID(pcImpl);

	return pcImpl->Attribute();
}

// 2. User Interface에 Item Expanded 처리
void KERNEL::Operator::ModelPanelImpl::UserInterfaceItemExpanded(ModelTreeItem * pcInItem, bool bRecursiveExpand)
{
	ModelTree().ExpandItem(pcInItem, false);

	Signal::TreeItems cTreeItems;
	Signal::TreeItem cItem;

	// 입력받은 Item의 Parent를 찾는다. Invisible인 경우는 다음 Parent를 찾는다.
	ModelTreeItem * pcParentItem = pcInItem;

	while (nullptr != pcParentItem) {
		// 부모가 Invisible이면 다음 Parent를 찾는다.
		if (pcParentItem->Status() & ModelTreeItemStatus::Invisible) {
			pcParentItem = pcParentItem->Parent();
		}
		else {
			break;
		}
	}

	cItem.ParentKey = (DWORD_PTR)pcParentItem;

	// 이미 Model Tree가 전계되어 있는 경우 처리
	if (false == pcInItem->Children().empty()) {
		for (auto pcChildItem : pcInItem->Children()) {

			// 이미 UI에 업데이트 한 경우는 
			if (pcChildItem->Status() & ModelTreeItemStatus::UiUpdate) {
				continue;
			}

			pcChildItem->AddStatus(ModelTreeItemStatus::UiUpdate);

			if (pcChildItem->Status() & ModelTreeItemStatus::Invisible) {
				UserInterfaceItemExpanded(pcChildItem, bRecursiveExpand);
				continue;
			}

// 			if (true == bRecursiveExpand) {
// 				if ((pcChildItem->Status() & ModelTreeItemStatus::Solid) || (pcChildItem->Status() & ModelTreeItemStatus::Surface)) {
// 					continue;
// 				}
// 			}

			CString strUserName;
			ModelTree().GetItemName(pcChildItem->KeyValue(), strUserName);

#ifdef _DEBUG
			HC_KEY nSegKey = INVALID_KEY, nIncKey = INVALID_KEY;
			H3DF::Type eType = H3DF::Utility::GetType(pcChildItem->KeyValue());
			if (H3DF::Type::IncludeKey == eType) {
				nIncKey = pcChildItem->KeyValue();
				IncludeKey cInInclude(nIncKey);
				nSegKey = cInInclude.GetTarget().KeyValue();
			}
			else if (H3DF::Type::SegmentKey == eType) {
				nSegKey = pcChildItem->KeyValue();
			}

			CString strText, strName;
			SegmentKey cSegment(nSegKey);
			strName = cSegment.Name(false);

			if (INVALID_KEY == nIncKey) {
				strText.Format(L": %s, Seg [%d]", strName, nSegKey);
			}
			else {
				strText.Format(L": %s, Inc [%d], Seg [%d]", strName, nIncKey, nSegKey);
			}

			strUserName += strText;
#endif

			cItem.Title = strUserName;
			cItem.Checked = (ModelTreeItemStatus::NoShow & pcChildItem->Status()) ? false : true;
			cItem.Key = (DWORD_PTR)pcChildItem;
			cTreeItems.push_back(cItem);
		}

		if (false == cTreeItems.empty()) {
			Delivery().modelPanel.AddChildren((DWORD_PTR)pcParentItem, cTreeItems);
		}

		for (auto & cItem : cTreeItems) {
			UserInterfaceItemExpanded((ModelTreeItem *)cItem.Key, bRecursiveExpand);
		}
	}
}

void KERNEL::Operator::ModelPanelImpl::Show(ModelTreeItem * pcInItem)
{

}

void KERNEL::Operator::ModelPanelImpl::NoShow(ModelTreeItem * pcInItem)
{

}


//== ModelPanel 관련 함수 ============================================================================

KERNEL::Operator::ModelPanel::ModelPanel(const DocView * pcInDocView)
{
	auto pcImpl = new ModelPanelImpl(pcInDocView);
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

	SegmentKey cModelSegment = pcImpl->View().GetModelOverrideSegmentKey();

	HC_KEY nModelKey = cModelSegment.KeyValue();

	ModelTreeItem* pcRootItem = pcImpl->m_cModelTree.AddItem(nModelKey, nullptr, true); // 내부 Tree 생성
	pcRootItem->AddStatus(ModelTreeItemStatus::UiUpdate);

	Signal::TreeItem cItem;
	cItem.ParentKey = 0;
	cItem.Key = (DWORD_PTR)pcRootItem;
	cItem.Title = strFileTitle;
	//cItem.HasChildren = false;

	//:Ken - 20240219, lock tree
	pcImpl->Delivery().modelPanel.RedrawTree(false);
	//:Ken - 20240205, Add root item
	pcImpl->Delivery().modelPanel.AddItem(cItem);

	SegmentKeyArray cChildren;
	cModelSegment.ShowSubsegments(cChildren);

	SegmentKey cSegment;

	// Model Group Item 생성, 이하에 CAD Model data를 저장한다.
	if (true == Utility::ShowSubSegment(cModelSegment, "models", cSegment)) {
		cItem.Title = L"Models";
		//cItem.HasChildren = (0 < cSegment.ShowIncluders()) ? true : false;
		bool hasChildren = (0 < cSegment.ShowIncluders()) ? true : false;

		//ModelTreeItem * pcItem = pcImpl->m_cModelTree.AddItem(cSegment.KeyValue(), pcRootItem, cItem.HasChildren); // 내부 Tree 생성
		ModelTreeItem * pcItem = pcImpl->m_cModelTree.AddItem(cSegment.KeyValue(), pcRootItem, hasChildren);
		pcItem->AddStatus(ModelTreeItemStatus::UiUpdate);
		cItem.Key = (DWORD_PTR)pcItem;

		pcImpl->ModelTree().SetModelsGroupItem(pcItem);
		cTreeItems.push_back(cItem);
	}

	// Measure Group Item 생성.
	if (true == Utility::ShowSubSegment(cModelSegment, "measurements", cSegment)) {
		cItem.Title = L"Measurements";
		//cItem.HasChildren = false;
		
		ModelTreeItem * pcItem = pcImpl->m_cModelTree.AddItem(cSegment.KeyValue(), pcRootItem, false); // 내부 Tree 생성
		pcItem->AddStatus(ModelTreeItemStatus::UiUpdate);
		cItem.Key = (DWORD_PTR) pcItem;

		pcImpl->ModelTree().SetMeasurementsGroupItem(pcItem);
		cTreeItems.push_back(cItem);
	}

	// Markup Group Item 생성.
	if (true == Utility::ShowSubSegment(cModelSegment, "markups", cSegment)) {
		cItem.Title = L"Markups";
		//cItem.HasChildren = false;

		ModelTreeItem * pcItem = pcImpl->m_cModelTree.AddItem(cSegment.KeyValue(), pcRootItem, false); // 내부 Tree 생성
		pcItem->AddStatus(ModelTreeItemStatus::UiUpdate);
		cItem.Key = (DWORD_PTR)pcItem;

		pcImpl->ModelTree().SetMarkupsGroupItem(pcItem);
		cTreeItems.push_back(cItem);
	}

	//:Ken - 20240205, Add predefined items on root item
	pcImpl->Delivery().modelPanel.AddChildren((DWORD_PTR)pcRootItem, cTreeItems);

	// Model Tree를 전개한다.
	ModelTreeItem * pcModelsGroupItem = pcImpl->ModelTree().ModelsGroupItem();
	DEBUG_VALID(pcModelsGroupItem);

	pcImpl->UserInterfaceItemExpanded(pcModelsGroupItem, true);

	//:Ken - 20240219, unlock and update tree
	pcImpl->Delivery().modelPanel.RedrawTree(true);
}

//== Signal 처리 관련 함수 ============================================================================

// 1. Signal 처리
void KERNEL::Operator::ModelPanel::Signal(Json::Object & cInObject)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	int nAction = cInObject.GetInteger(SKW_ACTION);

	switch ((Signal::ModelPanel::Action)nAction)
	{
		case Signal::ModelPanel::Action::OnItemChecked:
			OnItemCheckedSignal(cInObject);
			break;

		case Signal::ModelPanel::Action::OnItemExpanded:
			OnItemExpandedSignal(cInObject);
			break;

		case Signal::ModelPanel::Action::OnItemSelected:
			OnItemSelectedSignal(cInObject);
			break;

		default:
			ASSERT(false);
			break;
	}
}

//== Select 관련 함수 ===============================================================================

// 1. 외부에서 전달된 Selection Item을 이용해서 Model Tree를 설정한다.
void KERNEL::Operator::ModelPanel::SetSelectItem(H3DF::SelectionItem & cSelItem)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	H3DF::KeyPath cPath;
	cSelItem.ShowPath(cPath);

#ifdef _DEBUG
// 	CString strText;
// 	cSelItem.ShowPathString(strText);
#endif

	// 키값 배열을 가져온다.
	H3DF::KeyArray cKeys;
	cPath.ShowKeys(cKeys);

	std::reverse(cKeys.begin(), cKeys.end());

	ModelTreeItem * pcItem = &pcImpl->ModelTree().Root();
	ModelTreeItem * pcChildItem = nullptr;

	std::vector<ModelTreeItem *> vpcItems;

	// 키값을 이용해서 하부 Child Tree Item을 찾기전에 전개를 한다. 전개가 이미 되어 있으면 전개를 하지 않는다.
	// Model Tree를 전개하는 것이기 때문에, UI Tree에 나타는 것과 상관없이 전개한다.
	for (auto cKey : cKeys) {
		pcChildItem = nullptr;
		pcItem->ShowChild(cKey, pcChildItem);

		// 하부 아이템을 찾지 못한 경우는 전개를 해서 다시 검색한다.
		if (nullptr == pcChildItem) {
			// 찾지 못한 경우는 전개를 한다.
			pcImpl->ModelTree().ExpandItem(pcItem, false);

			// 전개된 후에 다시 찾는다.
			pcItem->ShowChild(cKey, pcChildItem);
		}

		// Selection Item의 맨 마지막은 Sgement이기 때문에 Tree에서 찾을 수 없다 그런 경우 다음으로 넘어간다.
		// 이런 경우 탐색이 끝나게 될것이다. Model Tree와 UI Tree의 형태가 다르기 때문이다.
		if (nullptr == pcChildItem) {
			continue;
		}

		vpcItems.push_back(pcChildItem);

		pcItem = pcChildItem;
	}
	
	// Model Tree Root를 기준으로 값을 찾는다.
	pcItem = &pcImpl->ModelTree().Root();
	ModelTreeItem * pcParentItem = nullptr;

	for (size_t nIndex = 0; nIndex < cKeys.size() - 1; nIndex++) {
		Key cChildKey = cKeys[nIndex];
		HC_KEY nChildKey = cChildKey.KeyValue();

		// 주어진 Key값을 이용해서 pcItem 하부에 있는 ChildItem을 찾는다.
		pcChildItem = nullptr;
		pcItem->ShowChild(cChildKey, pcChildItem);

		// part## 항목은 UI Tree에 표시하지 않으므로 처리하기 위해서 key값의 이름을 가져온다.
		// 여기서 이름은 User Defined Name이 아니라 Segment의 이름이다.
		CStringA strName = H3DF::Utility::GetName(cChildKey);
		CString strUserName;
		H3DF::UserData::ShowSegmentName(nChildKey, strUserName);

		if (pcItem->Status() & ModelTreeItemStatus::Invisible) {
			pcItem = pcChildItem;
			continue;
		}

		if (nullptr != pcChildItem) {
			pcImpl->UserInterfaceItemExpanded(pcItem);
		}

		vpcItems.push_back(pcChildItem);

 		pcItem = pcChildItem;
	}

	pcImpl->Delivery().modelPanel.ExpandParent((DWORD_PTR)pcItem);
}

//== Item Expanded 관련 함수 =========================================================================

// 1. Item Expanded Signal 처리
void KERNEL::Operator::ModelPanel::OnItemExpandedSignal(Json::Object & cInObject)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	DWORD_PTR nInItemKey = cInObject.GetDwordPtr(SKW_KEY);

	ModelTreeItem * pcItem = dynamic_cast<ModelTreeItem *>((ModelTreeItem *)nInItemKey);
	if (nullptr == pcItem) {
		DEBUG_STOP;
		return;
	}

	pcImpl->UserInterfaceItemExpanded(pcItem);
}

//== Item Selelect Changed 관련 함수 =================================================================

// 1. Item Select Changed Signal 처리
void KERNEL::Operator::ModelPanel::OnItemSelectedSignal(Json::Object & cInObject)
{
	return;

	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	// 대소문자 구별함. 주의할 것.
	DWORD_PTR nInItem = cInObject.GetDwordPtr(SKW_KEY);

	ModelTreeItem * pcItem = dynamic_cast<ModelTreeItem *>((ModelTreeItem *)nInItem);
	if (nullptr == pcItem) {
		DEBUG_STOP;
		return;
	}

// 	H3DF::KeyPath cPath;
// 	pcImpl->ModelTree().ShowPath(nInKey, cPath);

	std::vector<HC_KEY> vnKeys;

	H3DF::SelectionResults cResults;
	pcImpl->ModelTree().ShowSelectionResult(pcItem, cResults);

	pcImpl->Select().DynamicSelectByResult(cResults);

	return;
}

// 2. Item Checked Signal 처리
void KERNEL::Operator::ModelPanel::OnItemCheckedSignal(Json::Object & cInObject)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	DWORD_PTR nInItem = cInObject.GetDwordPtr(SKW_KEY);
	//:Ken - checked or unchecked...
	bool bChecked = cInObject.GetBoolean(SKW_CHECKED);

	ModelTreeItem * pcTreeItem = dynamic_cast<ModelTreeItem *>((ModelTreeItem *)nInItem);
	if (nullptr == pcTreeItem) {
		DEBUG_STOP;
		return;
	}

	HC_KEY nKey = pcTreeItem->KeyValue();
	if (INVALID_KEY == nKey) {
		return;
	}

	H3DF::Key cKey(nKey);
	if (true == bChecked) {
		pcImpl->Attribute().Show(cKey);
	}
	else {
		pcImpl->Attribute().NoShow(cKey);
	}

	return;

	H3DF::SelectionResults cResults;
	// 주어진 Item을 이용해서 Last Child까지 검색해서 결과값을 가져온다.
	pcImpl->ModelTree().ShowSelectionResult(pcTreeItem, cResults);

	if (true == bChecked) {
		pcImpl->Select().SelectByResult(cResults);
	}
	else {
		pcImpl->Select().Unhighlight(cResults);
	}
}