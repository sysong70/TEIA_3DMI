#include "StdAfx.h"

#include "3DF.Highlight.h"

#include "Private/3DF.SelectionPrivate.h"

#include "3DF.Window.h"

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
	}

	char m_chInStyleName[STYLE_BUFFER_SIZE];
	char m_chInSecondaryStyleName[STYLE_BUFFER_SIZE];
};

_3DF::HighlightOptionsKit::HighlightOptionsKit()
{
	m_pcImpl = new HighlightOptionsKitPrivate();
}

_3DF::HighlightOptionsKit::HighlightOptionsKit(char const * chInStyleName)
{
	HighlightOptionsKitPrivate * pcImpl = new HighlightOptionsKitPrivate();
	strncpy(pcImpl->m_chInStyleName, chInStyleName, STYLE_BUFFER_SIZE);
	m_pcImpl = pcImpl;
}

_3DF::HighlightOptionsKit::HighlightOptionsKit(char const * chInStyleName, char const * chInSecondaryStyleName)
{
	HighlightOptionsKitPrivate * pcImpl = new HighlightOptionsKitPrivate();
	strncpy(pcImpl->m_chInStyleName, chInStyleName, STYLE_BUFFER_SIZE);
	strncpy(pcImpl->m_chInSecondaryStyleName, chInSecondaryStyleName, STYLE_BUFFER_SIZE);
	m_pcImpl = pcImpl;
}

_3DF::HighlightOptionsKit::HighlightOptionsKit(HighlightOptionsKit const & cInThat)
{
	m_pcImpl = new HighlightOptionsKitPrivate();
	Set(cInThat);
}

_3DF::HighlightOptionsKit::~HighlightOptionsKit()
{

}

