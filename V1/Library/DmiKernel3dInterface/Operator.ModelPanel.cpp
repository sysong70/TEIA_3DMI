#include "StdAfx.h"

#include "Operator.ModelPanel.h"

#include "Impl/OperatorImpl.h"
#include "Operator.Select.h"

#include "Signal.Connector.h"

#include <3DF/KeyPath.h>
#include <3DF/Selection.h>
#include <3DF/Impl/SelectionImpl.h>
#include <3DF/Facility.AppOptions.h>
#include <3DF/3DF.Utility.h>

#include <3DF/Impl/SegmentImpl.h>

#include <3DF.View.h>

#include <Path.h>

#include <ranges>
#include <unordered_map>

#define MODELS_GROUP_KEY				1
#define MEASUREMENTS_GROUP_KEY			2
#define MARKUPS_GROUP_KEY				3

using namespace KERNEL;
using namespace H3DF;

namespace KERNEL
{
	namespace Operator
	{
		enum class TreeItemStatus
		{
			Normal,
			End
		};

		class ModelTreeItem
		{
		public:
			ModelTreeItem(HC_KEY nInKey);
			~ModelTreeItem();

			ModelTreeItem * AddChild(HC_KEY nInChildKey, bool bEndItem);

			bool ShowChild(HC_KEY nInChildKey, ModelTreeItem * pcOutTreeItem);

			HC_KEY m_nKey = INVALID_KEY;
			TreeItemStatus m_eStatus = TreeItemStatus::Normal;

			ModelTreeItem * m_pcParent = nullptr;
			std::vector<ModelTreeItem *> m_vpnChildren;
		};

		class ModelTree
		{
		public:
			ModelTree() = default;
			~ModelTree();

			DWORD_PTR AddItem(HC_KEY nInKey, DWORD_PTR nInParentItem, bool bHasChild);

			bool ShowSelectionResult(DWORD_PTR nInItem, H3DF::SelectionResults & cOutResults);
			bool ShowPath(DWORD_PTR nInItem, H3DF::KeyPath & cOutPath);

			ModelTreeItem * Root() { return m_pcRoot; }
		
		protected:
			bool ShowSelectionEndItems(ModelTreeItem * pcInTreeItem, H3DF::SelectionResults & cOutResults);
			
			bool ExpandItem(ModelTreeItem * pcInItem, bool bRecursiveExpand);

			ModelTreeItem * m_pcRoot = nullptr;
			//std::unordered_map<DWORD_PTR, ModelTreeItem *> m_cItems;
		};
	}
}

KERNEL::Operator::ModelTreeItem::ModelTreeItem(HC_KEY nInKey)
{
	m_nKey = nInKey;
}

KERNEL::Operator::ModelTreeItem::~ModelTreeItem()
{
	for (auto pcChild : m_vpnChildren) {
		delete pcChild;
	}
}

KERNEL::Operator::ModelTreeItem * KERNEL::Operator::ModelTreeItem::AddChild(HC_KEY nInChildKey, bool bEndItem)
{
	Operator::ModelTreeItem * pcItem = new Operator::ModelTreeItem(nInChildKey);
	DEBUG_VALID(pcItem);

	pcItem->m_pcParent = this;
	pcItem->m_nKey = nInChildKey;

	if (true == bEndItem) {
		pcItem->m_eStatus = TreeItemStatus::End;
	}

	m_vpnChildren.push_back(pcItem);

	return pcItem;
}

bool KERNEL::Operator::ModelTreeItem::ShowChild(HC_KEY nInChildKey, ModelTreeItem * pcOutTreeItem)
{
	if (true == m_vpnChildren.empty()) {
		return false;
	}

	for (auto pcChild : m_vpnChildren) {
		if (nInChildKey == pcChild->m_nKey) {
			pcOutTreeItem = pcChild;
			return true;
		}
	}

	return false;
}

//== ModelTree 관련 함수 =============================================================================

KERNEL::Operator::ModelTree::~ModelTree()
{
	if (nullptr != m_pcRoot) {
		delete m_pcRoot;
	}
}

