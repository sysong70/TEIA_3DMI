#include "StdAfx.h"

#include "Command.ModelPanel.h"

#include "Command.Attribute.h"
#include "Command.Select.h"

#include "Kernel.Session.h"
#include "Impl/Kernel.Session.Impl.h"
#include "Impl/Command.Set.Impl.h"

#include "Signal.Connector.h"

#include <3DF/CuttingSection.h>
#include <3DF/KeyPath.h>
#include <3DF/Selection.h>
#include <3DF/Impl/SelectionImpl.h>
#include <3DF/Bounding.h>
#include <3DF/Facility.AppOptions.h>
#include <3DF/3DF.Utility.h>

#include <3DF/Impl/SegmentImpl.h>

#include <Sprocket/3DF.View.h>
#include <Sprocket/3DF.CADModel.h>
#include <Sprocket/Impl/CADModelImpl.h>
#include <Sprocket/3DF.MetaData.h>

#include <Path.h>

#include <ranges>
#include <unordered_map>

#define MODELS_GROUP_KEY				1
#define MEASUREMENTS_GROUP_KEY			2
#define MARKUPS_GROUP_KEY				3

using namespace KERNEL;
using namespace H3DF;

//== ModelPanelImpl 관련 함수 ========================================================================

namespace KERNEL
{
	namespace Command
	{
		class ModelPanelImpl : public SetImpl
		{
		public:
			ModelPanelImpl(const Session * pcInSession);

			void Copy(ModelPanelImpl * pcInThat) {
				SetImpl::Copy(pcInThat);
			}

			H3DF::CADModel & CADModel() { return *m_pcCadModel; }
			H3DF::CADModel * m_pcCadModel = nullptr;

			KERNEL::Command::Select & Select();
			KERNEL::Command::Attribute & Attribute();
			KERNEL::Command::Camera & Camera();

			void ComponentExpanded(Component & cInComponent, int nLevel = 0);
			
			void ComponentChecked(Component & cInComponent, bool bChecked, bool bRecursiveExpand = false);

			void GetCheckedItemStatuses(Component & cInComponent, Signal::TreeItemStatuses & cInItemStatuses, bool bInRecursive = false);

			bool IsVisible(Component & cInComponent);

			void TreeReverseExpand(H3DF::Component & cInComponent);

			Component * GetPmiGroupComponent();

			Component * m_pcPmiGroupComponent = nullptr;

			std::vector<CuttingSectionKey> m_aCuttingSections;
		};
	}
}

KERNEL::Command::ModelPanelImpl::ModelPanelImpl(const Session * pcInSession) :
	SetImpl(pcInSession)
{

}

KERNEL::Command::Select & KERNEL::Command::ModelPanelImpl::Select()
{ 
	SessionImpl * pcImpl = (SessionImpl *)GetSession().GetImpl();
	DEBUG_VALID(pcImpl);

	return pcImpl->Select();
}

KERNEL::Command::Attribute & KERNEL::Command::ModelPanelImpl::Attribute()
{
	SessionImpl * pcImpl = (SessionImpl *)GetSession().GetImpl();
	DEBUG_VALID(pcImpl);

	return pcImpl->Attribute();
}

KERNEL::Command::Camera & KERNEL::Command::ModelPanelImpl::Camera()
{
	SessionImpl * pcImpl = (SessionImpl *)GetSession().GetImpl();
	DEBUG_VALID(pcImpl);

	return pcImpl->Camera();
}

