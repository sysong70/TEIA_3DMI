#include "StdAfx.h"

#include "3DF.Selection.h"
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

//== SelectionOptionsKit Class =====================================================================
TDF::SelectionOptionsKit::SelectionOptionsKit()
{
	m_pcImpl = new SelectionOptionsKitPrivate();
}

TDF::SelectionOptionsKit::SelectionOptionsKit(SelectionOptionsKit const & cInThat)
{
	m_pcImpl = new SelectionOptionsKitPrivate();
	Set(cInThat);
}

void TDF::SelectionOptionsKit::Set(SelectionOptionsKit const & cInThat)
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	SelectionOptionsKitPrivate * pcInThatImpl = (SelectionOptionsKitPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

SelectionOptionsKit & TDF::SelectionOptionsKit::operator =(SelectionOptionsKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

SelectionOptionsKit & TDF::SelectionOptionsKit::SetProximity(float fInProximity)
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	pcImpl->fProximity = fInProximity;
	pcImpl->bProximity = true;
	return *this;
}

SelectionOptionsKit & TDF::SelectionOptionsKit::SetLevel(Selection::Level eInLevel)
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	pcImpl->eLevel = eInLevel;
	pcImpl->bLevel = true;
	return *this;
}

SelectionOptionsKit & TDF::SelectionOptionsKit::SetInternalLimit(size_t nInLimit)
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	pcImpl->nInternalLimit = nInLimit;
	pcImpl->bInternalLimit = true;
	return *this;
}

SelectionOptionsKit & TDF::SelectionOptionsKit::SetRelatedLimit(size_t nInLimit)
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	pcImpl->nRelatedLimit = nInLimit;
	pcImpl->bRelatedLimit = true;
	return *this;
}

SelectionOptionsKit & TDF::SelectionOptionsKit::SetSorting(Selection::Sorting eInSorting)
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	pcImpl->eSorting = eInSorting;
	pcImpl->bSorting = true;
	return *this;
}

SelectionOptionsKit & TDF::SelectionOptionsKit::SetAlgorithm(Selection::Algorithm eInAlgorithm)
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	pcImpl->eAlgorithm = eInAlgorithm;
	pcImpl->bAlgorithm = true;
	return *this;
}

SelectionOptionsKit & TDF::SelectionOptionsKit::SetGranularity(Selection::Granularity eInGranularity)
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	pcImpl->eGranularity = eInGranularity;
	pcImpl->bGranularity = true;
	return *this;
}

SelectionOptionsKit & TDF::SelectionOptionsKit::SetBias(Selection::Bias eInBias)
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	pcImpl->eBias = eInBias;
	pcImpl->bBias = true;
	return *this;
}

bool TDF::SelectionOptionsKit::ShowProximity(float & fOutProximity) const
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	if (false == pcImpl->bProximity) {
		return false;
	}

	fOutProximity = pcImpl->fProximity;
	return true;
}

bool TDF::SelectionOptionsKit::ShowLevel(Selection::Level & eOutLevel) const
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	if (false == pcImpl->bLevel) {
		return false;
	}

	eOutLevel = pcImpl->eLevel;
	return true;
}

bool TDF::SelectionOptionsKit::ShowInternalLimit(size_t & nOutLimit) const
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	if (false == pcImpl->bInternalLimit) {
		return false;
	}

	nOutLimit = pcImpl->nInternalLimit;
	return true;
}

bool TDF::SelectionOptionsKit::ShowRelatedLimit(size_t & nOutLimit) const
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	if (false == pcImpl->bRelatedLimit) {
		return false;
	}

	nOutLimit = pcImpl->nRelatedLimit;
	return true;

}

bool TDF::SelectionOptionsKit::ShowSorting(Selection::Sorting & eOutSorting) const
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	if (false == pcImpl->bSorting) {
		return false;
	}

	eOutSorting = pcImpl->eSorting;
	return true;
}

