#include "StdAfx.h"

#include "3DF.Selection.h"
#include "Private/3DF.SelectionPrivate.h"

#include "Window.h"
#include "BaseView.h"

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
H3DF::SelectionOptionsKit::SelectionOptionsKit()
{
	m_pcImpl = new SelectionOptionsKitPrivate();
}

H3DF::SelectionOptionsKit::SelectionOptionsKit(SelectionOptionsKit const & cInThat)
{
	m_pcImpl = new SelectionOptionsKitPrivate();
	Set(cInThat);
}

void H3DF::SelectionOptionsKit::Set(SelectionOptionsKit const & cInThat)
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	SelectionOptionsKitPrivate * pcInThatImpl = (SelectionOptionsKitPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::operator =(SelectionOptionsKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetProximity(float fInProximity)
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	pcImpl->fProximity = fInProximity;
	pcImpl->bProximity = true;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetLevel(Selection::Level eInLevel)
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	pcImpl->eLevel = eInLevel;
	pcImpl->bLevel = true;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetInternalLimit(size_t nInLimit)
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	pcImpl->nInternalLimit = nInLimit;
	pcImpl->bInternalLimit = true;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetRelatedLimit(size_t nInLimit)
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	pcImpl->nRelatedLimit = nInLimit;
	pcImpl->bRelatedLimit = true;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetSorting(Selection::Sorting eInSorting)
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	pcImpl->eSorting = eInSorting;
	pcImpl->bSorting = true;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetAlgorithm(Selection::Algorithm eInAlgorithm)
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	pcImpl->eAlgorithm = eInAlgorithm;
	pcImpl->bAlgorithm = true;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetGranularity(Selection::Granularity eInGranularity)
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	pcImpl->eGranularity = eInGranularity;
	pcImpl->bGranularity = true;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetBias(Selection::Bias eInBias)
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	pcImpl->eBias = eInBias;
	pcImpl->bBias = true;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetScope(SegmentKey const & cInStartSegment, bool bInScopeOnly)
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	pcImpl->cStartSegment = cInStartSegment;
	pcImpl->bScopeOnly = bInScopeOnly;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetScope(KeyPath const & cInStartPath, bool bInScopeOnly)
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	pcImpl->cStartPath = cInStartPath;
	pcImpl->bScopeOnly = bInScopeOnly;
	return *this;
}

bool H3DF::SelectionOptionsKit::ShowProximity(float & fOutProximity) const
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	if (false == pcImpl->bProximity) {
		return false;
	}

	fOutProximity = pcImpl->fProximity;
	return true;
}

bool H3DF::SelectionOptionsKit::ShowLevel(Selection::Level & eOutLevel) const
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	if (false == pcImpl->bLevel) {
		return false;
	}

	eOutLevel = pcImpl->eLevel;
	return true;
}

bool H3DF::SelectionOptionsKit::ShowInternalLimit(size_t & nOutLimit) const
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	if (false == pcImpl->bInternalLimit) {
		return false;
	}

	nOutLimit = pcImpl->nInternalLimit;
	return true;
}

bool H3DF::SelectionOptionsKit::ShowRelatedLimit(size_t & nOutLimit) const
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	if (false == pcImpl->bRelatedLimit) {
		return false;
	}

	nOutLimit = pcImpl->nRelatedLimit;
	return true;

}

bool H3DF::SelectionOptionsKit::ShowSorting(Selection::Sorting & eOutSorting) const
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	if (false == pcImpl->bSorting) {
		return false;
	}

	eOutSorting = pcImpl->eSorting;
	return true;
}

bool H3DF::SelectionOptionsKit::ShowAlgorithm(Selection::Algorithm & eOutAlgorithm) const
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	if (false == pcImpl->bAlgorithm) {
		return false;
	}

	eOutAlgorithm = pcImpl->eAlgorithm;
	return true;
}

bool H3DF::SelectionOptionsKit::ShowGranularity(Selection::Granularity & eOutGranularity) const
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	if (false == pcImpl->bGranularity) {
		return false;
	}

	eOutGranularity = pcImpl->eGranularity;
	return true;
}

bool H3DF::SelectionOptionsKit::ShowBias(Selection::Bias & eOutBias) const
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	if (false == pcImpl->bBias) {
		return false;
	}

	eOutBias = pcImpl->eBias;
	return true;
}