// 1. Component 전개 처리
// bRecursiveExpand 계속해서 하부 전개를 하고, bTreeExpand는 Tree에 나타낼때 펼쳐진 상태인지 아닌지를 설정한다.
void KERNEL::Command::ModelPanelImpl::ComponentExpanded(H3DF::Component & cInComponent, int nLevel)
{
	ComponentArray * pcSubComponents = cInComponent.GetSubComponents();
	if(nullptr == pcSubComponents) {
		return;
	}

	Signal::TreeItems cTreeItems;
	Signal::TreeItem cItem;

	H3DF::Component * pcParentItem = &cInComponent;

	// 입력받은 Item이 Tree에 표시되지 않는 Component면 Parent로 사용하지 않고 상위 Parent를 찾는다.
	while (nullptr != pcParentItem) {
		// 부모가 Invisible이면 상위 Parent를 찾는다. 찾은 상위 Parent에 하부 Component를 추가한다.
		if (false == IsVisible(*pcParentItem)) {
			pcParentItem = pcParentItem->GetOwner();
		}
		else {
			break;
		}
	}

	cItem.ParentKey = (DWORD_PTR)pcParentItem;

	// View Group 및 PMI Group을 우선적으로 표시한다.
	// View Group Item 생성.
	for (auto pcComponent : *pcSubComponents) {
		if (H3DF::Component::Status::UiUpdate & pcComponent->GetStatus()) {
			continue;
		}

		if (H3DF::Component::Type::ViewGroupComponent == pcComponent->GetType()) {
			if (nullptr != pcComponent->GetSubComponents()) {
				cItem.Key = (DWORD_PTR)pcComponent;
#ifdef _DEBUG
				CString strText;

				SegmentKey cSegment(pcComponent->GetSegmentKey());
				CString strName;
				strName = cSegment.Name(false);
				strText.Format(L"%s : %s, Seg [%d]:", pcComponent->GetName(), strName, pcComponent->GetSegmentKey());

				cItem.Title = strText;
#else
				cItem.Title = pcComponent->GetName();
#endif
				cItem.HasChildren = true;
				pcComponent->AddStatus(H3DF::Component::Status::UiUpdate);
				cTreeItems.push_back(cItem);
			}
			break;
		}
	}

	// PMI Group Item 생성.
	for (auto pcComponent : *pcSubComponents) {
		if (H3DF::Component::Status::UiUpdate & pcComponent->GetStatus()) {
			continue;
		}

		if (H3DF::Component::Type::PMIGroupComponent == pcComponent->GetType()) {
			if (nullptr != pcComponent->GetSubComponents()) {
				cItem.Key = (DWORD_PTR)pcComponent;

#ifdef _DEBUG
				CString strText;

				SegmentKey cSegment(pcComponent->GetSegmentKey());
				CString strName;
				strName = cSegment.Name(false);
				strText.Format(L"%s : %s, Seg [%d]:", pcComponent->GetName(), strName, pcComponent->GetSegmentKey());

				cItem.Title = strText;
#else
				cItem.Title = pcComponent->GetName();
#endif
				cItem.HasChildren = true;
				pcComponent->AddStatus(H3DF::Component::Status::UiUpdate);
				cTreeItems.push_back(cItem);
			}
			break;
		}
	}

	// View나 PMI Group이 있는 경우 먼저 Update하도록 한다. View나 PMI Group이 먼저 표시되도록 하기 위해서임.
	if (false == cTreeItems.empty()) {
		Delivery().modelPanel.AddChildren((DWORD_PTR)pcParentItem, cTreeItems);
		cTreeItems.clear();
	}

	Signal::KeyItems cNoShowTreeItems;

	// 나머지 Component를 표시한다.
	for (auto pcComponent : *pcSubComponents) {
		if (false == IsVisible(*pcComponent)) {
			ComponentExpanded(*pcComponent, nLevel);
			continue;
		}

		if (H3DF::Component::Status::UiUpdate & pcComponent->GetStatus()) {
			continue;
		}

		if (H3DF::Component::Type::ExchangeProductOccurrence == cInComponent.GetType()) {
			if (H3DF::Component::Type::ViewGroupComponent == pcComponent->GetType() ||
				H3DF::Component::Type::PMIGroupComponent == pcComponent->GetType()) {
				continue;
			}
		}

		// Show/NoShow 상태를 설정.
		cItem.Checked = (H3DF::Component::Status::NoShow & pcComponent->GetStatus()) ? false : true;
		cItem.Key = (DWORD_PTR)pcComponent;

		// Title 설정
#ifdef _DEBUG
		CString strText;
		if (INVALID_KEY == pcComponent->GetIncludeKey()) {
			SegmentKey cSegment(pcComponent->GetSegmentKey());
			CString strName;
			strName = cSegment.Name(false);
			strText.Format(L"%s : %s, %s, Seg [%d]:", pcComponent->GetName(), strName, H3DF::ComponentUtility::TypeName(*pcComponent), pcComponent->GetSegmentKey());
		}
		else {
			SegmentKey cSegment(pcComponent->GetSegmentKey());
			CString strName;
			strName = cSegment.Name(false);
			strText.Format(L"%s : %s, %s, Seg [%d], Inc [%d]", pcComponent->GetName(), strName, H3DF::ComponentUtility::TypeName(*pcComponent), pcComponent->GetSegmentKey(), pcComponent->GetIncludeKey());
		}

		cItem.Title = strText;
#else
		cItem.Title = pcComponent->GetName();
#endif

		// 하부 Child가 있는 확인.
		if (H3DF::Component::Type::ExchangeMkpView != pcComponent->GetType()) {
			cItem.HasChildren = (nullptr != pcComponent->GetSubComponents()) ? true : false;
		}
		else {
			// #CADModel: Markup View의 하부 Component는 표시 제어
			// Markup View의 하부 Component는 표시하지 않는다.
			// Markyp View의	 하부 Component는 Visibility가 Off되어 있고, Include로 연결만 되어 있는 상태로 저장되어 있음.
			cItem.HasChildren = false;
			cNoShowTreeItems.push_back((DWORD_PTR)pcComponent);
		}

		if (true == cItem.Title.IsEmpty()) {
			cItem.Title = H3DF::CADModelUtility::TypeName(m_pcCadModel, *pcComponent);
		}

		pcComponent->AddStatus(H3DF::Component::Status::UiUpdate);

		cTreeItems.push_back(cItem);
	}

	if (false == cTreeItems.empty()) {
		Delivery().modelPanel.AddChildren((DWORD_PTR)pcParentItem, cTreeItems);
		//Delivery().modelPanel.ExpandParent((DWORD_PTR)pcParentItem);
	}

	if (false == cNoShowTreeItems.empty()) {
		Delivery().modelPanel.CheckItems(cNoShowTreeItems, false);
	}

	if (0 < nLevel)
	{
		for (auto pcSubComponent : *pcSubComponents) {
			ComponentExpanded(*pcSubComponent, nLevel - 1);
		}
	}
}

