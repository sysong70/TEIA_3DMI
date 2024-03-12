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

#include <Sprocket/3DF.View.h>
#include <Sprocket/3DF.CADModel.h>
#include <Sprocket/Impl/CADModelImpl.h>

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

			H3DF::CADModel & CadModel() { return *m_pcCadModel; }
			H3DF::CADModel * m_pcCadModel = nullptr;

			H3DF::Entity::ModelTree & ModelTree() { return m_cModelTree; }
			H3DF::Entity::ModelTree m_cModelTree;

			KERNEL::Operator::Select & Select();
			KERNEL::Operator::Attribute & Attribute();

			void ComponentExpanded(Component & cInComponent, bool bRecursiveExpand = false, bool bTreeExpand = true);

			void UserInterfaceItemExpanded(ModelTreeItem * pcInItem, bool bRecursiveExpand = false);

			void Show(ModelTreeItem * pcInItem);
			void NoShow(ModelTreeItem * pcInItem);

			bool IsVisible(Component & cInComponent);
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

// 2. Component 전개 처리
// bRecursiveExpand 계속해서 하부 전개를 하고, bTreeExpand는 Tree에 나타낼때 펼쳐진 상태인지 아닌지를 설정한다.
void KERNEL::Operator::ModelPanelImpl::ComponentExpanded(H3DF::Component & cInComponent, bool bRecursiveExpand, bool bTreeExpand)
{
	ComponentArray & cSubComponents = cInComponent.GetSubcomponents();

	Signal::TreeItems cTreeItems;
	Signal::TreeItem cItem;

	H3DF::Component * pcParentItem = &cInComponent;

	// 입력받은 Item이 Tree에 표시되지 않는 Component면 Parent로 사용하지 않고 상위 Parent를 찾는다.

	bool bInVisibleComponent = false;
	while (nullptr != pcParentItem) {
		// 부모가 Invisible이면 상위 Parent를 찾는다. 찾은 상위 Parent에 하부 Component를 추가한다.
		if (false == IsVisible(*pcParentItem)) {
			pcParentItem = &pcParentItem->GetOwner();
			bInVisibleComponent = true;
		}
		else {
			break;
		}
	}

	cItem.ParentKey = (DWORD_PTR)pcParentItem;

	H3DF::CADModelImpl * pcImpl = dynamic_cast<H3DF::CADModelImpl *>(m_pcCadModel->GetImpl());

	for (auto pcSubComponent : cSubComponents) {
		if (false == IsVisible(*pcSubComponent)) {
			continue;
		}

		CString strText = pcSubComponent->GetName();
		if (L"OP10_DIE_Split_Surface" == strText) {
			int i = 0;
		}

		cItem.Checked = (H3DF::Component::Status::NoShow & pcSubComponent->GetStatus()) ? false : true;

		if (false == cItem.Checked) {
			int i = 0;
		}

		cItem.Key = (DWORD_PTR)pcSubComponent;
		cItem.Title = pcSubComponent->GetName();

		if (true == cItem.Title.IsEmpty()) {
			cItem.Title = pcImpl->TypeName(*pcSubComponent);
		}

		cTreeItems.push_back(cItem);
	}

	if (true == bInVisibleComponent) {
		bTreeExpand = false;
	}

	if (false == cTreeItems.empty()) {
		Delivery().modelPanel.AddChildren((DWORD_PTR)pcParentItem, cTreeItems, bTreeExpand);
	}

	if (true == bRecursiveExpand) {
		for (auto pcSubComponent : cSubComponents) {
			ComponentExpanded(*pcSubComponent, bRecursiveExpand, bTreeExpand);
		}
	}
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
			if (true == strUserName.IsEmpty()) {
				strUserName = L"Unknown";
			}

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

bool KERNEL::Operator::ModelPanelImpl::IsVisible(Component & cInComponent)
{
	ComponentImpl * pcImpl = dynamic_cast<ComponentImpl *>(cInComponent.GetImpl());
	DEBUG_VALID(pcImpl);

	switch (pcImpl->m_eType)
	{
		case Component::Type::ExchangePartDefinition:
			return false;
			break;
	}

	return true;
}

//== ModelPanel 관련 함수 ============================================================================

KERNEL::Operator::ModelPanel::ModelPanel(const DocView * pcInDocView)
{
	auto pcImpl = new ModelPanelImpl(pcInDocView);
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;
}

// #ModelPanel: Initialize
void KERNEL::Operator::ModelPanel::Initialize(H3DF::CADModel & cInCadModel)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->m_pcCadModel = &cInCadModel;

	// Tree를 Update를 하지 않도록 설정
	pcImpl->Delivery().modelPanel.RedrawTree(false);

	Signal::TreeItem cItem;
	cItem.ParentKey = 0;
	cItem.Key = (DWORD_PTR)&cInCadModel;
	cItem.Title = cInCadModel.GetName();

	// Root Item을 추가
	pcImpl->Delivery().modelPanel.AddItem(cItem);

	pcImpl->ComponentExpanded(cInCadModel, true);

	pcImpl->Delivery().modelPanel.RedrawTree(true);
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

	ModelTreeItem * pcRootItem = pcImpl->m_cModelTree.AddItem(nModelKey, nullptr, true); // 내부 Tree 생성
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
	CString strText;
	cSelItem.ShowPathString(strText);
#endif

	// 키값 배열을 가져온다.
	H3DF::KeyArray cKeys;
	cPath.ShowKeys(cKeys);

	std::reverse(cKeys.begin(), cKeys.end());

	Component * pcComponent = &pcImpl->CadModel();
	Component * pcFindSubComponent = nullptr;

	ComponentArray cFindSubcomponents;

	// 가져온 키값을 이용해서 Component를 찾는다.
	for (auto & cKey : cKeys) {
		ComponentArray & cSubcomponents = pcComponent->GetSubcomponents();

		if (true == cSubcomponents.empty()) {
			continue;
		}

		for (auto pcSubComponent : cSubcomponents) {
			if (cKey.KeyValue() == pcSubComponent->GetIncludeKey() || cKey.KeyValue() == pcSubComponent->GetSegmentKey()) {
				pcFindSubComponent = pcSubComponent;
				break;
			}
		}

		if (nullptr == pcFindSubComponent) {
			DEBUG_STOP;
			continue;
		}

		pcComponent = pcFindSubComponent;
	}

	pcImpl->Delivery().modelPanel.ExpandParent((DWORD_PTR)pcComponent);

	return;
}

