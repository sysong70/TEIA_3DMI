#include "StdAfx.h"

#include "Highlight.h"

#include "Private/SelectionPrivate.h"
#include "Private/HighlightPrivate.h"

#include "Window.h"
#include "Private/WindowPrivate.h"
#include "../Private/View.Private.h"

#include "Line.h"
#include "LineAttribute.h"

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

using namespace H3DF;

//== HighlightOptionsKit Class =====================================================================

H3DF::HighlightOptionsKit::HighlightOptionsKit()
{
	m_pcImpl = new HighlightOptionsKitPrivate();
}

H3DF::HighlightOptionsKit::HighlightOptionsKit(char const * chInStyleName)
{
	HighlightOptionsKitPrivate * pcImpl = new HighlightOptionsKitPrivate();
	strncpy(pcImpl->m_chInStyleName, chInStyleName, STYLE_BUFFER_SIZE);
	m_pcImpl = pcImpl;
}

H3DF::HighlightOptionsKit::HighlightOptionsKit(char const * chInStyleName, char const * chInSecondaryStyleName)
{
	HighlightOptionsKitPrivate * pcImpl = new HighlightOptionsKitPrivate();
	strncpy(pcImpl->m_chInStyleName, chInStyleName, STYLE_BUFFER_SIZE);
	strncpy(pcImpl->m_chInSecondaryStyleName, chInSecondaryStyleName, STYLE_BUFFER_SIZE);
	m_pcImpl = pcImpl;
}

H3DF::HighlightOptionsKit::HighlightOptionsKit(HighlightOptionsKit const & cInThat)
{
	m_pcImpl = new HighlightOptionsKitPrivate();
	Set(cInThat);
}