DWORD_PTR KERNEL::Operator::ModelTree::AddItem(HC_KEY nInKey, DWORD_PTR nInParentItem, bool bHasChild)
{
	if (0 == nInParentItem) {
		m_pcRoot = new ModelTreeItem(nInKey);
		DEBUG_VALID(m_pcRoot);
		return (DWORD_PTR)m_pcRoot;
	}

	ModelTreeItem * pcParentItem = dynamic_cast<ModelTreeItem *>((ModelTreeItem *)nInParentItem);
	if (nullptr == pcParentItem) {
		DEBUG_STOP;
		return 0;
	}

	// 하부 Item이 추가되면 상태를 Normal로 변경한다.
	pcParentItem->m_eStatus = TreeItemStatus::Normal;

	ModelTreeItem * pcItem = pcParentItem->AddChild(nInKey, !bHasChild);
	DEBUG_VALID(pcItem);

	return (DWORD_PTR)pcItem;
}

// 2. 주어진 Key를 이용해서 Selection Result를 생성한다.
// 선택된 Key를 기준으로 하부 Item을 검색해서 개별적으로 Result에 저장한다.
// 필요한 경우에는 하위 Item을 전개하도록 한다.
// 선택한 Item이 End Item이 아니면 Assembly에서 Modeling Matrix가 이상하게 적용되어 있을 수 있으므로 선택은 End Item을 선택하도록 한다.
// 간단히 이야기 하면 RI Item만 선택하도록 해야 한다.
bool KERNEL::Operator::ModelTree::ShowSelectionResult(DWORD_PTR nInItem, H3DF::SelectionResults & cOutResults)
{
	if (0 == nInItem) {
		return false;
	}

	ModelTreeItem * pcInItem = dynamic_cast<ModelTreeItem *>((ModelTreeItem *)nInItem);
	if (nullptr == pcInItem) {
		DEBUG_STOP;
		return false;
	}

	ExpandItem(pcInItem, true);

	ShowSelectionEndItems(pcInItem, cOutResults);

	return true;
}

// 2.1 주어진 Tree Item을 이용해서, End Item을 찾아서 Selection Item을 생성한다.
bool KERNEL::Operator::ModelTree::ShowSelectionEndItems(ModelTreeItem * pcInItem, H3DF::SelectionResults & cOutResults)
{
	if (nullptr == pcInItem) {
		return false;
	}

	if (TreeItemStatus::End == pcInItem->m_eStatus) {

		SelectionItem cSelItem;
		SelectionItemImpl * pcImpl = dynamic_cast<SelectionItemImpl *>(cSelItem.GetImpl());
		DEBUG_VALID(pcImpl);

		ModelTreeItem * pcItem = pcInItem;
		while (nullptr != pcItem->m_pcParent) {
			pcImpl->m_nIncludeCount++;
			pcItem = pcItem->m_pcParent;
		}

		HC_KEY nSegmentKey = pcInItem->m_nKey;

		char chType[MVO_BUFFER_SIZE];
		HC_Show_Key_Type(nSegmentKey, chType);

		if (streq(chType, "include")) {
			nSegmentKey = HC_Show_Include_Segment(nSegmentKey, nullptr);
		}

		pcImpl->m_cKey = SegmentKey(nSegmentKey);

		pcImpl->m_pnIncludeKeys = new HC_KEY[pcImpl->m_nIncludeCount];

		pcItem = pcInItem;

		for (int nIndex = 0; nIndex < pcImpl->m_nIncludeCount; nIndex++) {
			pcImpl->m_pnIncludeKeys[nIndex] = pcItem->m_nKey;
			pcItem = pcItem->m_pcParent;
		}

		cOutResults.PushFront(cSelItem);

		return true;
	}

	for (auto pcChild : pcInItem->m_vpnChildren) {
		ShowSelectionEndItems(pcChild, cOutResults);
	}

	return true;
}

