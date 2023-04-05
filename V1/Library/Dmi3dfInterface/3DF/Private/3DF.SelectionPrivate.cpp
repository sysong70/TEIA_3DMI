#include "StdAfx.h"

#include "3DF.SelectionPrivate.h"

#include "../3DF.Window.h"

#include "../3DF.Line.h"
#include "../3DF.Shell.h"
#include "../3DF.Utility.h"

#include <vhash.h>
#include <vlist.h>

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

int SelectionControlPrivate::SelectByPoint(Point const & cInLocation, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults)
{
	int	 nResult = 0;

	char chAction[MVO_BUFFER_SIZE] = "v";

	// 선택 옵션을 문자열로 변환
	GetSelectOption(cInOptions, chAction);

	HSelectionSet * pcSelection = GetBaseView()->GetSelection();

	// 선택 옵션에 따라 선택 작업 실시
	HC_Open_Segment_By_Key(GetBaseView()->GetViewKey()); {
		nResult = HC_Compute_Selection(".",
			(pcSelection->GetSubwindowPenetration() ? "" : "./scene/overwrite"), chAction, cInLocation.x, cInLocation.y);
			//"v, selection level = entity, related selection limit = 0, selection sorting, internal selection limit = 0", cInLocation.x, cInLocation.y);
	} HC_Close_Segment();

	// 선택된 요소가 없음
	if (0 == nResult) {
		return 0;
	}

	HC_KEY  nKey = INVALID_KEY;
	int nOffset1, nOffset2, nOffset3;
	HC_KEY * pnKeys = nullptr;
	HC_KEY * pnIncludeKeys = nullptr;
	int	eSelectedType = SelType::None;
	char chKeyType[MVO_BUFFER_SIZE];
	int	nIncludeCount = 0;

	// 선택된 요소를 SelectionResults에 저장하기 위해서 새롭게 생성
	SelectionResultsPrivate * pcResultsPrivate = new SelectionResultsPrivate();

	do {
		// 선택된 요소를 저장하기 위해서 Item 생성
		SelectionItemPrivate * pcItemPrivate = new SelectionItemPrivate();

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
		pcItemPrivate->nKeyCount = nKeyCount;

		if (0 < nKeyCount) {
			pnKeys = new HC_KEY[nKeyCount];
			pnIncludeKeys = new HC_KEY[nKeyCount];
			HC_Show_Selection_Original_Keys(&nKeyCount, pnKeys);

			pcItemPrivate->pnKeys = pnKeys;
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
			pcItemPrivate->pcKey = new LineKey(Key(nKey));
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
			pcItemPrivate->pcKey = new ShellKey(Key(nKey));

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

		HC_KEY nTestKey2 = pcItemPrivate->pcKey->KeyValue();

		// Selection Item을 생성해서 Selection Item Private을 저장한다.
		SelectionItem * pcItem = new SelectionItem();
		pcItemPrivate->SetObject(pcItem);

		pcResultsPrivate->aItemList.AddTail(pcItem);

	} while (HC_Find_Related_Selection());

	pcResultsPrivate->SetObject(&cOutResults);

	return HOP_READY;
}


int SelectionControlPrivate::SelectByPoint(Point const & cInLocation, UINT const nFlags, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults)
{
	HPoint  new_pos;
	int		nResult = 0;
	bool	need_update = false;

	HSelectionSet * pcSelection = GetBaseView()->GetSelection();

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


// 입력되는 Location은 MouseWindowPos을 이용한다. HEventInfo에서 GetMouseWindowPos() 함수를 이용해서 가져올 수 있음
int SelectionControlPrivate::SelectButtonDown_V1(Point const & cInLocation, UINT const nFlags, SelectionResults & cOutResults)
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
void SelectionControlPrivate::HandleSelection(UINT const nFlags, SelectionResults & cOutResults)
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

	SelectionResultsPrivate * pcResultsPrivate = new SelectionResultsPrivate();

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

			SelectionItemPrivate * pcItemPrivate = new SelectionItemPrivate();
			pcItemPrivate->pcKey = new LineKey(Key(nKey));

			SelectionItem * pcItem = new SelectionItem();
			pcItemPrivate->SetObject(pcItem);
			
			pcResultsPrivate->aItemList.AddTail(pcItem);
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

	pcResultsPrivate->SetObject(&cOutResults);
}

void SelectionControlPrivate::GetSelectOption(SelectionOptionsKit const & cInOptions, char * pchOutOption)
{
	char chOption[MVO_BUFFER_SIZE] = "\0";

	Selection::Level eLevel;
	if (true == cInOptions.ShowLevel(eLevel)) {
		switch (eLevel)
		{
			case TDF::Selection::Level::Segment:
				sprintf(chOption, "selection level = segment");
				break;

			case TDF::Selection::Level::Entity:
				sprintf(chOption, "selection level = entity");
				break;

			case TDF::Selection::Level::Subentity:
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
			case TDF::Selection::Sorting::Off:
				sprintf(chOption, "no selection sorting");
				break;

			case TDF::Selection::Sorting::Default:
				sprintf(chOption, "selection sorting");
				break;

		}

		Utility::Set3DfOptionString(pchOutOption, chOption);
	}
}