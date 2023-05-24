#include "StdAfx.h"

#include "3DF.Highlight.h"

#include "Private/3DF.SelectionPrivate.h"

#include "3DF.Window.h"
#include "3DF.BaseView.h"

#include "3DF.Line.h"

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

//== HighlightOptionsKitPrivate Class =====================================================================
class HighlightOptionsKitPrivate : public PrivateImpl
{
public:
	void Copy(HighlightOptionsKitPrivate * pcInThat) {
		strncpy(m_chInStyleName, pcInThat->m_chInStyleName, STYLE_BUFFER_SIZE);
		strncpy(m_chInSecondaryStyleName, pcInThat->m_chInSecondaryStyleName, STYLE_BUFFER_SIZE);
		m_nNotification = pcInThat->m_nNotification;
	}

	char m_chInStyleName[STYLE_BUFFER_SIZE];
	char m_chInSecondaryStyleName[STYLE_BUFFER_SIZE];
	int m_nNotification = -1;
};

TDF::HighlightOptionsKit::HighlightOptionsKit()
{
	m_pcImpl = new HighlightOptionsKitPrivate();
}

TDF::HighlightOptionsKit::HighlightOptionsKit(char const * chInStyleName)
{
	HighlightOptionsKitPrivate * pcImpl = new HighlightOptionsKitPrivate();
	strncpy(pcImpl->m_chInStyleName, chInStyleName, STYLE_BUFFER_SIZE);
	m_pcImpl = pcImpl;
}

TDF::HighlightOptionsKit::HighlightOptionsKit(char const * chInStyleName, char const * chInSecondaryStyleName)
{
	HighlightOptionsKitPrivate * pcImpl = new HighlightOptionsKitPrivate();
	strncpy(pcImpl->m_chInStyleName, chInStyleName, STYLE_BUFFER_SIZE);
	strncpy(pcImpl->m_chInSecondaryStyleName, chInSecondaryStyleName, STYLE_BUFFER_SIZE);
	m_pcImpl = pcImpl;
}

TDF::HighlightOptionsKit::HighlightOptionsKit(HighlightOptionsKit const & cInThat)
{
	m_pcImpl = new HighlightOptionsKitPrivate();
	Set(cInThat);
}