bool KERNEL::Operator::ModelTree::ShowPath(DWORD_PTR nInItemKey, H3DF::KeyPath & cOutPath)
{
	ModelTreeItem * pcTargetItem = dynamic_cast<ModelTreeItem *>((ModelTreeItem *)nInItemKey);
	if (nullptr == pcTargetItem) {
		return false;
	}

	HC_KEY nInkey = pcTargetItem->m_nKey;

	size_t nIncludeCount = 1;

	ModelTreeItem * pcItem = pcTargetItem;
	while (nullptr != pcItem->m_pcParent) {
		nIncludeCount++;
		pcItem = pcItem->m_pcParent;
	}

	size_t nPathCount = nIncludeCount + 2;
	HC_KEY * pnPath = new HC_KEY[nPathCount];

	HC_KEY nSegmentKey = nInkey;

	char chType[MVO_BUFFER_SIZE];
	HC_Show_Key_Type(nSegmentKey, chType);

	if (streq(chType, "include")) {
		nSegmentKey = HC_Show_Include_Segment(nInkey, nullptr);
	}

	pnPath[0] = nSegmentKey;
	
	pcItem = pcTargetItem;

	for (int nIndex = 1; nIndex < nIncludeCount + 1; ++nIndex) {
		pnPath[nIndex] = pcItem->m_nKey;
		pcItem = pcItem->m_pcParent;
	}

	pnPath[nPathCount - 1] = INVALID_KEY;

	cOutPath = KeyPath(nPathCount, pnPath);

	return true;
}

// 3. 주어진 Item을 전개한다.
bool KERNEL::Operator::ModelTree::ExpandItem(ModelTreeItem * pcInItem, bool bRecursiveExpand)
{
	if (TreeItemStatus::End == pcInItem->m_eStatus) {
		return true;
	}

	if (INVALID_KEY == pcInItem->m_nKey) {
		return false;
	}

	// 이미 전개가 되어 있는 경우는 무시한다.
	if (false == pcInItem->m_vpnChildren.empty()) {
		return true;
	}

	IncludeKey cInInclude(pcInItem->m_nKey);
	SegmentKey cInSegment = cInInclude.GetTarget();

	H3DF::IncludeKeyArray cChildren;
	cInSegment.ShowIncluders(cChildren);

	for (auto & cInclude : cChildren) {

		SegmentKey cSegment = cInclude.GetTarget();

		size_t nCount = cSegment.ShowIncluders();
		bool bHasChildren = (0 < nCount) ? true : false;

		ModelTreeItem * pcItem = pcInItem->AddChild(cInclude.KeyValue(), bHasChildren);
		DEBUG_VALID(pcItem);

		if (true == bRecursiveExpand) {
			ExpandItem(pcItem, bRecursiveExpand);
		}
	}

	return true;
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

			KERNEL::Operator::ModelTree & ModelTree() { return m_cModelTree; }
			KERNEL::Operator::ModelTree m_cModelTree;

			KERNEL::Operator::Select & Select() { return *m_pcSelect; }
			KERNEL::Operator::Select * m_pcSelect = nullptr;

			DWORD_PTR m_nModelsGroupItem = 0;
			DWORD_PTR m_nMeasurementsGroupItem = 0;
			DWORD_PTR m_nMarkupsGroupItem = 0;
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

	SegmentKey cModelSegment = pcImpl->View().GetModelOverrideSegmentKey();

	HC_KEY nModelKey = cModelSegment.KeyValue();

	// Root Item 생성
	Signal::TreeItem cItem;
	cItem.Title = strFileTitle;
	cItem.HasChildren = false;
	cItem.Key = pcImpl->m_cModelTree.AddItem(nModelKey, 0, true); // 내부 Tree 생성
	cTreeItems.push_back(cItem);

	DWORD_PTR nParentItemKey = cItem.Key;
	cItem.ParentKey = nParentItemKey;

	SegmentKeyArray cChildren;
	cModelSegment.ShowSubsegments(cChildren);

	SegmentKey cSegment;

	// Model Group Item 생성, 이하에 CAD Model data를 저장한다.
	if (true == Utility::ShowSubSegment(cModelSegment, "models", cSegment)) {
		cItem.Title = L"Models";
		cItem.HasChildren = (0 < cSegment.ShowIncluders()) ? true : false;
		cItem.Key = pcImpl->m_cModelTree.AddItem(cSegment.KeyValue(), nParentItemKey, cItem.HasChildren); // 내부 Tree 생성
		pcImpl->m_nModelsGroupItem = cItem.Key;
		cTreeItems.push_back(cItem);
	}

	// Measure Group Item 생성.
	if (true == Utility::ShowSubSegment(cModelSegment, "measurements", cSegment)) {
		cItem.Title = L"Measurements";
		cItem.HasChildren = false;
		cItem.Key = pcImpl->m_cModelTree.AddItem(cSegment.KeyValue(), nParentItemKey, false); // 내부 Tree 생성
		pcImpl->m_nMeasurementsGroupItem = cItem.Key;
		cTreeItems.push_back(cItem);
	}

	// Markup Group Item 생성.
	if (true == Utility::ShowSubSegment(cModelSegment, "markups", cSegment)) {
		cItem.Title = L"Markups";
		cItem.HasChildren = false;
		cItem.Key = pcImpl->m_cModelTree.AddItem(cSegment.KeyValue(), nParentItemKey, false); // 내부 Tree 생성
		pcImpl->m_nMarkupsGroupItem = cItem.Key;
		cTreeItems.push_back(cItem);
	}

	pcImpl->Delivery().modelPanel.AddItems(cTreeItems);
}

