#include "StdAfx.h"

#include "Operator.SelectArea.h"

#include "../3DF.Canvas.h"
#include "Selection.h"
#include "SelectionSet.h"

#include <HTools.h>
#include <HBaseView.h>
#include <HSelectionSet.h>
#include <HEventManager.h>
#include <HConstantFrameRate.h>
#include <HPShowCache.h>


USING_3DF_NAMESPACE

Operator::SelectArea::SelectArea(HBaseView * Canvas, int DoRepeat, int DoCapture) :
	HOpSelectArea(Canvas, DoRepeat, DoCapture)
{

}

const char * Operator::SelectArea::GetName()
{
	return "3DF_SelectArea";
}

HBaseOperator * Operator::SelectArea::Clone()
{
	return new SelectArea(GetView());
}

int Operator::SelectArea::OnLButtonDown(HEventInfo & cEvent)
{
	//Canvas * pcView = (Canvas *)GetView();
	//pcView->SetSubentitySelectLevel();

/*
	// Control을 누른경우 Face 단위로 선택이 됨.
	if (MK_CONTROL & cEvent.GetFlags()) {
		View * pcView = (View *)GetView();
		pcView->SetSubentitySelectLevel();
		GetView()->GetSelection()->SetSelectionLevel(HSelectLevel::HSelectEntity);
	}
	else {
		GetView()->GetSelection()->SetSelectionLevel(HSelectLevel::HSelectSegment);
	}
*/

	return HOpSelectArea::OnLButtonDown(cEvent);
}

// int Operator::SelectArea::OnRButtonDownAndMove(HEventInfo & cEvent)
// {
// 	return HOpSelectArea::OnLButtonDownAndMove(cEvent);
// }

int Operator::SelectArea::OnLButtonUp(HEventInfo & cEvent)
{
	//return HOpSelectArea::OnLButtonUp(cEvent);

	return ButtonUp(cEvent);
}

