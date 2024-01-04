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

H3DF::HighlightControlImpl::HighlightControlImpl(WindowKey const & cInWindow)
{
	m_eType = H3DF::Type::HighlightControl;

	m_pcWindow = (WindowKey *) &cInWindow;

	// m_pcSelectionSet = ((HBaseView *)cInWindow.GetBaseView())->GetHighlightSelection();

	m_pcSelectionSet = new HSelectionSet((HBaseView *)cInWindow.GetBaseView());
	m_pcSelectionSet->Init();
	m_pcSelectionSet->UpdateHighlightStyle();
/*
	m_pcSelectionSet->SetSelectionLevel(HSelectEntity);
	
	m_pcSelectionSet->SetGrayScale(false);// ThePreset.GrayScaleSelection);
	m_pcSelectionSet->SetUseDefinedHighlight(false);// ThePreset.UseDefinedHighlighting);
	m_pcSelectionSet->SetAllowDisplacement(false);// ThePreset.DisplaceSelection);

	HPixelRGBA cHighlightSelectColor;
	cHighlightSelectColor.Set(255, 0, 0);
	m_pcSelectionSet->SetSelectionEdgeWeight(1.0);

	m_pcSelectionSet->SetSelectionFaceColor(cHighlightSelectColor);
	m_pcSelectionSet->SetSelectionEdgeColor(cHighlightSelectColor);
	m_pcSelectionSet->SetSelectionMarkerColor(cHighlightSelectColor);

	m_pcSelectionSet->SetHighlightMode(HighlightQuickmoves);

	m_pcSelectionSet->UpdateHighlightStyle();*/
}

BaseView * H3DF::HighlightControlImpl::GetBaseView()
{
	return (BaseView *)m_pcWindow->GetBaseView();
}

BaseView * H3DF::HighlightControlImpl::GetBaseView() const
{
	DEBUG_VALID(m_pcWindow);
	return (BaseView *) m_pcWindow->GetBaseView();
}

HSelectionSet * H3DF::HighlightControlImpl::SelectionSet()
{
	return GetBaseView()->GetHighlightSelection();

	if (nullptr == m_pcSelectionSet) {
		return GetBaseView()->GetHighlightSelection();
	}

	return m_pcSelectionSet;
}

void H3DF::HighlightControlImpl::Highlight(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions, bool bInRemoveExisting)
{
	HSelectionSet * pcSelSet = SelectionSet();

	char chType[MVO_BUFFER_SIZE];

	bool bNeedDeselect = true;
	bool bNeedUpdate = true;

	if (true == bInRemoveExisting) {
		pcSelSet->DeSelectAll();
	}

	SelectionResultsImpl * pcSelectionResultsImpl = (SelectionResultsImpl *) cInItems.GetImpl();

	for (auto pcItem : pcSelectionResultsImpl->GetItems()) {
		SelectionItemImpl * pcImpl = (SelectionItemImpl *) pcItem->GetImpl();

		HC_KEY nKey = pcImpl->cKey.KeyValue();

		// Region 선택 관련 처리 부분
		if (H3DF::Type::ShellKey == pcItem->Type() && (pcImpl->nLowest != pcImpl->nHighest || pcImpl->nLowest > 0)) {
			bNeedDeselect = false;

			if (!pcSelSet->IsRegionSelected(nKey, pcImpl->nIncludeCount, pcImpl->pnIncludeKeys, pcImpl->nRegion))
			{
				if (true == bInRemoveExisting) {
					pcSelSet->DeSelectAll();
				}

				pcSelSet->SelectRegion(nKey, pcImpl->nIncludeCount, pcImpl->pnIncludeKeys, pcImpl->nRegion, false);
			}
			else {
				bNeedUpdate = false;
			}
		}
		else {
			bNeedDeselect = false;

			if (!pcSelSet->IsSelected(nKey, pcImpl->nIncludeCount, pcImpl->pnIncludeKeys)) {
				HSelectLevel eSelectLevel = pcSelSet->GetSelectionLevel();

				if (pcSelSet->GetSelectionLevel() != HSelectSegment) // never should fail for dynamic highlighting, but let's be nice and check
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
						if (pcSelSet->IsHighlightSegment(segkey))
						{
							segkey = HC_KShow_Owner_Original_Key(segkey);
							HC_Show_Owner_By_Key(segkey, segname);
						}
					}
				}

				pcSelSet->Select(nKey, pcImpl->nIncludeCount, pcImpl->pnIncludeKeys, false);
			}
			else {
				bNeedUpdate = false;
			}
		}
	}

	if (bNeedDeselect) {
		pcSelSet->DeSelectAll();
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
	HSelectionSet * pcSelection = SelectionSet(); // HSelectionSet에서 Select 및 Highlight를 다 처리함.

	SelectionItemImpl * pcImpl = (SelectionItemImpl *) cInItem.GetImpl();

	HC_KEY nPrimitiveKey = pcImpl->cKey.KeyValue();
	pcSelection->Select(nPrimitiveKey, pcImpl->nIncludeCount, pcImpl->pnIncludeKeys, false);
}

void H3DF::HighlightControlImpl::Unhighlight(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions)
{
	if (0 == cInItems.GetCount()) {
		return;
	}

	HSelectionSet * pcSelection = SelectionSet(); // HSelectionSet에서 Select 및 Highlight를 다 처리함.

	SelectionResultsImpl * pcImpl = (SelectionResultsImpl *) cInItems.GetImpl();

	// cInItem를 순회하면서 Unhighlight를 수행한다.
	for (auto pcItem : pcImpl->GetItems()) {
		SelectionItemImpl * pcItemImpl = (SelectionItemImpl *) pcItem->GetImpl();
		HC_KEY nKey = pcItemImpl->cKey.KeyValue();
		pcSelection->DeSelect(nKey, pcItemImpl->nIncludeCount, pcItemImpl->pnIncludeKeys, false);
	}

	bool bShowNotification = false;
	cInOptions.ShowNotification(bShowNotification);

	if (true == bShowNotification) {
		GetBaseView()->ForceUpdate();
	}
}

void H3DF::HighlightControlImpl::Unhighlight(SelectionItem const & cInItem, HighlightOptionsKit const & cInOptions)
{
	// cInItem의 Impl을 가져와서 작업을 수행한다.
	SelectionItemImpl * pcSelectionItemImpl = (SelectionItemImpl *) cInItem.GetImpl();
	if (nullptr == pcSelectionItemImpl) {
		return;
	}

	HC_KEY nKey = pcSelectionItemImpl->cKey.KeyValue();
	SelectionSet()->DeSelect(nKey, pcSelectionItemImpl->nIncludeCount, pcSelectionItemImpl->pnIncludeKeys, false);

	bool bShowNotification = false;
	cInOptions.ShowNotification(bShowNotification);

	if (true == bShowNotification) {
		GetBaseView()->ForceUpdate();
	}
}