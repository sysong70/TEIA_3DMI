#include "StdAfx.h"

#include "SelectionImpl.h"

#include "../Window.h"
#include "../../Sprocket/Impl/3DF.View.Impl.h"

#include "../Line.h"
#include "../Shell.h"
#include "../3DF.Utility.h"

#include "WindowImpl.h"

#include <vhash.h>
#include <vlist.h>
#include <algorithm>

#include <atlcoll.h>

#include <HBaseOperator.h>
#include <HMarkupManager.h>
#include <HEventManager.h>
#include <HConstantFrameRate.h>

#define		SEGMENT_TYPE		1
#define		ENTITY_TYPE			2
#define		SUBENTITY_TYPE		3
#define		REGION_TYPE			4

// #define SELECTION_PRIVATE_TRACE

using namespace H3DF;

//== SelectionItemPrivate class ====================================================================

void H3DF::SelectionItemImpl::Reset()
{
	m_cKey.SetKeyValue(INVALID_KEY);

	if (nullptr != m_pnIncludeKeys) {
		delete m_pnIncludeKeys;
		m_pnIncludeKeys = nullptr;
	}

	m_nIncludeCount = 0;

	m_nOffset1 = 0;
	m_nOffset2 = 0;
	m_nOffset3 = 0;

	m_nRegion = 0;
	m_nLowest = 0;
	m_nHighest = 0;
}

//== SelectionResultsImpl class ====================================================================

bool H3DF::SelectionResultsImpl::Sort()
{
	if (2 > m_deItems.size()) {
		return false;
	}

	// 선택된 요소를 정렬하기 위해서 STL의 sort 함수를 사용
	std::sort(m_deItems.begin(), m_deItems.end(), [] (SelectionItem & cItem1, SelectionItem & cItem2) {
		WindowPoint cP1, cP2;
		cItem1.ShowSelectionPosition(cP1);
		cItem2.ShowSelectionPosition(cP2);

		if (cP1.z < cP2.z) {
			return true;
		}

		return false;
	});

#ifdef SELECTION_PRIVATE_TRACE
	TRACE(L"\n");

	int nIndex = 0;
	for (auto cItem : m_deItems) {
		Key cItemKey;

		if (true == cItem.ShowSelectedItem(cItemKey)) {
			WorldPoint cWorldPoint;
			cItem.ShowSelectionPosition(cWorldPoint);

			WindowPoint cWindowPoint;
			cItem.ShowSelectionPosition(cWindowPoint);

			H3DF::Type eType = cItemKey.Type();

			CString strTypeString = H3DF::Utility::GetTypeString(eType);

			TRACE(L"%02d.%s[%d]\t\t%f\t%f\n", nIndex++, strTypeString, cItemKey.KeyValue(), cWindowPoint.z, cWorldPoint.z);
		}
	}
#endif // SELECTION_PRIVATE_TRACE

	return true;
}

//== SelectionControlPrivate class =================================================================

HSelectionSet * H3DF::SelectionControlImpl::SelectionSet()
{
	return m_pcSelectionSet;
}