bool TDF::SelectionOptionsKit::ShowAlgorithm(Selection::Algorithm & eOutAlgorithm) const
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	if (false == pcImpl->bAlgorithm) {
		return false;
	}

	eOutAlgorithm = pcImpl->eAlgorithm;
	return true;
}

bool TDF::SelectionOptionsKit::ShowGranularity(Selection::Granularity & eOutGranularity) const
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	if (false == pcImpl->bGranularity) {
		return false;
	}

	eOutGranularity = pcImpl->eGranularity;
	return true;
}

bool TDF::SelectionOptionsKit::ShowBias(Selection::Bias & eOutBias) const
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	if (false == pcImpl->bBias) {
		return false;
	}

	eOutBias = pcImpl->eBias;
	return true;
}

//== SelectionOptionsControl Class =================================================================
SelectionOptionsControl::SelectionOptionsControl(TDF::WindowKey const & cInWindow)
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
		case TDF::Selection::Level::Segment:
			pcImpl->GetBaseView()->SetViewSelectionLevel(HSelectionLevelSegment);
			pcImpl->GetBaseView()->GetSelection()->SetSelectionLevel(HSelectSegment);
			break;

			// directs selection events to return the item that was selected.
		case TDF::Selection::Level::Entity:
			pcImpl->GetBaseView()->SetViewSelectionLevel(HSelectionLevelEntity);
			pcImpl->GetBaseView()->GetSelection()->SetSelectionLevel(HSelectEntity);
			break;

		case TDF::Selection::Level::Subentity:
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
			case TDF::Selection::Sorting::Off:
				HC_Set_Heuristics("selection sorting = off");
				break;

			case TDF::Selection::Sorting::Proximity:
				HC_Set_Heuristics("selection sorting = proximity");
				break;

			case TDF::Selection::Sorting::ZSorting:
				HC_Set_Heuristics("selection sorting = z-sort");
				break;

			// Uses the appropriate sorting based on the selection type: Proximity for point selections and z-sorting for all other selections.
			case TDF::Selection::Sorting::Default:
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
			case TDF::Selection::Bias::Lines:
				HC_Set_Heuristics("selection bias = lines");
				break;

			case TDF::Selection::Bias::NoLines:
				HC_Set_Heuristics("selection bias = no lines");
				break;

			case TDF::Selection::Bias::Markers:
				HC_Set_Heuristics("selection bias = markers");
				break;

			case TDF::Selection::Bias::NoMarkers:
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
TDF::SelectionItem::SelectionItem()
{
}

TDF::SelectionItem::SelectionItem(SelectionItem const & cInThat)
{
	m_pcImpl = new SelectionItemPrivate();

	Set(cInThat);
}

