#include "StdAfx.h"

#include "3DF.SelectionPrivate.h"

#include "../3DF.Window.h"
#include "../3DF.BaseView.h"

#include "../3DF.Line.h"
#include "../3DF.Shell.h"
#include "../3DF.Utility.h"

#include "3DF.WindowPrivate.h"

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

USING_3DF_NAMESPACE


//== SelectionResultsPrivate class =================================================================
bool H3DF::SelectionResultsPrivate::Sort()
{
	if (2 > deItems.size()) {
		return false;
	}

	// 선택된 요소를 정렬하기 위해서 STL의 sort 함수를 사용
	std::sort(deItems.begin(), deItems.end(), [] (SelectionItem * pcItem1, SelectionItem * pcItem2) {
		WindowPoint cP1, cP2;
		pcItem1->ShowSelectionPosition(cP1);
		pcItem2->ShowSelectionPosition(cP2);

		if (cP1.z < cP2.z) return true;
		return false;
	});
/*
	TRACE(L"\n");

	SelectionResultsIterator cIter = cOutResults.GetIterator();
	int nIndex = 0;
	while (true == cIter.IsValid()) {
		SelectionItem * pcItem = cIter.GetItem();

		Key cItemKey;

		if (true == pcItem->ShowSelectedItem(cItemKey)) {
			WorldPoint cWorldPoint;
			pcItem->ShowSelectionPosition(cWorldPoint);

			WindowPoint cWindowPoint;
			pcItem->ShowSelectionPosition(cWindowPoint);

			H3DF::Type eType = cItemKey.Type();

			CString strTypeString = H3DF::Utility::GetTypeString(eType);

			TRACE(L"%02d.%s[%d]\t\t%f\t%f\n", nIndex++, strTypeString, cItemKey.KeyValue(), cWindowPoint.z, cWorldPoint.z);
		}

		cIter.Next();
	}*/

	return true;
}

//== SelectionControlPrivate class =================================================================