void KERNEL::Command::ModelPanelImpl::ComponentChecked(H3DF::Component & cInComponent, bool bChecked, bool bRecursiveExpand)
{
	ComponentArray * pcSubComponents = cInComponent.GetSubComponents();
	if (nullptr == pcSubComponents) {
		return;
	}

	Signal::KeyItems cItems;

	for (auto * pcSubComponent : *pcSubComponents) {
		if (nullptr == pcSubComponent->GetSubComponents()) {
			cItems.push_back((DWORD_PTR)pcSubComponent);
		}
	}

	if (false == cItems.empty()) {

		for (auto Item : cItems) {
			if (0 == Item) {
				DEBUG_STOP;
			}
		}

		Delivery().modelPanel.CheckItems(cItems, bChecked);
	}

	if (true == bRecursiveExpand) {
		for (auto pcSubComponent : *pcSubComponents) {
			ComponentChecked(*pcSubComponent, bChecked, bRecursiveExpand);
		}
	}
}

// 2. Component Checked Update를 위해서 Component의 Checked 상태를 전송하기 위한 KeyItems를 구한다.
void KERNEL::Command::ModelPanelImpl::GetCheckedItemStatuses(Component & cInComponent, Signal::TreeItemStatuses & cInItemStatuses, bool bInRecursive)
{
	ComponentArray * paSubComponents = cInComponent.GetSubComponents();

	if(nullptr == paSubComponents && true == IsVisible(cInComponent)) {
		Signal::TreeItemStatus cItemStatus;
		cItemStatus.Key = (DWORD_PTR)&cInComponent;
		cItemStatus.Flag = (H3DF::Component::Status::NoShow & cInComponent.GetStatus()) ? false : true;
		cInItemStatuses.push_back(cItemStatus);
	}

	if (nullptr == paSubComponents) {
		return;
	}

	for (auto * pcSubComponent : *paSubComponents) {
		// 최하단 Ri Item만 처리한다.
		if (nullptr == pcSubComponent->GetSubComponents() && true == IsVisible(*pcSubComponent)) {
			Signal::TreeItemStatus cItemStatus;
			cItemStatus.Key = (DWORD_PTR)pcSubComponent;
			cItemStatus.Flag = (H3DF::Component::Status::NoShow & pcSubComponent->GetStatus()) ? false : true;

			cInItemStatuses.push_back(cItemStatus);
		}
	}

	if (true == bInRecursive) {
		for (auto pcSubComponent : *paSubComponents) {
			GetCheckedItemStatuses(*pcSubComponent, cInItemStatuses, bInRecursive);
		}
	}
}

