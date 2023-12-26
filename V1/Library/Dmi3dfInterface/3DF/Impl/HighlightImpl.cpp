#include "StdAfx.h"

#include "HighlightImpl.h"
#include "../Highlight.h"

#include "../Selection.h"
#include "SelectionImpl.h"

#include "../Window.h"

#include "../Line.h"
#include "../LineAttribute.h"

#include "../Style.h"

#include <vhash.h>
#include <vlist.h>

#include <atlcoll.h>

#include <HTools.h>
#include <HBaseView.h>
#include <HSelectionSet.h>
#include <HBaseOperator.h>
#include <HMarkupManager.h>
#include <HEventManager.h>
#include <HConstantFrameRate.h>

#define		SEGMENT_TYPE		1
#define		ENTITY_TYPE			2
#define		SUBENTITY_TYPE		3
#define		REGION_TYPE			4

#define MVO_HIGHLIGHT_STYLE_SEGMENT        "?Style Library/mvo_highlight_style_segment"
#define MVO_TRANSPARENCY_HIGHLIGHT_SEGMENT "?Style Library/mvo_transparent_highlight_style"
#define MVO_REFERENCE_SELECTION_SEGMENT    "overwrite/selection_segment"

using namespace H3DF;

H3DF::HighlightControlImpl::HighlightControlImpl()
{
	m_eType = H3DF::Type::HighlightControl;
}

H3DF::HighlightControlImpl::HighlightControlImpl(WindowKey const & cInWindow)
{
	m_eType = H3DF::Type::HighlightControl;

	m_pcWindow = (WindowKey *) &cInWindow;
}

BaseView * H3DF::HighlightControlImpl::GetBaseView()
{
	return (BaseView *)m_pcWindow->GetBaseView();
}

BaseView * H3DF::HighlightControlImpl::GetBaseView() const
{
	return (BaseView *) m_pcWindow->GetBaseView();
}

HSelectionSet * H3DF::HighlightControlImpl::GetHighlightSelection()
{
	return GetBaseView()->GetHighlightSelection();
}

void H3DF::HighlightControlImpl::Highlight(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions, bool bInRemoveExisting)
{
	HSelectionSet * pcHighlightSelection = GetHighlightSelection();

	char chType[MVO_BUFFER_SIZE];

	bool bNeedDeselect = true;
	bool bNeedUpdate = true;

	if (true == bInRemoveExisting) {
		pcHighlightSelection->DeSelectAll();
	}

	SelectionResultsImpl * pcSelectionResultsImpl = (SelectionResultsImpl *) cInItems.GetImpl();

	for (auto pcItem : pcSelectionResultsImpl->GetItems()) {
		SelectionItemImpl * pcImpl = (SelectionItemImpl *) pcItem->GetImpl();

		HC_KEY nKey = pcImpl->cKey.KeyValue();

		// Region 선택 관련 처리 부분
		if (H3DF::Type::ShellKey == pcItem->Type() && (pcImpl->nLowest != pcImpl->nHighest || pcImpl->nLowest > 0)) {
			bNeedDeselect = false;

			if (!pcHighlightSelection->IsRegionSelected(nKey, pcImpl->nIncludeCount, pcImpl->pnIncludeKeys, pcImpl->nRegion))
			{
				if (true == bInRemoveExisting) {
					pcHighlightSelection->DeSelectAll();
				}

				pcHighlightSelection->SelectRegion(nKey, pcImpl->nIncludeCount, pcImpl->pnIncludeKeys, pcImpl->nRegion, false);
			}
			else {
				bNeedUpdate = false;
			}
		}
		else {
			bNeedDeselect = false;

			if (!pcHighlightSelection->IsSelected(nKey, pcImpl->nIncludeCount, pcImpl->pnIncludeKeys)) {
				HSelectLevel eSelectLevel = pcHighlightSelection->GetSelectionLevel();
				if (pcHighlightSelection->GetSelectionLevel() != HSelectSegment) // never should fail for dynamic highlighting, but let's be nice and check
				{
					// the key is to a geometric entity.  If we are in segment selection mode,
					// then we need to get the key to its parent segment.
					HC_Show_Key_Type(nKey, chType);

					if (!streq("segment", chType))
					{
						char segname[MVO_BUFFER_SIZE];
						HC_KEY segkey;

						segkey = HC_KShow_Owner_Original_Key(nKey);
						HC_Show_Owner_By_Key(nKey, segname);

						// climb up one more level if this is the temporary highlight key
						if (pcHighlightSelection->IsHighlightSegment(segkey))
						{
							segkey = HC_KShow_Owner_Original_Key(segkey);
							HC_Show_Owner_By_Key(segkey, segname);
						}
					}
				}

				pcHighlightSelection->Select(nKey, pcImpl->nIncludeCount, pcImpl->pnIncludeKeys, false);
			}
			else {
				bNeedUpdate = false;
			}
		}
	}

	if (bNeedDeselect) {
		pcHighlightSelection->DeSelectAll();
	}

	if (bNeedUpdate) {
		bool bShowNotification = false;
		cInOptions.ShowNotification(bShowNotification);

		if (true == bShowNotification) {
			H3DF::BaseView * pcView = GetBaseView();
			pcView->ForceUpdate();
		}
	}
}