bool H3DF::SelectionOptionsKit::ShowScope(SegmentKey & cOutStartSegment, bool & bOutScopeOnly) const
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	if (INVALID_KEY == pcImpl->cStartSegment.KeyValue()) {
		return false;
	}

	cOutStartSegment = pcImpl->cStartSegment;
	bOutScopeOnly = pcImpl->bScopeOnly;
	
	return true;
}

bool H3DF::SelectionOptionsKit::ShowScope(KeyPath & cOutStartPath, bool & bOutScopeOnly) const
{
	SelectionOptionsKitPrivate * pcImpl = (SelectionOptionsKitPrivate *)m_pcImpl;
	if (true == pcImpl->cStartPath.Empty()) {
		return false;
	}

	cOutStartPath = pcImpl->cStartPath;
	bOutScopeOnly = pcImpl->bScopeOnly;

	return true;
}

//== SelectionOptionsControl Class =================================================================
SelectionOptionsControl::SelectionOptionsControl(H3DF::WindowKey const & cInWindow)
{
	SelectionOptionsControlPrivate * pcImpl = new SelectionOptionsControlPrivate();
	pcImpl->m_pcWindow = &cInWindow;

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
	HC_KEY nViewKey = pcImpl->GetWindow()->GetBaseView()->GetViewKey();

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
		case H3DF::Selection::Level::Segment:
			pcImpl->m_pcSelectionSet->SetSelectionLevel(HSelectSegment);
			break;

			// directs selection events to return the item that was selected.
		case H3DF::Selection::Level::Entity:
			pcImpl->m_pcSelectionSet->SetSelectionLevel(HSelectEntity);
			break;

		case H3DF::Selection::Level::Subentity:
			pcImpl->m_pcSelectionSet->SetSelectionLevel(HSelectSubentity);
			break;
		default:
			break;
	}

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::SetInternalLimit(size_t nInLimit)
{
	SelectionOptionsControlPrivate * pcImpl = (SelectionOptionsControlPrivate *)m_pcImpl;
	HC_KEY nViewKey = pcImpl->GetWindow()->GetBaseView()->GetViewKey();

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
	HC_KEY nViewKey = pcImpl->GetWindow()->GetBaseView()->GetViewKey();

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
	HC_KEY nViewKey = pcImpl->GetWindow()->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		switch (eInSorting)
		{
			case H3DF::Selection::Sorting::Off:
				HC_Set_Heuristics("selection sorting = off");
				break;

			case H3DF::Selection::Sorting::Proximity:
				HC_Set_Heuristics("selection sorting = proximity");
				break;

			case H3DF::Selection::Sorting::ZSorting:
				HC_Set_Heuristics("selection sorting = z-sort");
				break;

			// Uses the appropriate sorting based on the selection type: Proximity for point selections and z-sorting for all other selections.
			case H3DF::Selection::Sorting::Default:
				HC_Set_Heuristics("selection sorting = default");
				break;
		}
	} HC_Close_Segment();

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::SetAlgorithm(Selection::Algorithm eInAlgorithm)
{
	SelectionOptionsControlPrivate * pcImpl = (SelectionOptionsControlPrivate *)m_pcImpl;
	HC_KEY nViewKey = pcImpl->GetWindow()->GetBaseView()->GetViewKey();

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
	HC_KEY nViewKey = pcImpl->GetWindow()->GetBaseView()->GetViewKey();

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
	HC_KEY nViewKey = pcImpl->GetWindow()->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		switch (eInBias)
		{
			case H3DF::Selection::Bias::Lines:
				HC_Set_Heuristics("selection bias = lines");
				break;

			case H3DF::Selection::Bias::NoLines:
				HC_Set_Heuristics("selection bias = no lines");
				break;

			case H3DF::Selection::Bias::Markers:
				HC_Set_Heuristics("selection bias = markers");
				break;

			case H3DF::Selection::Bias::NoMarkers:
				HC_Set_Heuristics("selection bias = no markers");
				break;
		}
	} HC_Close_Segment();

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::UnsetProximity()
{
	SelectionOptionsControlPrivate * pcImpl = (SelectionOptionsControlPrivate *)m_pcImpl;
	HC_KEY nViewKey = pcImpl->GetWindow()->GetBaseView()->GetViewKey();

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
	HC_KEY nViewKey = pcImpl->GetWindow()->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		HC_Set_Heuristics("no internal selection limit");
	} HC_Close_Segment();

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::UnsetRelatedLimit()
{
	SelectionOptionsControlPrivate * pcImpl = (SelectionOptionsControlPrivate *)m_pcImpl;
	HC_KEY nViewKey = pcImpl->GetWindow()->GetBaseView()->GetViewKey();

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
	HC_KEY nViewKey = pcImpl->GetWindow()->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		HC_Set_Heuristics("no] visual selection");
	} HC_Close_Segment();

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::UnsetGranularity()
{
	SelectionOptionsControlPrivate * pcImpl = (SelectionOptionsControlPrivate *)m_pcImpl;
	HC_KEY nViewKey = pcImpl->GetWindow()->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		HC_Set_Heuristics("no detail selection");
	} HC_Close_Segment();

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::UnsetBias()
{
	SelectionOptionsControlPrivate * pcImpl = (SelectionOptionsControlPrivate *)m_pcImpl;
	HC_KEY nViewKey = pcImpl->GetWindow()->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		HC_Set_Heuristics("no selection bias");
	} HC_Close_Segment();

	return *this;
}