void TDF::SelectionItem::Set(SelectionItem const & cInThat)
{
	SelectionItemPrivate * pcImpl = (SelectionItemPrivate *)m_pcImpl;
	SelectionItemPrivate * pcInThatImpl = (SelectionItemPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

SelectionItem & TDF::SelectionItem::operator=(SelectionItem const & cInThat)
{
	Set(cInThat);
	return *this;
}

bool TDF::SelectionItem::operator==(SelectionItem const & cInThat) const
{
	if (nullptr == m_pcImpl) {
		return false;
	}

	SelectionItemPrivate * pcImpl = (SelectionItemPrivate *)m_pcImpl;
	SelectionItemPrivate * pcInThatImpl = (SelectionItemPrivate *)cInThat.m_pcImpl;

	if (pcImpl->pcKey->KeyValue() != pcInThatImpl->pcKey->KeyValue()) {
		return false;
	}

	if (pcImpl->nKeyCount != pcInThatImpl->nKeyCount) {
		return false;
	}

	for (int nIndex = 0; nIndex < pcImpl->nKeyCount; nIndex++) {
		if (pcImpl->pnKeys[nIndex] != pcInThatImpl->pnKeys[nIndex]) {
			return false;
		}
	}

	if (pcImpl->nIncludeCount != pcInThatImpl->nIncludeCount) {
		return false;
	}

	for (int nIndex = 0; nIndex < pcImpl->nIncludeCount; nIndex++) {
		if (pcImpl->pnIncludeKeys[nIndex] != pcInThatImpl->pnIncludeKeys[nIndex]) {
			return false;
		}
	}

	if (pcImpl->nOffset1 != pcInThatImpl->nOffset1) {
		return false;
	}

	if (pcImpl->nOffset2 != pcInThatImpl->nOffset2) {
		return false;
	}

	if (pcImpl->nOffset3 != pcInThatImpl->nOffset3) {
		return false;
	}

	if (pcImpl->nRegion != pcInThatImpl->nRegion) {
		return false;
	}

	if (pcImpl->nLowest != pcInThatImpl->nLowest) {
		return false;
	}

	if (pcImpl->nHighest != pcInThatImpl->nHighest) {
		return false;
	}

	return true;
}

bool TDF::SelectionItem::operator!=(SelectionItem const & cInThat) const
{
	return !(*this == cInThat);
}

bool TDF::SelectionItem::ShowSelectedItem(Key *& pcOutSelection)
{
	if (nullptr == m_pcImpl) { 
		return false;
	}

	SelectionItemPrivate * pcImpl = (SelectionItemPrivate *)m_pcImpl;
	pcOutSelection = pcImpl->pcKey;

	return true;
}

bool TDF::SelectionItem::ShowSelectionPosition(WindowPoint & cOutLocation) const
{
	if (nullptr == m_pcImpl) {
		return false;
	}

	SelectionItemPrivate * pcImpl = (SelectionItemPrivate *)m_pcImpl;
	cOutLocation = pcImpl->cWindowPoint;

	return true;
}

bool TDF::SelectionItem::ShowSelectionPosition(WorldPoint & cOutLocation) const
{
	if (nullptr == m_pcImpl) {
		return false;
	}

	SelectionItemPrivate * pcImpl = (SelectionItemPrivate *)m_pcImpl;
	cOutLocation = pcImpl->cWorldPoint;

	return true;
}

//== SelectionResults Class ========================================================================
SelectionResults::SelectionResults()
{
	m_pcImpl = new SelectionResultsPrivate();
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
	// 복사하기 전에 기존의 결과값을 삭제한다.
	Reset();

	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;
	SelectionResultsPrivate * pcInThatImpl = (SelectionResultsPrivate *)cInThat.m_pcImpl;

	pcImpl->Copy(pcInThatImpl);
}

SelectionResults & SelectionResults::operator=(SelectionResults const & cInThat)
{
	Set(cInThat);
	return *this;
}

bool SelectionResults::operator==(SelectionResults const & cInThat) const
{
	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;
	SelectionResultsPrivate * pcInThatImpl = (SelectionResultsPrivate *)cInThat.m_pcImpl;

	POSITION pcPosition = pcImpl->aItemList.GetHeadPosition();

	while (nullptr != pcPosition)
	{
		SelectionItem * pcItem = pcImpl->aItemList.GetNext(pcPosition);

		POSITION pcInThatPosition = pcInThatImpl->aItemList.GetHeadPosition();

		bool bFindSameItemFlag = false;
		while (nullptr != pcInThatPosition) {
			SelectionItem * pcInThatItem = pcInThatImpl->aItemList.GetNext(pcInThatPosition);
			if (*pcItem == *pcInThatItem) {
				bFindSameItemFlag = true;
				break;
			}
		}

		if (false == bFindSameItemFlag) {
			return false;
		}
	}

	return true;
}

bool SelectionResults::operator!=(SelectionResults const & cInThat) const
{
	if (*this == cInThat) {
		return false;
	}

	return true;
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

POSITION SelectionResults::GetHeadPosition() const
{
	if (nullptr == m_pcImpl) {
		return nullptr;
	}

	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;
	return  pcImpl->aItemList.GetHeadPosition();
}

SelectionItem * SelectionResults::GetAt(POSITION & pcPosition)
{
	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;
	return pcImpl->aItemList.GetAt(pcPosition);
}

SelectionItem * SelectionResults::GetAt(POSITION & pcPosition) const
{
	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;
	return pcImpl->aItemList.GetAt(pcPosition);
}

SelectionItem * SelectionResults::GetNext(POSITION & pcPosition)
{
	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;
	return  pcImpl->aItemList.GetNext(pcPosition);
}

SelectionItem * SelectionResults::GetNext(POSITION & pcPosition) const
{
	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;
	return  pcImpl->aItemList.GetNext(pcPosition);
}

void SelectionResults::RemoveAt(POSITION & pcPosition)
{
	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;

	SelectionItem * pcItem = pcImpl->aItemList.GetAt(pcPosition);
	if (nullptr != pcItem) {
		delete pcItem;
		pcItem = nullptr;
	}

	pcImpl->aItemList.RemoveAt(pcPosition);
}

void SelectionResults::RemoveAt(POSITION & pcPosition) const
{
	if (nullptr == m_pcImpl) {
		return;
	}

	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;

	SelectionItem * pcItem = pcImpl->aItemList.GetAt(pcPosition);
	if (nullptr != pcItem) {
		delete pcItem;
		pcItem = nullptr;
	}

	pcImpl->aItemList.RemoveAt(pcPosition);
}

// 내부 요소가 Size보다 큰 경우 Size 보다 큰 부분은 삭제한다.
void SelectionResults::SetSize(size_t nInSize)
{
	if (nullptr == m_pcImpl) {
		return;
	}

	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;

	size_t nCount = pcImpl->aItemList.GetCount();
	if (nCount <= nInSize) {
		return;
	}

	POSITION pcPosition = pcImpl->aItemList.GetHeadPosition();
	POSITION pcCurrentPosition = nullptr;

	size_t nIndex = 0;
	while (nullptr != pcPosition) {
		pcCurrentPosition = pcPosition;
		SelectionItem * pcItem = pcImpl->aItemList.GetNext(pcPosition);

		nIndex++;

		// Size보다 작을때는 삭제를 하지 않는다.
		if (nIndex <= nInSize) {
			continue;
		}

		pcImpl->aItemList.RemoveAt(pcCurrentPosition);
		delete pcItem;
	}
}

// 들어오는 SelectionResults 값을 추가시킨다. 
bool SelectionResults::Union(SelectionResults const & cInThat)
{
	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;
	SelectionResultsPrivate * pcInThatImpl = (SelectionResultsPrivate *)cInThat.m_pcImpl;

	if (nullptr == pcImpl || nullptr == pcInThatImpl) {
		return false;
	}

	POSITION pcInThatPosition = pcInThatImpl->aItemList.GetHeadPosition();

	while (nullptr != pcInThatPosition)
	{
		SelectionItem * pcInThatItem = pcInThatImpl->aItemList.GetNext(pcInThatPosition);

		POSITION pcPosition = pcImpl->aItemList.GetHeadPosition();

		bool bFindFlag = false;
		while (nullptr != pcPosition) {
			SelectionItem * pcItem = pcImpl->aItemList.GetNext(pcPosition);

			if (*pcItem == *pcInThatItem) {
				bFindFlag = true;
				break;
			}
		}

		if (true == bFindFlag) {
			continue;
		}

		SelectionItem * pcNewItem = new SelectionItem(*pcInThatItem);
		pcImpl->aItemList.AddHead(pcNewItem);
	}

	return true;
}

void SelectionResults::LeaveType(DWORD nType)
{
	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;

	POSITION pcPosition = pcImpl->aItemList.GetHeadPosition();
	POSITION pcCurrentPosition = nullptr;

	while (nullptr != pcPosition)
	{
		pcCurrentPosition = pcPosition;
		SelectionItem * pcItem = pcImpl->aItemList.GetNext(pcPosition);

		Key * pcItemKey = nullptr;
		if(true == pcItem->ShowSelectedItem(pcItemKey)) {
			DWORD nItemType = (DWORD)pcItemKey->Type();
			// 원하는 Type이면 삭제하지 않는다.
			if (nItemType == (nType & nItemType)) {
				continue;
			}
		}

		pcImpl->aItemList.RemoveAt(pcCurrentPosition);
	}
}

void SelectionResults::RemoveType(DWORD nType)
{
	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;

	POSITION pcPosition = pcImpl->aItemList.GetHeadPosition();
	POSITION pcCurrentPosition = nullptr;

	while (nullptr != pcPosition)
	{
		pcCurrentPosition = pcPosition;
		SelectionItem * pcItem = pcImpl->aItemList.GetNext(pcPosition);

		Key * pcItemKey = nullptr;
		if (true == pcItem->ShowSelectedItem(pcItemKey)) {
			DWORD nItemType = (DWORD)pcItemKey->Type();
			// 원하는 Type이면 삭제한다.
			if (nItemType == (nType & nItemType)) {
				pcImpl->aItemList.RemoveAt(pcCurrentPosition);
			}
		}
	}
}

//== SelectionControl Class ========================================================================
TDF::SelectionControl::SelectionControl(WindowKey const & cInWindow)
{
	SelectionControlPrivate * pcImpl = new SelectionControlPrivate();
	pcImpl->m_pcWindow = &cInWindow;
	pcImpl->m_pcBaseView = cInWindow.GetBaseView();

	m_pcImpl = pcImpl;
}

TDF::SelectionControl::SelectionControl(SelectionControl const & cInThat)
{
	m_pcImpl = new SelectionControlPrivate();
	Set(cInThat);
}

TDF::SelectionControl::SelectionControl() {}

TDF::SelectionControl::~SelectionControl()
{
}

void TDF::SelectionControl::Set(SelectionControl const & cInThat)
{
	SelectionControlPrivate * pcImpl = (SelectionControlPrivate *)m_pcImpl;
	SelectionControlPrivate * pcInThatImpl = (SelectionControlPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

SelectionControl & TDF::SelectionControl::operator =(SelectionControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

size_t TDF::SelectionControl::SelectByPoint(HEventInfo & cEvent, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults) const
{
	Point cInLocation;
	cInLocation.x = cEvent.GetMouseWindowPos().x;
	cInLocation.y = cEvent.GetMouseWindowPos().y;

	//return SelectByPoint(cInLocation, cEvent.GetFlags(), cInOptions, cOutResults);
	return SelectByPoint(cInLocation, cInOptions, cOutResults);
}

size_t TDF::SelectionControl::SelectByPoint(HEventInfo & cEvent, SelectionResults & cOutResults) const
{
	Point cInLocation;
	cInLocation.x = cEvent.GetMouseWindowPos().x;
	cInLocation.y = cEvent.GetMouseWindowPos().y;

	SelectionOptionsKit cInOptions;
	return SelectByPoint(cInLocation, cInOptions, cOutResults);
}

size_t TDF::SelectionControl::SelectByPoint(Point const & cInLocation, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults) const
{
	SelectionControlPrivate * pcImpl = (SelectionControlPrivate *)m_pcImpl;
	return pcImpl->SelectByPoint(cInLocation, cInOptions, cOutResults);
}

size_t TDF::SelectionControl::SelectByPoint(Point const & cInLocation, UINT const nFlags, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults) const
{
	return 0;
}

size_t TDF::SelectionControl::SelectByPoint(Point const & cInLocation, UINT const nFlags, SelectionResults & cOutResults) const
{
	SelectionControlPrivate * pcImpl = (SelectionControlPrivate *)m_pcImpl;
	SelectionOptionsKit cInOptions;
	pcImpl->SelectByPoint(cInLocation, nFlags, cInOptions, cOutResults);

	return 0;
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
