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

			ModelTreeItem * AddChild(HC_KEY nInChildKey, bool bHasChild);

			bool ShowChild(H3DF::Key & cKey, ModelTreeItem *& pcOutTreeItem);
			bool ShowChild(HC_KEY nInChildKey, ModelTreeItem *& pcOutTreeItem);

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

			ModelTreeItem * AddItem(HC_KEY nInKey, ModelTreeItem * pcInParentItem, bool bHasChild);

			bool ShowSelectionResult(ModelTreeItem * nInItem, H3DF::SelectionResults & cOutResults);
			//bool ShowPath(ModelTreeItem * nInItem, H3DF::KeyPath & cOutPath);

			ModelTreeItem & Root() { return *m_pcRoot; }

			ModelTreeItem * GetModelsGroupItem() { return m_pcModelsGroupItem; }
			ModelTreeItem * GetMeasurementsGroupItem() { return m_pcMeasurementsGroupItem; }
			ModelTreeItem * GetMarkupsGroupItem() { return m_pcMarkupsGroupItem; }

			void SetModelsGroupItem(ModelTreeItem * pcInItem) { m_pcModelsGroupItem = pcInItem; }
			void SetMeasurementsGroupItem(ModelTreeItem * pcInItem) { m_pcMeasurementsGroupItem = pcInItem; }
			void SetMarkupsGroupItem(ModelTreeItem * pcInItem) { m_pcMarkupsGroupItem = pcInItem; }
		
			bool ExpandItem(ModelTreeItem * pcInItem, bool bRecursiveExpand);

		protected:
			bool ShowSelectionEndItems(ModelTreeItem * pcInTreeItem, H3DF::SelectionResults & cOutResults);

			ModelTreeItem * m_pcRoot = nullptr;

			ModelTreeItem * m_pcModelsGroupItem = nullptr;
			ModelTreeItem * m_pcMeasurementsGroupItem = nullptr;
			ModelTreeItem * m_pcMarkupsGroupItem = nullptr;

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

KERNEL::Operator::ModelTreeItem * KERNEL::Operator::ModelTreeItem::AddChild(HC_KEY nInChildKey, bool bHasChild)
{
	Operator::ModelTreeItem * pcItem = new Operator::ModelTreeItem(nInChildKey);
	DEBUG_VALID(pcItem);

	pcItem->m_pcParent = this;
	pcItem->m_nKey = nInChildKey;

	if (false == bHasChild) {
		pcItem->m_eStatus = TreeItemStatus::End;
	}

	m_vpnChildren.push_back(pcItem);

	return pcItem;
}

bool KERNEL::Operator::ModelTreeItem::ShowChild(H3DF::Key & cKey, ModelTreeItem *& pcOutTreeItem)
{
	ShowChild(cKey.KeyValue(), pcOutTreeItem);
	return true;
}

bool KERNEL::Operator::ModelTreeItem::ShowChild(HC_KEY nInChildKey, ModelTreeItem *& pcOutTreeItem)
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

KERNEL::Operator::ModelTreeItem * KERNEL::Operator::ModelTree::AddItem(HC_KEY nInKey, ModelTreeItem * pcInParentItem, bool bHasChild)
{
	if (0 == pcInParentItem) {
		m_pcRoot = new ModelTreeItem(nInKey);
		DEBUG_VALID(m_pcRoot);
		return m_pcRoot;
	}

	// 하부 Item이 추가되면 상태를 Normal로 변경한다.
	pcInParentItem->m_eStatus = TreeItemStatus::Normal;

	ModelTreeItem * pcItem = pcInParentItem->AddChild(nInKey, bHasChild);
	DEBUG_VALID(pcItem);

	return pcItem;
}