void H3DF::HighlightControlImpl::Highlight(SelectionItem const & cInItem, HighlightOptionsKit const & cInOptions, bool bInRemoveExisting)
{
	HSelectionSet * pcSelection = GetHighlightSelection(); // HSelectionSet에서 Select 및 Highlight를 다 처리함.

	SelectionItemImpl * pcImpl = (SelectionItemImpl *) cInItem.GetImpl();

	HC_KEY nPrimitiveKey = pcImpl->cKey.KeyValue();
	pcSelection->Select(nPrimitiveKey, pcImpl->nIncludeCount, pcImpl->pnIncludeKeys, false);
}

void H3DF::HighlightControlImpl::Unhighlight(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions)
{
	if (0 == cInItems.GetCount()) {
		return;
	}

	HBaseView * pcView = GetBaseView();

	SelectionResultsImpl * pcImpl = (SelectionResultsImpl *) cInItems.GetImpl();

	// cInItem를 순회하면서 Unhighlight를 수행한다.
	for (auto pcItem : pcImpl->GetItems()) {
		SelectionItemImpl * pcItemImpl = (SelectionItemImpl *) pcItem->GetImpl();
		HC_KEY nKey = pcItemImpl->cKey.KeyValue();
		pcView->GetHighlightSelection()->DeSelect(nKey, pcItemImpl->nIncludeCount, pcItemImpl->pnIncludeKeys, false);
	}

	bool bShowNotification = false;
	cInOptions.ShowNotification(bShowNotification);

	if (true == bShowNotification) {
		pcView->ForceUpdate();
	}
}

void H3DF::HighlightControlImpl::Unhighlight(SelectionItem const & cInItem, HighlightOptionsKit const & cInOptions)
{
	// cInItem의 Impl을 가져와서 작업을 수행한다.
	SelectionItemImpl * pcSelectionItemImpl = (SelectionItemImpl *) cInItem.GetImpl();
	if (nullptr == pcSelectionItemImpl) {
		return;
	}

	HBaseView * pcView = GetBaseView();

	HC_KEY nKey = pcSelectionItemImpl->cKey.KeyValue();
	pcView->GetHighlightSelection()->DeSelect(nKey, pcSelectionItemImpl->nIncludeCount, pcSelectionItemImpl->pnIncludeKeys, false);

	bool bShowNotification = false;
	cInOptions.ShowNotification(bShowNotification);

	if (true == bShowNotification) {
		pcView->ForceUpdate();
	}
}

int H3DF::HighlightControlImpl::NoButtonDownAndMove(int nFlags, int x, int y, SelectionResults & cOutSelections)
{
	DEBUG_VALID(m_pcWindow);

	PixelPoint cMousePoint(x, y);

	WindowPoint cWindowPoint(*m_pcWindow, cMousePoint);

	DoDynamicHighlighting(cWindowPoint, cOutSelections);

	return 0;
}