void KERNEL::Operator::ModelPanel::SetSelect(Select * pcInSelect)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->m_pcSelect = pcInSelect;
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
			SelChangedSignal(cInObject);
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
			ASSERT(false);
			break;
	}
}

//== Item Expanded 관련 함수 =========================================================================

// 1. Item Expanded Signal 처리
void KERNEL::Operator::ModelPanel::ItemExpandedSignal(Json::Object & cInObject)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	DWORD_PTR nInItemKey = cInObject.GetDwordPtr(SKW_KEY);

	ItemExpanded(nInItemKey);

/*
	if ((DWORD_PTR)ModelTree().Root() == nInItemKey) {
		return;
	}
	else if (pcImpl->m_nModelsGroupItem == nInItemKey) {
		ModelGroupItemExpanded();
	}
	else if (pcImpl->m_nMeasurementsGroupItem == nInItemKey) {
		MeasurementsGroupItemExpanded();
	}
	else if (pcImpl->m_nMarkupsGroupItem == nInItemKey) {
		MarkupsGroupItemExpanded();
	}
	else {
		ItemExpanded(nInItemKey);
	}*/
}

// 2.1 Model Group Item Expanded 처리
void KERNEL::Operator::ModelPanel::ModelGroupItemExpanded()
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	Signal::TreeItems cTreeItems;
	Signal::TreeItem cItem;

	ModelTreeItem * pcItem = dynamic_cast<ModelTreeItem *>((ModelTreeItem *)pcImpl->m_nModelsGroupItem);
	if (nullptr == pcItem) {
		DEBUG_STOP;
		return;
	}

	// 이미 전개가 되어 있는 경우는 무시한다. Child가 있는 경우는 이미 전개되어 있는 경우를 처리한다.
	if (false == pcItem->m_vpnChildren.empty()) {
		return;
	}

	cItem.ParentKey = pcImpl->m_nModelsGroupItem;
	
	SegmentKey cModels(pcItem->m_nKey);

	H3DF::IncludeKeyArray cChildren;
	cModels.ShowIncluders(cChildren);
	HC_KEY nModelKey = cModels.KeyValue();

// 	pcImpl->View().GetModelOverrideSegmentKey().ShowIncluders(cChildren);
// 	HC_KEY nModelKey = pcImpl->View().GetModelOverrideSegmentKey().KeyValue();

	for (auto & cInclude : cChildren) {
		SegmentKey cSegment = cInclude.GetTarget();

		CString strName;
		if (false == H3DF::UserData::ShowSegmentName(cSegment, strName)) {
			strName = cSegment.Name();
		}

		size_t nCount = cSegment.ShowIncluders();

		cItem.Title = strName;
		cItem.HasChildren = (0 < nCount) ? true : false;
		cItem.Key = pcImpl->m_cModelTree.AddItem(cInclude.KeyValue(), cItem.ParentKey, cItem.HasChildren);
		cTreeItems.push_back(cItem);
	}

	pcImpl->Delivery().modelPanel.AddItems(cTreeItems);
}

// 3. Measurements Group Item Expanded 처리
void KERNEL::Operator::ModelPanel::MeasurementsGroupItemExpanded()
{
}