void H3DF::HighlightOptionsKit::Set(HighlightOptionsKit const & cInThat)
{
	HighlightOptionsKitPrivate * pcImpl = (HighlightOptionsKitPrivate *)m_pcImpl;
	HighlightOptionsKitPrivate * pcInThatImpl = (HighlightOptionsKitPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

HighlightOptionsKit & H3DF::HighlightOptionsKit::operator=(HighlightOptionsKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

HighlightOptionsKit & H3DF::HighlightOptionsKit::SetNotification(bool bInState)
{
	HighlightOptionsKitPrivate * pcImpl = (HighlightOptionsKitPrivate *)m_pcImpl;

	if (true == bInState) {
		pcImpl->m_nNotification = true;
	}
	else {
		pcImpl->m_nNotification = false;
	}

	return *this;
}

HighlightOptionsKit & H3DF::HighlightOptionsKit::UnsetNotification()
{
	HighlightOptionsKitPrivate * pcImpl = (HighlightOptionsKitPrivate *)m_pcImpl;
	pcImpl->m_nNotification = -1;
	return *this;
}

bool H3DF::HighlightOptionsKit::ShowNotification(bool & bOutState) const
{
	HighlightOptionsKitPrivate * pcImpl = (HighlightOptionsKitPrivate *)m_pcImpl;
	if (0 > pcImpl->m_nNotification) {
		return false;
	}

	bOutState = (0 != pcImpl->m_nNotification);
	return true;
}

//== HighlightControl Class ========================================================================

H3DF::HighlightControl::HighlightControl(WindowKey const & cInWindow)
{
	HighlightControlPrivate * pcImpl = new HighlightControlPrivate(cInWindow);
	m_pcImpl = pcImpl;
}

/*
H3DF::HighlightControl::HighlightControl(HighlightControl const & cInThat)
{
	m_pcImpl = new HighlightControlPrivate();
	Set(cInThat);
}
*/

H3DF::HighlightControl::HighlightControl() {}

void H3DF::HighlightControl::Set(HighlightControl const & cInThat)
{
	HighlightControlPrivate * pcImpl = (HighlightControlPrivate *)m_pcImpl;
	HighlightControlPrivate * pcInThatImpl = (HighlightControlPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

HighlightControl & H3DF::HighlightControl::operator=(HighlightControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

//== Mouse Event 처리 ===============================================================================
int H3DF::HighlightControl::NoButtonDownAndMove(int nFlags, int x, int y, SelectionResults & cOutSelections)
{
	HighlightControlPrivate * pcHighlightControlImpl = (HighlightControlPrivate *) m_pcImpl;
	DEBUG_VALID(pcHighlightControlImpl);

	return pcHighlightControlImpl->NoButtonDownAndMove(nFlags, x, y, cOutSelections);
}

//== Highlight 관련 함수 =============================================================================
HighlightControl & H3DF::HighlightControl::Highlight(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions, bool bInRemoveExisting)
{
	HighlightControlPrivate * pcImpl = (HighlightControlPrivate *)m_pcImpl;
	pcImpl->Highlight(cInItems, cInOptions, bInRemoveExisting);
	return *this;
}

HighlightControl & H3DF::HighlightControl::Highlight(SelectionItem const & cInItem, HighlightOptionsKit const & cInOptions, bool bInRemoveExisting)
{
	HighlightControlPrivate * pcImpl = (HighlightControlPrivate *)m_pcImpl;
	pcImpl->Highlight(cInItem, cInOptions, bInRemoveExisting);
	return *this;
}

HighlightControl & H3DF::HighlightControl::Unhighlight(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions)
{
	HighlightControlPrivate * pcImpl = (HighlightControlPrivate *) m_pcImpl;
	pcImpl->Unhighlight(cInItems, cInOptions);
	return *this;
}

HighlightControl & H3DF::HighlightControl::Unhighlight(SelectionItem const & cInItem, HighlightOptionsKit const & cInOptions)
{
	HighlightControlPrivate * pcImpl = (HighlightControlPrivate *) m_pcImpl;
	pcImpl->Unhighlight(cInItem, cInOptions);
	return *this;
}

//== Material Mapping 관련 함수 ======================================================================
HighlightControl & H3DF::HighlightControl::SetMaterialMapping(MaterialMappingKit const & cInKit)
{
	HighlightControlPrivate * pcHighlightControlImpl = (HighlightControlPrivate *)m_pcImpl;
	HC_KEY nKey = pcHighlightControlImpl->GetHighlightSelection()->GetHighlightStyle();
	
	SegmentKey cSegmentKey(nKey);
	cSegmentKey.SetMaterialMapping(cInKit);

	return *this;
}

MaterialMappingControl H3DF::HighlightControl::GetMaterialMappingControl()
{
	HighlightControlPrivate * pcHighlightControlImpl = (HighlightControlPrivate *)m_pcImpl;
	HC_KEY nKey = pcHighlightControlImpl->GetHighlightSelection()->GetHighlightStyle();

	SegmentKey cSegmentKey(nKey);
	MaterialMappingControl cMaterialMappingControl(cSegmentKey);

	return cMaterialMappingControl;
}

MaterialMappingControl const H3DF::HighlightControl::GetMaterialMappingControl() const
{
	HighlightControlPrivate * pcHighlightControlImpl = (HighlightControlPrivate *)m_pcImpl;
	HC_KEY nKey = pcHighlightControlImpl->GetHighlightSelection()->GetHighlightStyle();

	SegmentKey cSegmentKey(nKey);
	MaterialMappingControl cMaterialMappingControl(cSegmentKey);

	return cMaterialMappingControl;
}

//== Line Attribute 관련 함수 ====================================================================
/*
HighlightControl & H3DF::HighlightControl::SetLineAttribute(LineAttributeKit const & cInKit)
{
	HighlightControlPrivate * pcHighlightControlImpl = (HighlightControlPrivate *)m_pcImpl;
	HC_KEY nKey = pcHighlightControlImpl->m_pcSelectionSet->GetHighlightStyle();

	SegmentKey cSegmentKey(nKey);
	cSegmentKey.SetMaterialMapping(cInKit);

	return *this;
}
*/

LineAttributeControl H3DF::HighlightControl::GetLineAttributeControl()
{
	HighlightControlPrivate * pcHighlightControlImpl = (HighlightControlPrivate *)m_pcImpl;
	HC_KEY nKey = pcHighlightControlImpl->GetHighlightSelection()->GetHighlightStyle();

	SegmentKey cSegmentKey(nKey);
	LineAttributeControl cControl(cSegmentKey);

	return cControl;
}

LineAttributeControl const H3DF::HighlightControl::GetLineAttributeControl() const
{
	HighlightControlPrivate * pcHighlightControlImpl = (HighlightControlPrivate *)m_pcImpl;
	HC_KEY nKey = pcHighlightControlImpl->GetHighlightSelection()->GetHighlightStyle();

	SegmentKey cSegmentKey(nKey);
	LineAttributeControl cControl(cSegmentKey);

	return cControl;
}

HighlightControl & H3DF::HighlightControl::Highlight_ORG(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions, bool bInRemoveExisting)
{
	HighlightControlPrivate * pcHighlightControlImpl = (HighlightControlPrivate *)m_pcImpl;
	H3DF::BaseView * pcView = pcHighlightControlImpl->GetBaseView();

	char chType[MVO_BUFFER_SIZE];

	bool bNeedDeselect = true;
	bool bNeedUpdate = true;

	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *) cInItems.GetImpl();

	for (auto pcItem : pcImpl->GetItems()) {
		SelectionItemPrivate * pcImpl = (SelectionItemPrivate *)pcItem->GetImpl();

		HC_KEY nKey = pcImpl->cKey.KeyValue();

		// Region 선택 관련 처리 부분
		if (H3DF::Type::ShellKey == pcItem->Type() && (pcImpl->nLowest != pcImpl->nHighest || pcImpl->nLowest > 0)) {
			bNeedDeselect = false;

			if (!pcView->GetHighlightSelection()->IsRegionSelected(nKey, pcImpl->nIncludeCount, pcImpl->pnIncludeKeys, pcImpl->nRegion))
			{
				if (true == bInRemoveExisting) {
					pcView->GetHighlightSelection()->DeSelectAll();
				}

				pcView->GetHighlightSelection()->SelectRegion(nKey, pcImpl->nIncludeCount, pcImpl->pnIncludeKeys, pcImpl->nRegion, false);
			}
			else {
				bNeedUpdate = false;
			}
		}
		else {
			bNeedDeselect = false;

			if (!pcView->GetHighlightSelection()->IsSelected(nKey, pcImpl->nIncludeCount, pcImpl->pnIncludeKeys)) {
				if (pcView->GetHighlightSelection()->GetSelectionLevel() != HSelectSegment) // never should fail for dynamic highlighting, but let's be nice and check
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
						if (pcView->GetHighlightSelection()->IsHighlightSegment(segkey))
						{
							segkey = HC_KShow_Owner_Original_Key(segkey);
							HC_Show_Owner_By_Key(segkey, segname);
						}
					}
				}

				if (true == bInRemoveExisting) {
					pcView->GetHighlightSelection()->DeSelectAll();
				}
				
				pcView->GetHighlightSelection()->Select(nKey, pcImpl->nIncludeCount, pcImpl->pnIncludeKeys, false);
			}
			else {
				bNeedUpdate = false;
			}
		}
	}

	if (bNeedDeselect) {
		pcView->GetHighlightSelection()->DeSelectAll();
	}

	if (bNeedUpdate) {
		bool bShowNotification = false;
		cInOptions.ShowNotification(bShowNotification);

		if (true == bShowNotification) {
			pcView->ForceUpdate();
		}
	}

	return *this;
}
