#include "StdAfx.h"

#include "3DF.Selection.h"

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

//== SelectionOptionsKit Class =====================================================================
class SelectionOptionsKitPrivate : public PrivateImpl
{
public:
	void Copy(SelectionOptionsKitPrivate * pcInThat) {
		fProximity = pcInThat->fProximity;
		eLevel = pcInThat->eLevel;
		nInternalLimit = pcInThat->nInternalLimit;
		nRelatedLimit = pcInThat->nRelatedLimit;
		eSorting = pcInThat->eSorting;
		eAlgorithm = pcInThat->eAlgorithm;
		eGranularity = pcInThat->eGranularity;
	}

	float fProximity = 0.0f;
	Selection::Level eLevel = Selection::Level::Entity;
	size_t nInternalLimit = 1;
	size_t nRelatedLimit = 5;
	Selection::Sorting eSorting = Selection::Sorting::Default;
	Selection::Algorithm eAlgorithm = Selection::Algorithm::Analytic;
	Selection::Granularity eGranularity = Selection::Granularity::General;
	Selection::Bias eBias = Selection::Bias::None;
};


SelectionOptionsKit::SelectionOptionsKit()
{
	SelectionOptionsKitPrivate * pcImpl = new SelectionOptionsKitPrivate();
}

SelectionOptionsKit::SelectionOptionsKit(SelectionOptionsKit const & cInThat)
{
	m_pcImpl = new SelectionOptionsKitPrivate();
	Set(cInThat);
}