// 주어진 Point와 Selection Option을 이용해서 선택 작업을 수행하고, 선택된 요소를 SelectionResults에 저장한다.
size_t H3DF::SelectionControlPrivate::SelectByPoint(Point const & cInLocation, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults)
{
	int	 nResult = 0;

	char chAction[MVO_BUFFER_SIZE] = "v";

	// 선택 옵션을 문자열로 변환
	GetSelectOption(cInOptions, chAction);

	char chScope[MVO_BUFFER_SIZE] = "";
	GetScope(cInOptions, chScope);

	HSelectionSet * pcSelection = GetBaseView()->GetSelection();

	// 선택 옵션에 따라 선택 작업 실시
/*
	HC_Open_Segment_By_Key(GetBaseView()->GetViewKey()); {
		nResult = HC_Compute_Selection(".", "", chAction, cInLocation.x, cInLocation.y);
		//"v, selection level = entity, related selection limit = 0, selection sorting, internal selection limit = 0", cInLocation.x, cInLocation.y);
	} HC_Close_Segment();
*/

	if (0 < strlen(chScope)) {
		HC_Open_Segment(chScope); {
			HC_Set_Rendering_Options("attribute lock=(selectability)");
			HC_Set_Selectability("geometry = on");
			//nResult = HC_Compute_Selection(GetBaseView()->GetDriverPath(), ".", "v, selection level = entity, no related selection limit, visual selection = off", cInLocation.x, cInLocation.y);
			nResult = HC_Compute_Selection(GetBaseView()->GetDriverPath(), ".", chAction, cInLocation.x, cInLocation.y);
			HC_Set_Selectability("everything = off");
		} HC_Close_Segment();
	}
	else {
		HC_Open_Segment_By_Key(GetBaseView()->GetViewKey()); {
			nResult = HC_Compute_Selection(".", "./scene/overwrite", chAction, cInLocation.x, cInLocation.y);
			//(pcSelection->GetSubwindowPenetration() ? "" : "./scene/overwrite"), chAction, cInLocation.x, cInLocation.y);
			//"v, selection level = entity, related selection limit = 0, selection sorting, internal selection limit = 0", cInLocation.x, cInLocation.y);
		} HC_Close_Segment();
	}

	// 선택된 요소가 없음
	if (0 == nResult) {
		return 0;
	}

	HC_KEY  nKey = INVALID_KEY;
	int nOffset1, nOffset2, nOffset3;
	HC_KEY * pnIncludeKeys = nullptr;
	int	eSelectedType = SelType::None;
	char chKeyType[MVO_BUFFER_SIZE];
	int	nIncludeCount = 0;

	// 선택된 요소를 SelectionResults에 저장하기 위해서 새롭게 생성
	SelectionResultsPrivate * pcResultsPrivate = (SelectionResultsPrivate *)cOutResults.GetImpl();

	do {
		// 선택된 요소를 저장하기 위해서 Item 생성
		SelectionItem * pcItem = new SelectionItem();
		SelectionItemPrivate * pcItemPrivate = (SelectionItemPrivate *)pcItem->GetImpl();

		HC_Show_Selection_Element(&nKey, &nOffset1, &nOffset2, &nOffset3);
		HC_Show_Selection_Original_Key(&nKey);
		
		pcItemPrivate->nOffset1 = nOffset1;
		pcItemPrivate->nOffset2 = nOffset2;
		pcItemPrivate->nOffset3 = nOffset3;

		WindowPoint cWindowPoint;
		WorldPoint cWorldPoint;
		HC_Show_Selection_Position(&cWindowPoint.x, &cWindowPoint.y, &cWindowPoint.z, &cWorldPoint.x, &cWorldPoint.y, &cWorldPoint.z);

		pcItemPrivate->cWindowPoint = cWindowPoint;
		pcItemPrivate->cWorldPoint = cWorldPoint;
		
		// build up an array of include keys to pass with the selection
		int nKeyCount = 0;
		HC_Show_Selection_Keys_Count(&nKeyCount);

		if (0 < nKeyCount) {
			WindowKeyPrivate * pcImpl = (WindowKeyPrivate *)m_pcWindow->GetImpl();
			HC_KEY * pnKeys = pcImpl->GetSelectBufferKey(nKeyCount);

			pnIncludeKeys = new HC_KEY[nKeyCount];
			HC_Show_Selection_Original_Keys(&nKeyCount, pnKeys);

			pcItemPrivate->pnIncludeKeys = pnIncludeKeys;

			nIncludeCount = 0;
			for (int nIndex = nKeyCount - 1; nIndex >= 0; nIndex--)
			{
				HC_Show_Key_Type(pnKeys[nIndex], chKeyType);
				if (strstr(chKeyType, "include"))
				{
					pnIncludeKeys[nIncludeCount] = pnKeys[nIndex];
					nIncludeCount++;
				}
				else if (streq(chKeyType, "reference")) {
					nKey = pnKeys[nIndex];
				}
			}

			pcItemPrivate->nIncludeCount = nIncludeCount;
		}

		HC_Show_Key_Type(nKey, chKeyType);

		HC_KEY nTestKey3 = INVALID_KEY;
		HC_KEY nTestKey4 = INVALID_KEY;

		if (streq(chKeyType, "line") || streq(chKeyType, "polyline") || streq(chKeyType, "circular arc") || streq(chKeyType, "elliptical arc")) {
			eSelectedType = SelType::Line;
			pcItemPrivate->cKey = LineKey(Key(nKey));
		}
		else if (streq(chKeyType, "marker")) {
			eSelectedType = SelType::Marker;
		}
		else if (streq(chKeyType, "text leader")) {
			eSelectedType = SelType::Shell;	//?
			nKey = HC_Show_Owner_Original_Key(nKey);		// move up to text;
		}
		else {
			// This may be shell, mesh, cyliner, etc...
			eSelectedType = SelType::Shell;
			pcItemPrivate->cKey = ShellKey(Key(nKey));

			// But if it really is a shell, check for regions.
			if (streq(chKeyType, "shell") && nOffset3 != -1) {
				if (true == pcSelection->GetAllowRegionSelection()) {
					int nRegion = 0;
					int nLowest = 0;
					int nHighest = 0;

					HC_Show_Region_Range(nKey, &nLowest, &nHighest);

					if ((nLowest != nHighest || nLowest > 0)) {
						eSelectedType |= SelType::Region;
						HC_Open_Geometry(nKey); {
							HC_Open_Face(nOffset3); {
								HC_Show_Region(&nRegion);
							}HC_Close_Face();
						}HC_Close_Geometry();

						pcItemPrivate->nRegion = nRegion;
						pcItemPrivate->nLowest = nLowest;
						pcItemPrivate->nHighest = nHighest;
					}
				}
			}
		}

		pcResultsPrivate->PushBack(pcItem);

	} while (HC_Find_Related_Selection());

	return pcResultsPrivate->Size();
}

