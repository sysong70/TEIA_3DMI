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
		class ModelTreeItem
		{
		public:
			ModelTreeItem(HC_KEY nInKey);
			~ModelTreeItem();

			ModelTreeItem * AddChild(HC_KEY nInKey);

			HC_KEY m_cKey = INVALID_KEY;
			int m_nStatus = 0;

			ModelTreeItem * m_pcParent = nullptr;
			std::vector<ModelTreeItem *> vpnChildren;
		};

		class ModelTree
		{
		public:
			ModelTree() = default;
			~ModelTree();

			bool AddItem(HC_KEY nInKey, HC_KEY nInParentKey);

			bool ShowSelectionItem(HC_KEY nInkey, H3DF::SelectionItem & cSelItem);
			bool ShowPath(HC_KEY nInkey, H3DF::KeyPath & cOutPath);

			ModelTreeItem * m_pcRoot = nullptr;
			std::unordered_map<HC_KEY, ModelTreeItem *> m_cItems;
		};
	}
}

KERNEL::Operator::ModelTreeItem::ModelTreeItem(HC_KEY nInKey)
{
	m_cKey = nInKey;
}

KERNEL::Operator::ModelTreeItem::~ModelTreeItem()
{
	for (auto pcChild : vpnChildren) {
		delete pcChild;
	}
}

KERNEL::Operator::ModelTreeItem * KERNEL::Operator::ModelTreeItem::AddChild(HC_KEY nInKey)
{
	Operator::ModelTreeItem * pcItem = new Operator::ModelTreeItem(nInKey);
	DEBUG_VALID(pcItem);

	pcItem->m_pcParent = this;
	pcItem->m_cKey = nInKey;

	vpnChildren.push_back(pcItem);

	return pcItem;
}

KERNEL::Operator::ModelTree::~ModelTree()
{
	if (nullptr != m_pcRoot) {
		delete m_pcRoot;
	}

	m_cItems.clear();
}

bool KERNEL::Operator::ModelTree::AddItem(HC_KEY nInKey, HC_KEY nInParentKey)
{
	if (0 == nInParentKey) {
		m_pcRoot = new ModelTreeItem(nInKey);
		DEBUG_VALID(m_pcRoot);
		m_cItems.insert(std::make_pair(nInKey, m_pcRoot));
		return true;
	}

	auto pcParentItem = m_cItems[nInParentKey];
	if (nullptr == pcParentItem) {
		return false;
	}
	
	ModelTreeItem * pcItem = pcParentItem->AddChild(nInKey);
	DEBUG_VALID(pcItem);

	m_cItems.insert(std::make_pair(nInKey, pcItem));

	return true;
}

bool KERNEL::Operator::ModelTree::ShowSelectionItem(HC_KEY nInkey, H3DF::SelectionItem & cSelItem)
{
	ModelTreeItem * pcTargetItem = m_cItems[nInkey];
	if (nullptr == pcTargetItem) {
		return false;
	}

	H3DF::SelectionItemImpl * pcImpl = dynamic_cast<H3DF::SelectionItemImpl *>(cSelItem.GetImpl());
	DEBUG_VALID(pcImpl);

	ModelTreeItem * pcItem = pcTargetItem;
	while (nullptr != pcItem->m_pcParent) {
		pcImpl->m_nIncludeCount++;
		pcItem = pcItem->m_pcParent;
	}

	HC_KEY nSegmentKey = nInkey;

	char chType[MVO_BUFFER_SIZE];
	HC_Show_Key_Type(nSegmentKey, chType);

	if (streq(chType, "include")) {
		nSegmentKey = HC_Show_Include_Segment(nInkey, nullptr);
	}

	pcImpl->m_cKey = SegmentKey(nSegmentKey);

	pcImpl->m_pnIncludeKeys = new HC_KEY[pcImpl->m_nIncludeCount];

	pcItem = pcTargetItem;

	for (int nIndex = 0; nIndex < pcImpl->m_nIncludeCount; nIndex++) {
		pcImpl->m_pnIncludeKeys[nIndex] = pcItem->m_cKey;
		pcItem = pcItem->m_pcParent;
	}

	return true;
}

