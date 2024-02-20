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
		enum TreeItemStatus
		{
			Normal			= 0x0001,
			End				= 0x0002,
			UiUpdate		= 0x0004,
			Invisible		= 0x0008,
			Solid			= 0x0010,
			Surface			= 0x0020,
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
			DWORD m_nStatus = TreeItemStatus::Normal;

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

			ModelTreeItem * ModelsGroupItem() { return m_pcModelsGroupItem; }
			ModelTreeItem * MeasurementsGroupItem() { return m_pcMeasurementsGroupItem; }
			ModelTreeItem * MarkupsGroupItem() { return m_pcMarkupsGroupItem; }

			void SetModelsGroupItem(ModelTreeItem * pcInItem) { m_pcModelsGroupItem = pcInItem; }
			void SetMeasurementsGroupItem(ModelTreeItem * pcInItem) { m_pcMeasurementsGroupItem = pcInItem; }
			void SetMarkupsGroupItem(ModelTreeItem * pcInItem) { m_pcMarkupsGroupItem = pcInItem; }
		
			bool ExpandItem(ModelTreeItem * pcInItem, bool bRecursiveExpand);

			bool GetItemName(HC_KEY nInKey, CString & strOutName);

		protected:
			bool ShowSelectionEndItems(ModelTreeItem * pcInTreeItem, H3DF::SelectionResults & cOutResults);

			ModelTreeItem * m_pcRoot = nullptr;

			ModelTreeItem * m_pcModelsGroupItem = nullptr;
			ModelTreeItem * m_pcMeasurementsGroupItem = nullptr;
			ModelTreeItem * m_pcMarkupsGroupItem = nullptr;

			DWORD m_nSolidIndex = 1;
			DWORD m_nSurfaceIndex = 1;

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
		pcItem->m_nStatus |= TreeItemStatus::End;
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

	// 하부 Item이 추가되면 상태를 Normal로 변경한다. End는 삭제한다.
	pcInParentItem->m_nStatus &= ~TreeItemStatus::End;
	pcInParentItem->m_nStatus |= TreeItemStatus::Normal;

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

	if (TreeItemStatus::End & pcInItem->m_nStatus) {

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

		cOutResults.PushFront(cSelItem);

		return true;
	}

	for (auto pcChild : pcInItem->m_vpnChildren) {
		ShowSelectionEndItems(pcChild, cOutResults);
	}

	return true;
}