//== SelectionItem Class ===========================================================================
H3DF::SelectionItem::SelectionItem()
{
	m_pcImpl = new SelectionItemPrivate();
}

H3DF::SelectionItem::SelectionItem(SelectionItem const & cInThat)
{
	m_pcImpl = new SelectionItemPrivate();

	Set(cInThat);
}

void H3DF::SelectionItem::Set(SelectionItem const & cInThat)
{
	SelectionItemPrivate * pcImpl = (SelectionItemPrivate *)m_pcImpl;
	SelectionItemPrivate * pcInThatImpl = (SelectionItemPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

SelectionItem & H3DF::SelectionItem::operator=(SelectionItem const & cInThat)
{
	Set(cInThat);
	return *this;
}

bool H3DF::SelectionItem::operator==(SelectionItem const & cInThat) const
{
	if (nullptr == m_pcImpl) {
		return false;
	}

	SelectionItemPrivate * pcImpl = (SelectionItemPrivate *)m_pcImpl;
	SelectionItemPrivate * pcInThatImpl = (SelectionItemPrivate *)cInThat.m_pcImpl;

	if (pcImpl->cKey.KeyValue() != pcInThatImpl->cKey.KeyValue()) {
		return false;
	}

	if (pcImpl->nIncludeCount != pcInThatImpl->nIncludeCount) {
		return false;
	}

	for (int nIndex = 0; nIndex < pcImpl->nIncludeCount; nIndex++) {
		if (pcImpl->pnIncludeKeys[nIndex] != pcInThatImpl->pnIncludeKeys[nIndex]) {
			return false;
		}
	}

	// Arc나 Polygon, Polyline 등에서 특성값이나, 몇번째 요소들이 선택되었는지 여부를 나타내는 값들이다.
	// 값이 같지 않아도 같은 요소들이 선택된것일 수 있으므로 비교처리하지 않는다.
/*  
	if (pcImpl->nOffset1 != pcInThatImpl->nOffset1) {
		return false;
	}

	if (pcImpl->nOffset2 != pcInThatImpl->nOffset2) {
		return false;
	}

	if (pcImpl->nOffset3 != pcInThatImpl->nOffset3) {
		return false;
	}
*/

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

bool H3DF::SelectionItem::operator!=(SelectionItem const & cInThat) const
{
	return !(*this == cInThat);
}


bool H3DF::SelectionItem::ShowSelectedItem(Key & cOutSelection)
{
	if (nullptr == m_pcImpl) {
		return false;
	}

	SelectionItemPrivate * pcImpl = (SelectionItemPrivate *)m_pcImpl;
	cOutSelection = pcImpl->cKey;

	return true;
}

const bool H3DF::SelectionItem::ShowSelectedItem(Key & cOutSelection) const
{
	if (nullptr == m_pcImpl) {
		return false;
	}

	SelectionItemPrivate * pcImpl = (SelectionItemPrivate *)m_pcImpl;
	cOutSelection = pcImpl->cKey;

	return true;
}

bool H3DF::SelectionItem::ShowPath(KeyPath & cOutPath) const
{
	if (nullptr == m_pcImpl) {
		return false;
	}

	SelectionItemPrivate * pcImpl = (SelectionItemPrivate *)m_pcImpl;

	cOutPath = KeyPath(pcImpl->nIncludeCount, pcImpl->pnIncludeKeys);

	return true;
}

bool H3DF::SelectionItem::ShowSelectionPosition(WindowPoint & cOutLocation) const
{
	if (nullptr == m_pcImpl) {
		return false;
	}

	SelectionItemPrivate * pcImpl = (SelectionItemPrivate *)m_pcImpl;
	cOutLocation = pcImpl->cWindowPoint;

	return true;
}

bool H3DF::SelectionItem::ShowSelectionPosition(WorldPoint & cOutLocation) const
{
	if (nullptr == m_pcImpl) {
		return false;
	}

	SelectionItemPrivate * pcImpl = (SelectionItemPrivate *)m_pcImpl;
	cOutLocation = pcImpl->cWorldPoint;

	return true;
}
//== SelectionResultsIterator Class ================================================================
SelectionResultsIterator::SelectionResultsIterator()
{
	m_pcImpl = new SelectionResultsIteratorPrivate();
}

SelectionResultsIterator::SelectionResultsIterator(SelectionResultsIterator const & cInThat)
{
	m_pcImpl = new SelectionResultsIteratorPrivate();
	Set(cInThat);
}

void SelectionResultsIterator::Set(SelectionResultsIterator const & cInThat)
{
	SelectionResultsIteratorPrivate * pcImpl = (SelectionResultsIteratorPrivate *)m_pcImpl;
	SelectionResultsIteratorPrivate * pcInThatImpl = (SelectionResultsIteratorPrivate *)cInThat.m_pcImpl;

	pcImpl->Copy(pcInThatImpl);
}

SelectionResultsIterator & SelectionResultsIterator::operator=(SelectionResultsIterator const & cInThat)
{
	Set(cInThat);
	return *this;
}

void SelectionResultsIterator::Next()
{
	SelectionResultsIteratorPrivate * pcImpl = (SelectionResultsIteratorPrivate *)m_pcImpl;
	++pcImpl->pcIterator;
}

SelectionResultsIterator & SelectionResultsIterator::operator++()
{
	Next();
	return *this;
}

SelectionResultsIterator & SelectionResultsIterator::operator++(int nInVal)
{
	SelectionResultsIteratorPrivate * pcImpl = (SelectionResultsIteratorPrivate *)m_pcImpl;
	std::advance(pcImpl->pcIterator, nInVal);
	return *this;
}

bool SelectionResultsIterator::operator == (SelectionResultsIterator const & cInSearchResultsIterator)
{
	SelectionResultsIteratorPrivate * pcImpl = (SelectionResultsIteratorPrivate *)m_pcImpl;
	SelectionResultsIteratorPrivate * pcInThatImpl = (SelectionResultsIteratorPrivate *)cInSearchResultsIterator.m_pcImpl;
	if(pcImpl->pcIterator != pcInThatImpl->pcIterator) {
		return false;
	}

	if(pcImpl->pcBeginIterator != pcInThatImpl->pcBeginIterator) {
		return false;
	}

	if(pcImpl->pcEndIterator != pcInThatImpl->pcEndIterator) {
		return false;
	}

	return true;
}

bool SelectionResultsIterator::operator != (SelectionResultsIterator const & cInSearchResultsIterator)
{
	return !(*this == cInSearchResultsIterator);
}

bool SelectionResultsIterator::IsValid() const
{
	SelectionResultsIteratorPrivate * pcImpl = (SelectionResultsIteratorPrivate *)m_pcImpl;
	return pcImpl->pcIterator != pcImpl->pcEndIterator;
}

void SelectionResultsIterator::Reset()
{
	SelectionResultsIteratorPrivate * pcImpl = (SelectionResultsIteratorPrivate *)m_pcImpl;
	pcImpl->pcIterator = pcImpl->pcBeginIterator;
}

SelectionItem * SelectionResultsIterator::GetItem() const
{
	SelectionResultsIteratorPrivate * pcImpl = (SelectionResultsIteratorPrivate *)m_pcImpl;
	return *pcImpl->pcIterator;
}

SelectionItem * SelectionResultsIterator::operator * () const
{
	return GetItem();
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

	if (pcImpl->GetItems().size() != pcInThatImpl->GetItems().size()) {
		return false;
	}

	bool bSameFlag = false;

	for (auto pcItem : pcImpl->GetItems()) {
		for (auto pcInThatItem : pcInThatImpl->GetItems()) {
			if (*pcItem == *pcInThatItem) {
				bSameFlag = true;
			}
			else {
				return false;
			}
		}
	}

	return bSameFlag;
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

	for (auto pcItem : pcImpl->GetItems()) {
		delete pcItem;
	}

	pcImpl->Clear();
}

size_t SelectionResults::GetCount() const
{
	if (nullptr == m_pcImpl) {
		return 0;
	}

	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;
	return  pcImpl->Size();
}

SelectionResultsIterator SelectionResults::GetIterator() const
{
	SelectionResultsIterator cIterator;
	SelectionResultsIteratorPrivate * pcIteratorImpl = (SelectionResultsIteratorPrivate *)cIterator.GetImpl();

	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;
	pcIteratorImpl->pcBeginIterator = pcImpl->Begin();
	pcIteratorImpl->pcEndIterator = pcImpl->End();
	pcIteratorImpl->pcIterator = pcIteratorImpl->pcBeginIterator;

	return cIterator;
}

SelectionItem * SelectionResults::Front()
{
	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;
	return pcImpl->Front();
}

SelectionItem * SelectionResults::Front() const
{
	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;
	return pcImpl->Front();
}

void SelectionResults::PushBack(SelectionItem * pcInItem)
{
	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;
	pcImpl->PushBack(pcInItem);
}

// 내부 요소가 Size보다 큰 경우 Size 보다 큰 부분은 삭제한다.
void SelectionResults::SetSize(size_t nInSize)
{
	if (nullptr == m_pcImpl) {
		return;
	}

	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;
	
	size_t nIndex = 0;
	for (auto pcItem : pcImpl->GetItems()) {
		nIndex++;
		if (nIndex <= nInSize) {
			continue;
		}

		delete pcItem;
	}

	pcImpl->Resize(nInSize);

/*
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
	}*/
}

// 들어오는 SelectionResults 값을 추가시킨다. 
bool SelectionResults::Union(SelectionResults const & cInThat)
{
	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;
	SelectionResultsPrivate * pcInThatImpl = (SelectionResultsPrivate *)cInThat.m_pcImpl;

	if (nullptr == pcImpl || nullptr == pcInThatImpl) {
		return false;
	}

	if (true == pcInThatImpl->Empty()) {
		return false;
	}

	for (auto pcInThatItem : pcInThatImpl->GetItems()) {
		bool bFindFlag = false;

		for (auto pcItemIter = pcImpl->Begin(); pcItemIter != pcImpl->End();) {
			Key cItemKey, cInThatItemKey;
			(*pcItemIter)->ShowSelectedItem(cItemKey);
			pcInThatItem->ShowSelectedItem(cInThatItemKey);

			// 들어온 요소에 대해서 기존에 있는 요소와 비교해서 같은 것이 있으면 삭제한다.
			if (*(*pcItemIter) == *pcInThatItem) {
				//요소 삭제 후, 다음 iterator 반환
				pcItemIter = pcImpl->Erase(pcItemIter);
			}
			else {
				++pcItemIter; // 다음 요소로 이동
			}
		}
	}
/*

	// 기존에 List에서 같은 값이 있으면 삭제한다.
	// 새롭게 들어오는 항목이 앞쪽에 있도록 정렬하기 위함.
	POSITION pcInThatPosition = pcInThatImpl->aItemList.GetHeadPosition();
	while (nullptr != pcInThatPosition)
	{
		SelectionItem * pcInThatItem = pcInThatImpl->aItemList.GetNext(pcInThatPosition);

		POSITION pcCurrentPosition = nullptr;
		POSITION pcPosition = pcImpl->aItemList.GetHeadPosition();

		bool bFindFlag = false;
		while (nullptr != pcPosition) {
			
			pcCurrentPosition = pcPosition;
			SelectionItem * pcItem = pcImpl->aItemList.GetNext(pcPosition);

			Key cItemKey, cInThatItemKey;
			pcItem->ShowSelectedItem(cItemKey);
			pcInThatItem->ShowSelectedItem(cInThatItemKey);

			TRACE(L"Item Key: %d, InThat Item Key: %d\n", cItemKey.KeyValue(), cInThatItemKey.KeyValue());

			// 들어온 요소에 대해서 기존에 있는 요소와 비교해서 같은 것이 있으면 삭제한다.
			if (*pcItem == *pcInThatItem) {
				pcImpl->aItemList.RemoveAt(pcCurrentPosition);
			}
		}
	}*/

	for (auto pcInThatItem : pcInThatImpl->GetItems()) {
		SelectionItem * pcNewItem = new SelectionItem(*pcInThatItem);
		pcImpl->PushFront(pcNewItem);
	}
/*

	pcInThatPosition = pcInThatImpl->aItemList.GetHeadPosition();
	while (nullptr != pcInThatPosition)
	{
		SelectionItem * pcInThatItem = pcInThatImpl->aItemList.GetNext(pcInThatPosition);
		SelectionItem * pcNewItem = new SelectionItem(*pcInThatItem);
		pcImpl->aItemList.AddHead(pcNewItem);
	}
*/

	return true;
}

void SelectionResults::LeaveType(DWORD nType)
{
	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;
	for (auto pcItemIter = pcImpl->Begin(); pcItemIter != pcImpl->End();) {
		Key cItemKey;
		if (true == (*pcItemIter)->ShowSelectedItem(cItemKey)) {
			DWORD nItemType = (DWORD)cItemKey.Type();
			// 원하는 Type이면 삭제하지 않는다.
			if (nType == nItemType) {
				++pcItemIter; // 다음 요소로 이동
				continue;
			}

			//요소 삭제 후, 다음 iterator 반환
			pcItemIter = pcImpl->Erase(pcItemIter);
		}
		else {
			++pcItemIter; // 다음 요소로 이동
		}
	}
}

void SelectionResults::RemoveType(DWORD nType)
{
	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;
	for (auto pcItemIter = pcImpl->Begin(); pcItemIter != pcImpl->End();) {
		Key cItemKey;
		if (true == (*pcItemIter)->ShowSelectedItem(cItemKey)) {
			DWORD nItemType = (DWORD)cItemKey.Type();
			// 원하는 Type이면 삭제한다.
			if (nItemType == (nType & nItemType)) {
				pcItemIter = pcImpl->Erase(pcItemIter);
			}
			else {
				++pcItemIter; // 다음 요소로 이동
			}
		}
		else {
			++pcItemIter; // 다음 요소로 이동
		}
	}
}

// Selection Item point의 Z값을 이용해서 정렬한다.
bool SelectionResults::Sort()
{
	SelectionResultsPrivate * pcImpl = (SelectionResultsPrivate *)m_pcImpl;
	return pcImpl->Sort();
}

//== SelectionControl Class ========================================================================
H3DF::SelectionControl::SelectionControl(WindowKey const & cInWindow)
{
	SelectionControlPrivate * pcImpl = new SelectionControlPrivate();
	pcImpl->m_pcWindow = &cInWindow;
	m_pcImpl = pcImpl;
}

H3DF::SelectionControl::SelectionControl(SelectionControl const & cInThat)
{
	m_pcImpl = new SelectionControlPrivate();
	Set(cInThat);
}

H3DF::SelectionControl::SelectionControl() {}

H3DF::SelectionControl::~SelectionControl()
{
}

void H3DF::SelectionControl::Set(SelectionControl const & cInThat)
{
	SelectionControlPrivate * pcImpl = (SelectionControlPrivate *)m_pcImpl;
	SelectionControlPrivate * pcInThatImpl = (SelectionControlPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

SelectionControl & H3DF::SelectionControl::operator =(SelectionControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

size_t H3DF::SelectionControl::SelectByPoint(HEventInfo & cEvent, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults) const
{
	Point cInLocation;
	cInLocation.x = cEvent.GetMouseWindowPos().x;
	cInLocation.y = cEvent.GetMouseWindowPos().y;

	//return SelectByPoint(cInLocation, cEvent.GetFlags(), cInOptions, cOutResults);
	return SelectByPoint(cInLocation, cInOptions, cOutResults);
}

size_t H3DF::SelectionControl::SelectByPoint(HEventInfo & cEvent, SelectionResults & cOutResults) const
{
	Point cInLocation;
	cInLocation.x = cEvent.GetMouseWindowPos().x;
	cInLocation.y = cEvent.GetMouseWindowPos().y;

	SelectionOptionsKit cInOptions;
	return SelectByPoint(cInLocation, cInOptions, cOutResults);
}

size_t H3DF::SelectionControl::SelectByPoint(Point const & cInLocation, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults) const
{
	SelectionControlPrivate * pcImpl = (SelectionControlPrivate *)m_pcImpl;
	return pcImpl->SelectByPoint(cInLocation, cInOptions, cOutResults);
}

size_t H3DF::SelectionControl::SelectByPoint(Point const & cInLocation, UINT const nFlags, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults) const
{
	return 0;
}

size_t H3DF::SelectionControl::SelectByPoint(Point const & cInLocation, UINT const nFlags, SelectionResults & cOutResults) const
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