bool H3DF::SelectionControlPrivate::SorterFunction(const void * pcArg1, const void * pcArg2)
{
	SelectionItem * pcItem1 = (SelectionItem *)pcArg1;
	SelectionItem * pcItem2 = (SelectionItem *)pcArg2;

	WindowPoint cP1, cP2;
	pcItem1->ShowSelectionPosition(cP1);
	pcItem2->ShowSelectionPosition(cP2);

	if (cP1.z < cP2.z) return true;
	return false;
}


int H3DF::SelectionControlPrivate::SelectByPoint(Point const & cInLocation, UINT const nFlags, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults)
{
	if (nullptr == m_pcSelectionSet) {
		return HOP_NOT_HANDLED;
	}

	HPoint  new_pos;
	int		nResult = 0;
	bool	need_update = false;

	//HSelectionSet * pcSelection = GetBaseView()->GetSelection();

	// Markup 선택
	HMarkupManager * pcMarkupManager;
	if (nullptr != (pcMarkupManager = GetBaseView()->GetMarkupManager())) {
		HC_Open_Segment_By_Key(pcMarkupManager->GetMarkupKey()); {
			// compute the selection using the HOOPS window coordinate of the the pick location
			nResult = HC_Compute_Selection(GetBaseView()->GetDriverPath(),
				(m_pcSelectionSet->GetSubwindowPenetration() ? "" : "."),
				//"v", cInLocation.x, cInLocation.y);
				"v, selection level = entity, selection sorting, internal selection limit = 0", cInLocation.x, cInLocation.y);
		} HC_Close_Segment();
	}

	if (nResult == 0) {
		HC_Open_Segment_By_Key(GetBaseView()->GetViewKey()); {
			nResult = HC_Compute_Selection(".",
				(m_pcSelectionSet->GetSubwindowPenetration() ? "" : "./scene/overwrite"),
				//"v", cInLocation.x, cInLocation.y);
				"v, selection level = entity, related selection limit = 0, selection sorting, internal selection limit = 0", cInLocation.x, cInLocation.y);
		} HC_Close_Segment();
	}

	if (nResult > 0) {
		HandleSelection(nFlags, cOutResults);
		size_t nCount = cOutResults.GetCount();
		need_update = true;
	}

	GetBaseView()->GetConstantFrameRateObject()->SetDisableIncreaseTemp(true);

	//GetBaseView()->SetGeometryChanged();
	if (need_update) {
		GetBaseView()->Update();	// update the scene to reflect the new highlight attributes
	}

	GetBaseView()->GetConstantFrameRateObject()->SetDisableIncreaseTemp(false);

	if (GetBaseView()->GetFocusOnSelection()) {
		GetBaseView()->FocusOnSelection();
	}

	// of the selected items
	return HOP_READY;
}