bool H3DF::HighlightControlImpl::DoDynamicHighlighting(WindowPoint cMousePoint, SelectionResults & cOutSelections)
{
	DEBUG_VALID(m_pcWindow);

	BaseView * pcView = GetBaseView();
	DEBUG_VALID(pcView);

	if (pcView->GetSuppressUpdateTick() || pcView->GetSuppressUpdate() || !pcView->GetModel()->GetFileLoadComplete()) {
		return false;
	}

	bool bNeedUpdate = true;

/*	HC_Open_Segment_By_Key(m_pcWindow->GetBaseView()->GetOverwriteKey()); {
		//HC_Set_Selectability("everything = off, lines = on");
// 		HC_Set_Rendering_Options("attribute lock = (line weight)");
		HC_Set_Line_Weight(1.9);
// 		HC_Set_Line_Weight(SELECT_EDGE_WEIGHT);
	} HC_Close_Segment(); */

	float fProximity = 0.2f;
	SelectionOptionsKit cSelectOption;
	cSelectOption.SetLevel(Selection::Level::Entity).SetRelatedLimit(5).SetProximity(0.2f).SetBias(Selection::Bias::Lines);
	SelectionResults cHighlightSelection;
	size_t nResult = m_pcWindow->GetSelectionControl().SelectByPoint(cMousePoint, cSelectOption, cHighlightSelection);

	// 	HC_Open_Segment_By_Key(m_pcWindow->GetBaseView()->GetOverwriteKey()); {
	// 		HC_Set_Selectability("everything = off, faces = on");
	// 		HC_UnSet_Line_Weight();
	// 	} HC_Close_Segment();

	// 	cSelectOption.SetLevel(Selection::Level::Entity).SetRelatedLimit(3).SetProximity(0.1f);// .SetBias(Selection::Bias::None);
	// 	nResult += m_pcWindow->GetSelectionControl().SelectByPoint(cMousePoint, cSelectOption, cHighlightSelection);

		// 선택된 요소가 없는 경우 Deselect All을 하고 Update를 한다.
	if (0 == nResult) {
		if (0 < m_cOldHighlightSelection.GetCount()) {
			GetBaseView()->GetHighlightSelection()->DeSelectAll();
			GetBaseView()->ForceUpdate();
			m_cOldHighlightSelection.Reset();
		}

		return false;
	}

	// 선택결과를 Z값으로 Sort한다.
	cHighlightSelection.Sort();

	// 첫번째 요소를 저장한다.
	SelectionItem * pcFrontItem = nullptr;
	if (0 < nResult) {
		pcFrontItem = cHighlightSelection.Front();
	}

	// #Todo: Selection Filter를 적용해야 함.

	// 2개 이상의 요소가 선택된 경우 처리한다.
	if (1 < cHighlightSelection.GetCount()) {
		// ----- 선택된 요소에서 Line이나 Edge를 우선적으로 찾도록 한다. -----
		DEBUG_VALID(pcFrontItem);

		// 1. 첫번째 요소가 Shell인 경우 다음 요소에서 Line을 찾는다. 
		if (H3DF::Type::ShellKey == pcFrontItem->Type()) {
			WorldPoint cFaceWordlPoint;
			WindowPoint cFaceWindowPoint;
			pcFrontItem->ShowSelectionPosition(cFaceWordlPoint);
			pcFrontItem->ShowSelectionPosition(cFaceWindowPoint);

			SelectionResultsIterator cIter = cHighlightSelection.GetIterator();
			// 첫번째 요소 다음을 선택한다.
			cIter.Next();

			while (true == cIter.IsValid()) {
				SelectionItem * pcNextItem = cIter.GetItem();
				// Line을 선택한다. Line을 우선적으로 선택하기 위한 것임.
				// Line과 첫번째 Shell과 선택점과의 Z값을 비교한다. 값의 공차가 Proximity보다 작은 경우 Line을 선택한다.
				if (H3DF::Type::LineKey == pcNextItem->Type()) {
					WorldPoint cLineWordlPoint;
					WindowPoint cLineWindowPoint;
					pcNextItem->ShowSelectionPosition(cLineWordlPoint);
					pcNextItem->ShowSelectionPosition(cLineWindowPoint);

					TRACE(L"Face Line Distance: %f, %f\n", fabs(cFaceWindowPoint.z - cLineWindowPoint.z), cLineWordlPoint.DistanceWith(cFaceWordlPoint));

					// 첫번째에 Shell이 선택되고 다른 Item에서 Line이 공차내로 들어오면 Shell 대신 Line을 선택하고 끝낸다.
					if (0.001 > fabs(cFaceWindowPoint.z - cLineWindowPoint.z)) {
						if (2.0 > cLineWordlPoint.DistanceWith(cFaceWordlPoint)) {
							pcFrontItem = pcNextItem;
							break;
						}
					}
				}
				cIter.Next();
			}
		}
	}

	GetBaseView()->GetHighlightSelection()->DeSelectAll();

	H3DF::HighlightOptionsKit cOption;
	if (0 < cHighlightSelection.GetCount()) {
		if (H3DF::Type::LineKey == pcFrontItem->Type()) {
			float fLineWeight = 3.0;
			HC_KEY nHighlightSelectionKey = GetBaseView()->GetHighlightSelection()->GetSelectionSegment();
			HC_Open_Segment_By_Key(nHighlightSelectionKey); {
				HC_Set_Line_Weight(fLineWeight);
			} HC_Close_Segment();
		}
		else {
			float fLineWeight = 1.0;
			HC_KEY nHighlightSelectionKey = GetBaseView()->GetHighlightSelection()->GetSelectionSegment();
			HC_Open_Segment_By_Key(nHighlightSelectionKey); {
				HC_Set_Line_Weight(fLineWeight);
			} HC_Close_Segment();
		}

		cOutSelections.PushBack(new SelectionItem(*pcFrontItem));
		Highlight(*pcFrontItem, cOption);

		bNeedUpdate = true;
	}

	m_cOldHighlightSelection = cOutSelections;

	return true;
}