bool KERNEL::Operator::ModelTree::ShowPath(HC_KEY nInkey, H3DF::KeyPath & cOutPath)
{
	ModelTreeItem * pcTargetItem = m_cItems[nInkey];
	if (nullptr == pcTargetItem) {
		return false;
	}

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
		pnPath[nIndex] = pcItem->m_cKey;
		pcItem = pcItem->m_pcParent;
	}

	pnPath[nPathCount - 1] = INVALID_KEY;

	cOutPath = KeyPath(nPathCount, pnPath);

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

	// Root Item 생성
	Signal::TreeItem cItem;
	cItem.Title = strFileTitle;
	cItem.HasChildren = true;
	cItem.Key = (DWORD_PTR)pcImpl->View().GetModelOverrideSegmentKey().KeyValue();
	cTreeItems.push_back(cItem);
	pcImpl->Delivery().modelPanel.AddItems(cTreeItems);

	// 내부 Tree 생성
	pcImpl->m_cModelTree.AddItem(cItem.Key, 0);
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

// 2. Model Item Expanded 처리
void KERNEL::Operator::ModelPanel::ModelItemExpanded(HC_KEY nModelKey)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);
	
	Signal::TreeItems cTreeItems;
	Signal::TreeItem cItem;

	cItem.ParentKey = nModelKey;

	// Model Group Item 생성, 이하에 CAD Model data를 저장한다.
	cItem.Title = L"Models";
	cItem.HasChildren = true;
	cItem.Key = MODELS_GROUP_KEY;
	cTreeItems.push_back(cItem);

	// Measure Group Item 생성.
	cItem.Title = L"Measurements";
	cItem.HasChildren = false;
	cItem.Key = MEASUREMENTS_GROUP_KEY;
	cTreeItems.push_back(cItem);

	// Measure Group Item 생성.
	cItem.Title = L"Markups";
	cItem.HasChildren = false;
	cItem.Key = MARKUPS_GROUP_KEY;
	cTreeItems.push_back(cItem);

	pcImpl->Delivery().modelPanel.AddItems(cTreeItems);
	//pcImpl->Delivery().modelPanel.AddChildren(nModelKey, cTreeItems);
}

// 2.1 Model Group Item Expanded 처리
void KERNEL::Operator::ModelPanel::ModelGroupItemExpanded()
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	Signal::TreeItems cTreeItems;
	Signal::TreeItem cItem;

	cItem.ParentKey = MODELS_GROUP_KEY;

	H3DF::IncludeKeyArray cChildren;
	pcImpl->View().GetModelOverrideSegmentKey().ShowIncluders(cChildren);
	HC_KEY nModelKey = pcImpl->View().GetModelOverrideSegmentKey().KeyValue();

	for (auto & cInclude : cChildren) {
		SegmentKey cSegment = cInclude.GetTarget();

		SegmentKeyImpl::ForcedOpen(cSegment);

		CString strName;
		if (false == H3DF::UserData::ShowSegmentName(cSegment, strName)) {
			strName = cSegment.Name();
		}

		size_t nCount = cSegment.ShowSubsegments() + cSegment.ShowIncluders();

		SegmentKeyImpl::ForcedClose(cSegment);

		cItem.Title = strName;
		cItem.Key = cInclude.KeyValue();
		cItem.HasChildren = (0 < nCount) ? true : false;
		cTreeItems.push_back(cItem);

		// 내부 Tree 생성
		pcImpl->m_cModelTree.AddItem(cItem.Key, nModelKey);
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
void KERNEL::Operator::ModelPanel::ItemExpanded(HC_KEY nInItemKey)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	H3DF::Type eType = H3DF::Utility::GetType(nInItemKey);

	SegmentKey cInSegment;

	if (H3DF::Type::SegmentKey == eType) {
		cInSegment = SegmentKey(nInItemKey);
	}
	else if (H3DF::Type::IncludeKey == eType) {
		IncludeExpanded(nInItemKey);
		return;
	}

	Signal::TreeItems cTreeItems;
	Signal::TreeItem cItem;

	cItem.ParentKey = nInItemKey;

	H3DF::IncludeKeyArray cChildren;
	cInSegment.ShowIncluders(cChildren);

// 	H3DF::SegmentKeyArray cChildren1;
// 	cInSegment.ShowSubsegments(cChildren1);

	for (auto cInclude : cChildren) {
		SegmentKey cSegment = cInclude.GetTarget();

		CString strName;
		if (false == H3DF::UserData::ShowSegmentName(cSegment, strName)) {
			strName = cSegment.Name();
		}

		size_t nCount = cSegment.ShowIncluders();

		cItem.Title = strName;
		cItem.Key = cInclude.KeyValue();
		cItem.HasChildren = (0 < nCount) ? true : false;
		cTreeItems.push_back(cItem);

		// 내부 Tree 생성
		pcImpl->m_cModelTree.AddItem(cItem.Key, cItem.ParentKey);
	}

	pcImpl->Delivery().modelPanel.AddItems(cTreeItems);
}

