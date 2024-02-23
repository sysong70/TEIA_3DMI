#include "StdAfx.h"

#include "Entity.ModelTree.h"

#include "Common_Define.h"

#include "3DF/Segment.h"
#include "3DF/3DF.Utility.h"
#include "3DF/KeyPath.h"
#include "3DF/Selection.h"
#include "3DF/Impl/SelectionImpl.h"

#include <ranges>

using namespace H3DF;
using namespace H3DF::Entity;

//== ModelTreeItem Implement Class =================================================================
namespace H3DF
{
	namespace Entity
	{
		class ModelTreeItemImpl : public Impl
		{
		public:
			void Copy(ModelTreeItemImpl * pcInThat);

			HC_KEY m_nKey = INVALID_KEY;
			ModelTreeItemType m_eType = ModelTreeItemType::None;
			DWORD m_nStatus = ModelTreeItemStatus::Normal;

			ModelTreeItem * m_pcParent = nullptr;
			std::vector<ModelTreeItem *> m_vpnChildren;
		};
	}
}

void H3DF::Entity::ModelTreeItemImpl::Copy(ModelTreeItemImpl * pcInThat)
{
	m_nKey = pcInThat->m_nKey;
	m_nStatus = pcInThat->m_nStatus;
	m_pcParent = pcInThat->m_pcParent;
	m_vpnChildren = pcInThat->m_vpnChildren;
}

//== ModelTreeItem Class ===========================================================================
H3DF::Entity::ModelTreeItem::ModelTreeItem(HC_KEY nInKey)
{
	ModelTreeItemImpl * pcImpl = new ModelTreeItemImpl();
	DEBUG_VALID(pcImpl);

	pcImpl->m_nKey = nInKey;

	m_pcImpl = pcImpl;
}