// 입력되는 Location은 MouseWindowPos을 이용한다. HEventInfo에서 GetMouseWindowPos() 함수를 이용해서 가져올 수 있음
// Select 처리, 재선택 및 최초 선택 Region, PMI 선택등을 처리.
void H3DF::SelectionControlImpl::HandleSelection(UINT const nFlags, SelectionResults & cOutResults)
{
	HC_KEY  nKey = INVALID_KEY;
	int nOffset1, nOffset2, nOffset3;
	HC_KEY * pnKeys = nullptr;
	HC_KEY * pnIncludeKeys = nullptr;
	int	eSelectedType = SelType::None;
	int nRegion = 0;
	char chType[64] = { "" };
	int	nIncludeCount = 0;

	HSelectionSet * pcSelection = GetBaseView()->GetSelection();

	// Control을 누른경우 Face 단위로 선택이 됨.

/*
	if (MVO_CONTROL & nFlags) {
		pcSelection->SetSelectionLevel(HSelectLevel::HSelectEntity);
	}
	else {
		pcSelection->SetSelectionLevel(HSelectLevel::HSelectSegment);
	}
*/

	SelectionResultsImpl * pcResultsPrivate = (SelectionResultsImpl *)cOutResults.GetImpl();

	//don't notify in the selection set, we'll do that at the end
	pcSelection->SetSelectWillNotify(false);

	// Mouse went down; find the selected items
	do {
		HC_Show_Selection_Element(&nKey, &nOffset1, &nOffset2, &nOffset3);
		HC_Show_Selection_Original_Key(&nKey);

		//this function makes sure that only the top level segment of an "object" is selectable. If a segment/geometry 
		//is not part of an object definition the key is not changed
		nKey = HObjectManager::FindHObjectSegment(nKey);

		if (INVALID_KEY == nKey) {
			continue;
		}

		// if locally renumbered, grab the original HOOPS keys
		// NOTE: This will only take care of locally renumbered entities (not segments)
		if (0 <= nKey)
		{
			char status[MVO_BUFFER_SIZE];
			HC_Show_Key_Status(nKey, status);
			if (streq(status, "invalid")) {
				HC_Show_Selection_Original_Key(&nKey);
			}
		}

		HC_Show_Key_Type(nKey, chType);

		if (streq(chType, "line") || streq(chType, "polyline") || streq(chType, "circular arc") || streq(chType, "elliptical arc")) {
			eSelectedType = SelType::Line;

			SelectionItem cItem;
			SelectionItemImpl * pcItemImpl = dynamic_cast<SelectionItemImpl *>(cItem.GetImpl());
			pcItemImpl->m_cKey = LineKey(Key(nKey));
			
			pcResultsPrivate->PushBack(cItem);
		}
		else if (streq(chType, "marker")) {
			eSelectedType = SelType::Marker;
		}
		else if (streq(chType, "text leader")) {
			eSelectedType = SelType::Shell;	//?
			nKey = HC_Show_Owner_Original_Key(nKey);		// move up to text;
		}
		else {
			// This may be shell, mesh, cyliner, etc...
			eSelectedType = SelType::Shell;
			// But if it really is a shell, check for regions.
			if (streq(chType, "shell") && nOffset3 != -1) {
				int low, high;
				HC_Show_Region_Range(nKey, &low, &high);
				if ((low != high || low > 0) && pcSelection->GetAllowRegionSelection()) {
					eSelectedType |= SelType::Region;
					HC_Open_Geometry(nKey); {
						HC_Open_Face(nOffset3); {
							HC_Show_Region(&nRegion);
						}HC_Close_Face();
					}HC_Close_Geometry();
				}
			}
		}

		// build up an array of include keys to pass with the selection
		int nSelectKeyCount;
		char chSelectKeyType[MVO_BUFFER_SIZE];
		HC_Show_Selection_Keys_Count(&nSelectKeyCount);

		if (0 < nSelectKeyCount) {
			delete[] pnKeys;
			delete[] pnIncludeKeys;
			pnKeys = new HC_KEY[nSelectKeyCount];
			pnIncludeKeys = new HC_KEY[nSelectKeyCount];
			HC_Show_Selection_Original_Keys(&nSelectKeyCount, pnKeys);

			int nIndex;

			nIncludeCount = 0;
			for (nIndex = nSelectKeyCount - 1; nIndex >= 0; nIndex--)
			{
				HC_Show_Key_Type(pnKeys[nIndex], chSelectKeyType);
				if (strstr(chSelectKeyType, "include"))
				{
					pnIncludeKeys[nIncludeCount] = pnKeys[nIndex];
					nIncludeCount++;
				}
				else if (streq(chSelectKeyType, "reference"))
					nKey = pnKeys[nIndex];
			}

			if (HSelectSubentity == pcSelection->GetSelectionLevel() && (eSelectedType & SelType::Shell))
			{
				if (MVO_SHIFT & nFlags) {
					pcSelection->DeSelectSubentity(nKey, nIncludeCount, pnIncludeKeys, 1, &nOffset3, &nOffset1, &nOffset2, true);
				}
				else {
					pcSelection->SelectSubentity(nKey, nIncludeCount, pnIncludeKeys, 1, &nOffset3, &nOffset1, &nOffset2, true);
				}
			}
			else {
				// if selected, deselect, else select
				if (pcSelection->IsSelected(nKey, nIncludeCount, pnIncludeKeys) ||
					(eSelectedType & SelType::Region && pcSelection->IsRegionSelected(nKey, nIncludeCount, pnIncludeKeys, nRegion)))
				{
					if (eSelectedType & SelType::Region) {
						pcSelection->DeSelectRegion(nKey, nIncludeCount, pnIncludeKeys, nRegion, true);
					}
					else {
						pcSelection->DeSelect(nKey, nIncludeCount, pnIncludeKeys, true);
					}
				}
				else
				{
					if (eSelectedType & SelType::Region)
						pcSelection->SelectRegion(nKey, nIncludeCount, pnIncludeKeys, nRegion, true);
					else
						pcSelection->Select(nKey, nIncludeCount, pnIncludeKeys, true);
				}
			}
		}
	} while (HC_Find_Related_Selection());

	pcSelection->SetSelectWillNotify(true);

	// notify the event to view
	GetBaseView()->Notify(HSignalSelected);

	delete[] pnKeys;
	delete[] pnIncludeKeys;
}