// 3. Component가 Visible인지 확인한다.
bool KERNEL::Command::ModelPanelImpl::IsVisible(Component & cInComponent)
{
#ifdef _DEBUG
	//return true;
#endif
	switch (cInComponent.GetType())
	{
		case Component::Type::ExchangePartDefinition:
			return false;
			break;
	}

	return true;
}

// 4. Component가 UI상에서 Update되어 있지 않은 경우 역방향으로 Tree를 전개한다.
void KERNEL::Command::ModelPanelImpl::TreeReverseExpand(H3DF::Component & cInComponent)
{
	// 입력된 Component가 UI Update되어 있지 않은 경우, 상위 Component를 찾아서 Tree를 전개한다.
	if (H3DF::Component::Status::UiUpdate & cInComponent.GetStatus()) {
		return;
	}

	// 전개하지 않는 요소는 더이상 전개하지 않도록 한다.
	if (H3DF::Component::Type::ExchangeMkpView == cInComponent.GetType()) {
		return;
	}

	Component * pcOwner = nullptr;

	std::vector<H3DF::Component *> aOwnerComponents;
	
	pcOwner = cInComponent.GetOwner();
	while (nullptr != pcOwner) {
#ifdef _DEBUG
		CString strName = pcOwner->GetName();

		SegmentKey cSegmentKey(pcOwner->GetSegmentKey());
		CStringA strSegmentName = cSegmentKey.Name(false);
#endif
		aOwnerComponents.push_back(pcOwner);

		// UI에 Component는 Update되어 있어도 하부 SubComponent는 Update되어 있지 않기 때문에 Update된 Owner까지 찾아서 Expand를 한다.
		if (H3DF::Component::Status::UiUpdate & pcOwner->GetStatus()) {
			break;
		}

		pcOwner = pcOwner->GetOwner();
	}

	if (true == aOwnerComponents.empty()) {
		DEBUG_STOP;
		return;
	}

	std::reverse(aOwnerComponents.begin(), aOwnerComponents.end());

	for (auto * pcOwnerComponent : aOwnerComponents) {
		ComponentExpanded(*pcOwnerComponent);
	}
}

// 5. 주어진 Type의 Component를 찾아서 반환한다.
Component * KERNEL::Command::ModelPanelImpl::GetPmiGroupComponent()
{
	if (nullptr != m_pcPmiGroupComponent) {
		return m_pcPmiGroupComponent;
	}

	if (nullptr == m_pcCadModel) {
		DEBUG_STOP;
		return nullptr;
	}

	ComponentArray * pcSubcomponents = m_pcCadModel->GetAllSubcomponents(H3DF::Component::Type::PMIGroupComponent);
	if (nullptr == pcSubcomponents) {
		DEBUG_STOP
		return nullptr;
	}

	if (true == pcSubcomponents->empty()) {
		return nullptr;
	}

	if (1 < pcSubcomponents->size()) {
		DEBUG_STOP;
	}

	m_pcPmiGroupComponent = pcSubcomponents->front();
	
	delete pcSubcomponents;

	return m_pcPmiGroupComponent;
}

//== ModelPanel 관련 함수 ============================================================================

KERNEL::Command::ModelPanel::ModelPanel(const Session * pcInSession) :
	Set(pcInSession)
{
	auto pcImpl = new ModelPanelImpl(pcInSession);
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;
}

// #ModelPanel: Initialize
void KERNEL::Command::ModelPanel::Initialize(H3DF::CADModel & cInCadModel)
{
	auto * pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
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

	pcImpl->ComponentExpanded(cInCadModel, 2);

	pcImpl->Delivery().modelPanel.RedrawTree(true);
	//:Ken - 20240419
	pcImpl->Delivery().modelPanel.ViewItem((DWORD_PTR)&cInCadModel);
}