HC_KEY H3DF::Entity::ModelTreeItem::KeyValue()
{
	ModelTreeItemImpl * pcImpl = (ModelTreeItemImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_nKey;
}

DWORD H3DF::Entity::ModelTreeItem::Status()
{
	ModelTreeItemImpl * pcImpl = (ModelTreeItemImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_nStatus;
}

DWORD H3DF::Entity::ModelTreeItem::AddStatus(ModelTreeItemStatus eStatus)
{
	ModelTreeItemImpl * pcImpl = (ModelTreeItemImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_nStatus |= eStatus;

	return pcImpl->m_nStatus;
}

DWORD H3DF::Entity::ModelTreeItem::RemoveStatus(ModelTreeItemStatus eStatus)
{
	ModelTreeItemImpl * pcImpl = (ModelTreeItemImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_nStatus &= ~eStatus;

	return pcImpl->m_nStatus;
}

ModelTreeItemType H3DF::Entity::ModelTreeItem::Type()
{
	ModelTreeItemImpl * pcImpl = (ModelTreeItemImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_eType;
}

ModelTreeItem * H3DF::Entity::ModelTreeItem::Parent()
{
	ModelTreeItemImpl * pcImpl = (ModelTreeItemImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_pcParent;
}
std::vector<ModelTreeItem *> & H3DF::Entity::ModelTreeItem::Children()
{
	ModelTreeItemImpl * pcImpl = (ModelTreeItemImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_vpnChildren;
}

ModelTreeItem * H3DF::Entity::ModelTreeItem::AddChild(HC_KEY nInChildKey, bool bHasChild)
{
	ModelTreeItemImpl * pcImpl = (ModelTreeItemImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	ModelTreeItem * pcChildItem = new ModelTreeItem(nInChildKey);
	DEBUG_VALID(pcChildItem);

	ModelTreeItemImpl * pcChildItemImpl = (ModelTreeItemImpl *)pcChildItem->GetImpl();

	pcChildItemImpl->m_pcParent = this;
	pcChildItemImpl->m_nKey = nInChildKey;

	if (false == bHasChild) {
		pcChildItemImpl->m_nStatus |= ModelTreeItemStatus::End;
	}

	pcImpl->m_vpnChildren.push_back(pcChildItem);

	return pcChildItem;
}

bool H3DF::Entity::ModelTreeItem::ShowChild(H3DF::Key & cKey, ModelTreeItem *& pcOutTreeItem)
{
	ShowChild(cKey.KeyValue(), pcOutTreeItem);
	return true;
}

bool H3DF::Entity::ModelTreeItem::ShowChild(HC_KEY nInChildKey, ModelTreeItem *& pcOutTreeItem)
{
	ModelTreeItemImpl * pcImpl = (ModelTreeItemImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	if (true == pcImpl->m_vpnChildren.empty()) {
		return false;
	}

	for (auto pcChild : pcImpl->m_vpnChildren) {
		if (nInChildKey == pcChild->KeyValue()) {
			pcOutTreeItem = pcChild;
			return true;
		}
	}

	return false;
}

//== ModelTree Implement Class =====================================================================

namespace H3DF
{
	namespace Entity
	{
		class ModelTreeImpl : public Impl
		{
		public:
			virtual ~ModelTreeImpl();

			void Copy(ModelTreeImpl * pcInThat);

			bool ShowSelectionEndItems(ModelTreeItem * pcInTreeItem, H3DF::SelectionResults & cOutResults);

			ModelTreeItem * m_pcRoot = nullptr;

			ModelTreeItem * m_pcModelsGroupItem = nullptr;
			ModelTreeItem * m_pcMeasurementsGroupItem = nullptr;
			ModelTreeItem * m_pcMarkupsGroupItem = nullptr;

			DWORD m_nSolidIndex = 1;
			DWORD m_nSurfaceIndex = 1;
			DWORD m_nCurveIndex = 1;
			DWORD m_nPointIndex = 1;
		};
	}
}

H3DF::Entity::ModelTreeImpl::~ModelTreeImpl()
{
	if (nullptr != m_pcRoot) {
		delete m_pcRoot;
		m_pcRoot = nullptr;
	}

	if (nullptr != m_pcModelsGroupItem) {
		delete m_pcModelsGroupItem;
		m_pcModelsGroupItem = nullptr;
	}

	if (nullptr != m_pcMeasurementsGroupItem) {
		delete m_pcMeasurementsGroupItem;
		m_pcMeasurementsGroupItem = nullptr;
	}

	if (nullptr != m_pcMarkupsGroupItem) {
		delete m_pcMarkupsGroupItem;
		m_pcMarkupsGroupItem = nullptr;
	}
}

void H3DF::Entity::ModelTreeImpl::Copy(ModelTreeImpl * pcInThat)
{
	m_pcRoot = pcInThat->m_pcRoot;
	m_pcModelsGroupItem = pcInThat->m_pcModelsGroupItem;
	m_pcMeasurementsGroupItem = pcInThat->m_pcMeasurementsGroupItem;
	m_pcMarkupsGroupItem = pcInThat->m_pcMarkupsGroupItem;
	m_nSolidIndex = pcInThat->m_nSolidIndex;
	m_nSurfaceIndex = pcInThat->m_nSurfaceIndex;
}

// 1. 주어진 Tree Item을 이용해서, End Item을 찾아서 Selection Item을 생성한다.
bool H3DF::Entity::ModelTreeImpl::ShowSelectionEndItems(ModelTreeItem * pcInTreeItem, H3DF::SelectionResults & cOutResults)
{
	if (nullptr == pcInTreeItem) {
		return false;
	}

	if (ModelTreeItemStatus::End & pcInTreeItem->Status()) {

		// 선택된 Item을 상위 탐색을 통해서, Models Group Item까지 값을 저장한다.
		std::vector<HC_KEY> vnKeys;
		while (nullptr != pcInTreeItem) {
			vnKeys.push_back(pcInTreeItem->KeyValue());
			if (pcInTreeItem == m_pcModelsGroupItem) {
				break;
			}
			pcInTreeItem = pcInTreeItem->Parent();
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

	for (auto pcChild : pcInTreeItem->Children()) {
		ShowSelectionEndItems(pcChild, cOutResults);
	}

	return true;
}

//== ModelTree Class ===============================================================================

H3DF::Entity::ModelTree::ModelTree()
{
	m_pcImpl = new ModelTreeImpl();
	DEBUG_VALID(m_pcImpl);
}

ModelTreeItem & H3DF::Entity::ModelTree::Root() 
{ 
	ModelTreeImpl * pcImpl = (ModelTreeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return *pcImpl->m_pcRoot; 
}

ModelTreeItem * H3DF::Entity::ModelTree::ModelsGroupItem()
{
	ModelTreeImpl * pcImpl = (ModelTreeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_pcModelsGroupItem; 
}

ModelTreeItem * H3DF::Entity::ModelTree::MeasurementsGroupItem()
{
	ModelTreeImpl * pcImpl = (ModelTreeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_pcMeasurementsGroupItem; 
}

ModelTreeItem * H3DF::Entity::ModelTree::MarkupsGroupItem()
{
	ModelTreeImpl * pcImpl = (ModelTreeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_pcMarkupsGroupItem; 
}

void H3DF::Entity::ModelTree::SetModelsGroupItem(ModelTreeItem * pcInItem)
{
	ModelTreeImpl * pcImpl = (ModelTreeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_pcModelsGroupItem = pcInItem;
}

void H3DF::Entity::ModelTree::SetMeasurementsGroupItem(ModelTreeItem * pcInItem)
{
	ModelTreeImpl * pcImpl = (ModelTreeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_pcMeasurementsGroupItem = pcInItem;
}

void H3DF::Entity::ModelTree::SetMarkupsGroupItem(ModelTreeItem * pcInItem)
{
	ModelTreeImpl * pcImpl = (ModelTreeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_pcMarkupsGroupItem = pcInItem;
}

//== 기능 관련 함수 ===================================================================================

ModelTreeItem * H3DF::Entity::ModelTree::AddItem(HC_KEY nInKey, ModelTreeItem * pcInParentItem, bool bHasChild)
{
	ModelTreeImpl * pcImpl = (ModelTreeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	if (0 == pcInParentItem) {
		pcImpl->m_pcRoot = new ModelTreeItem(nInKey);
		DEBUG_VALID(pcImpl->m_pcRoot);
		return pcImpl->m_pcRoot;
	}

	ModelTreeItemImpl * pcInParentItemImpl = dynamic_cast<ModelTreeItemImpl *>(pcInParentItem->GetImpl());

	// 하부 Item이 추가되면 상태를 Normal로 변경한다. End는 삭제한다.
	pcInParentItemImpl->m_nStatus &= ~ModelTreeItemStatus::End;
	pcInParentItemImpl->m_nStatus |= ModelTreeItemStatus::Normal;

	ModelTreeItem * pcItem = pcInParentItem->AddChild(nInKey, bHasChild);
	DEBUG_VALID(pcItem);

	return pcItem;
}

// 2. 주어진 Key를 이용해서 Selection Result를 생성한다.
// 선택된 Key를 기준으로 하부 Item을 검색해서 개별적으로 Result에 저장한다.
// 필요한 경우에는 하위 Item을 전개하도록 한다.
// 선택한 Item이 End Item이 아니면 Assembly에서 Modeling Matrix가 이상하게 적용되어 있을 수 있으므로 선택은 End Item을 선택하도록 한다.
// 간단히 이야기 하면 RI Item만 선택하도록 해야 한다.
bool H3DF::Entity::ModelTree::ShowSelectionResult(ModelTreeItem * pcInItem, H3DF::SelectionResults & cOutResults)
{
	if (nullptr == pcInItem) {
		DEBUG_STOP;
		return false;
	}

	ModelTreeImpl * pcImpl = (ModelTreeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	ExpandItem(pcInItem, true);

	pcImpl->ShowSelectionEndItems(pcInItem, cOutResults);

	return true;
}

// 3. 주어진 Item을 전개한다. 
// 입력값은 Include값이거나 Segment값이 들어올수 있다. 입력되는 값들의 규칙은 중간값은 Include, 마지막 값은 Segment이다.
bool H3DF::Entity::ModelTree::ExpandItem(ModelTreeItem * pcInItem, bool bRecursiveExpand)
{
	ModelTreeImpl * pcImpl = (ModelTreeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	if (ModelTreeItemStatus::End & pcInItem->Status()) {
		return true;
	}

	if (INVALID_KEY == pcInItem->KeyValue()) {
		return false;
	}

	// 이미 전개가 되어 있는 경우는 무시한다.
	if (false == pcInItem->Children().empty()) {
		return true;
	}

	SegmentKey cInSegment;

	H3DF::Type eType = H3DF::Utility::GetType(pcInItem->KeyValue());
	if (H3DF::Type::IncludeKey == eType) {
		IncludeKey cInInclude(pcInItem->KeyValue());
		cInSegment = cInInclude.GetTarget();
	}
	else if (H3DF::Type::SegmentKey == eType) {
		cInSegment = SegmentKey(pcInItem->KeyValue());
	}

	H3DF::IncludeKeyArray cChildren;
	cInSegment.ShowIncluders(cChildren);

	for (auto & cInclude : std::ranges::reverse_view(cChildren)) {

		SegmentKey cSegment = cInclude.GetTarget();

		size_t nCount = cSegment.ShowIncluders();
		bool bHasChildren = (0 < nCount) ? true : false;

		ModelTreeItem * pcItem = pcInItem->AddChild(cInclude.KeyValue(), bHasChildren);
		DEBUG_VALID(pcItem);

		ModelTreeItemImpl * pcItemImpl = (ModelTreeItemImpl *)pcItem->GetImpl();
		DEBUG_VALID(pcItemImpl);

		// part## 항목은 UI Tree에 표시하지 않으므로 처리하기 위해서 key값의 이름을 가져온다.
		// 여기서 이름은 User Defined Name이 아니라 Segment의 이름이다.
		CStringA strName = H3DF::Utility::GetName(cSegment);
		if ("part" == strName.Left(4)) {
			pcItemImpl->m_nStatus |= ModelTreeItemStatus::Invisible;
		}

		bool bNoShowFlag = false;
		StyleKeyArray cArray;
		if (true == cSegment.GetStyleControl().Show(cArray)) {
			for (auto & cKey : cArray) {
				CStringA strName = cKey.Name(false);
				if (0 == strName.CompareNoCase("noshow_style")) {
					bNoShowFlag = true;
					break;
				}
			}
		}

		if (true == bNoShowFlag) {
			pcItemImpl->m_nStatus |= ModelTreeItemStatus::NoShow;
		}

		DWORD nType;
		if (true == H3DF::UserData::ShowTopologyType(cSegment, nType)) {
			if ((DWORD)TopologyType::Solid == nType) {
				pcItemImpl->m_eType = ModelTreeItemType::Solid;
			}
			else if ((DWORD)TopologyType::Surface == nType) {
				pcItemImpl->m_eType = ModelTreeItemType::Surface;
			}
			else if ((DWORD)TopologyType::Curve	 == nType) {
				pcItemImpl->m_eType = ModelTreeItemType::Curve;
			}
			else if ((DWORD)TopologyType::Point == nType) {
				pcItemImpl->m_eType = ModelTreeItemType::Point;
			}
		}

		if (true == bRecursiveExpand) {
			ExpandItem(pcItem, bRecursiveExpand);
		}
	}

	return true;
}

// 4. 주어진 Key값의 Item Name을 가져온다.
bool H3DF::Entity::ModelTree::GetItemName(HC_KEY nInKey, CString & strOutName)
{
	ModelTreeImpl * pcImpl = (ModelTreeImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

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
			strOutName.Format(L"Solid %d", pcImpl->m_nSolidIndex++);

		}
		else if ((DWORD)TopologyType::Surface == nType) {
			strOutName.Format(L"Surface %d", pcImpl->m_nSurfaceIndex++);
		}
		else if ((DWORD)TopologyType::Curve == nType) {
			strOutName.Format(L"Curve %d", pcImpl->m_nCurveIndex++);
		}
		else if ((DWORD)TopologyType::Point == nType) {
			strOutName.Format(L"Curve %d", pcImpl->m_nPointIndex++);
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