void H3DF::SelectionControlImpl::GetSelectOption(SelectionOptionsKit const & cInOptions, char * pchOutOption)
{
	char chOption[MVO_BUFFER_SIZE] = "\0";

	Selection::Level eLevel;
	if (true == cInOptions.ShowLevel(eLevel)) {
		switch (eLevel)
		{
			case H3DF::Selection::Level::Segment:
				sprintf(chOption, "selection level = segment");
				break;

			case H3DF::Selection::Level::Entity:
				sprintf(chOption, "selection level = entity");
				break;

			case H3DF::Selection::Level::Subentity:
				chOption[0] = '\0';
				break;
		}

		Utility::Set3DfOptionString(pchOutOption, chOption);
	}

	// Related Limit 관련 설정
	size_t nLimit = 0;
	if (true == cInOptions.ShowRelatedLimit(nLimit)) {
		sprintf(chOption, "related selection limit = %d", (int)nLimit);
	}
	else {
		sprintf(chOption, "no related selection limit");
	}
	Utility::Set3DfOptionString(pchOutOption, chOption);

	// Internal Limit 관련 설정
	if (true == cInOptions.ShowInternalLimit(nLimit)) {
		sprintf(chOption, "internal selection limit = %d", (int)nLimit);
	}
	else {
		sprintf(chOption, "no internal selection limit");
	}
	Utility::Set3DfOptionString(pchOutOption, chOption);

	// Sorting 관련 설정
	Selection::Sorting eSorting;
	if (true == cInOptions.ShowSorting(eSorting)) {
		switch (eSorting)
		{
			case H3DF::Selection::Sorting::Off:
				sprintf(chOption, "selection sorting = off");
				break;

			case H3DF::Selection::Sorting::Default:
				sprintf(chOption, "selection sorting = default");
				break;

			case H3DF::Selection::Sorting::Proximity:
				sprintf(chOption, "selection sorting = proximity");
				break;

			case H3DF::Selection::Sorting::ZSorting:
				sprintf(chOption, "selection sorting = on");
				break;
		}

		Utility::Set3DfOptionString(pchOutOption, chOption);
	}

	// Proximity 관련 설정은 View의 Driver 옵션이기 때문에 별도 처리해야 함.

	float fProximity = 0.0;
	if (true == cInOptions.ShowProximity(fProximity)) {
		sprintf(chOption, "selection proximity = %f", fProximity);

		Utility::Set3DfOptionString(pchOutOption, chOption);
	}

	Selection::Bias eBias;
	if (true == cInOptions.ShowBias(eBias)) {
		switch (eBias)
		{
			case H3DF::Selection::Bias::Lines:
				sprintf(chOption, "selection bias = lines");
				break;

			case H3DF::Selection::Bias::NoLines:
				sprintf(chOption, "selection bias = no lines");
				break;

			case H3DF::Selection::Bias::Markers:
				sprintf(chOption, "selection bias = markers");
				break;

			case H3DF::Selection::Bias::NoMarkers:
				sprintf(chOption, "selection bias = no markers");
				break;
		}

		Utility::Set3DfOptionString(pchOutOption, chOption);
	}
}

void H3DF::SelectionControlImpl::GetScope(SelectionOptionsKit const & cInOptions, char * pchOutScope)
{
	SegmentKey cStartSegment;
	KeyPath cStartPath;
	bool bScopeOnly;

	if (true == cInOptions.ShowScope(cStartSegment, bScopeOnly)) {
		HC_Show_Segment(cStartSegment.KeyValue(), pchOutScope);
	}
	else if (true == cInOptions.ShowScope(cStartPath, bScopeOnly)) {
	}
}

HBaseView * H3DF::SelectionControlImpl::GetBaseView()
{ 
	return (HBaseView *)m_pcWindow->GetBaseView(); 
}