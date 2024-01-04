#include "StdAfx.h"

#include "Highlight.h"

#include "Impl/SelectionImpl.h"
#include "Impl/HighlightImpl.h"

#include "Window.h"
#include "Impl/WindowImpl.h"
#include "../Impl/ViewImpl.h"

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
	m_pcImpl = new HighlightOptionsKitImpl();
}

H3DF::HighlightOptionsKit::HighlightOptionsKit(CStringA strInStyleName)
{
	HighlightOptionsKitImpl * pcImpl = new HighlightOptionsKitImpl();
	pcImpl->m_strInStyleName = strInStyleName;
	m_pcImpl = pcImpl;
}

H3DF::HighlightOptionsKit::HighlightOptionsKit(CStringA strInStyleName, CStringA strInSecondaryStyleName)
{
	HighlightOptionsKitImpl * pcImpl = new HighlightOptionsKitImpl();
	pcImpl->m_strInStyleName = strInStyleName;
	pcImpl->m_strInSecondaryStyleName = strInSecondaryStyleName;
	m_pcImpl = pcImpl;
}

H3DF::HighlightOptionsKit::HighlightOptionsKit(HighlightOptionsKit const & cInThat)
{
	m_pcImpl = new HighlightOptionsKitImpl();
	Set(cInThat);
}

void H3DF::HighlightOptionsKit::Set(HighlightOptionsKit const & cInThat)
{
	HighlightOptionsKitImpl * pcImpl = (HighlightOptionsKitImpl *)m_pcImpl;
	HighlightOptionsKitImpl * pcInThatImpl = (HighlightOptionsKitImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

HighlightOptionsKit & H3DF::HighlightOptionsKit::operator=(HighlightOptionsKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

HighlightOptionsKit & H3DF::HighlightOptionsKit::SetNotification(bool bInState)
{
	HighlightOptionsKitImpl * pcImpl = (HighlightOptionsKitImpl *)m_pcImpl;

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
	HighlightOptionsKitImpl * pcImpl = (HighlightOptionsKitImpl *)m_pcImpl;
	pcImpl->m_nNotification = -1;
	return *this;
}

bool H3DF::HighlightOptionsKit::ShowNotification(bool & bOutState) const
{
	HighlightOptionsKitImpl * pcImpl = (HighlightOptionsKitImpl *)m_pcImpl;
	if (0 > pcImpl->m_nNotification) {
		return false;
	}

	bOutState = (0 != pcImpl->m_nNotification);
	return true;
}

//== HighlightControl Class ========================================================================

H3DF::HighlightControl::HighlightControl(WindowKey const & cInWindow)
{
	HighlightControlImpl * pcImpl = new HighlightControlImpl(cInWindow);
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
	HighlightControlImpl * pcImpl = (HighlightControlImpl *)m_pcImpl;
	HighlightControlImpl * pcInThatImpl = (HighlightControlImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

HighlightControl & H3DF::HighlightControl::operator=(HighlightControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

//== Highlight 관련 함수 =============================================================================
HighlightControl & H3DF::HighlightControl::Highlight(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions, bool bInRemoveExisting)
{
	HighlightControlImpl * pcImpl = (HighlightControlImpl *)m_pcImpl;
	pcImpl->Highlight(cInItems, cInOptions, bInRemoveExisting);
	return *this;
}

HighlightControl & H3DF::HighlightControl::Highlight(SelectionItem const & cInItem, HighlightOptionsKit const & cInOptions, bool bInRemoveExisting)
{
	HighlightControlImpl * pcImpl = (HighlightControlImpl *)m_pcImpl;
	pcImpl->Highlight(cInItem, cInOptions, bInRemoveExisting);
	return *this;
}

HighlightControl & H3DF::HighlightControl::Unhighlight(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions)
{
	HighlightControlImpl * pcImpl = (HighlightControlImpl *) m_pcImpl;
	pcImpl->Unhighlight(cInItems, cInOptions);
	return *this;
}

HighlightControl & H3DF::HighlightControl::Unhighlight(SelectionItem const & cInItem, HighlightOptionsKit const & cInOptions)
{
	HighlightControlImpl * pcImpl = (HighlightControlImpl *) m_pcImpl;
	pcImpl->Unhighlight(cInItem, cInOptions);
	return *this;
}

HighlightControl & H3DF::HighlightControl::UnhighlightEverything()
{
	HighlightControlImpl * pcImpl = dynamic_cast<HighlightControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);
	pcImpl->SelectionSet()->DeSelectAll();
	return *this;
}

//== Material Mapping 관련 함수 ======================================================================
HighlightControl & H3DF::HighlightControl::SetMaterialMapping(MaterialMappingKit const & cInKit)
{
	HighlightControlImpl * pcHighlightControlImpl = (HighlightControlImpl *)m_pcImpl;
	HC_KEY nKey = pcHighlightControlImpl->SelectionSet()->GetHighlightStyle();
	
	SegmentKey cSegmentKey(nKey);
	cSegmentKey.SetMaterialMapping(cInKit);

	return *this;
}

MaterialMappingControl H3DF::HighlightControl::GetMaterialMappingControl()
{
	HighlightControlImpl * pcHighlightControlImpl = (HighlightControlImpl *)m_pcImpl;
	HC_KEY nKey = pcHighlightControlImpl->SelectionSet()->GetHighlightStyle();

	SegmentKey cSegmentKey(nKey);
	MaterialMappingControl cMaterialMappingControl(cSegmentKey);

	return cMaterialMappingControl;
}

MaterialMappingControl const H3DF::HighlightControl::GetMaterialMappingControl() const
{
	HighlightControlImpl * pcHighlightControlImpl = (HighlightControlImpl *)m_pcImpl;
	HC_KEY nKey = pcHighlightControlImpl->SelectionSet()->GetHighlightStyle();

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
	auto * pcImpl = dynamic_cast<HighlightControlImpl *>(m_pcImpl);
	HC_KEY nKey = pcImpl->GetBaseView()->GetHighlightSelection()->GetSelectionSegment();

	SegmentKey cSegmentKey(nKey);
	LineAttributeControl cControl(cSegmentKey);

	return cControl;
}

LineAttributeControl const H3DF::HighlightControl::GetLineAttributeControl() const
{
	auto * pcImpl = dynamic_cast<HighlightControlImpl *>(m_pcImpl);
	HC_KEY nKey = pcImpl->GetBaseView()->GetHighlightSelection()->GetSelectionSegment();

	SegmentKey cSegmentKey(nKey);
	LineAttributeControl cControl(cSegmentKey);

	return cControl;
}

HighlightControl & H3DF::HighlightControl::Highlight_ORG(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions, bool bInRemoveExisting)
{
	HighlightControlImpl * pcHighlightControlImpl = (HighlightControlImpl *)m_pcImpl;
	H3DF::BaseView * pcView = pcHighlightControlImpl->GetBaseView();

	char chType[MVO_BUFFER_SIZE];

	bool bNeedDeselect = true;
	bool bNeedUpdate = true;

	SelectionResultsImpl * pcImpl = (SelectionResultsImpl *) cInItems.GetImpl();

	for (auto pcItem : pcImpl->GetItems()) {
		SelectionItemImpl * pcImpl = (SelectionItemImpl *)pcItem->GetImpl();

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