// 2. 주어진 Key를 이용해서 Selection Result를 생성한다.
// 선택된 Key를 기준으로 하부 Item을 검색해서 개별적으로 Result에 저장한다.
// 필요한 경우에는 하위 Item을 전개하도록 한다.
// 선택한 Item이 End Item이 아니면 Assembly에서 Modeling Matrix가 이상하게 적용되어 있을 수 있으므로 선택은 End Item을 선택하도록 한다.
// 간단히 이야기 하면 RI Item만 선택하도록 해야 한다.
bool KERNEL::Operator::ModelTree::ShowSelectionResult(ModelTreeItem * pcInItem, H3DF::SelectionResults & cOutResults)
{
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

		// 선택된 Item을 상위 탐색을 통해서, Models Group Item까지 값을 저장한다.
		std::vector<HC_KEY> vnKeys;
		while (nullptr != pcInItem) {
			vnKeys.push_back(pcInItem->m_nKey);
			if(pcInItem == m_pcModelsGroupItem) {
				break;
			}
			pcInItem = pcInItem->m_pcParent;
		}

		// 적어도 2개 이상의 Key가 있어야 한다.
		if (2 > vnKeys.size()) {
			return false;
		}

		SelectionItem cSelItem;
		SelectionItemImpl * pcImpl = dynamic_cast<SelectionItemImpl *>(cSelItem.GetImpl());
		DEBUG_VALID(pcImpl);

		H3DF::Type eType = H3DF::Utility::GetType(vnKeys[0]);

		int nIncludeCount = 0;
		int nStartIndex = 0;

		if (H3DF::Type::IncludeKey == eType) {
			IncludeKey cInclude(vnKeys[0]);
			pcImpl->m_cKey = cInclude.GetTarget();
			nIncludeCount = (int)vnKeys.size();
			nStartIndex = 0;
		}
		else {
			pcImpl->m_cKey = SegmentKey(vnKeys[0]);
			nIncludeCount = (int)vnKeys.size() - 1;
			nStartIndex = 1;
		}

		pcImpl->m_nIncludeCount = nIncludeCount;
		pcImpl->m_pnIncludeKeys = new HC_KEY[nIncludeCount];

		// 역방향으로 넣어야 순서가 Selection이 됨.
		int nIncludeIndex = 0;
		for (int nIndex = nStartIndex; nIndex < nIncludeCount; nIndex++) {
			pcImpl->m_pnIncludeKeys[nIncludeIndex++] = vnKeys[nIncludeCount - nIndex - 1];
		}

#ifdef _DEBUG
// 		CString strPath;
// 		cSelItem.ShowPathString(strPath);
#endif
	/*	SelectionItem cSelItem1;
		SelectionItemImpl * pcImpl1 = dynamic_cast<SelectionItemImpl *>(cSelItem1.GetImpl());
		DEBUG_VALID(pcImpl1);

		ModelTreeItem * pcItem = pcInItem;
		while (nullptr != pcItem->m_pcParent) {
			pcImpl1->m_nIncludeCount++;
			pcItem = pcItem->m_pcParent;
		}

		HC_KEY nSegmentKey = pcInItem->m_nKey;

		char chType[MVO_BUFFER_SIZE];
		HC_Show_Key_Type(nSegmentKey, chType);

		if (streq(chType, "include")) {
			nSegmentKey = HC_Show_Include_Segment(nSegmentKey, nullptr);
		}

		pcImpl1->m_cKey = SegmentKey(nSegmentKey);

		pcImpl1->m_pnIncludeKeys = new HC_KEY[pcImpl1->m_nIncludeCount];

		pcItem = pcInItem;

		for (int nIndex = 0; nIndex < pcImpl1->m_nIncludeCount; nIndex++) {
			pcImpl1->m_pnIncludeKeys[nIndex] = pcItem->m_nKey;
			pcItem = pcItem->m_pcParent;
		}*/

		cOutResults.PushFront(cSelItem);

		return true;
	}

	for (auto pcChild : pcInItem->m_vpnChildren) {
		ShowSelectionEndItems(pcChild, cOutResults);
	}

	return true;
}
/*

bool KERNEL::Operator::ModelTree::ShowPath(ModelTreeItem * pcInItem, H3DF::KeyPath & cOutPath)
{
	ModelTreeItem * pcTargetItem = dynamic_cast<ModelTreeItem *>((ModelTreeItem *)pcInItem);
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
*/

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

	SegmentKey cInSegment;

	H3DF::Type eType = H3DF::Utility::GetType(pcInItem->m_nKey);
	if (H3DF::Type::IncludeKey == eType) {
		IncludeKey cInInclude(pcInItem->m_nKey);
		cInSegment = cInInclude.GetTarget();
	}
	else if (H3DF::Type::SegmentKey == eType) {
		cInSegment = SegmentKey(pcInItem->m_nKey);
	}