void _3DF::HighlightOptionsKit::Set(HighlightOptionsKit const & cInThat)
{
	HighlightOptionsKitPrivate * pcImpl = (HighlightOptionsKitPrivate *)m_pcImpl;
	HighlightOptionsKitPrivate * pcInThatImpl = (HighlightOptionsKitPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

HighlightOptionsKit & _3DF::HighlightOptionsKit::operator=(HighlightOptionsKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

//== HighlightControl Class ========================================================================

class HighlightControlPrivate : public PrivateImpl
{
public:
	_3DF::Type Type() const override { return _3DF::Type::SelectionControl; }

	void Copy(HighlightControlPrivate * pcInThat) {
		m_pcBaseView = pcInThat->m_pcBaseView;
		m_pcWindow = pcInThat->m_pcWindow;
	}

// 	int SelectButtonDown(Point const & cInLocation, UINT const nFlags, SelectionResults & cOutResults);
// 	void HandleSelection(UINT const nFlags, SelectionResults & cOutResults);

	WindowKey * GetWindow() { return (WindowKey *)m_pcWindow; }
	const WindowKey * m_pcWindow = nullptr;

	HBaseView * GetBaseView() { return (HBaseView *)m_pcBaseView; }
	const HBaseView * m_pcBaseView = nullptr;
};

_3DF::HighlightControl::HighlightControl(WindowKey const & cInWindow)
{
	HighlightControlPrivate * pcImpl = new HighlightControlPrivate();
	pcImpl->m_pcWindow = &cInWindow;
	pcImpl->m_pcBaseView = cInWindow.GetBaseView();

	m_pcImpl = pcImpl;
}

_3DF::HighlightControl::HighlightControl(HighlightControl const & cInThat)
{
	m_pcImpl = new HighlightControlPrivate();
	Set(cInThat);
}

_3DF::HighlightControl::HighlightControl() {}

_3DF::HighlightControl::~HighlightControl()
{

}

void _3DF::HighlightControl::Set(HighlightControl const & cInThat)
{
	HighlightControlPrivate * pcImpl = (HighlightControlPrivate *)m_pcImpl;
	HighlightControlPrivate * pcInThatImpl = (HighlightControlPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

HighlightControl & _3DF::HighlightControl::operator=(HighlightControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

void _3DF::HighlightControl::DynamicHighlight(Point const & cInLocation)
{
	HighlightControlPrivate * pcImpl = (HighlightControlPrivate *)m_pcImpl;
	HBaseView * pcView = pcImpl->GetBaseView();

	int res, offset1, offset2, offset3;
	char pathname[MVO_SEGMENT_PATHNAME_BUFFER], type[MVO_BUFFER_SIZE];
	HC_KEY nKey;

	if (!pcView->GetDynamicHighlighting()
		|| pcView->GetSuppressUpdateTick()
		|| pcView->GetSuppressUpdate()
		|| !pcView->GetModel()->GetFileLoadComplete())
		return;

	HC_Open_Segment_By_Key(pcView->GetViewKey());
	res = HC_Compute_Selection(".", "./scene/overwrite", "v, selection level = entity", cInLocation.x, cInLocation.y);
	HC_Close_Segment();

	// compute the selection using the HOOPS window coordinate of the pick location
	bool bNeedDeselect = true;
	bool bNeedUpdate = true;

	if (res)
	{
		HC_Show_Selection_Element(&nKey, &offset1, &offset2, &offset3);
		HC_Show_Selection_Pathname(pathname);

		int incl_count;
		int skey_count;
		char skey_type[MVO_BUFFER_SIZE];

		HC_Show_Selection_Keys_Count(&skey_count);

		HC_KEY * keys = new HC_KEY[skey_count];
		HC_KEY * incl_keys = new HC_KEY[skey_count];
		HC_Show_Selection_Keys(&skey_count, keys);

		incl_count = 0;
		for (int i = skey_count - 1; i >= 0; i--)
		{
			HC_Show_Key_Type(keys[i], skey_type);
			if (strstr(skey_type, "include"))
			{
				incl_keys[incl_count] = keys[i];
				incl_count++;
			}
			else if (streq(skey_type, "reference")) {
				nKey = keys[i];
				break;
			}
		}

		// Get the type of the selected
		HC_Show_Key_Type(nKey, type);

		//if we have a shell with visible faces, we may need to select regions
		if (streq(type, "shell") && offset3 != -1)
		{
			int region;
			int lowest = 0;
			int highest = 0;

			if (pcView->GetHighlightSelection()->GetAllowRegionSelection()) {
				HC_Show_Region_Range(nKey, &lowest, &highest);

				if (lowest != highest || lowest > 0)
				{
					HC_Open_Geometry(nKey); {
						HC_Open_Face(offset3); {
							HC_Show_Region(&region);
						}HC_Close_Face();
					}HC_Close_Geometry();

					bNeedDeselect = false;

					if (!pcView->GetHighlightSelection()->IsRegionSelected(nKey, incl_count, incl_keys, region))
					{
						pcView->GetHighlightSelection()->DeSelectAll();
						pcView->GetHighlightSelection()->SelectRegion(nKey, incl_count, incl_keys, region, false);
					}
					else {
						bNeedUpdate = false;
					}

					goto DONE;
				}
			}

			//NON-REGION SELECT FALLS THROUGH
		}

		bNeedDeselect = false;

		if (!pcView->GetHighlightSelection()->IsSelected(nKey, incl_count, incl_keys)) {
			if (pcView->GetHighlightSelection()->GetSelectionLevel() != HSelectSegment) // never should fail for dynamic highlighting, but let's be nice and check
			{
				// the key is to a geometric entity.  If we are in segment selection mode,
				// then we need to get the key to its parent segment.

				HC_Show_Key_Type(nKey, type);

				if (!streq("segment", type))
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

			pcView->GetHighlightSelection()->DeSelectAll();
			pcView->GetHighlightSelection()->Select(nKey, incl_count, incl_keys, false);
		}
		else {
			bNeedUpdate = false;
		}

	DONE:
		delete[] keys;
		delete[] incl_keys;
	}

	if (bNeedDeselect) {
		pcView->GetHighlightSelection()->DeSelectAll();
	}

	if (bNeedUpdate) {
		pcView->ForceUpdate();
	}
}

HighlightControl & _3DF::HighlightControl::Highlight(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions, bool bInRemoveExisting)
{
	HighlightControlPrivate * pcHighlightControlImpl = (HighlightControlPrivate *)m_pcImpl;
	HBaseView * pcView = pcHighlightControlImpl->GetBaseView();

	char chType[MVO_BUFFER_SIZE];

	bool bNeedDeselect = true;
	bool bNeedUpdate = true;

	for (POSITION pcPosition = cInItems.GetHeadPosition(); nullptr != pcPosition; ) {
		SelectionItem * pcItem = cInItems.GetNext(pcPosition);
		SelectionItemPrivate * pcImpl = (SelectionItemPrivate *)pcItem->GetImpl();

		HC_KEY nKey = pcImpl->pcKey->KeyValue();

		if (_3DF::Type::ShellKey == pcItem->Type() && (pcImpl->nLowest != pcImpl->nHighest || pcImpl->nLowest > 0)) {
			bNeedDeselect = false;

			if (!pcView->GetHighlightSelection()->IsRegionSelected(nKey, pcImpl->nIncludeCount, pcImpl->pnIncludeKeys, pcImpl->nRegion))
			{
				pcView->GetHighlightSelection()->DeSelectAll();
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

				pcView->GetHighlightSelection()->DeSelectAll();
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
		pcView->ForceUpdate();
	}

	return *this;
}

HighlightControl & HighlightControl::Unhighlight(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions)
{
	if (0 == cInItems.GetCount()) {
		return *this;
	}

	HighlightControlPrivate * pcHighlightControlImpl = (HighlightControlPrivate *)m_pcImpl;
	HBaseView * pcView = pcHighlightControlImpl->GetBaseView();

	// cInItem를 순회하면서 Unhighlight를 수행한다.
	for (POSITION pcPosition = cInItems.GetHeadPosition(); nullptr != pcPosition; ) {
		SelectionItem * pcItem = cInItems.GetNext(pcPosition);
		SelectionItemPrivate * pcImpl = (SelectionItemPrivate *)pcItem->GetImpl();

		HC_KEY nKey = pcImpl->pcKey->KeyValue();
		pcView->GetHighlightSelection()->DeSelect(nKey, pcImpl->nIncludeCount, pcImpl->pnIncludeKeys, false);
	}

	pcView->ForceUpdate();

	return *this;
}

HighlightControl & HighlightControl::Unhighlight(SelectionItem const & cInItem, HighlightOptionsKit const & cInOptions)
{
	// cInItem의 Impl을 가져와서 작업을 수행한다.
	SelectionItemPrivate * pcImpl = (SelectionItemPrivate *)cInItem.GetImpl();
	if (nullptr == pcImpl) {
		return *this;
	}
	
	HighlightControlPrivate * pcHighlightControlImpl = (HighlightControlPrivate *)m_pcImpl;
	HBaseView * pcView = pcHighlightControlImpl->GetBaseView();

	HC_KEY nKey = pcImpl->pcKey->KeyValue();
	pcView->GetHighlightSelection()->DeSelect(nKey, pcImpl->nIncludeCount, pcImpl->pnIncludeKeys, false);

	pcView->ForceUpdate();

	return *this;
}
