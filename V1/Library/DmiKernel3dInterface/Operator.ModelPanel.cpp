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

			H3DF::CADModel & CADModel() { return *m_pcCadModel; }
			H3DF::CADModel * m_pcCadModel = nullptr;

			H3DF::Entity::ModelTree & ModelTree() { return m_cModelTree; }
			H3DF::Entity::ModelTree m_cModelTree;

			KERNEL::Operator::Select & Select();
			KERNEL::Operator::Attribute & Attribute();

			void ComponentExpanded(Component & cInComponent, bool bRecursiveExpand = false, bool bTreeExpand = true);
			
			void ComponentChecked(Component & cInComponent, bool bChecked, bool bRecursiveExpand = false);

			void GetCheckedItemStatuses(Component & cInComponent, Signal::TreeItemStatuses & cInItemStatuses, bool bInRecursive = false);

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

// 1. Component 전개 처리
// bRecursiveExpand 계속해서 하부 전개를 하고, bTreeExpand는 Tree에 나타낼때 펼쳐진 상태인지 아닌지를 설정한다.
void KERNEL::Operator::ModelPanelImpl::ComponentExpanded(H3DF::Component & cInComponent, bool bRecursiveExpand, bool bTreeExpand)
{
	ComponentArray & cSubComponents = cInComponent.GetSubComponents();

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

	for (auto pcComponent : cSubComponents) {
		if (false == IsVisible(*pcComponent)) {
			continue;
		}

		cItem.Checked = (H3DF::Component::Status::NoShow & pcComponent->GetStatus()) ? false : true;
		cItem.Key = (DWORD_PTR)pcComponent;
#ifdef _DEBUG
		CString strText;
		if (INVALID_KEY == pcComponent->GetIncludeKey()) {
			SegmentKey cSegment(pcComponent->GetSegmentKey());
			CString strName;
			strName = cSegment.Name(false);
			strText.Format(L"%s : %s, %s, Seg [%d]:", pcComponent->GetName(), strName, H3DF::ComponentImpl::TypeName(*pcComponent), pcComponent->GetSegmentKey());
		}
		else {
			SegmentKey cSegment(pcComponent->GetSegmentKey());
			CString strName;
			strName = cSegment.Name(false);
			strText.Format(L"%s : %s, %s, Seg [%d], Inc [%d]", pcComponent->GetName(), strName, H3DF::ComponentImpl::TypeName(*pcComponent), pcComponent->GetSegmentKey(), pcComponent->GetIncludeKey());
		}
		
		cItem.Title = strText;
#else
		cItem.Title = pcComponent->GetName();
#endif
		

		if (true == cItem.Title.IsEmpty()) {
			cItem.Title = pcImpl->TypeName(*pcComponent);
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

void KERNEL::Operator::ModelPanelImpl::ComponentChecked(H3DF::Component & cInComponent, bool bChecked, bool bRecursiveExpand)
{
	Signal::KeyItems cItems;

	ComponentArray & cSubComponents = cInComponent.GetSubComponents();

	for (auto * pcSubComponent : cSubComponents) {
		if (true == pcSubComponent->GetSubComponents().empty()) {
			cItems.push_back((DWORD_PTR)pcSubComponent);
		}
	}

	Delivery().modelPanel.CheckItems(cItems, bChecked);

	if (true == bRecursiveExpand) {
		for (auto pcSubComponent : cSubComponents) {
			ComponentChecked(*pcSubComponent, bChecked, bRecursiveExpand);
		}
	}
}

// 2. Component Checked Update를 위해서 Component의 Checked 상태를 전송하기 위한 KeyItems를 구한다.
void KERNEL::Operator::ModelPanelImpl::GetCheckedItemStatuses(Component & cInComponent, Signal::TreeItemStatuses & cInItemStatuses, bool bInRecursive)
{
	ComponentArray & cSubComponents = cInComponent.GetSubComponents();

	for (auto pcSubComponent : cSubComponents) {
		if (true == pcSubComponent->GetSubComponents().empty()) {
			Signal::TreeItemStatus cItemStatus;
			cItemStatus.Key = (DWORD_PTR)pcSubComponent;
			cItemStatus.Flag = (H3DF::Component::Status::NoShow & pcSubComponent->GetStatus()) ? false : true;

			cInItemStatuses.push_back(cItemStatus);
		}
	}

	if (true == bInRecursive) {
		for (auto pcSubComponent : cSubComponents) {
			GetCheckedItemStatuses(*pcSubComponent, cInItemStatuses, bInRecursive);
		}
	}
}

// 3. Component가 Visible인지 확인한다.
bool KERNEL::Operator::ModelPanelImpl::IsVisible(Component & cInComponent)
{
#ifdef _DEBUG
	return true;
#endif

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

	//pcImpl->UserInterfaceItemExpanded(pcModelsGroupItem, true);

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
void KERNEL::Operator::ModelPanel::SelectItem(H3DF::SelectionItem & cSelItem)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	Component * pcComponent = pcImpl->CADModel().GetComponent(cSelItem);

	pcImpl->Delivery().modelPanel.ExpandParent((DWORD_PTR)pcComponent);

	return;
}

//== Item Checked 관련 함수 ==========================================================================

// 1. 입력받은 SelectionResults를 이용해서 Component의 Checked를 설정한다.
void KERNEL::Operator::ModelPanel::Checked(H3DF::SelectionResults & cInResults, bool bInChecked)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	H3DF::CADModel & cCadModel = pcImpl->CADModel();

	Signal::KeyItems cItems;

	SelectionResultsIterator cIter = cInResults.GetIterator();

	while (true == cIter.IsValid()) {
		SelectionItem cItem = cIter.GetItem();

		Component * pcComponent = pcImpl->GetDocView().CADModel().GetComponent(cItem);
		if (nullptr == pcComponent) {
			DEBUG_STOP;
			cIter.Next();
			continue;
		}

		cItems.push_back((DWORD_PTR)pcComponent);

		cIter.Next();
	}

	pcImpl->Delivery().modelPanel.CheckItems(cItems, bInChecked);
}



// 2. CADModel에 들어	있는 모든 Component를 Checked로 설정한다.
void KERNEL::Operator::ModelPanel::CheckedAll(bool bChecked)
{
	auto * pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	H3DF::CADModel & cCadModel = pcImpl->CADModel();

	pcImpl->ComponentChecked(cCadModel, bChecked, true);
}

void KERNEL::Operator::ModelPanel::CheckedUpdate(Component & cInComponent)
{
	auto * pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	Signal::TreeItemStatuses cItemStatuses;

	pcImpl->GetCheckedItemStatuses(cInComponent, cItemStatuses, true);

	pcImpl->Delivery().modelPanel.CheckItems(cItemStatuses);
}

//== Item Expanded 관련 함수 =========================================================================

// 1. Item Expanded Signal 처리
//== Item Selelect Changed 관련 함수 =================================================================

void KERNEL::Operator::ModelPanel::OnItemExpandedSignal(Json::Object & cInObject)
{
	// 이미 전개되어 있기때문에 특별히 할일이 없음.

	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	DWORD_PTR nInItemKey = cInObject.GetDwordPtr(SKW_KEY);
}

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

	pcImpl->PrepareUpdate();

	if (true == bChecked) {
		pcImpl->Attribute().Show(pcComponent);
	}
	else {
		pcImpl->Attribute().NoShow(pcComponent);
	}

	pcImpl->Updated();

	pcImpl->GetDocView().Camera().FitWorldOnly();

	pcImpl->GetDocView().Save(L"Z:/OnItemCheckedSignal.hsf");
}