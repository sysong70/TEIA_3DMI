#include "StdAfx.h"

#include "3DF.OpCameraOrbitSelect.h"

#include <Common_Define.h>

#include <HTools.h>
#include <HBaseView.h>
#include <HSelectionSet.h>
#include <HMarkupManager.h>
#include <HEventManager.h>
#include <HConstantFrameRate.h>

USING_3DF_NAMESPACE

OpCameraOrbitSelect::OpCameraOrbitSelect(HBaseView * view, int DoRepeat, int DoCapture) :
	HOpCameraOrbit(view, DoRepeat, DoCapture)
{
	m_nSelectPickCount = 200;
	m_nMouseDownTickCount = 0;
	m_bOrbitMode = false;

}

OpCameraOrbitSelect::~OpCameraOrbitSelect()
{
	HC_Open_Segment_By_Key(GetView()->GetConstructionKey());
	HC_Flush_Contents(".", "geometry");
	HC_Close_Segment();
}

const char * OpCameraOrbitSelect::GetName()
{
	return "3DF_OpCameraOrbitSelect";
}

HBaseOperator * OpCameraOrbitSelect::Clone()
{
	return new OpCameraOrbitSelect(GetView());
}
//== Mouse Event 처리 ===============================================================================

int OpCameraOrbitSelect::OnLButtonDown(HEventInfo & cEvent)
{
	m_cMouseDownPoint = cEvent.GetMousePixelPos();
	m_nMouseDownTickCount = GetTickCount();
	m_bOrbitMode = false;

	// Shift & L Button 이벤트는 Area Select
	if (MVO_SHIFT & cEvent.GetFlags()) {
	}

	return HOpCameraOrbit::OnLButtonDown(cEvent);
}

int OpCameraOrbitSelect::OnLButtonUp(HEventInfo & cEvent)
{
	DWORD nMouseUpTickCount = GetTickCount();
	DWORD nTickCount = nMouseUpTickCount - m_nMouseDownTickCount;

	// 2 Pixel이하 200 Tick이하에서만 선택하는 것으로 판정한다.
	if (m_nSelectPickCount > nTickCount) {
		const HPoint & cMoustPoint = cEvent.GetMousePixelPos();
		HVector cVector = cMoustPoint - m_cMouseDownPoint;
		double dLength = HC_Compute_Vector_Length(&cVector);

		if (2.0 > dLength) {
			SelectButtonDown(cEvent);
		}
	}

	m_bOrbitMode = false;

	return HOpCameraOrbit::OnLButtonUp(cEvent);
}

int OpCameraOrbitSelect::OnLButtonDownAndMove(HEventInfo & cEvent)
{
	m_bOrbitMode = true;
	return HOpCameraOrbit::OnLButtonDownAndMove(cEvent);
}

//== Selection 처리 =================================================================================

// HOpSelectAperture Class에서 가져온 함수를 가공함.

int OpCameraOrbitSelect::SelectButtonDown(HEventInfo & cEvent)
{
	HPoint  new_pos;
	int		nResult = 0;
	bool	need_update = false;

	// Mouse went down; find the selected items
	HC_Open_Segment_By_Key(GetView()->GetConstructionKey());
	HC_Flush_Contents(".", "geometry");
	HC_Close_Segment();

	HSelectionSet * selection = GetView()->GetSelection();

	new_pos = cEvent.GetMouseWindowPos();

	// perform standard Windows explorer method of selecting and deselecting
	// using Shift and Control keys

	// if neither the Control key nor Shift key is down, deselect everything
	if (selection->GetSize() > 0 && !cEvent.Control() && !cEvent.Shift())
	{
// 		selection->DeSelectAll();
// 		GetView()->EmitDeSelectAllMessage();
// 		need_update = true;
	}

	HMarkupManager * pcMarkupManager;
	if (nullptr != (pcMarkupManager = GetView()->GetMarkupManager())) {
		HC_Open_Segment_By_Key(pcMarkupManager->GetMarkupKey());
		// compute the selection using the HOOPS window coordinate of the the pick location

		nResult = HC_Compute_Selection(GetView()->GetDriverPath(),
			(selection->GetSubwindowPenetration() ? "" : "."),
			"v, selection level = entity, selection sorting, internal selection limit = 0", new_pos.x, new_pos.y);

		HC_Close_Segment();
	}

	if (nResult == 0) {
		HC_Open_Segment_By_Key(GetView()->GetViewKey());

		nResult = HC_Compute_Selection(".",
			(selection->GetSubwindowPenetration() ? "" : "./scene/overwrite"),
			"v, selection level = entity, related selection limit = 0, selection sorting, internal selection limit = 0", new_pos.x, new_pos.y);

		HC_Close_Segment();
	}

	if (nResult > 0) {
		HandleSelection(cEvent);
		need_update = true;
	}

	GetView()->GetConstantFrameRateObject()->SetDisableIncreaseTemp(true);

	//GetView()->SetGeometryChanged();
	if (need_update) {
		GetView()->Update();	// update the scene to reflect the new highlight attributes
	}

	GetView()->GetConstantFrameRateObject()->SetDisableIncreaseTemp(false);

	if (GetView()->GetFocusOnSelection()) {
		GetView()->FocusOnSelection();
	}
	
	// of the selected items
	return HOP_READY;
}