int Operator::SelectArea::ButtonUp(HEventInfo & cEvent)
{
	if (!OperatorStarted()) {
		return HBaseOperator::OnLButtonDownAndMove(cEvent);
	}

	HOpConstructRectangle::OnLButtonUp(cEvent);

	//GetView()->SetVisibilitySelectionMode(true);

	H3DF::SelectionSet  * pcSelection = (H3DF::SelectionSet *)GetView()->GetSelection();

	// Perform standard Windows explorer method of selecting and deselecting
	// using Shift and Control keys

	// If neither the Control key nor Shift key is down, deselect everything
/*
	if (!cEvent.Control() && !cEvent.Shift())
	{
		pcSelection->DeSelectAll();
		GetView()->EmitDeSelectAllMessage();
	}*/

	HC_Open_Segment_By_Key(GetView()->GetViewKey());
	
	char cval[MVO_BUFFER_SIZE];
	HC_Show_One_Net_Heuristic("visual selection", cval);

	// compute the selection using the HOOPS window coordinate of the the pick location
	int nSelectionCount = 0;

	HSelectLevel eSelectLevel = pcSelection->GetSelectionLevel();

	bool bNeedInternalSelection = (pcSelection->GetSelectionLevel() == HSelectSubentity || pcSelection->GetSelectionLevel() == HSelectRegion);

	if (m_bRectangleExists)
	{
		const char * chOption = bNeedInternalSelection ?
			"v, no related selection limit, no selection sorting, no internal selection limit" :
			"v, no related selection limit, no selection sorting, internal selection limit=0";
			//"v, no related selection limit, no selection sorting, internal selection limit=0";
			//"v, selection level = entity, no related selection limit, no selection sorting, internal selection limit=0";
		//HC_QSet_Heuristics ("./scene/overwrite", "no related selection limit, no selection sorting, internal selection limit=0");
		HUtility::Order(&m_ptRectangle[0], &m_ptRectangle[1]);

		char chStartSeg[128];
		if (true == pcSelection->GetSubwindowPenetration()) {
			sprintf(chStartSeg, "");
		}
		else {
			sprintf(chStartSeg, "./scene/overwrite");
		}

		nSelectionCount = HC_Compute_Selection_By_Area(".", chStartSeg, chOption,
			m_ptRectangle[0].x, m_ptRectangle[1].x, m_ptRectangle[0].y, m_ptRectangle[1].y);
	}
	else
	{
		const char * chOption = bNeedInternalSelection ?
			"v, related selection limit = 0, selection sorting, no internal selection limit" :
			"v, related selection limit = 0, selection sorting, internal selection limit=0";

		char chStartSeg[128];
		if (true == pcSelection->GetSubwindowPenetration()) {
			sprintf(chStartSeg, "");
		}
		else {
			sprintf(chStartSeg, "./scene/overwrite");
		}

		//HC_QSet_Heuristics ("./scene/overwrite", "related selection limit = 0, selection sorting, internal selection limit=0");
		HPoint new_pos = cEvent.GetMouseWindowPos();
		nSelectionCount = HC_Compute_Selection(".", chStartSeg, chOption, new_pos.x, new_pos.y);
	}
	//HC_QUnSet_Heuristics ("./scene/overwrite");
	HC_Close_Segment();

	// BIG NOTE AND A BIG TODO:
	//	I am disabling this optimization of not having to check for selected for now
	// The problem is when you have segment level selection and you selection multiple 
	// entities owned by same segment. With this optimization you end up in duplicate selection
	// which causes problems when deselection etc. (primitive.hmf will show the problem)
	// With this optimization - selection is about 26% faster for roughly 500,000 entity selection.
	// flooplan.hmf.
	// So - we need to revisit this - Rajesh B (22-Mar-05)
	bool bNeedToCheckSelected = /* DISABLES CODE */ (true) || (pcSelection->GetSize() > 0);

	if (nSelectionCount)
	{
		int nCount = 0;
		int * vertex = 0;
		int * faces = 0;
		int array_size = 0;
		bool * regions = 0;
		int region_array_size = 0;

		pcSelection->SetPShowCache(new HVHashPShowCache);

		pcSelection->SetSelectWillNotify(false);

		char ** pchIncludeSegment = new char * [1024];// [MVO_BUFFER_SIZE] ;
		for (int nIndex = 0; nIndex < 1024; nIndex++) {
			pchIncludeSegment[nIndex] = new char[MVO_BUFFER_SIZE];
		}

		int i = 0;
		do	// process each item in the selection queue
		{
			i++;

			bool bOnlySelectRegions = false;
			HC_KEY nKey, nObjectKey;
			int nOffset1, nOffset2, nOffset3;

			HC_Show_Selection_Element(&nKey, &nOffset1, &nOffset2, &nOffset3);
			HC_Show_Selection_Original_Key(&nKey);

			char chType[MVO_BUFFER_SIZE] = "\0";
			HC_Show_Key_Type(nKey, chType);

			char chSegment[MVO_BUFFER_SIZE] = "\0";
			if (streq(chType, "segment")) {
				HC_Show_Segment(nKey, chSegment);
			}

			// Use this function instead of HC_Show_Selection_Element so we get the internal key 3dGS
			// uses to refer to the piece of geometry.  key will hold this value
//			HC_Show_Selection_Original_Key(&nKey);

			//this function makes sure that only the top level segment of an "object" is selectable. If a segment/geometry 
			//is not part of an object definition the key is not changed
			nObjectKey = HObjectManager::FindHObjectSegment(nKey);

			if (nObjectKey != nKey) {
				bNeedToCheckSelected = true;
				nKey = nObjectKey;
			}

			if (INVALID_KEY == nKey) {
				continue;
			}

			// build up an array of include keys to pass with the selection
			int nSelectKeyCount = 0;
			char skey_type[MVO_BUFFER_SIZE];
			HC_Show_Selection_Keys_Count(&nSelectKeyCount);

			HC_KEY * pbKeys = new HC_KEY[nSelectKeyCount];
			HC_KEY * pcIncludeKeys = new HC_KEY[nSelectKeyCount];
			

			HC_Show_Selection_Original_Keys(&nSelectKeyCount, pbKeys);

			int nIndex, nIncludeCount = 0;
			for (nIndex = nSelectKeyCount - 1; nIndex >= 0; nIndex--)
			{
				//HC_Show_Segment(pbKeys[nIndex], chSegment);
				HC_Show_Key_Type(pbKeys[nIndex], skey_type);

				if (streq(skey_type, "include")) {
					pcIncludeKeys[nIncludeCount] = pbKeys[nIndex];
					HC_Show_Include_Segment(pcIncludeKeys[nIncludeCount], pchIncludeSegment[nIncludeCount]);
					nIncludeCount++;
				}
				else if (streq(skey_type, "reference")) {
					nKey = pbKeys[nIndex];
				}
				else if (streq(skey_type, "shell"))
				{
					int nLow = 0, nHigh = 0;
					HC_Show_Region_Range(pbKeys[nIndex], &nLow, &nHigh);
					if (pcSelection->GetSelectionLevel() == HSelectRegion && (nLow != nHigh || nLow > 0))
					{
						// Select regions in the shell
						HC_Show_Selection_Elements_Coun(nullptr, &nCount);

						if (nCount > 0) {
							/* Get the selected faces. */
							if (nCount > array_size) {
								delete[] vertex;
								delete[] faces;
								array_size = nCount;
								vertex = new int[array_size];
								faces = new int[array_size];
							}
							
							HC_KEY shell_key = pbKeys[nIndex];
							HC_Show_Selection_Elements(nullptr, &nCount, vertex, vertex, faces);

							/* build an array that represents selected regions. */
							if (nHigh + 1 > region_array_size) {
								delete[] regions;
								regions = new bool[nHigh + 1];
								region_array_size = nHigh + 1;
							}
							
							memset(regions, 0, sizeof(bool) * (nHigh + 1));
							
							// TODO: find a faster way to do this.
							HC_Open_Geometry(shell_key); {
								int region;
								for (int ii = 0; ii < nCount; ++ii) {
									if (faces[ii] == INVALID_KEY) {
										continue;
									}
									HC_Open_Face(faces[ii]); {
										HC_Show_Region(&region);
										regions[region] = true;
									}HC_Close_Face();
								}
							}HC_Close_Geometry();

							HShellObject oShellObj(shell_key);

							for (int ii = 0; ii <= nHigh; ++ii) {
								if (regions[ii] && !pcSelection->IsRegionSelected(shell_key, nIncludeCount, pcIncludeKeys, ii)) {
									pcSelection->SelectRegion(oShellObj, nIncludeCount, pcIncludeKeys, ii, true);
								}
							}

							bOnlySelectRegions = true;
						}

					}
				}
				else if (streq(chType, "segment")) {
					HC_Show_Segment(pbKeys[nIndex], chSegment);
				}
			}

			if (pcSelection->GetSelectionLevel() == HSelectSubentity)
			{
				HC_KEY sub_key;
				int sub_count;
				int * sub_v1, * sub_v2, * sub_faces;

				HC_Show_Selection_Elements_Coun(&sub_key, &sub_count);

				sub_v1 = new int[sub_count];
				sub_v2 = new int[sub_count];
				sub_faces = new int[sub_count];

				HC_Show_Selection_Elements(&sub_key, &sub_count, sub_v1, sub_v2, sub_faces);

				//In the case of Select Area, we want to ignore nearest vertices/edges from faces
				for (int j = 0; j < sub_count; j++)
				{
					if (sub_faces[j] != -1)
					{
						sub_v1[j] = -1;
						sub_v2[j] = -1;
					}
				}

				pcSelection->SelectSubentity(nKey, nIncludeCount, pcIncludeKeys, sub_count, sub_faces, sub_v1, sub_v2, true);

				delete[] sub_v1;
				delete[] sub_v2;
				delete[] sub_faces;
			}
			// handle control, shift and no_key cases
			else if (!bOnlySelectRegions && (!bNeedToCheckSelected || !(pcSelection->IsSelected(nKey, nIncludeCount, pcIncludeKeys)))) {
				pcSelection->Select(nKey, nIncludeCount, pcIncludeKeys, true);
			}

			delete[] pbKeys;
			delete[] pcIncludeKeys;

			bOnlySelectRegions = false;

		} while (HC_Find_Related_Selection());

		pcSelection->SetSelectWillNotify(true);

		// notify the cEvent to Canvas
		GetView()->Notify(HSignalSelected, pcSelection);

		pcSelection->FlushPShowCache();

		delete[] regions;
		delete[] vertex;
		delete[] faces;
	}

 	if (GetView()->GetFocusOnSelection()) {
 		GetView()->FocusOnSelection();
 	}

	GetView()->Update();

	GetView()->GetConstantFrameRateObject()->SetDisableIncreaseTemp(false);

	char chBufferHeuristics[MVO_BUFFER_SIZE] = "\0";
	char chNetBufferHeuristics[MVO_BUFFER_SIZE] = "\0";

	char chBufferDriverOption[MVO_BUFFER_SIZE] = "\0";
	char chNetBufferDriverOption[MVO_BUFFER_SIZE] = "\0";

	char chRenderingOptions[MVO_BUFFER_SIZE] = "\0";
	char chNetBufferRenderingOptions[MVO_BUFFER_SIZE] = "\0";

	char chSelectability[MVO_BUFFER_SIZE] = "\0";
	char chVisibility[MVO_BUFFER_SIZE] = "\0";

	HC_Open_Segment_By_Key(GetView()->GetViewKey()); {
		HC_Show_Heuristics(chBufferHeuristics);
		HC_Show_Net_Heuristics(chNetBufferHeuristics);

		HC_Show_Driver_Options(chBufferDriverOption);
		HC_Show_Net_Driver_Options(chNetBufferDriverOption);

		HC_Show_Rendering_Options(chRenderingOptions);
		HC_Show_Net_Rendering_Options(chNetBufferRenderingOptions);
	} HC_Close_Segment();

	HC_Open_Segment_By_Key(GetView()->GetSceneKey()); {
		HC_Show_Heuristics(chBufferHeuristics);
		HC_Show_Net_Heuristics(chNetBufferHeuristics);

		HC_Show_Driver_Options(chBufferDriverOption);
		HC_Show_Net_Driver_Options(chNetBufferDriverOption);

		HC_Show_Rendering_Options(chRenderingOptions);
		HC_Show_Net_Rendering_Options(chNetBufferRenderingOptions);

		HC_Show_Selectability(chSelectability);
		HC_Show_Visibility(chVisibility);
	} HC_Close_Segment();

	char chOverwriteVisibility[MVO_BUFFER_SIZE] = "\0";

	HC_Open_Segment_By_Key(GetView()->GetOverwriteKey()); {
		HC_Open_Segment("./selectionconditionincluder"); {
			HC_Show_Rendering_Options(chRenderingOptions);
		} HC_Close_Segment();

		HC_Show_Net_Visibility(chOverwriteVisibility);

	} HC_Close_Segment();

	char chSystemOption[MVO_BUFFER_SIZE];
	HC_Show_System_Options(chSystemOption);

	return HOP_READY;
}