// 1. 초기화 함수
void KERNEL::Command::ModelPanel::Initialize(CString strFilePathName)
{
/*
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
	//:Ken - 20240419
	pcImpl->Delivery().modelPanel.ViewItem((DWORD_PTR)pcRootItem);*/
}

//== Signal 처리 관련 함수 ============================================================================

// 1. Signal 처리
void KERNEL::Command::ModelPanel::Signal(Json::Object & cInObject)
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

		case Signal::ModelPanel::Action::OnItemDblClicked:
			OnItemDblClickedSignal(cInObject);
			break;

		default:
			DEBUG_STOP;
			break;
	}
}

//== Select 관련 함수 ===============================================================================

// 1. 외부에서 전달된 Selection Item을 이용해서 Model Tree를 설정한다.
void KERNEL::Command::ModelPanel::SelectTreeItem(H3DF::SelectionItem & cSelItem)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	H3DF::Component * pcComponent = pcImpl->CADModel().GetComponent(cSelItem);

	H3DF::Component::Type eType = pcComponent->GetType();

	pcImpl->Delivery().modelPanel.RedrawTree(false);

	if (!(H3DF::Component::Status::UiUpdate & pcComponent->GetStatus())) {
		pcImpl->TreeReverseExpand(*pcComponent);
	}

	pcImpl->Delivery().modelPanel.ExpandParent((DWORD_PTR)pcComponent);

	pcImpl->Delivery().modelPanel.RedrawTree(true);
	//:Ken - 20240403
	pcImpl->Delivery().modelPanel.SelectItem((DWORD_PTR)pcComponent);

	return;
}

//== Item Checked 관련 함수 ==========================================================================

// 1. 입력받은 SelectionResults를 이용해서 Component의 Checked를 설정한다.
void KERNEL::Command::ModelPanel::Checked(H3DF::SelectionResults & cInResults, bool bInChecked)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	H3DF::CADModel & cCadModel = pcImpl->CADModel();

	Signal::KeyItems cItems;

	SelectionResultsIterator cIter = cInResults.GetIterator();

	while (true == cIter.IsValid()) {
		SelectionItem cItem = cIter.GetItem();

		Component * pcComponent = pcImpl->GetSession().CADModel().GetComponent(cItem);
		if (nullptr == pcComponent) {
			DEBUG_STOP;
			cIter.Next();
			continue;
		}

		cItems.push_back((DWORD_PTR)pcComponent);

		cIter.Next();
	}

	if (false == cItems.empty()) {
		for (auto Item : cItems) {
			if (0 == Item) {
				DEBUG_STOP;
			}
		}

		pcImpl->Delivery().modelPanel.CheckItems(cItems, bInChecked);
	}
}



// 2. CADModel에 들어	있는 모든 Component를 Checked로 설정한다.
void KERNEL::Command::ModelPanel::CheckedAll(bool bChecked)
{
	auto * pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	H3DF::CADModel & cCadModel = pcImpl->CADModel();

	pcImpl->ComponentChecked(cCadModel, bChecked, true);
}

void KERNEL::Command::ModelPanel::CheckedUpdate(Component & cInComponent)
{
	auto * pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	Signal::TreeItemStatuses cItemStatuses;

	pcImpl->GetCheckedItemStatuses(cInComponent, cItemStatuses, true);

	if (false == cItemStatuses.empty()) {

		for (auto cStatus : cItemStatuses) {
			if (0 == cStatus.Key) {
				DEBUG_STOP;
			}
		}

		pcImpl->Delivery().modelPanel.CheckItems(cItemStatuses);
	}
}

//== Item Expanded 관련 함수 =========================================================================

//== Item Selelect Changed 관련 함수 =================================================================

// 1. Item Expanded Signal 처리
void KERNEL::Command::ModelPanel::OnItemExpandedSignal(Json::Object & cInObject)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	Component * pcComponent = dynamic_cast<Component *>((Component *)cInObject.GetDwordPtr(SKW_KEY));
	if (nullptr == pcComponent) {
		DEBUG_STOP;
		return;
	}

	// Tree를 Update를 하지 않도록 설정
	pcImpl->Delivery().modelPanel.RedrawTree(false);

	pcImpl->ComponentExpanded(*pcComponent);

	// Tree를 Update를 하도록 설정
	pcImpl->Delivery().modelPanel.RedrawTree(true);

	// bool bExpanded = cInObject.GetBoolean(SKW_EXPANDED);
}