//== Item Expanded 관련 함수 =========================================================================

// 1. Item Expanded Signal 처리
void KERNEL::Operator::ModelPanel::OnItemExpandedSignal(Json::Object & cInObject)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	DWORD_PTR nInItemKey = cInObject.GetDwordPtr(SKW_KEY);

/*
	ModelTreeItem * pcItem = dynamic_cast<ModelTreeItem *>((ModelTreeItem *)nInItemKey);
	if (nullptr == pcItem) {
		DEBUG_STOP;
		return;
	}

	pcImpl->UserInterfaceItemExpanded(pcItem);
*/
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

	DWORD_PTR nInComponent = cInObject.GetDwordPtr(SKW_KEY);
	//:Ken - checked or unchecked...
	bool bChecked = cInObject.GetBoolean(SKW_CHECKED);

	Component * pcComponent = dynamic_cast<Component *>((Component *)nInComponent);
	if (nullptr == pcComponent) {
		DEBUG_STOP;
		return;
	}

	HC_KEY nKey = pcComponent->GetSegmentKey();
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
	pcImpl->CadModel().ShowSelectionResult(pcComponent, cResults);
	//pcImpl->ModelTree().ShowSelectionResult(pcComponent, cResults);

	if (true == bChecked) {
		pcImpl->Select().SelectByResult(cResults);
	}
	else {
		pcImpl->Select().Unhighlight(cResults);
	}
}