// 	IncludeKey cInInclude(pcInItem->m_nKey);
// 	SegmentKey cInSegment = cInInclude.GetTarget();

	H3DF::IncludeKeyArray cChildren;
	cInSegment.ShowIncluders(cChildren);

	for (auto & cInclude : std::ranges::reverse_view(cChildren)) {

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
	ModelTreeItem * pcRootItem = pcImpl->m_cModelTree.AddItem(nModelKey, nullptr, true); // 내부 Tree 생성
	cItem.Key = (DWORD_PTR)pcRootItem;
	cTreeItems.push_back(cItem);

	cItem.ParentKey = (DWORD_PTR)pcRootItem;

	SegmentKeyArray cChildren;
	cModelSegment.ShowSubsegments(cChildren);

	SegmentKey cSegment;

	// Model Group Item 생성, 이하에 CAD Model data를 저장한다.
	if (true == Utility::ShowSubSegment(cModelSegment, "models", cSegment)) {
		cItem.Title = L"Models";
		cItem.HasChildren = (0 < cSegment.ShowIncluders()) ? true : false;
		cItem.Key = (DWORD_PTR)pcImpl->m_cModelTree.AddItem(cSegment.KeyValue(), pcRootItem, cItem.HasChildren); // 내부 Tree 생성
		pcImpl->ModelTree().SetModelsGroupItem((ModelTreeItem *)cItem.Key);
		cTreeItems.push_back(cItem);
	}

	// Measure Group Item 생성.
	if (true == Utility::ShowSubSegment(cModelSegment, "measurements", cSegment)) {
		cItem.Title = L"Measurements";
		cItem.HasChildren = false;
		cItem.Key = (DWORD_PTR)pcImpl->m_cModelTree.AddItem(cSegment.KeyValue(), pcRootItem, false); // 내부 Tree 생성
		pcImpl->ModelTree().SetMeasurementsGroupItem((ModelTreeItem *)cItem.Key);
		cTreeItems.push_back(cItem);
	}

	// Markup Group Item 생성.
	if (true == Utility::ShowSubSegment(cModelSegment, "markups", cSegment)) {
		cItem.Title = L"Markups";
		cItem.HasChildren = false;
		cItem.Key = (DWORD_PTR)pcImpl->m_cModelTree.AddItem(cSegment.KeyValue(), pcRootItem, false); // 내부 Tree 생성
		pcImpl->ModelTree().SetMarkupsGroupItem((ModelTreeItem *)cItem.Key);
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

		case Signal::ModelPanel::Action::OnItemChecked:
			OnItemCheckedSignal(cInObject);
			break;

		case Signal::ModelPanel::Action::OnItemExpanded:
			OnItemExpandedSignal(cInObject);
			break;

		case Signal::ModelPanel::Action::OnItemExpanding:
			break;

		case Signal::ModelPanel::Action::OnRClick:
			break;

		case Signal::ModelPanel::Action::OnRDbClick:
			break;

		case Signal::ModelPanel::Action::OnSelChanged:
			OnSelChangedSignal(cInObject);
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

//== Select 관련 함수 ===============================================================================

// 1. 외부에서 전달된 Selection Item을 이용해서 Model Tree를 설정한다.
void KERNEL::Operator::ModelPanel::SetSelectItem(H3DF::SelectionItem & cSelItem)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	H3DF::KeyPath cPath;
	cSelItem.ShowPath(cPath);

	CString strText;
	cSelItem.ShowPathString(strText);

	// 키값 배열을 가져온다.
	H3DF::KeyArray cKeys;
	cPath.ShowKeys(cKeys);

	ModelTreeItem * pcItem = &pcImpl->ModelTree().Root();
	ModelTreeItem * pcFindItem = nullptr;

	std::vector<ModelTreeItem *> vpcItems;

	// Key값을 이용해서 하부 Child Tree Item을 찾아서 전계를 한다.
	// Child Item을 찾지 못한 경우 Tree Item을 전개해서 하부 Item을 찾는다.
	for (auto cKey : std::ranges::reverse_view(cKeys)) {
		HC_KEY nKey = cKey.KeyValue();
		pcFindItem = nullptr;
		pcItem->ShowChild(cKey, pcFindItem);

		// 하부 아이템을 찾지 못한 경우는 전개를 해서 다시 검색한다.
		if (nullptr == pcFindItem) {
			// 찾지 못한 경우는 전개를 한다.
			pcImpl->ModelTree().ExpandItem(pcItem, false);
			// 전개된 후에 다시 찾는다.
			pcItem->ShowChild(cKey, pcFindItem);

			// 찾은 경우는 UI Tree Item이 전개되어 있지 않은 경우이기 때문에 전개한다.
			if (nullptr != pcFindItem) {
/*
				Signal::TreeItems cTreeItems;
				Signal::TreeItem cItem;

				cItem.Title = L"Test";
				cItem.HasChildren = true;;
				cItem.ParentKey = (DWORD_PTR)pcItem;
				cItem.Key = (DWORD_PTR)pcFindItem;
				cTreeItems.push_back(cItem);

				pcImpl->Delivery().modelPanel.AddChildren((DWORD_PTR)pcItem, cTreeItems);
*/
			}
		}

		// Selection Item의 맨 마지막은 Sgement이기 때문에 Tree에서 찾을 수 없다 그런 경우 다음으로 넘어간다.
		// 이런 경우 탐색이 끝나게 될것이다. Model Tree와 UI Tree의 형태가 다르기 때문이다.
		if (nullptr == pcFindItem) {
			continue;
		}

		vpcItems.push_back(pcFindItem);

		pcItem = pcFindItem;
	}

	int i = 0;
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

	ItemExpanded(pcItem);

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

	ModelTreeItem * pcItem = ModelTree().GetModelsGroupItem();
	if (nullptr == pcItem) {
		DEBUG_STOP;
		return;
	}

	// 이미 전개가 되어 있는 경우는 무시한다. Child가 있는 경우는 이미 전개되어 있는 경우를 처리한다.
	if (false == pcItem->m_vpnChildren.empty()) {
		return;
	}

	cItem.ParentKey = (DWORD_PTR)pcItem;
	
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
		cItem.Key = (DWORD_PTR)pcImpl->m_cModelTree.AddItem(cInclude.KeyValue(), pcItem, cItem.HasChildren);
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
void KERNEL::Operator::ModelPanel::ItemExpanded(ModelTreeItem * pcInItem, ModelTreeItem * pcInParentItem)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	if (nullptr == pcInItem) {
		DEBUG_STOP;
		return;
	}

	Signal::TreeItems cTreeItems;
	Signal::TreeItem cItem;

	// 입력된 Parent Key가 유효하지 않으면 nInItemKey값을 Parent Key로 사용한다.
	// 별도로 지정된 Parent가 없는 경우는 Item의 하부로 지정하도록 한다.
	ModelTreeItem * pcParentItem = (nullptr == pcInParentItem) ? pcInItem : pcInParentItem;
	
	cItem.ParentKey = (DWORD_PTR)pcParentItem;

	// 이미 Model Tree가 전계되어 있는 경우 처리
	if (false == pcInItem->m_vpnChildren.empty()) {
		for (auto pcChildItem : pcInItem->m_vpnChildren) {
			IncludeKey cInclude(pcChildItem->m_nKey);
			SegmentKey cSegment = cInclude.GetTarget();

			// Segment의 이름에 part가 포함되어 있는 경우는 표시하지 않고 바로 하부 Item을 표시한다.
			CStringA strName = cSegment.Name(false);
			if ("part" == strName.Left(4)) {
				ItemExpanded(pcChildItem, pcParentItem);
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

		pcImpl->Delivery().modelPanel.AddChildren((DWORD_PTR)pcParentItem, cTreeItems);

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
			ModelTreeItem * pcPartItem = pcImpl->m_cModelTree.AddItem(cInclude.KeyValue(), pcParentItem, (0 < nCount) ? true : false);
			if (nullptr == pcPartItem) {
				DEBUG_STOP;
				continue;
			}

			ItemExpanded(pcPartItem, pcInItem);
			continue;
		}

		CString strUserName;
		if (false == H3DF::UserData::ShowSegmentName(cSegment, strUserName)) {
			strUserName = cSegment.Name();
		}

		cItem.Title = strUserName;
		cItem.HasChildren = (0 < nCount) ? true : false;
		cItem.Key = (DWORD_PTR)pcImpl->m_cModelTree.AddItem(cInclude.KeyValue(), pcParentItem, cItem.HasChildren);
		cTreeItems.push_back(cItem);
	}

	pcImpl->Delivery().modelPanel.AddChildren((DWORD_PTR)pcParentItem, cTreeItems);
}

//== Item Selelect Changed 관련 함수 =================================================================

// 1. Item Select Changed Signal 처리
void KERNEL::Operator::ModelPanel::OnSelChangedSignal(Json::Object & cInObject)
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

	ModelTreeItem * pcItem = dynamic_cast<ModelTreeItem *>((ModelTreeItem *)nInItem);
	if (nullptr == pcItem) {
		DEBUG_STOP;
		return;
	}

	CString strText;
	cInObject.Stringify(strText);

	H3DF::SelectionResults cResults;
	pcImpl->ModelTree().ShowSelectionResult(pcItem, cResults);

	pcImpl->Select().SelectByResult(cResults);
}