// 4. Markups Group Item Expanded 처리
void KERNEL::Operator::ModelPanel::MarkupsGroupItemExpanded()
{
}

// 5. 일반 Item Expanded 처리
void KERNEL::Operator::ModelPanel::ItemExpanded(DWORD_PTR nInItem, DWORD_PTR nInParentItem)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	ModelTreeItem * pcInItem = dynamic_cast<ModelTreeItem *>((ModelTreeItem *)nInItem);
	if (nullptr == pcInItem) {
		DEBUG_STOP;
		return;
	}

	Signal::TreeItems cTreeItems;
	Signal::TreeItem cItem;

	// 입력된 Parent Key가 유효하지 않으면 nInItemKey값을 Parent Key로 사용한다.
	// 별도로 지정된 Parent가 없는 경우는 Item의 하부로 지정하도록 한다.
	DWORD_PTR nParentItem = (0 == nInParentItem) ? nInItem : nInParentItem;

	// 이미 Model Tree가 전계되어 있는 경우 처리
	if (false == pcInItem->m_vpnChildren.empty()) {
		for (auto pcChildItem : pcInItem->m_vpnChildren) {
			IncludeKey cInclude(pcChildItem->m_nKey);
			SegmentKey cSegment = cInclude.GetTarget();

			// Segment의 이름에 part가 포함되어 있는 경우는 표시하지 않고 바로 하부 Item을 표시한다.
			CStringA strName = cSegment.Name(false);
			if ("part" == strName.Left(4)) {
				IncludeExpanded((DWORD_PTR)pcChildItem, nParentItem);
				continue;
			}

			CString strUserName;
			if (false == H3DF::UserData::ShowSegmentName(cSegment, strUserName)) {
				strUserName = cSegment.Name();
			}

			cItem.Title = strUserName;
			cItem.HasChildren = (TreeItemStatus::Normal == pcChildItem->m_eStatus) ? true : false;
			cItem.Key = (DWORD_PTR)pcChildItem;
			cTreeItems.push_back(cItem);
		}

		pcImpl->Delivery().modelPanel.AddChildren(nParentItem, cTreeItems);

		return;
	}

	SegmentKey cInSegment;

	H3DF::Type eType = H3DF::Utility::GetType(pcInItem->m_nKey);
	if (H3DF::Type::IncludeKey == eType) {
		IncludeKey cInInclude(pcInItem->m_nKey);
		cInSegment = cInInclude.GetTarget();
	}
	else if(H3DF::Type::SegmentKey == eType) {
		cInSegment = SegmentKey(pcInItem->m_nKey);
	}

	H3DF::IncludeKeyArray cChildren;
	cInSegment.ShowIncluders(cChildren);

	for (auto & cInclude : std::ranges::reverse_view(cChildren)) {
		SegmentKey cSegment = cInclude.GetTarget();

		size_t nCount = cSegment.ShowIncluders();

		// Segment의 이름에 part가 포함되어 있는 경우는 표시하지 않고 바로 하부 Item을 표시한다.
		CStringA strName = cSegment.Name(false);
		if ("part" == strName.Left(4)) {
			DWORD_PTR nPartItem = pcImpl->m_cModelTree.AddItem(cInclude.KeyValue(), nParentItem, (0 < nCount) ? true : false);
			IncludeExpanded(nPartItem, nInItem);
			continue;
		}

		CString strUserName;
		if (false == H3DF::UserData::ShowSegmentName(cSegment, strUserName)) {
			strUserName = cSegment.Name();
		}

		cItem.Title = strUserName;
		cItem.HasChildren = (0 < nCount) ? true : false;
		cItem.Key = pcImpl->m_cModelTree.AddItem(cInclude.KeyValue(), cItem.ParentKey, cItem.HasChildren);
		cTreeItems.push_back(cItem);
	}

	pcImpl->Delivery().modelPanel.AddChildren(nParentItem, cTreeItems);
}