// 1. Item Select Changed Signal 처리
void KERNEL::Command::ModelPanel::OnItemSelectedSignal(Json::Object & cInObject)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	H3DF::Component * pcInComponent = dynamic_cast<Component *>((Component *) cInObject.GetDwordPtr(SKW_KEY));
	if (nullptr == pcInComponent) {
		DEBUG_STOP;
		return;
	}

	H3DF::Component::Type eType = pcInComponent->GetType();

	if (H3DF::Component::Type::ExchangeMkpView == eType) {
		ExchangeMkpViewSelectedSignal(pcInComponent);
		return;
	}
	// Product Occurrence인 경우 처리, 일단 Product Occurrence를 대상으로 작업한다.
	// 다만 다른 아이탬과 차이가 있는지는 알 수 없음.
	else if (H3DF::Component::Type::ExchangeProductOccurrence == eType) {
		ProductOccurrenceSelectedSignal(pcInComponent);
		return;
	}
}
	

// 1.1 ExchangeMkpView Select Changed Signal 처리 #Tree-Select
void KERNEL::Command::ModelPanel::ExchangeMkpViewSelectedSignal(H3DF::Component * pcInComponent)
{
	if (nullptr == pcInComponent) {
		DEBUG_STOP;
		return;
	}

	if (H3DF::Component::Type::ExchangeMkpView != pcInComponent->GetType()) {
		DEBUG_STOP;
		return;
	}

	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	// 선택된 요소가 Markup View인 경우 처리.
	// 1. 저장되어 있는 Camera 정보를 이용해서 View Position을 설정
	// 2. View 하부에 있는 Include로 Link 되어 있는 PMI를 Show 처리 한다.
	// 3. PMI Group 하부에 있는 PMI들은 Noshow 처리

	H3DF::Component * pcMkpViewComponent = pcInComponent;

	DwordPtrMetaData * pcCameraData = (DwordPtrMetaData *)pcMkpViewComponent->GetMetaData(H3DF::MetaDataIndex::Camera);

	DwordPtrMetaData * pcMatrixData = (DwordPtrMetaData *)pcMkpViewComponent->GetMetaData(H3DF::MetaDataIndex::ViewMatrix);

	DwordPtrMetaData * pcCuttingPlanesData = (DwordPtrMetaData *)pcMkpViewComponent->GetMetaData(H3DF::MetaDataIndex::CuttingPlanes);

	// Pmi Group 하부에 있는 PMI들은 Noshow 처리
	H3DF::Component * pcPmiGroupComp = H3DF::ComponentUtility::GetPmiGroupComponent(*pcInComponent);
	if (nullptr == pcPmiGroupComp) {
		DEBUG_STOP;
		return;
	}

	H3DF::Component * pcViewGroupComp = H3DF::ComponentUtility::GetViewGroupComponent(*pcInComponent);
	if (nullptr == pcViewGroupComp) {
		DEBUG_STOP;
		return;
	}

	// PMI를 NoShow 처리
	pcImpl->Attribute().NoShow(pcPmiGroupComp);

	pcImpl->Delivery().modelPanel.RedrawTree(false);
	
	pcImpl->Delivery().modelPanel.CheckItem((DWORD_PTR)pcViewGroupComp, false);

	pcImpl->Delivery().modelPanel.CheckItem((DWORD_PTR)pcPmiGroupComp, false);
	
	pcImpl->Delivery().modelPanel.CheckItem((DWORD_PTR)pcInComponent, true);

	// Markup View 하부에 PMI들이 있는 경우 Show 처리
	if (nullptr != pcMkpViewComponent->GetSubComponents()) {
		if (true == pcMkpViewComponent->GetSubComponents()->empty()) {
			DEBUG_STOP;
			return;
		}

		Signal::KeyItems cShowItems;
		// PMI를 Show하는 부분
		// Markup View 하부에 있는 PMI를 PMI Group에서 찾아서 Show 처리
		for (auto * pcPmiComponent : *pcMkpViewComponent->GetSubComponents()) {
			H3DF::Component * pcFindComp = H3DF::ComponentUtility::FindSubComponentBySegmentKey(*pcPmiGroupComp, pcPmiComponent->GetSegmentKey(), true);

			if (nullptr != pcFindComp) {
				pcImpl->Attribute().Show(pcFindComp);

				if (H3DF::Component::Status::UiUpdate & pcFindComp->GetStatus()) {
					cShowItems.push_back((DWORD_PTR)pcFindComp);
				}
			}
		}
		pcImpl->Delivery().modelPanel.CheckItems(cShowItems, true);
	}

	pcImpl->Delivery().modelPanel.RedrawTree(true);

	SegmentKey cSegment = pcImpl->GetSession().Canvas().GetModel().GetSegmentKey().Subsegment("cutting_section");
	cSegment.Flush(H3DF::Search::Type::Geometry, H3DF::Search::Space::SubsegmentsAndIncludes);

	// Cutting Section 설정, 정보가 있다면 Cutting Section을 설정한다.
	if (nullptr != pcCuttingPlanesData) {
		H3DF::PlaneArray * pcCuttingPlanes = (H3DF::PlaneArray *)pcCuttingPlanesData->GetValue();
		SegmentKey cSegment = pcImpl->GetSession().Canvas().GetModel().GetSegmentKey().Subsegment("cutting_section");

		for (auto cPlane : *pcCuttingPlanes) {
			cSegment.InsertCuttingSection(cPlane);
		}
	}

	// View를 Update해야 Fitting이 정확하게 됨.
	pcImpl->GetSession().Canvas().GetFrontView().Update();

	// Makrup View에 Sub component가 없는 경우 Camera 정보를 이용해서 설정한다.
	// Sub component가 없다는 것은, 하부에 PMI가 없는 경우임.
	if (nullptr != pcCameraData) { // && nullptr == pcInComponent->GetSubComponents()) {
		H3DF::CameraKit * pcCamera = (H3DF::CameraKit *)pcCameraData->GetValue();
		pcImpl->Camera().SetCamera(*pcCamera);
	}
	// Matrix 정보가 있는 경우 Matrix정보를 이용해서 처리.
	else if (nullptr != pcMatrixData) {
		SegmentKey cSegment(pcInComponent->GetSegmentKey());
		H3DF::MatrixKit * pcMatrix = (H3DF::MatrixKit *)pcMatrixData->GetValue();

		pcImpl->Camera().SetCameraFitSelection(*pcMatrix, cSegment);
	}
	// 별다른 정보가 없는 경우는 보이는 View를 Fit 하도록 한다.
	else { 
		pcImpl->Camera().FitWorldOnly();
	}
}