void TDF::HighlightOptionsKit::Set(HighlightOptionsKit const & cInThat)
{
	HighlightOptionsKitPrivate * pcImpl = (HighlightOptionsKitPrivate *)m_pcImpl;
	HighlightOptionsKitPrivate * pcInThatImpl = (HighlightOptionsKitPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

HighlightOptionsKit & TDF::HighlightOptionsKit::operator=(HighlightOptionsKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

HighlightOptionsKit & HighlightOptionsKit::SetNotification(bool bInState)
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

HighlightOptionsKit & HighlightOptionsKit::UnsetNotification()
{
	HighlightOptionsKitPrivate * pcImpl = (HighlightOptionsKitPrivate *)m_pcImpl;
	pcImpl->m_nNotification = -1;
	return *this;
}

bool HighlightOptionsKit::ShowNotification(bool & bOutState) const
{
	HighlightOptionsKitPrivate * pcImpl = (HighlightOptionsKitPrivate *)m_pcImpl;
	if (0 > pcImpl->m_nNotification) {
		return false;
	}

	bOutState = (0 != pcImpl->m_nNotification);
	return true;
}

//== HighlightControl Class ========================================================================

class HighlightControlPrivate : public PrivateImpl
{
public:
	HighlightControlPrivate() { m_eType = TDF::Type::HighlightControl; }

	void Copy(HighlightControlPrivate * pcInThat) {
		m_pcBaseView = pcInThat->m_pcBaseView;
		m_pcWindow = pcInThat->m_pcWindow;
	}

// 	int SelectButtonDown(Point const & cInLocation, UINT const nFlags, SelectionResults & cOutResults);
// 	void HandleSelection(UINT const nFlags, SelectionResults & cOutResults);

	WindowKey * GetWindow() { return (WindowKey *)m_pcWindow; }
	const WindowKey * m_pcWindow = nullptr;

	TDF::BaseView * GetBaseView() { return (TDF::BaseView *)m_pcBaseView; }
	const TDF::BaseView * m_pcBaseView = nullptr;
};

TDF::HighlightControl::HighlightControl(WindowKey const & cInWindow)
{
	HighlightControlPrivate * pcImpl = new HighlightControlPrivate();
	pcImpl->m_pcWindow = &cInWindow;
	pcImpl->m_pcBaseView = cInWindow.GetBaseView();

	m_pcImpl = pcImpl;
}

TDF::HighlightControl::HighlightControl(HighlightControl const & cInThat)
{
	m_pcImpl = new HighlightControlPrivate();
	Set(cInThat);
}

TDF::HighlightControl::HighlightControl() {}

TDF::HighlightControl::~HighlightControl()
{

}

void TDF::HighlightControl::Set(HighlightControl const & cInThat)
{
	HighlightControlPrivate * pcImpl = (HighlightControlPrivate *)m_pcImpl;
	HighlightControlPrivate * pcInThatImpl = (HighlightControlPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

HighlightControl & TDF::HighlightControl::operator=(HighlightControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

HighlightControl & TDF::HighlightControl::Highlight(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions, bool bInRemoveExisting)
{
	HighlightControlPrivate * pcHighlightControlImpl = (HighlightControlPrivate *)m_pcImpl;
	TDF::BaseView * pcView = pcHighlightControlImpl->GetBaseView();

	char chType[MVO_BUFFER_SIZE];

	bool bNeedDeselect = true;
	bool bNeedUpdate = true;

	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *) cInItems.GetImpl();

	for (auto pcItem : pcImpl->GetItems()) {
		SelectionItemPrivate * pcImpl = (SelectionItemPrivate *)pcItem->GetImpl();

		HC_KEY nKey = pcImpl->cKey.KeyValue();

		// Region 선택 관련 처리 부분
		if (TDF::Type::ShellKey == pcItem->Type() && (pcImpl->nLowest != pcImpl->nHighest || pcImpl->nLowest > 0)) {
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

HighlightControl & TDF::HighlightControl::Unhighlight(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions)
{
	if (0 == cInItems.GetCount()) {
		return *this;
	}

	HighlightControlPrivate * pcHighlightControlImpl = (HighlightControlPrivate *)m_pcImpl;
	HBaseView * pcView = pcHighlightControlImpl->GetBaseView();

	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)cInItems.GetImpl();
	
	// cInItem를 순회하면서 Unhighlight를 수행한다.
	for (auto pcItem : pcImpl->GetItems()) {
		SelectionItemPrivate * pcItemImpl = (SelectionItemPrivate *)pcItem->GetImpl();
		HC_KEY nKey = pcItemImpl->cKey.KeyValue();
		pcView->GetHighlightSelection()->DeSelect(nKey, pcItemImpl->nIncludeCount, pcItemImpl->pnIncludeKeys, false);
	}
	
	bool bShowNotification = false;
	cInOptions.ShowNotification(bShowNotification);

	if (true == bShowNotification) {
		pcView->ForceUpdate();
	}

	return *this;
}

HighlightControl & TDF::HighlightControl::Unhighlight(SelectionItem const & cInItem, HighlightOptionsKit const & cInOptions)
{
	// cInItem의 Impl을 가져와서 작업을 수행한다.
	SelectionItemPrivate * pcImpl = (SelectionItemPrivate *)cInItem.GetImpl();
	if (nullptr == pcImpl) {
		return *this;
	}
	
	HighlightControlPrivate * pcHighlightControlImpl = (HighlightControlPrivate *)m_pcImpl;
	HBaseView * pcView = pcHighlightControlImpl->GetBaseView();

	HC_KEY nKey = pcImpl->cKey.KeyValue();
	pcView->GetHighlightSelection()->DeSelect(nKey, pcImpl->nIncludeCount, pcImpl->pnIncludeKeys, false);

	bool bShowNotification = false;
	cInOptions.ShowNotification(bShowNotification);

	if (true == bShowNotification) {
		pcView->ForceUpdate();
	}

	return *this;
}