// 5.2 Include Item Expanded 처리
void KERNEL::Operator::ModelPanel::IncludeExpanded(DWORD_PTR nInItem, DWORD_PTR nInParentItem)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	ModelTreeItem * pcItem = dynamic_cast<ModelTreeItem *>((ModelTreeItem *)nInItem);
	if (nullptr == pcItem) {
		DEBUG_STOP;
		return;
	}

	Signal::TreeItems cTreeItems;
	Signal::TreeItem cItem;

	// 입력된 Parent Key가 유효하지 않으면 nInItemKey값을 Parent Key로 사용한다.
	// 별도로 지정된 Parent가 없는 경우는 Item의 하부로 지정하도록 한다.
	DWORD_PTR nParentItem = (0 == nInParentItem) ? nInItem : nInParentItem;

	// 이미 Model Tree가 전계되어 있는 경우 처리
	if (false == pcItem->m_vpnChildren.empty()) {
		for (auto pcChildItem : pcItem->m_vpnChildren) {
			IncludeKey cInclude(pcChildItem->m_nKey);
			SegmentKey cSegment = cInclude.GetTarget();

			// Segment의 이름에 part가 포함되어 있는 경우는 표시하지 않고 바로 하부 Item을 표시한다.
			CStringA strName = cSegment.Name(false);
			if ("part" == strName.Left(4)) {
				IncludeExpanded((DWORD_PTR)pcChildItem, nParentItem);
				continue;
			}

			CString strUserName;
			if (false == H3DF::UserData::ShowSegmentName(cSegment, strUserName)) {
				strUserName = cSegment.Name();
			}

			cItem.Title = strUserName;
			cItem.HasChildren = (TreeItemStatus::Normal == pcChildItem->m_eStatus) ? true : false;
			cItem.Key = (DWORD_PTR)pcChildItem;
			cTreeItems.push_back(cItem);
		}
		
		pcImpl->Delivery().modelPanel.AddChildren(nParentItem, cTreeItems);

		return;
	}

	IncludeKey cInInclude(pcItem->m_nKey);
	SegmentKey cInSegment = cInInclude.GetTarget();

	H3DF::IncludeKeyArray cChildren;
	cInSegment.ShowIncluders(cChildren);

	for (auto & cInclude : std::ranges::reverse_view(cChildren)) {
		SegmentKey cSegment = cInclude.GetTarget();

		size_t nCount = cSegment.ShowIncluders();

		// Segment의 이름에 part가 포함되어 있는 경우는 표시하지 않고 바로 하부 Item을 표시한다.
		CStringA strName = cSegment.Name(false);
		if ("part" == strName.Left(4)) {
			DWORD_PTR nPartItem = pcImpl->m_cModelTree.AddItem(cInclude.KeyValue(), nParentItem, (0 < nCount) ? true : false);
			IncludeExpanded(nPartItem, nInItem);
			continue;
		}

		CString strUserName;
		if (false == H3DF::UserData::ShowSegmentName(cSegment, strUserName)) {
			strUserName = cSegment.Name();
		}

		cItem.Title = strUserName;
		cItem.HasChildren = (0 < nCount) ? true : false;
		cItem.Key = pcImpl->m_cModelTree.AddItem(cInclude.KeyValue(), cItem.ParentKey, cItem.HasChildren);
		cTreeItems.push_back(cItem);
	}

	pcImpl->Delivery().modelPanel.AddChildren(nParentItem, cTreeItems);
}

//== Item Selelect Changed 관련 함수 =================================================================

// 1. Item Select Changed Signal 처리
void KERNEL::Operator::ModelPanel::SelChangedSignal(Json::Object & cInObject)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	// 대소문자 구별함. 주의할 것.
	DWORD_PTR nInItem = cInObject.GetDwordPtr(SKW_KEY);

	ModelTreeItem * pcItem = dynamic_cast<ModelTreeItem *>((ModelTreeItem *)nInItem);
	if (nullptr == pcItem) {
		DEBUG_STOP;
		return;
	}

	CString strText;
	cInObject.Stringify(strText);

// 	H3DF::KeyPath cPath;
// 	pcImpl->ModelTree().ShowPath(nInKey, cPath);

	std::vector<HC_KEY> vnKeys;

	H3DF::SelectionResults cResults;
	pcImpl->ModelTree().ShowSelectionResult(nInItem, cResults);

	pcImpl->Select().SelectByResult(cResults);

	return;
}