// 1.2 Product Occurrence Select Changed Signal 처리 #Tree-Select
void KERNEL::Command::ModelPanel::ProductOccurrenceSelectedSignal(H3DF::Component * pcInComponent)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->Select().SelectByComponent(pcInComponent);

}

// 2. Item Checked Signal 처리
void KERNEL::Command::ModelPanel::OnItemCheckedSignal(Json::Object & cInObject)
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

	if (H3DF::Component::Type::ExchangeMkpView == pcComponent->GetType() || H3DF::Component::Type::ViewGroupComponent == pcComponent->GetType()) {
		pcImpl->Delivery().modelPanel.CheckItem((DWORD_PTR)pcComponent, false);
		return;
	}

	pcImpl->PrepareUpdate();

	// 화면상에 표시되는 Component를 설정.
	if (true == bChecked) {
		pcImpl->Attribute().Show(pcComponent);
	}
	else {
		pcImpl->Attribute().NoShow(pcComponent);
	}

	pcImpl->Updated();

	//pcImpl->GetDocView().Camera().FitWorldOnly();
	//pcImpl->GetDocView().Save(L"Z:/OnItemCheckedSignal.hsf");
}

// 3. Left Double Click Signal 처리
void KERNEL::Command::ModelPanel::OnItemDblClickedSignal(Json::Object & cInObject)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	Component * pcComponent = dynamic_cast<Component *>((Component *)cInObject.GetDwordPtr(SKW_KEY));
	if (nullptr == pcComponent) {
		DEBUG_STOP;
		return;
	}
}