void SelectionOptionsKit::Set(SelectionOptionsKit const & cInThat)
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	SelectionOptionsKitPrivate * pcInThatImpl = (SelectionOptionsKitPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

SelectionOptionsKit & SelectionOptionsKit::operator =(SelectionOptionsKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

SelectionOptionsKit & SelectionOptionsKit::SetProximity(float fInProximity) 
{
	((SelectionOptionsKitPrivate *)m_pcImpl)->fProximity = fInProximity;
	return *this;
}

SelectionOptionsKit & SelectionOptionsKit::SetLevel(Selection::Level eInLevel) 
{
	((SelectionOptionsKitPrivate *)m_pcImpl)->eLevel = eInLevel;
	return *this;
}

SelectionOptionsKit & SelectionOptionsKit::SetInternalLimit(size_t nInLimit) 
{
	((SelectionOptionsKitPrivate *)m_pcImpl)->nInternalLimit = nInLimit;
	return *this;
}

SelectionOptionsKit & SelectionOptionsKit::SetRelatedLimit(size_t nInLimit) 
{
	((SelectionOptionsKitPrivate *)m_pcImpl)->nRelatedLimit = nInLimit;
	return *this;
}

SelectionOptionsKit & SelectionOptionsKit::SetSorting(Selection::Sorting eInSorting)
{
	((SelectionOptionsKitPrivate *)m_pcImpl)->eSorting = eInSorting;
	return *this;
}

SelectionOptionsKit & SelectionOptionsKit::SetAlgorithm(Selection::Algorithm eInAlgorithm) 
{
	((SelectionOptionsKitPrivate *)m_pcImpl)->eAlgorithm = eInAlgorithm;
	return *this;
}

SelectionOptionsKit & SelectionOptionsKit::SetGranularity(Selection::Granularity eInGranularity) 
{
	((SelectionOptionsKitPrivate *)m_pcImpl)->eGranularity = eInGranularity;
	return *this;
}

SelectionOptionsKit & SelectionOptionsKit::SetBias(Selection::Bias eInBias) 
{
	((SelectionOptionsKitPrivate *)m_pcImpl)->eBias = eInBias;
	return *this;
}

//== SelectionOptionsControl Class =================================================================

class SelectionOptionsControlPrivate : public PrivateImpl
{
public:
	void Copy(SelectionOptionsControlPrivate * pcInThat) {
		m_pcBaseView = pcInThat->m_pcBaseView;
	}

	HBaseView * GetBaseView() { return (HBaseView *)m_pcBaseView; }

	const HBaseView * m_pcBaseView = nullptr;
};

SelectionOptionsControl::SelectionOptionsControl(_3DF::WindowKey const & cInWindow)
{
	SelectionOptionsControlPrivate * pcImpl = new SelectionOptionsControlPrivate();
	pcImpl->m_pcBaseView = cInWindow.GetBaseView();

	m_pcImpl = pcImpl;
}

SelectionOptionsControl::SelectionOptionsControl(SelectionOptionsControl const & cInThat)
{
	m_pcImpl = new SelectionOptionsControlPrivate();
	Set(cInThat);
}

SelectionOptionsControl::SelectionOptionsControl() {}

SelectionOptionsControl::~SelectionOptionsControl()
{

}

void SelectionOptionsControl::Set(SelectionOptionsControl const & cInThat)
{
	SelectionOptionsControlPrivate * pcImpl = (SelectionOptionsControlPrivate *)m_pcImpl;
	SelectionOptionsControlPrivate * pcInThatImpl = (SelectionOptionsControlPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

SelectionOptionsControl & SelectionOptionsControl::operator =(SelectionOptionsControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::SetProximity(float fInProximity)
{
	SelectionOptionsControlPrivate * pcImpl = (SelectionOptionsControlPrivate *)m_pcImpl;
	HC_KEY nViewKey = pcImpl->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		char chDriverOptions[MVO_BUFFER_SIZE];
		sprintf(chDriverOptions, "selection proximity = %f", fInProximity);
		HC_Set_Driver_Options(chDriverOptions);
	} HC_Close_Segment();

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::SetLevel(Selection::Level eInLevel)
{
	SelectionOptionsControlPrivate * pcImpl = (SelectionOptionsControlPrivate *)m_pcImpl;

	switch (eInLevel)
	{
		// directs selection events to return the identifier of the containing segment of the item selected.
		case _3DF::Selection::Level::Segment:
			pcImpl->GetBaseView()->SetViewSelectionLevel(HSelectionLevelSegment);
			pcImpl->GetBaseView()->GetSelection()->SetSelectionLevel(HSelectSegment);
			break;

			// directs selection events to return the item that was selected.
		case _3DF::Selection::Level::Entity:
			pcImpl->GetBaseView()->SetViewSelectionLevel(HSelectionLevelEntity);
			pcImpl->GetBaseView()->GetSelection()->SetSelectionLevel(HSelectEntity);
			break;

		case _3DF::Selection::Level::Subentity:
			pcImpl->GetBaseView()->SetViewSelectionLevel(HSelectionLevelSegment);
			pcImpl->GetBaseView()->GetSelection()->SetSelectionLevel(HSelectSubentity);
			break;
		default:
			break;
	}

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::SetInternalLimit(size_t nInLimit)
{
	SelectionOptionsControlPrivate * pcImpl = (SelectionOptionsControlPrivate *)m_pcImpl;
	HC_KEY nViewKey = pcImpl->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		char chHeuristics[MVO_BUFFER_SIZE];
		sprintf(chHeuristics, "internal selection limit = %d", (int)nInLimit);
		HC_Set_Heuristics(chHeuristics);
	} HC_Close_Segment();

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::SetRelatedLimit(size_t nInLimit)
{
	SelectionOptionsControlPrivate * pcImpl = (SelectionOptionsControlPrivate *)m_pcImpl;
	HC_KEY nViewKey = pcImpl->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		char chHeuristics[MVO_BUFFER_SIZE];
		sprintf(chHeuristics, "related selection limit = %d", (int)nInLimit);
		HC_Set_Heuristics(chHeuristics);
	} HC_Close_Segment();

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::SetSorting(Selection::Sorting eInSorting)
{
	SelectionOptionsControlPrivate * pcImpl = (SelectionOptionsControlPrivate *)m_pcImpl;
	HC_KEY nViewKey = pcImpl->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		switch (eInSorting)
		{
			case _3DF::Selection::Sorting::Off:
				HC_Set_Heuristics("selection sorting = off");
				break;

			case _3DF::Selection::Sorting::Proximity:
				HC_Set_Heuristics("selection sorting = proximity");
				break;

			case _3DF::Selection::Sorting::ZSorting:
				HC_Set_Heuristics("selection sorting = z-sort");
				break;

			// Uses the appropriate sorting based on the selection type: Proximity for point selections and z-sorting for all other selections.
			case _3DF::Selection::Sorting::Default:
				HC_Set_Heuristics("selection sorting = default");
				break;
		}
	} HC_Close_Segment();

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::SetAlgorithm(Selection::Algorithm eInAlgorithm)
{
	SelectionOptionsControlPrivate * pcImpl = (SelectionOptionsControlPrivate *)m_pcImpl;
	HC_KEY nViewKey = pcImpl->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		if (Selection::Algorithm::Visual == eInAlgorithm) {
			HC_Set_Heuristics("visual selection = on");
		}
		else {
			HC_Set_Heuristics("visual selection = off");
		}
	} HC_Close_Segment();

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::SetGranularity(Selection::Granularity eInGranularity)
{
	SelectionOptionsControlPrivate * pcImpl = (SelectionOptionsControlPrivate *)m_pcImpl;
	HC_KEY nViewKey = pcImpl->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		if (Selection::Granularity::Detailed == eInGranularity) {
			HC_Set_Heuristics("detail selection = on");
		}
		else {
			HC_Set_Heuristics("detail selection = off");
		}
	} HC_Close_Segment();

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::SetBias(Selection::Bias eInBias)
{
	SelectionOptionsControlPrivate * pcImpl = (SelectionOptionsControlPrivate *)m_pcImpl;
	HC_KEY nViewKey = pcImpl->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		switch (eInBias)
		{
			case _3DF::Selection::Bias::Lines:
				HC_Set_Heuristics("selection bias = lines");
				break;

			case _3DF::Selection::Bias::NoLines:
				HC_Set_Heuristics("selection bias = no lines");
				break;

			case _3DF::Selection::Bias::Markers:
				HC_Set_Heuristics("selection bias = markers");
				break;

			case _3DF::Selection::Bias::NoMarkers:
				HC_Set_Heuristics("selection bias = no markers");
				break;
		}
	} HC_Close_Segment();

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::UnsetProximity()
{
	SelectionOptionsControlPrivate * pcImpl = (SelectionOptionsControlPrivate *)m_pcImpl;
	HC_KEY nViewKey = pcImpl->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		HC_Set_Heuristics("no selection proximity");
	} HC_Close_Segment();

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::UnsetLevel()
{
	SetLevel(Selection::Level::Entity);
	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::UnsetInternalLimit()
{
	SelectionOptionsControlPrivate * pcImpl = (SelectionOptionsControlPrivate *)m_pcImpl;
	HC_KEY nViewKey = pcImpl->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		HC_Set_Heuristics("no internal selection limit");
	} HC_Close_Segment();

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::UnsetRelatedLimit()
{
	SelectionOptionsControlPrivate * pcImpl = (SelectionOptionsControlPrivate *)m_pcImpl;
	HC_KEY nViewKey = pcImpl->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		HC_Set_Heuristics("no related selection limit");
	} HC_Close_Segment();

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::UnsetSorting()
{
	SetSorting(Selection::Sorting::Off);
	return *this;
}
SelectionOptionsControl & SelectionOptionsControl::UnsetAlgorithm()
{
	SelectionOptionsControlPrivate * pcImpl = (SelectionOptionsControlPrivate *)m_pcImpl;
	HC_KEY nViewKey = pcImpl->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		HC_Set_Heuristics("no] visual selection");
	} HC_Close_Segment();

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::UnsetGranularity()
{
	SelectionOptionsControlPrivate * pcImpl = (SelectionOptionsControlPrivate *)m_pcImpl;
	HC_KEY nViewKey = pcImpl->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		HC_Set_Heuristics("no detail selection");
	} HC_Close_Segment();

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::UnsetBias()
{
	SelectionOptionsControlPrivate * pcImpl = (SelectionOptionsControlPrivate *)m_pcImpl;
	HC_KEY nViewKey = pcImpl->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		HC_Set_Heuristics("no selection bias");
	} HC_Close_Segment();

	return *this;
}

//== SelectionItem Class ===========================================================================
class SelectionItemPrivate : public PrivateImpl
{
public:
	~SelectionItemPrivate() {
		if (nullptr != pcSelection) {
			REMOVE_POINTER(pcSelection);
		}
	}

	_3DF::Type Type() const override { return _3DF::Type::SelectionItem; }

	void Copy(SelectionItemPrivate * pcInThat) {
		pcSelection = pcInThat->pcSelection;
	}

	// LineKey 등이 저장되는 변수
	Key * pcSelection = nullptr;
};

SelectionItem::SelectionItem()
{
}

SelectionItem::SelectionItem(SelectionItem const & cInThat)
{
	m_pcImpl = new SelectionItemPrivate();
	Set(cInThat);
}

void SelectionItem::Set(SelectionItem const & cInThat)
{
	SelectionItemPrivate * pcImpl = (SelectionItemPrivate *)m_pcImpl;
	SelectionItemPrivate * pcInThatImpl = (SelectionItemPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

SelectionItem & SelectionItem::operator=(SelectionItem const & cInThat)
{
	Set(cInThat);
	return *this;
}

bool SelectionItem::ShowSelectedItem(Key & cOutSelection) const
{
	if (nullptr == m_pcImpl) {
		return false;
	}

	SelectionItemPrivate * pcImpl = (SelectionItemPrivate *)m_pcImpl;
	cOutSelection = *pcImpl->pcSelection;

	return true;
}

//== SelectionResults Class ========================================================================

class SelectionResultsPrivate : public PrivateImpl
{
public:
	_3DF::Type Type() const override { return _3DF::Type::SelectionResults; }

	void Copy(SelectionResultsPrivate * pcInThat) {
		for (POSITION pcPosition = pcInThat->aItemList.GetHeadPosition(); pcPosition != NULL; ) {
			aItemList.AddTail(pcInThat->aItemList.GetNext(pcPosition));
		}
	}

	CAtlList<SelectionItem *> aItemList;
};

SelectionResults::SelectionResults()
{
}

SelectionResults::SelectionResults(SelectionResults const & cInThat)
{
	m_pcImpl = new SelectionResultsPrivate();
	Set(cInThat);
}

SelectionResults::~SelectionResults()
{
	Reset();
}

void SelectionResults::Set(SelectionResults const & cInThat)
{
	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;
	SelectionResultsPrivate * pcInThatImpl = (SelectionResultsPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

SelectionResults & SelectionResults::operator=(SelectionResults const & cInThat)
{
	Set(cInThat);
	return *this;
}

// 결과값을 삭제한다.
void SelectionResults::Reset()
{
	if (nullptr == m_pcImpl) {
		return;
	}

	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;

	for (POSITION pcPosition = pcImpl->aItemList.GetHeadPosition(); pcPosition != NULL; ) {
		SelectionItem * pcItem = pcImpl->aItemList.GetNext(pcPosition);
		delete pcItem;
	}

	pcImpl->aItemList.RemoveAll();
}

size_t SelectionResults::GetCount() const
{
	if (nullptr == m_pcImpl) {
		return 0;
	}

	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;
	return  pcImpl->aItemList.GetCount();
}

//== SelectionControl Class ========================================================================

class SelectionControlPrivate : public PrivateImpl
{
public:
	_3DF::Type Type() const override { return _3DF::Type::SelectionControl; }

	void Copy(SelectionControlPrivate * pcInThat) {
		m_pcBaseView = pcInThat->m_pcBaseView;
	}

	int SelectButtonDown(Point const & cInLocation, UINT const nFlags, SelectionResults & cOutResults);
	void HandleSelection(UINT const nFlags, SelectionResults & cOutResults);

	HBaseView * GetBaseView() { return (HBaseView *)m_pcBaseView; }

	const HBaseView * m_pcBaseView = nullptr;

private:
	// & 연사을 해야하므로 enum class를 사용하지 않는다.
	enum SelType {
		None,
		Shell,
		Region,
		Marker,
		Line
	};
};

// 입력되는 Location은 MouseWindowPos을 이용한다. HEventInfo에서 GetMouseWindowPos() 함수를 이용해서 가져올 수 있음
int SelectionControlPrivate::SelectButtonDown(Point const & cInLocation, UINT const nFlags, SelectionResults & cOutResults)
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
				"v, selection level = entity, selection sorting, internal selection limit = 0", cInLocation.x, cInLocation.y);
		} HC_Close_Segment();
	}

	if (nResult == 0) {
		HC_Open_Segment_By_Key(GetBaseView()->GetViewKey()); {
			nResult = HC_Compute_Selection(".",
				(pcSelection->GetSubwindowPenetration() ? "" : "./scene/overwrite"),
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
	HC_KEY * pnKeys = 0;
	HC_KEY * pnIncludeKeys = 0;
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
			pcItemPrivate->pcSelection = new LineKey(Key(nKey));

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

SelectionControl::SelectionControl(WindowKey const & cInWindow)
{
	SelectionControlPrivate * pcImpl = new SelectionControlPrivate();
	pcImpl->m_pcBaseView = cInWindow.GetBaseView();

	m_pcImpl = pcImpl;
}

SelectionControl::SelectionControl(SelectionControl const & cInThat)
{
	m_pcImpl = new SelectionControlPrivate();
	Set(cInThat);
}

SelectionControl::SelectionControl() {}

SelectionControl::~SelectionControl()
{
}

void SelectionControl::Set(SelectionControl const & cInThat)
{
	SelectionControlPrivate * pcImpl = (SelectionControlPrivate *)m_pcImpl;
	SelectionControlPrivate * pcInThatImpl = (SelectionControlPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

SelectionControl & SelectionControl::operator =(SelectionControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

size_t SelectionControl::SelectByPoint(HEventInfo & cEvent, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults) const
{
	Point cInLocation;
	cInLocation.x = cEvent.GetMouseWindowPos().x;
	cInLocation.y = cEvent.GetMouseWindowPos().y;

	return SelectByPoint(cInLocation, cEvent.GetFlags(), cInOptions, cOutResults);
}

size_t SelectionControl::SelectByPoint(HEventInfo & cEvent, SelectionResults & cOutResults) const
{
	Point cInLocation;
	cInLocation.x = cEvent.GetMouseWindowPos().x;
	cInLocation.y = cEvent.GetMouseWindowPos().y;

	return SelectByPoint(cInLocation, cEvent.GetFlags(), cOutResults);
}

size_t SelectionControl::SelectByPoint(Point const & cInLocation, UINT const nFlags, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults) const
{
	return 0;
}

size_t SelectionControl::SelectByPoint(Point const & cInLocation, UINT const nFlags, SelectionResults & cOutResults) const
{
	SelectionControlPrivate * pcImpl = (SelectionControlPrivate *)m_pcImpl;

	pcImpl->SelectButtonDown(cInLocation, nFlags, cOutResults);

/*
	SelectionControlPrivate * pcImpl = (SelectionControlPrivate *)m_pcImpl;
	HC_KEY nViewKey = pcImpl->GetBaseView()->GetViewKey();
	
	HC_Open_Segment_By_Key(nViewKey);
	{
		// We need some sort of path to properly find whether we have a screen range
		// and to compute the coordinates if we do.  Selection is the easiest way to get this.
		const char * opt = "v, related selection limit = 0, no selection sorting, internal selection limit=0, selection level = segment"; //, selection first match";

		res = HC_Compute_Selection(".",
			(view->GetSelection()->GetSubwindowPenetration() ? "" : "./scene/overwrite"),
			opt, window_position.x, window_position.y);

	}
	HC_Close_Segment();

	int nResult = 0;
	HC_Open_Segment_By_Key(nViewKey); {
		nResult = HC_Compute_Selection(".",
			(selection->GetSubwindowPenetration() ? "" : "./scene/overwrite"),
			"v, selection level = entity, related selection limit = 0, selection sorting, internal selection limit = 0", new_pos.x, new_pos.y);
	} HC_Close_Segment();
*/

	return 0;
}

//== HighlightControl Class ========================================================================

class HighlightControlPrivate : public PrivateImpl
{
public:
	_3DF::Type Type() const override { return _3DF::Type::SelectionControl; }

	void Copy(HighlightControlPrivate * pcInThat) {
		m_pcBaseView = pcInThat->m_pcBaseView;
	}

// 	int SelectButtonDown(Point const & cInLocation, UINT const nFlags, SelectionResults & cOutResults);
// 	void HandleSelection(UINT const nFlags, SelectionResults & cOutResults);

	HBaseView * GetBaseView() { return (HBaseView *)m_pcBaseView; }
	const HBaseView * m_pcBaseView = nullptr;
};

HighlightControl::HighlightControl(WindowKey const & cInWindow)
{
	HighlightControlPrivate * pcImpl = new HighlightControlPrivate();
	pcImpl->m_pcBaseView = cInWindow.GetBaseView();

	m_pcImpl = pcImpl;

}

HighlightControl::HighlightControl(HighlightControl const & cInThat)
{
	m_pcImpl = new HighlightControlPrivate();
	Set(cInThat);
}

HighlightControl::HighlightControl() {}

HighlightControl::~HighlightControl()
{

}

void HighlightControl::Set(HighlightControl const & cInThat)
{
	HighlightControlPrivate * pcImpl = (HighlightControlPrivate *)m_pcImpl;
	HighlightControlPrivate * pcInThatImpl = (HighlightControlPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

HighlightControl & HighlightControl::operator=(HighlightControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

//== DmiSelectionControl ===========================================================================

DmiSelectionControl::DmiSelectionControl(HBaseView * pcView, bool bReferenceSelection) :
	HSelectionSet(pcView, bReferenceSelection)
{
	m_nSelectLevel = SEGMENT_TYPE;
	m_pcSelection = nullptr;
	m_bShowFacesAsLines = false;
	//SetAllowEntitySelection(false);
}

DmiSelectionControl::~DmiSelectionControl()
{
	if (nullptr != m_pcSelection)
	{
		delete_vlist(m_pcSelection);
		m_pcSelection = nullptr;
	}
}

// create a new list  object
void DmiSelectionControl::Init()
{
	m_pcSelection = new_vlist(malloc, free);
	HSelectionSet::Init();
}

void DmiSelectionControl::Select(HC_KEY key, int num_include_keys, HC_KEY * include_keys, bool emit_message)
{
	char	keyType[MVO_BUFFER_SIZE];

	if (!m_pView->GetEmitMessageFunction() || emit_message)	//disregard segment level setting if messsage
		//from other client			
	{
		if (!GetAllowEntitySelection()) // nonzero if NOT a segment
		{
			// the key is to a geometric entity.  If we are in segment selection mode,
			// then we need to get the key to its parent segment.

			HC_Show_Key_Type(key, keyType);

			if (!streq("segment", keyType))
			{
				char segname[MVO_BUFFER_SIZE];
				HC_KEY segkey;

				segkey = HC_KShow_Owner_Original_Key(key);
				HC_Show_Owner_By_Key(key, segname);

				// climb up one more level if this is the temporary highlight key
				if (IsHighlightSegment(segkey))
				{
					segkey = HC_KShow_Owner_Original_Key(segkey);
					HC_Show_Owner_By_Key(segkey, segname);
				}
				key = segkey;
			}
		}
	}

	HSelectionSet::Select(key, num_include_keys, include_keys, emit_message);
}