// 3. 주어진 Item을 전개한다. 
// 입력값은 Include값이거나 Segment값이 들어올수 있다. 입력되는 값들의 규칙은 중간값은 Include, 마지막 값은 Segment이다.
bool KERNEL::Operator::ModelTree::ExpandItem(ModelTreeItem * pcInItem, bool bRecursiveExpand)
{
	if (TreeItemStatus::End & pcInItem->m_nStatus) {
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

	H3DF::IncludeKeyArray cChildren;
	cInSegment.ShowIncluders(cChildren);

	for (auto & cInclude : std::ranges::reverse_view(cChildren)) {

		SegmentKey cSegment = cInclude.GetTarget();

		size_t nCount = cSegment.ShowIncluders();
		bool bHasChildren = (0 < nCount) ? true : false;

		ModelTreeItem * pcItem = pcInItem->AddChild(cInclude.KeyValue(), bHasChildren);
		DEBUG_VALID(pcItem);

		// part## 항목은 UI Tree에 표시하지 않으므로 처리하기 위해서 key값의 이름을 가져온다.
		// 여기서 이름은 User Defined Name이 아니라 Segment의 이름이다.
		CStringA strName = H3DF::Utility::GetName(cSegment);
		if ("part" == strName.Left(4)) {
			pcItem->m_nStatus |= TreeItemStatus::Invisible;
		}

		DWORD nType;
		if (true == H3DF::UserData::ShowTopologyType(cSegment, nType)) {
			if ((DWORD)TopologyType::Solid == nType) {
				pcItem->m_nStatus |= TreeItemStatus::Solid;
			}
			else if ((DWORD)TopologyType::Surface == nType) {
				pcItem->m_nStatus |= TreeItemStatus::Surface;
			}
		}

		if (true == bRecursiveExpand) {
			ExpandItem(pcItem, bRecursiveExpand);
		}
	}

	return true;
}

// 4. 주어진 Key값의 Item Name을 가져온다.
bool KERNEL::Operator::ModelTree::GetItemName(HC_KEY nInKey, CString & strOutName)
{
	DWORD nType;

	H3DF::Type eType = H3DF::Utility::GetType(nInKey);

	H3DF::SegmentKey cSegment;
	if (H3DF::Type::IncludeKey == eType) {
		IncludeKey cInInclude(nInKey);
		cSegment = cInInclude.GetTarget();
	}
	else if (H3DF::Type::SegmentKey == eType) {
		cSegment = H3DF::SegmentKey(nInKey);
	}

	if (true == H3DF::UserData::ShowTopologyType(cSegment, nType)) {
		if ((DWORD)TopologyType::Solid == nType) {
			strOutName.Format(L"Solid %d", m_nSolidIndex++);

		}
		else if ((DWORD)TopologyType::Surface == nType) {
			strOutName.Format(L"Surface %d", m_nSurfaceIndex++);
		}
	}
	else {
		if (false == H3DF::UserData::ShowSegmentName(nInKey, strOutName)) {
			strOutName = H3DF::Utility::GetName(nInKey);
		}
	}

	if (true == strOutName.IsEmpty()) {
		return false;
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
			ModelPanelImpl(const DocView * pcInDocView);

			void Copy(ModelPanelImpl * pcInThat) {
				OperatorImpl::Copy(pcInThat);
			}

			KERNEL::Operator::ModelTree & ModelTree() { return m_cModelTree; }
			KERNEL::Operator::ModelTree m_cModelTree;

			KERNEL::Operator::Select & Select() { return *m_pcSelect; }
			KERNEL::Operator::Select * m_pcSelect = nullptr;

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

// 5. 일반 Item Expanded 처리
void KERNEL::Operator::ModelPanelImpl::UserInterfaceItemExpanded(ModelTreeItem * pcInItem, bool bRecursiveExpand)
{
	ModelTree().ExpandItem(pcInItem, false);

	Signal::TreeItems cTreeItems;
	Signal::TreeItem cItem;

	// 입력받은 Item의 Parent를 찾는다. Invisible인 경우는 다음 Parent를 찾는다.
	ModelTreeItem * pcParentItem = pcInItem;

	while (nullptr != pcParentItem) {
		// 부모가 Invisible이면 다음 Parent를 찾는다.
		if (pcParentItem->m_nStatus & TreeItemStatus::Invisible) {
			pcParentItem = pcParentItem->m_pcParent;
		}
		else {
			break;
		}
	}

	cItem.ParentKey = (DWORD_PTR)pcParentItem;

	// 이미 Model Tree가 전계되어 있는 경우 처리
	if (false == pcInItem->m_vpnChildren.empty()) {
		for (auto pcChildItem : pcInItem->m_vpnChildren) {

			// 이미 UI에 업데이트 한 경우는 
			if (pcChildItem->m_nStatus & TreeItemStatus::UiUpdate) {
				continue;
			}

			pcChildItem->m_nStatus |= TreeItemStatus::UiUpdate;

			if (pcChildItem->m_nStatus & TreeItemStatus::Invisible) {
				UserInterfaceItemExpanded(pcChildItem, bRecursiveExpand);
				continue;
			}

// 			if (true == bRecursiveExpand) {
// 				if ((pcChildItem->m_nStatus & TreeItemStatus::Solid) || (pcChildItem->m_nStatus & TreeItemStatus::Surface)) {
// 					continue;
// 				}
// 			}

			CString strUserName;
			ModelTree().GetItemName(pcChildItem->m_nKey, strUserName);

#if 0
			HC_KEY nSegKey = INVALID_KEY, nIncKey = INVALID_KEY;
			H3DF::Type eType = H3DF::Utility::GetType(pcChildItem->m_nKey);
			if (H3DF::Type::IncludeKey == eType) {
				nIncKey = pcChildItem->m_nKey;
				IncludeKey cInInclude(nIncKey);
				nSegKey = cInInclude.GetTarget().KeyValue();
			}
			else if (H3DF::Type::SegmentKey == eType) {
				nSegKey = pcChildItem->m_nKey;
			}

			CString strText;
			if (INVALID_KEY == nIncKey) {
				strText.Format(L": Seg [%d]", nSegKey);
			}
			else {
				strText.Format(L": Inc [%d], Seg [%d]", nIncKey, nSegKey);
			}
			
			strUserName += strText;
#endif

			cItem.Title = strUserName;
			cItem.HasChildren = (TreeItemStatus::End & pcChildItem->m_nStatus) ? false : true;
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
	pcRootItem->m_nStatus |= TreeItemStatus::UiUpdate;

	Signal::TreeItem cItem;
	cItem.ParentKey = 0;
	cItem.Key = (DWORD_PTR)pcRootItem;
	cItem.Title = strFileTitle;
	cItem.HasChildren = false;

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
		cItem.HasChildren = (0 < cSegment.ShowIncluders()) ? true : false;

		ModelTreeItem * pcItem = pcImpl->m_cModelTree.AddItem(cSegment.KeyValue(), pcRootItem, cItem.HasChildren); // 내부 Tree 생성
		pcItem->m_nStatus |= TreeItemStatus::UiUpdate;
		cItem.Key = (DWORD_PTR)pcItem;

		pcImpl->ModelTree().SetModelsGroupItem(pcItem);
		cTreeItems.push_back(cItem);
	}

	// Measure Group Item 생성.
	if (true == Utility::ShowSubSegment(cModelSegment, "measurements", cSegment)) {
		cItem.Title = L"Measurements";
		cItem.HasChildren = false;
		
		ModelTreeItem * pcItem = pcImpl->m_cModelTree.AddItem(cSegment.KeyValue(), pcRootItem, false); // 내부 Tree 생성
		pcItem->m_nStatus |= TreeItemStatus::UiUpdate;
		cItem.Key = (DWORD_PTR) pcItem;

		pcImpl->ModelTree().SetMeasurementsGroupItem(pcItem);
		cTreeItems.push_back(cItem);
	}

	// Markup Group Item 생성.
	if (true == Utility::ShowSubSegment(cModelSegment, "markups", cSegment)) {
		cItem.Title = L"Markups";
		cItem.HasChildren = false;

		ModelTreeItem * pcItem = pcImpl->m_cModelTree.AddItem(cSegment.KeyValue(), pcRootItem, false); // 내부 Tree 생성
		pcItem->m_nStatus |= TreeItemStatus::UiUpdate;
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

// 2. Select 관련 Control 설정 함수
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

		if (pcItem->m_nStatus & TreeItemStatus::Invisible) {
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

	ModelTreeItem * pcItem = dynamic_cast<ModelTreeItem *>((ModelTreeItem *)nInItem);
	if (nullptr == pcItem) {
		DEBUG_STOP;
		return;
	}

	H3DF::SelectionResults cResults;
	pcImpl->ModelTree().ShowSelectionResult(pcItem, cResults);

	if (true == bChecked) {
		pcImpl->Select().SelectByResult(cResults);
	}
	else {
		pcImpl->Select().Unhighlight(cResults);
	}
}

void KERNEL::Operator::ModelPanel::OnItemCheckedSignalSelect(Json::Object & cInObject)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	DWORD_PTR nInItem = cInObject.GetDwordPtr(SKW_KEY);
	//:Ken - checked or unchecked...
	bool bChecked = cInObject.GetBoolean(SKW_CHECKED);

	ModelTreeItem * pcItem = dynamic_cast<ModelTreeItem *>((ModelTreeItem *)nInItem);
	if (nullptr == pcItem) {
		DEBUG_STOP;
		return;
	}

	H3DF::SelectionResults cResults;
	pcImpl->ModelTree().ShowSelectionResult(pcItem, cResults);

	if (true == bChecked) {
		pcImpl->Select().SelectByResult(cResults);
	}
	else {
		pcImpl->Select().Unhighlight(cResults);
	}
}