// 입력되는 Location은 MouseWindowPos을 이용한다. HEventInfo에서 GetMouseWindowPos() 함수를 이용해서 가져올 수 있음
int H3DF::SelectionControlPrivate::SelectButtonDown_V1(Point const & cInLocation, UINT const nFlags, SelectionResults & cOutResults)
{
	HPoint  new_pos;
	int		nResult = 0;
	bool	need_update = false;
	
	// Mouse went down; find the selected items
	HC_Open_Segment_By_Key(GetBaseView()->GetConstructionKey()); {
		HC_Flush_Contents(".", "geometry");
	} HC_Close_Segment();

	HSelectionSet * pcSelection = GetBaseView()->GetSelection();

	//new_pos = cEvent.GetMouseWindowPos();

	// perform standard Windows explorer method of selecting and deselecting
	// using Shift and Control keys


	// if neither the Control key nor Shift key is down, deselect everything
	if (pcSelection->GetSize() > 0 && 0 == (MVO_CONTROL & nFlags) && 0 == (MVO_SHIFT & nFlags))
	{
		// 		selection->DeSelectAll();
		// 		GetBaseView()->EmitDeSelectAllMessage();
		// 		need_update = true;
	}

	// Markup 선택
	HMarkupManager * pcMarkupManager;
	if (nullptr != (pcMarkupManager = GetBaseView()->GetMarkupManager())) {
		HC_Open_Segment_By_Key(pcMarkupManager->GetMarkupKey()); {
			// compute the selection using the HOOPS window coordinate of the the pick location
			nResult = HC_Compute_Selection(GetBaseView()->GetDriverPath(),
				(pcSelection->GetSubwindowPenetration() ? "" : "."),
				//"v", cInLocation.x, cInLocation.y);
				"v, selection level = entity, selection sorting, internal selection limit = 0", cInLocation.x, cInLocation.y);
		} HC_Close_Segment();
	}

	if (nResult == 0) {
		HC_Open_Segment_By_Key(GetBaseView()->GetViewKey()); {
			nResult = HC_Compute_Selection(".",
				(pcSelection->GetSubwindowPenetration() ? "" : "./scene/overwrite"),
				//"v", cInLocation.x, cInLocation.y);
				"v, selection level = entity, related selection limit = 0, selection sorting, internal selection limit = 0", cInLocation.x, cInLocation.y);
		} HC_Close_Segment();
	}

	if (nResult > 0) {
		HandleSelection(nFlags, cOutResults);
		size_t nCount = cOutResults.GetCount();
		need_update = true;
	}

	GetBaseView()->GetConstantFrameRateObject()->SetDisableIncreaseTemp(true);

	//GetBaseView()->SetGeometryChanged();
	if (need_update) {
		GetBaseView()->Update();	// update the scene to reflect the new highlight attributes
	}

	GetBaseView()->GetConstantFrameRateObject()->SetDisableIncreaseTemp(false);

	if (GetBaseView()->GetFocusOnSelection()) {
		GetBaseView()->FocusOnSelection();
	}

	// of the selected items
	return HOP_READY;
}

// Select 처리, 재선택 및 최초 선택 Region, PMI 선택등을 처리.
void H3DF::SelectionControlPrivate::HandleSelection(UINT const nFlags, SelectionResults & cOutResults)
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

	SelectionResultsPrivate * pcResultsPrivate = (SelectionResultsPrivate *)cOutResults.GetImpl();

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

			SelectionItem * pcItem = new SelectionItem();
			SelectionItemPrivate * pcItemPrivate = (SelectionItemPrivate *)pcItem->GetImpl();
			pcItemPrivate->cKey = LineKey(Key(nKey));
			
			pcResultsPrivate->PushBack(pcItem);
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

void H3DF::SelectionControlPrivate::GetSelectOption(SelectionOptionsKit const & cInOptions, char * pchOutOption)
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

/*
	// Proximity 관련 설정은 View의 Driver 옵션이기 때문에 별도 처리해야 함.
	float fProximity = 0.0;
	if (true == cInOptions.ShowProximity(fProximity)) {
		sprintf(chOption, "selection proximity = %f", fProximity);

		Utility::Set3DfOptionString(pchOutOption, chOption);
	}
*/

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

void H3DF::SelectionControlPrivate::GetScope(SelectionOptionsKit const & cInOptions, char * pchOutScope)
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

HBaseView * H3DF::SelectionControlPrivate::GetBaseView()
{ 
	return (HBaseView *)m_pcWindow->GetBaseView(); 
}