// Select 처리, 재선택 및 최초 선택 Region, PMI 선택등을 처리.
void OpCameraOrbitSelect::HandleSelection(HEventInfo & cEvent)
{
	HC_KEY  nKey = INVALID_KEY;
	int nOffset1, nOffset2, nOffset3;
	HC_KEY * pnKeys = 0;
	HC_KEY * pnIncludeKeys = 0;
	int	nSelectedType = SelType::None;
	int nRegion = 0;
	char chType[64] = { "" };
	int	nIncludeCount = 0;

	HSelectionSet * pcSelection = GetView()->GetSelection();

	// Control을 누른경우 Face 단위로 선택이 됨.
	if (MVO_CONTROL & cEvent.GetFlags()) {
		pcSelection->SetSelectionLevel(HSelectLevel::HSelectEntity);
	}
	else {
		pcSelection->SetSelectionLevel(HSelectLevel::HSelectSegment);
	}

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
			nSelectedType = SelType::Line;
		}
		else if (streq(chType, "marker")) {
			nSelectedType = SelType::Marker;
		}
		else if (streq(chType, "text leader")) {
			nSelectedType = SelType::Shell;	//?
			nKey = HC_Show_Owner_Original_Key(nKey);		// move up to text;
		}
		else {
			// This may be shell, mesh, cyliner, etc...
			nSelectedType = SelType::Shell;
			// But if it really is a shell, check for regions.
			if (streq(chType, "shell") && nOffset3 != -1) {
				int low, high;
				HC_Show_Region_Range(nKey, &low, &high);
				if ((low != high || low > 0) && pcSelection->GetAllowRegionSelection()) {
					nSelectedType |= SelType::Region;
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

			if (HSelectSubentity == pcSelection->GetSelectionLevel() && (nSelectedType & SelType::Shell))
			{
				if (cEvent.Shift()) {
					pcSelection->DeSelectSubentity(nKey, nIncludeCount, pnIncludeKeys, 1, &nOffset3, &nOffset1, &nOffset2, true);
				}
				else {
					pcSelection->SelectSubentity(nKey, nIncludeCount, pnIncludeKeys, 1, &nOffset3, &nOffset1, &nOffset2, true);
				}
			}
			else {
				// if selected, deselect, else select
				if (pcSelection->IsSelected(nKey, nIncludeCount, pnIncludeKeys) ||
					(nSelectedType & SelType::Region && pcSelection->IsRegionSelected(nKey, nIncludeCount, pnIncludeKeys, nRegion)))
				{
					if (nSelectedType & SelType::Region) {
						pcSelection->DeSelectRegion(nKey, nIncludeCount, pnIncludeKeys, nRegion, true);
					}
					else {
						pcSelection->DeSelect(nKey, nIncludeCount, pnIncludeKeys, true);
					}
				}
				else
				{
					if (nSelectedType & SelType::Region)
						pcSelection->SelectRegion(nKey, nIncludeCount, pnIncludeKeys, nRegion, true);
					else
						pcSelection->Select(nKey, nIncludeCount, pnIncludeKeys, true);
				}
			}
		}

	} while (HC_Find_Related_Selection());

	pcSelection->SetSelectWillNotify(true);

	// notify the event to view
	GetView()->Notify(HSignalSelected);

	delete[] pnKeys;
	delete[] pnIncludeKeys;

}