// 5.1 Include Item Expanded 처리
void KERNEL::Operator::ModelPanel::IncludeExpanded(HC_KEY nInItemKey, HC_KEY nInParentKey)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	IncludeKey cInInclude(nInItemKey);
	SegmentKey cInSegment = cInInclude.GetTarget();

	Signal::TreeItems cTreeItems;
	Signal::TreeItem cItem;

	// 입력된 Parent Key가 유효하지 않으면 nInItemKey값을 Parent Key로 사용한다.
	if (INVALID_KEY == nInParentKey) {
		cItem.ParentKey = nInItemKey;
	}
	else {
		cItem.ParentKey = nInParentKey;
	}

	H3DF::IncludeKeyArray cChildren;
	cInSegment.ShowIncluders(cChildren);

	for (auto & cInclude : std::ranges::reverse_view(cChildren)) {
		SegmentKey cSegment = cInclude.GetTarget();

		// Segment의 이름에 part가 포함되어 있는 경우는 표시하지 않고 바로 하부 Item을 표시한다.
		CStringA strName = cSegment.Name(false);
		if ("part" == strName.Left(4)) {
			IncludeExpanded(cInclude.KeyValue(), nInItemKey);
			continue;
		}

		CString strUserName;
		if (false == H3DF::UserData::ShowSegmentName(cSegment, strUserName)) {
			strUserName = cSegment.Name();
		}

		size_t nCount = cSegment.ShowIncluders();

		cItem.Title = strUserName;
		cItem.Key = cInclude.KeyValue();
		cItem.HasChildren = (0 < nCount) ? true : false;
		cTreeItems.push_back(cItem);
		pcImpl->m_cModelTree.AddItem(cItem.Key, cItem.ParentKey);
	}

	pcImpl->Delivery().modelPanel.AddItems(cTreeItems);
}

//== Item Selelect Changed 관련 함수 =================================================================

// 1. Item Select Changed Signal 처리
void KERNEL::Operator::ModelPanel::SelChangedSignal(Json::Object & cInObject)
{
	auto pcImpl = dynamic_cast<ModelPanelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	// 대소문자 구별함. 주의할 것.
	HC_KEY nInKey = (HC_KEY)cInObject.GetDwordPtr("Key", INVALID_KEY);
	if (INVALID_KEY == nInKey) {
		return;
	}

	H3DF::Type eType = H3DF::Utility::GetType(nInKey);

	H3DF::KeyPath cPath;
	pcImpl->ModelTree().ShowPath(nInKey, cPath);

	H3DF::SelectionItem cItem;
	pcImpl->ModelTree().ShowSelectionItem(nInKey, cItem);

	pcImpl->Select().SelectByItem(cItem);

	return;
}