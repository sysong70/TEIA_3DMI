#include "StdAfx.h"

#include "Selection.h"
#include "Impl/Selection.Impl.h"

#include "Window.h"
#include "Impl/WindowImpl.h"

#include "../Sprocket/Impl/3DF.View.Impl.h"

#include "Line.h"

#include "3DF.Utility.h"

#include <vhash.h>
#include <vlist.h>

#include <atlcoll.h>

#include <HBaseView.h>
#include <HUtility.h>
#include <HTools.h>
#include <HSelectionSet.h>

#include <HBaseOperator.h>
#include <HMarkupManager.h>
#include <HConstantFrameRate.h>

#include <ranges>

#define		SEGMENT_TYPE		1
#define		ENTITY_TYPE			2
#define		SUBENTITY_TYPE		3
#define		REGION_TYPE			4

using namespace H3DF;

//== SelectionOptionsKit Class =====================================================================
H3DF::SelectionOptionsKit::SelectionOptionsKit()
{
	m_pcImpl = std::make_unique<SelectionOptionsKitImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::SelectionOptionsKit::SelectionOptionsKit(SelectionOptionsKit const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::operator =(SelectionOptionsKit const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetProximity(float fInProximity)
{
	auto pcImpl = static_cast<SelectionOptionsKitImpl *>(m_pcImpl.get());
	pcImpl->fProximity = fInProximity;
	pcImpl->bProximity = true;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetLevel(Selection::Level eInLevel)
{
	auto pcImpl = static_cast<SelectionOptionsKitImpl *>(m_pcImpl.get());
	pcImpl->eLevel = eInLevel;
	pcImpl->bLevel = true;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetInternalLimit(size_t nInLimit)
{
	auto pcImpl = static_cast<SelectionOptionsKitImpl *>(m_pcImpl.get());
	pcImpl->nInternalLimit = nInLimit;
	pcImpl->bInternalLimit = true;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetRelatedLimit(size_t nInLimit)
{
	auto pcImpl = static_cast<SelectionOptionsKitImpl *>(m_pcImpl.get());
	pcImpl->nRelatedLimit = nInLimit;
	pcImpl->bRelatedLimit = true;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetSorting(Selection::Sorting eInSorting)
{
	auto pcImpl = static_cast<SelectionOptionsKitImpl *>(m_pcImpl.get());
	pcImpl->eSorting = eInSorting;
	pcImpl->bSorting = true;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetAlgorithm(Selection::Algorithm eInAlgorithm)
{
	auto pcImpl = static_cast<SelectionOptionsKitImpl *>(m_pcImpl.get());
	pcImpl->eAlgorithm = eInAlgorithm;
	pcImpl->bAlgorithm = true;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetGranularity(Selection::Granularity eInGranularity)
{
	auto pcImpl = static_cast<SelectionOptionsKitImpl *>(m_pcImpl.get());
	pcImpl->eGranularity = eInGranularity;
	pcImpl->bGranularity = true;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetBias(Selection::Bias eInBias)
{
	auto pcImpl = static_cast<SelectionOptionsKitImpl *>(m_pcImpl.get());
	pcImpl->eBias = eInBias;
	pcImpl->bBias = true;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetScope(SegmentKey const & cInStartSegment, bool bInScopeOnly)
{
	auto pcImpl = static_cast<SelectionOptionsKitImpl *>(m_pcImpl.get());
	pcImpl->cStartSegment = cInStartSegment;
	pcImpl->bScopeOnly = bInScopeOnly;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetScope(KeyPath const & cInStartPath, bool bInScopeOnly)
{
	auto pcImpl = static_cast<SelectionOptionsKitImpl *>(m_pcImpl.get());
	pcImpl->cStartPath = cInStartPath;
	pcImpl->bScopeOnly = bInScopeOnly;
	return *this;
}

bool H3DF::SelectionOptionsKit::ShowProximity(float & fOutProximity) const
{
	auto pcImpl = static_cast<SelectionOptionsKitImpl *>(m_pcImpl.get());
	if (false == pcImpl->bProximity) {
		return false;
	}

	fOutProximity = pcImpl->fProximity;
	return true;
}

bool H3DF::SelectionOptionsKit::ShowLevel(Selection::Level & eOutLevel) const
{
	auto pcImpl = static_cast<SelectionOptionsKitImpl *>(m_pcImpl.get());
	if (false == pcImpl->bLevel) {
		return false;
	}

	eOutLevel = pcImpl->eLevel;
	return true;
}

bool H3DF::SelectionOptionsKit::ShowInternalLimit(size_t & nOutLimit) const
{
	auto pcImpl = static_cast<SelectionOptionsKitImpl *>(m_pcImpl.get());
	if (false == pcImpl->bInternalLimit) {
		return false;
	}

	nOutLimit = pcImpl->nInternalLimit;
	return true;
}

bool H3DF::SelectionOptionsKit::ShowRelatedLimit(size_t & nOutLimit) const
{
	auto pcImpl = static_cast<SelectionOptionsKitImpl *>(m_pcImpl.get());
	if (false == pcImpl->bRelatedLimit) {
		return false;
	}

	nOutLimit = pcImpl->nRelatedLimit;
	return true;

}

bool H3DF::SelectionOptionsKit::ShowSorting(Selection::Sorting & eOutSorting) const
{
	auto pcImpl = static_cast<SelectionOptionsKitImpl *>(m_pcImpl.get());
	if (false == pcImpl->bSorting) {
		return false;
	}

	eOutSorting = pcImpl->eSorting;
	return true;
}

bool H3DF::SelectionOptionsKit::ShowAlgorithm(Selection::Algorithm & eOutAlgorithm) const
{
	auto pcImpl = static_cast<SelectionOptionsKitImpl *>(m_pcImpl.get());
	if (false == pcImpl->bAlgorithm) {
		return false;
	}

	eOutAlgorithm = pcImpl->eAlgorithm;
	return true;
}

bool H3DF::SelectionOptionsKit::ShowGranularity(Selection::Granularity & eOutGranularity) const
{
	auto pcImpl = static_cast<SelectionOptionsKitImpl *>(m_pcImpl.get());
	if (false == pcImpl->bGranularity) {
		return false;
	}

	eOutGranularity = pcImpl->eGranularity;
	return true;
}

bool H3DF::SelectionOptionsKit::ShowBias(Selection::Bias & eOutBias) const
{
	auto pcImpl = static_cast<SelectionOptionsKitImpl *>(m_pcImpl.get());
	if (false == pcImpl->bBias) {
		return false;
	}

	eOutBias = pcImpl->eBias;
	return true;
}

bool H3DF::SelectionOptionsKit::ShowScope(SegmentKey & cOutStartSegment, bool & bOutScopeOnly) const
{
	auto pcImpl = static_cast<SelectionOptionsKitImpl *>(m_pcImpl.get());

	if (false == pcImpl->bSetScopeFlag) {
		return false;
	}

	cOutStartSegment = pcImpl->cStartSegment;
	bOutScopeOnly = pcImpl->bScopeOnly;
	
	return true;
}

bool H3DF::SelectionOptionsKit::ShowScope(KeyPath & cOutStartPath, bool & bOutScopeOnly) const
{
	auto pcImpl = static_cast<SelectionOptionsKitImpl *>(m_pcImpl.get());

	if (false == pcImpl->bSetScopeFlag) {
		return false;
	}

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
	m_pcImpl = std::make_unique<SelectionOptionsControlImpl>();
	DEBUG_VALID(m_pcImpl);

	auto pcImpl = static_cast<SelectionOptionsControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_pcWindow = &cInWindow;
}

SelectionOptionsControl::SelectionOptionsControl(SelectionOptionsControl const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
}

SelectionOptionsControl::~SelectionOptionsControl()
{

}

SelectionOptionsControl & SelectionOptionsControl::operator =(SelectionOptionsControl const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::SetProximity(float fInProximity)
{
	auto pcImpl = static_cast<SelectionOptionsControlImpl *>(m_pcImpl.get());
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
	auto pcImpl = static_cast<SelectionOptionsControlImpl *>(m_pcImpl.get());

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
	auto pcImpl = static_cast<SelectionOptionsControlImpl *>(m_pcImpl.get());
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
	auto pcImpl = static_cast<SelectionOptionsControlImpl *>(m_pcImpl.get());
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
	auto pcImpl = static_cast<SelectionOptionsControlImpl *>(m_pcImpl.get());
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
	auto pcImpl = static_cast<SelectionOptionsControlImpl *>(m_pcImpl.get());
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
	auto pcImpl = static_cast<SelectionOptionsControlImpl *>(m_pcImpl.get());
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
	auto pcImpl = static_cast<SelectionOptionsControlImpl *>(m_pcImpl.get());
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
	auto pcImpl = static_cast<SelectionOptionsControlImpl *>(m_pcImpl.get());
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
	auto pcImpl = static_cast<SelectionOptionsControlImpl *>(m_pcImpl.get());
	HC_KEY nViewKey = pcImpl->GetWindow()->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		HC_Set_Heuristics("no internal selection limit");
	} HC_Close_Segment();

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::UnsetRelatedLimit()
{
	auto pcImpl = static_cast<SelectionOptionsControlImpl *>(m_pcImpl.get());
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
	auto pcImpl = static_cast<SelectionOptionsControlImpl *>(m_pcImpl.get());
	HC_KEY nViewKey = pcImpl->GetWindow()->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		HC_Set_Heuristics("no] visual selection");
	} HC_Close_Segment();

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::UnsetGranularity()
{
	auto pcImpl = static_cast<SelectionOptionsControlImpl *>(m_pcImpl.get());
	HC_KEY nViewKey = pcImpl->GetWindow()->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		HC_Set_Heuristics("no detail selection");
	} HC_Close_Segment();

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::UnsetBias()
{
	auto pcImpl = static_cast<SelectionOptionsControlImpl *>(m_pcImpl.get());
	HC_KEY nViewKey = pcImpl->GetWindow()->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		HC_Set_Heuristics("no selection bias");
	} HC_Close_Segment();

	return *this;
}

//== SelectionItem Class ===========================================================================
H3DF::SelectionItem::SelectionItem()
{
	m_pcImpl = std::make_unique<SelectionItemImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::SelectionItem::SelectionItem(SelectionItem const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
}

H3DF::Type H3DF::SelectionItem::ItemType() const
{
	auto pcImpl = static_cast<SelectionItemImpl *>(m_pcImpl.get());

	if (true == pcImpl->Types().empty()) {
		return H3DF::Type::None;
	}

	return pcImpl->Types().front();
}

SelectionItem & H3DF::SelectionItem::operator=(SelectionItem const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

bool H3DF::SelectionItem::operator==(SelectionItem const & cInThat) const
{
	if (nullptr == m_pcImpl) {
		return false;
	}

	auto pcImpl = static_cast<SelectionItemImpl *> (m_pcImpl.get());
	auto pcInThatImpl = static_cast<SelectionItemImpl *> (cInThat.m_pcImpl.get());

	if (true == pcImpl->Keys().empty() || true == pcInThatImpl->Keys().empty()) {
		return false;
	}

	if (pcImpl->Keys().front() != pcInThatImpl->Keys().front()) {
		return false;
	}

	if (pcImpl->Keys() != pcInThatImpl->Keys()) {
		return false;
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

	if (pcImpl->m_nRegion != pcInThatImpl->m_nRegion) {
		return false;
	}

	if (pcImpl->m_nLowest != pcInThatImpl->m_nLowest) {
		return false;
	}

	if (pcImpl->m_nHighest != pcInThatImpl->m_nHighest) {
		return false;
	}

	return true;
}

bool H3DF::SelectionItem::operator!=(SelectionItem const & cInThat) const
{
	return !(*this == cInThat);
}

void H3DF::SelectionItem::Reset()
{
	auto pcImpl = static_cast<SelectionItemImpl *> (m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->Reset();
}

void H3DF::SelectionItem::Reset() const
{
	auto pcImpl = static_cast<SelectionItemImpl *> (m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->Reset();
}

bool H3DF::SelectionItem::IsValid()
{
	auto pcImpl = static_cast<SelectionItemImpl *> (m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	if (true == pcImpl->Keys().empty()) {
		return false;
	}

	return (INVALID_KEY != pcImpl->Keys().front()) ? true : false;
}

bool H3DF::SelectionItem::IsValid() const
{
	auto pcImpl = static_cast<SelectionItemImpl *> (m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	if (true == pcImpl->Keys().empty()) {
		return false;
	}

	return (INVALID_KEY != pcImpl->Keys().front()) ? true : false;
}

bool H3DF::SelectionItem::ShowSelectedItem(Key & cOutSelection)
{
	if (nullptr == m_pcImpl) {
		return false;
	}

	auto pcImpl = static_cast<SelectionItemImpl *> (m_pcImpl.get());

	if (true == pcImpl->Keys().empty() || true == pcImpl->Types().empty()) {
		return false;
	}

	HC_KEY nKey = pcImpl->Keys().front();
	if (INVALID_KEY == nKey) {
		return false;
	}

	H3DF::Type eType = pcImpl->Types().front();

	if (H3DF::Type::LineKey == eType) {
		cOutSelection = LineKey(nKey);
	}
	else if (H3DF::Type::ShellKey == eType) {
		cOutSelection = ShellKey(nKey);
	}
	else if (H3DF::Type::SegmentKey == eType) {
		cOutSelection = SegmentKey(nKey);
	}
	else {
		cOutSelection = H3DF::Key(nKey);
	}

	return true;
}

const bool H3DF::SelectionItem::ShowSelectedItem(Key & cOutSelection) const
{
	if (nullptr == m_pcImpl) {
		return false;
	}

	auto pcImpl = static_cast<SelectionItemImpl *> (m_pcImpl.get());

	if (true == pcImpl->Keys().empty()) {
		return false;
	}

	cOutSelection = pcImpl->Keys().front();

	if (INVALID_KEY == cOutSelection.KeyValue()) {
		return false;
	}

	return true;
}

bool H3DF::SelectionItem::ShowPath(KeyPath & cOutPath) const
{
	auto pcImpl = static_cast<SelectionItemImpl *> (m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	if (true == pcImpl->Keys().empty()) {
		return false;
	}

	HC_KEY nKey = pcImpl->Keys().front();
	if(INVALID_KEY == nKey) {
		return false;
	}

	KeyArray cKeys;

	for (auto nKey : pcImpl->Keys()) {
		cKeys.emplace_back(nKey);
	}

	cOutPath = KeyPath(cKeys);

	return true;
}

bool H3DF::SelectionItem::ShowSelectionPosition(WindowPoint & cOutLocation) const
{
	if (nullptr == m_pcImpl) {
		return false;
	}

	auto pcImpl = static_cast<SelectionItemImpl *> (m_pcImpl.get());
	cOutLocation = pcImpl->m_cWindowPoint;

	return true;
}

bool H3DF::SelectionItem::ShowSelectionPosition(WorldPoint & cOutLocation) const
{
	if (nullptr == m_pcImpl) {
		return false;
	}

	auto pcImpl = static_cast<SelectionItemImpl *> (m_pcImpl.get());
	cOutLocation = pcImpl->m_cWorldPoint;

	return true;
}

bool H3DF::SelectionItem::KeyFront(Key & cInKey, H3DF::Type eInType)
{
	return KeyFront(cInKey.KeyValue(), eInType);
}

bool H3DF::SelectionItem::KeyFront(HC_KEY nInKey, H3DF::Type eInType)
{
	auto pcImpl = static_cast<SelectionItemImpl *> (m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->Keys().front() = nInKey;

	if (H3DF::Type::None == eInType) {
		char chType[MVO_BUFFER_SIZE];
		HC_Show_Key_Type(nInKey, chType);

		eInType = H3DF::Utility::GetType(chType);
	}

	pcImpl->Types().front() = eInType;

	return true;
}

bool H3DF::SelectionItem::KeyPushBack(Key & cInKey, H3DF::Type eInType)
{
	return KeyPushBack(cInKey.KeyValue(), eInType);
}

bool H3DF::SelectionItem::KeyPushBack(HC_KEY nInKey, H3DF::Type eInType)
{
	auto pcImpl = static_cast<SelectionItemImpl *> (m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->Keys().push_back(nInKey);

	if (H3DF::Type::None == eInType) {
		char chType[MVO_BUFFER_SIZE];
		HC_Show_Key_Type(nInKey, chType);

		eInType = H3DF::Utility::GetType(chType);
	}

	pcImpl->Types().push_back(eInType);

	return true;
}

//== SelectionResultsIterator Class ================================================================
SelectionResultsIterator::SelectionResultsIterator()
{
	m_pcImpl = std::make_unique<SelectionResultsIteratorImpl>();
	DEBUG_VALID(m_pcImpl);
}

SelectionResultsIterator::SelectionResultsIterator(SelectionResultsIterator const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
}

SelectionResultsIterator & SelectionResultsIterator::operator=(SelectionResultsIterator const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

void SelectionResultsIterator::Next()
{
	auto pcImpl = static_cast<SelectionResultsIteratorImpl *>(m_pcImpl.get());
	++pcImpl->pcIterator;
}

SelectionResultsIterator & SelectionResultsIterator::operator++()
{
	Next();
	return *this;
}

SelectionResultsIterator & SelectionResultsIterator::operator++(int nInVal)
{
	auto pcImpl = static_cast<SelectionResultsIteratorImpl *>(m_pcImpl.get());
	std::advance(pcImpl->pcIterator, nInVal);
	return *this;
}

bool SelectionResultsIterator::operator == (SelectionResultsIterator const & cInSearchResultsIterator)
{
	auto pcImpl = static_cast<SelectionResultsIteratorImpl *>(m_pcImpl.get());
	auto pcInThatImpl = static_cast<SelectionResultsIteratorImpl *>(cInSearchResultsIterator.m_pcImpl.get());

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
	auto pcImpl = static_cast<SelectionResultsIteratorImpl *>(m_pcImpl.get());
	return pcImpl->pcIterator != pcImpl->pcEndIterator;
}

void SelectionResultsIterator::Reset()
{
	auto pcImpl = static_cast<SelectionResultsIteratorImpl *>(m_pcImpl.get());
	pcImpl->pcIterator = pcImpl->pcBeginIterator;
}

SelectionItem & SelectionResultsIterator::GetItem() const
{
	auto pcImpl = static_cast<SelectionResultsIteratorImpl *>(m_pcImpl.get());
	return *pcImpl->pcIterator;
}

SelectionItem & SelectionResultsIterator::operator * () const
{
	return GetItem();
}

//== SelectionResults Class ========================================================================
H3DF::SelectionResults::SelectionResults()
{
	m_pcImpl = std::make_unique<SelectionResultsImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::SelectionResults::SelectionResults(SelectionResults const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
}

H3DF::SelectionResults::~SelectionResults()
{
	Reset();
}

SelectionResults & H3DF::SelectionResults::operator=(SelectionResults const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

H3DF::SelectionResults::SelectionResults(SelectionResults && cInThat) noexcept :
	Object(cInThat)
{

}

SelectionResults & H3DF::SelectionResults::operator = (SelectionResults && cInThat) noexcept
{
	this->Object::operator = (std::move(cInThat));
	return *this;
}

bool H3DF::SelectionResults::operator==(SelectionResults const & cInThat) const
{
	auto pcImpl = static_cast<SelectionResultsImpl *>(m_pcImpl.get());
	auto pcInThatImpl = static_cast<SelectionResultsImpl *>(cInThat.m_pcImpl.get());

	if (pcImpl->GetItems().size() != pcInThatImpl->GetItems().size()) {
		return false;
	}

	bool bSameFlag = false;

	for (auto cItem : pcImpl->GetItems()) {
		for (auto cInThatItem : pcInThatImpl->GetItems()) {
			if (cItem == cInThatItem) {
				bSameFlag = true;
			}
			else {
				return false;
			}
		}
	}

	return bSameFlag;
}

bool H3DF::SelectionResults::operator!=(SelectionResults const & cInThat) const
{
	if (*this == cInThat) {
		return false;
	}

	return true;
}

// 결과값을 삭제한다.
void H3DF::SelectionResults::Reset()
{
	if (nullptr == m_pcImpl) {
		return;
	}

	auto pcImpl = static_cast<SelectionResultsImpl *>(m_pcImpl.get());
	pcImpl->Clear();
}

void H3DF::SelectionResults::Reset() const
{
	if (nullptr == m_pcImpl) {
		return;
	}

	auto pcImpl = static_cast<SelectionResultsImpl *>(m_pcImpl.get());
	pcImpl->Clear();
}

size_t H3DF::SelectionResults::GetCount() const
{
	if (nullptr == m_pcImpl) {
		return 0;
	}

	auto pcImpl = static_cast<SelectionResultsImpl *>(m_pcImpl.get());
	return  pcImpl->Size();
}

SelectionResultsIterator H3DF::SelectionResults::GetIterator() const
{
	SelectionResultsIterator cIterator;
	auto pcIteratorImpl = static_cast<SelectionResultsIteratorImpl *>(cIterator.GetImpl());
	DEBUG_VALID(pcIteratorImpl);

	auto pcImpl = static_cast<SelectionResultsImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcIteratorImpl->pcBeginIterator = pcImpl->Begin();
	pcIteratorImpl->pcEndIterator = pcImpl->End();
	pcIteratorImpl->pcIterator = pcIteratorImpl->pcBeginIterator;

	return cIterator;
}

SelectionItem & H3DF::SelectionResults::Front()
{
	auto pcImpl = static_cast<SelectionResultsImpl *>(m_pcImpl.get());
	return pcImpl->Front();
}

SelectionItem & H3DF::SelectionResults::Front() const
{
	auto pcImpl = static_cast<SelectionResultsImpl *>(m_pcImpl.get());
	return pcImpl->Front();
}

void H3DF::SelectionResults::PushFront(SelectionItem & cInItem)
{
	auto pcImpl = static_cast<SelectionResultsImpl *>(m_pcImpl.get());
	pcImpl->PushFront(cInItem);
}

void H3DF::SelectionResults::PushBack(SelectionItem & cInItem)
{
	auto pcImpl = static_cast<SelectionResultsImpl *>(m_pcImpl.get());
	pcImpl->PushBack(cInItem);
}

bool H3DF::SelectionResults::Erase(SelectionItem & cInItem)
{
	auto pcImpl = static_cast<SelectionResultsImpl *>(m_pcImpl.get());
	for (auto pcItemIter = pcImpl->Begin(); pcItemIter != pcImpl->End();) {
		if (*pcItemIter == cInItem) {
			pcItemIter = pcImpl->Erase(pcItemIter);
			return true;
		}
		else {
			++pcItemIter; // 다음 요소로 이동
		}
	}

	return false;
}

bool H3DF::SelectionResults::Erase(SelectionResults const & cInResults)
{
	auto pcImpl = static_cast<SelectionResultsImpl *>(m_pcImpl.get());
	auto pcInThatImpl = static_cast<SelectionResultsImpl *>(cInResults.m_pcImpl.get());

	if (nullptr == pcImpl || nullptr == pcInThatImpl) {
		return false;
	}

	for (auto & cInThatItem : pcInThatImpl->GetItems()) {
		for (auto pcItemIter = pcImpl->Begin(); pcItemIter != pcImpl->End();) {
			if (*pcItemIter == cInThatItem) {
				pcItemIter = pcImpl->Erase(pcItemIter);
			}
			else {
				++pcItemIter; // 다음 요소로 이동
			}
		}
	}

	return true;
}

// 내부 요소가 Size보다 큰 경우 Size 보다 큰 부분은 삭제한다.
void H3DF::SelectionResults::SetSize(size_t nInSize)
{
	if (nullptr == m_pcImpl) {
		return;
	}

	auto pcImpl = static_cast<SelectionResultsImpl *>(m_pcImpl.get());
	
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
bool H3DF::SelectionResults::Union(SelectionResults const & cInThat)
{
	auto pcImpl = static_cast<SelectionResultsImpl *>(m_pcImpl.get());
	auto pcInThatImpl = static_cast<SelectionResultsImpl *>(cInThat.m_pcImpl.get());

	if (nullptr == pcImpl || nullptr == pcInThatImpl) {
		return false;
	}

	if (true == pcInThatImpl->Empty()) {
		return false;
	}

	for (auto cInThatItem : pcInThatImpl->GetItems()) {
		bool bFindFlag = false;

		for (auto pcItemIter = pcImpl->Begin(); pcItemIter != pcImpl->End();) {
			Key cItemKey, cInThatItemKey;
			pcItemIter->ShowSelectedItem(cItemKey);
			cInThatItem.ShowSelectedItem(cInThatItemKey);

			// 들어온 요소에 대해서 기존에 있는 요소와 비교해서 같은 것이 있으면 삭제한다.
			if (*pcItemIter == cInThatItem) {
				//요소 삭제 후, 다음 iterator 반환
				pcItemIter = pcImpl->Erase(pcItemIter);
			}
			else {
				++pcItemIter; // 다음 요소로 이동
			}
		}
	}

	for (auto cInThatItem : pcInThatImpl->GetItems()) {
		pcImpl->PushFront(cInThatItem);
	}

	return true;
}

void H3DF::SelectionResults::LeaveType(DWORD nType)
{
	auto pcImpl = static_cast<SelectionResultsImpl *>(m_pcImpl.get());
	for (auto pcItemIter = pcImpl->Begin(); pcItemIter != pcImpl->End();) {
		Key cItemKey;
		if (true == pcItemIter->ShowSelectedItem(cItemKey)) {
			DWORD nItemType = (DWORD)cItemKey.ObjectType();
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

void H3DF::SelectionResults::RemoveType(DWORD nType)
{
	auto pcImpl = static_cast<SelectionResultsImpl *>(m_pcImpl.get());
	for (auto pcItemIter = pcImpl->Begin(); pcItemIter != pcImpl->End();) {
		Key cItemKey;
		if (true == pcItemIter->ShowSelectedItem(cItemKey)) {
			DWORD nItemType = (DWORD)cItemKey.ObjectType();
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
bool H3DF::SelectionResults::Sort()
{
	auto pcImpl = static_cast<SelectionResultsImpl *>(m_pcImpl.get());
	return pcImpl->Sort();
}

bool H3DF::SelectionResults::IsExist(SelectionItem & cInItem)
{
	auto pcImpl = static_cast<SelectionResultsImpl *>(m_pcImpl.get());
	for (auto pcItemIter = pcImpl->Begin(); pcItemIter != pcImpl->End();) {

		if (*pcItemIter == cInItem) {
			return true;
		}

		++pcItemIter; // 다음 요소로 이동
	}

	return false;
}

//== SelectionControl Class ========================================================================
H3DF::SelectionControl::SelectionControl(WindowKey const & cInWindow)
{
	m_pcImpl = std::make_unique<SelectionControlImpl>();
	DEBUG_VALID(m_pcImpl);

	auto pcImpl = static_cast<SelectionControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_cWindow = cInWindow;
}

H3DF::SelectionControl::SelectionControl(SelectionControl const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
}

H3DF::SelectionControl::SelectionControl() {}

H3DF::SelectionControl::~SelectionControl()
{
}

SelectionControl & H3DF::SelectionControl::operator =(SelectionControl const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

// 1. 주어진 Point와 Selection Option을 이용해서 선택 작업을 수행하고, 선택된 요소를 SelectionResults에 저장한다.
size_t H3DF::SelectionControl::SelectByPoint(Point const & cInLocation, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults) const
{
	auto pcSelCtrlImpl = static_cast<SelectionControlImpl *>(m_pcImpl.get());

	int	 nResult = 0;

	char chAction[MVO_BUFFER_SIZE] = "v";

	// 선택 옵션을 문자열로 변환
	pcSelCtrlImpl->GetSelectOption(cInOptions, chAction);

	char chScope[MVO_BUFFER_SIZE] = "";
	pcSelCtrlImpl->GetScope(cInOptions, chScope);

	// 선택 옵션에 따라 선택 작업 실시
	if (0 < strlen(chScope)) {
		HC_Open_Segment(chScope); {
			HC_Set_Rendering_Options("attribute lock=(selectability)");
			HC_Set_Selectability("geometry = on");
			//nResult = HC_Compute_Selection(GetBaseView()->GetDriverPath(), ".", "v, selection level = entity, no related selection limit, visual selection = off", cInLocation.x, cInLocation.y);
			nResult = HC_Compute_Selection(pcSelCtrlImpl->GetBaseView()->GetDriverPath(), ".", chAction, cInLocation.x, cInLocation.y);
			HC_Set_Selectability("everything = off");
		} HC_Close_Segment();
	}
	else {
// 		float fProximity = 0.0;
// 		if (true == cInOptions.ShowProximity(fProximity)) {
// 			GetBaseView()->SetDefaultSelectionProximity(fProximity);
// 		}

		HC_Open_Segment_By_Key(pcSelCtrlImpl->GetBaseView()->GetViewKey()); {
			//nResult = HC_Compute_Selection(".", "./scene/overwrite", "v, selection level = entity", cInLocation.x, cInLocation.y);
			nResult = HC_Compute_Selection(".", "./scene/overwrite", chAction, cInLocation.x, cInLocation.y);
			//(pcSelection->GetSubwindowPenetration() ? "" : "./scene/overwrite"), chAction, cInLocation.x, cInLocation.y);
			//"v, selection level = entity, related selection limit = 0, selection sorting, internal selection limit = 0", cInLocation.x, cInLocation.y);
		} HC_Close_Segment();
	}


	if (nResult > 0) {
		pcSelCtrlImpl->SelectionResult(pcSelCtrlImpl, cOutResults);
		return cOutResults.GetCount();
	}

	return 0;

/*
	// 선택된 요소가 없음
	if (0 == nResult) {
		return 0;
	}

	HC_KEY  nSelectKey = INVALID_KEY;
	HC_KEY * pnSelectKeys = nullptr;
	HC_KEY * pnIncludeKeys = nullptr;
	int	eSelectedType = SelectionControlImpl::SelType::None;
	char chKeyType[MVO_BUFFER_SIZE];

	// 선택된 요소를 SelectionResults에 저장하기 위해서 새롭게 생성
	SelectionResultsImpl * pcResultsImpl = static_cast<SelectionResultsImpl *>(cOutResults.GetImpl());
	DEBUG_VALID(pcResultsImpl);

	do {
		// 선택된 요소를 저장하기 위해서 Item 생성
		SelectionItem cItem;
		SelectionItemImpl * pcItemImpl = static_cast<SelectionItemImpl *>(cItem.GetImpl());
		DEBUG_VALID(pcItemImpl);

		pcItemImpl->m_pcWindow = pcSelCtrlImpl->m_pcWindow;

		HC_Show_Selection_Element(&nSelectKey, &pcItemImpl->m_nOffset1, &pcItemImpl->m_nOffset2, &pcItemImpl->m_nOffset3);
		HC_Show_Selection_Original_Key(&nSelectKey);

		if (INVALID_KEY == nSelectKey) {
			DEBUG_STOP;
			continue;
		}

		HC_Show_Key_Type(nSelectKey, chKeyType);

		if (streq(chKeyType, "line") || streq(chKeyType, "polyline") || streq(chKeyType, "circular arc") || streq(chKeyType, "elliptical arc")) {
			eSelectedType = SelectionControlImpl::SelType::Line;
			pcItemImpl->m_cKey = LineKey(nSelectKey);
		}
		else if (streq(chKeyType, "marker")) {
			eSelectedType = SelectionControlImpl::SelType::Marker;
		}
		else if (streq(chKeyType, "text leader")) {
			eSelectedType = SelectionControlImpl::SelType::Shell;	//?
			nSelectKey = HC_Show_Owner_Original_Key(nSelectKey);		// move up to text;
		}
		else {
			// This may be shell, mesh, cyliner, etc...
			eSelectedType = SelectionControlImpl::SelType::Shell;
			pcItemImpl->m_cKey = ShellKey(nSelectKey);

			// But if it really is a shell, check for regions.
			if (streq(chKeyType, "shell") && pcItemImpl->m_nOffset3 != -1) {

				int nRegion = 0;
				int nLowest = 0;
				int nHighest = 0;

				HC_Show_Region_Range(nSelectKey, &nLowest, &nHighest);

				if ((nLowest != nHighest || nLowest > 0)) {
					// eSelectedType |= SelectionControlImpl::SelType::Region;

					HC_Open_Geometry(nSelectKey); {
						HC_Open_Face(pcItemImpl->m_nOffset3); {
							HC_Show_Region(&nRegion);
						}HC_Close_Face();
					}HC_Close_Geometry();

					pcItemImpl->m_nRegion = nRegion;
					pcItemImpl->m_nLowest = nLowest;
					pcItemImpl->m_nHighest = nHighest;
				}
			}
		}

		WindowPoint cWindowPoint;
		WorldPoint cWorldPoint;
		HC_Show_Selection_Position(&cWindowPoint.x, &cWindowPoint.y, &cWindowPoint.z, &cWorldPoint.x, &cWorldPoint.y, &cWorldPoint.z);

		pcItemImpl->m_cWindowPoint = cWindowPoint;
		pcItemImpl->m_cWorldPoint = cWorldPoint;
		
		// build up an array of include keys to pass with the selection
		int nKeyCount = 0;
		HC_Show_Selection_Keys_Count(&nKeyCount);

		//TRACE(L"\nSelection_Keys_Count: %d", nKeyCount);

		if (0 < nKeyCount) {
			WindowKeyImpl * pcImpl = (WindowKeyImpl *) pcSelCtrlImpl->m_pcWindow->GetImpl();
			HC_KEY * pnKeys = pcImpl->GetSelectBufferKey(nKeyCount);

			pnIncludeKeys = new HC_KEY[nKeyCount];
			HC_Show_Selection_Original_Keys(&nKeyCount, pnKeys);

			// Include Key값을 찾아서 저장한다. 저장할 때는 역순으로 저장한다.
			for (int nIndex = nKeyCount - 1; nIndex >= 0; nIndex--)
			{
				HC_Show_Key_Type(pnKeys[nIndex], chKeyType);

				pcItemImpl->Keys().emplace_back(pnKeys[nIndex]);

				H3DF::Type eType = Utility::GetType(chKeyType);
				pcItemImpl->Types().emplace_back(eType);

				if (H3DF::Type::ReferenceKey == eType) {
					nSelectKey = pnKeys[nIndex];
				}
			}
		}

		pcResultsImpl->PushBack(cItem);

	} while (HC_Find_Related_Selection());

	return cOutResults.GetCount();
*/
}

size_t H3DF::SelectionControl::SelectByPoint(Point const & cInLocation, SelectionResults & cOutResults) const
{
	auto pcImpl = static_cast<SelectionControlImpl *>(m_pcImpl.get());

	SelectionOptionsKit cOptions;
	if(false == pcImpl->GetWindow().ShowSelectionOptions(cOptions)) {
		return 0;
	}

	return SelectByPoint(cInLocation, cOptions, cOutResults);
}

size_t H3DF::SelectionControl::SelectByPoint(Point const & cInLocation, UINT const nFlags, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults) const
{
	return 0;
}

size_t H3DF::SelectionControl::SelectByPoint(Point const & cInLocation, UINT const nFlags, SelectionResults & cOutResults) const
{
	auto pcSelCtrlImpl = static_cast<SelectionControlImpl *>(m_pcImpl.get());

	SelectionOptionsKit cInOptions;

	if (nullptr == pcSelCtrlImpl->m_pcSelectionSet) {
		return HOP_NOT_HANDLED;
	}

	HPoint  new_pos;
	int		nResult = 0;
	bool	bNeedUpdate = false;

	//HSelectionSet * pcSelection = GetBaseView()->GetSelection();

	// Markup 선택
	HMarkupManager * pcMarkupManager;
	if (nullptr != (pcMarkupManager = pcSelCtrlImpl->GetBaseView()->GetMarkupManager())) {
		HC_Open_Segment_By_Key(pcMarkupManager->GetMarkupKey()); {
			// compute the selection using the HOOPS window coordinate of the the pick location
			nResult = HC_Compute_Selection(pcSelCtrlImpl->GetBaseView()->GetDriverPath(),
				(pcSelCtrlImpl->SelectionSet()->GetSubwindowPenetration() ? "" : "."),
				//"v", cInLocation.x, cInLocation.y);
				"v, selection level = entity, selection sorting, internal selection limit = 0", cInLocation.x, cInLocation.y);
		} HC_Close_Segment();
	}

	if (nResult == 0) {
		HC_Open_Segment_By_Key(pcSelCtrlImpl->GetBaseView()->GetViewKey()); {
			nResult = HC_Compute_Selection(".",
				(pcSelCtrlImpl->SelectionSet()->GetSubwindowPenetration() ? "" : "./scene/overwrite"),
				//"v", cInLocation.x, cInLocation.y);
				"v, selection level = entity, related selection limit = 0, selection sorting, internal selection limit = 0", cInLocation.x, cInLocation.y);
		} HC_Close_Segment();
	}

	if (nResult > 0) {
		pcSelCtrlImpl->HandleSelection(nFlags, cOutResults);
		size_t nCount = cOutResults.GetCount();
		bNeedUpdate = true;
	}

	pcSelCtrlImpl->GetBaseView()->GetConstantFrameRateObject()->SetDisableIncreaseTemp(true);

	//GetBaseView()->SetGeometryChanged();
	if (bNeedUpdate) {
		pcSelCtrlImpl->GetBaseView()->Update();	// update the scene to reflect the new highlight attributes
	}

	pcSelCtrlImpl->GetBaseView()->GetConstantFrameRateObject()->SetDisableIncreaseTemp(false);

	if (pcSelCtrlImpl->GetBaseView()->GetFocusOnSelection()) {
		pcSelCtrlImpl->GetBaseView()->FocusOnSelection();
	}

	// of the selected items
	return HOP_READY;
}