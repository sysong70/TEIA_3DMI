#include "StdAfx.h"

#include "Selection.h"
#include "Impl/SelectionImpl.h"

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
	m_pcImpl = new SelectionOptionsKitImpl();
}

H3DF::SelectionOptionsKit::SelectionOptionsKit(SelectionOptionsKit const & cInThat)
{
	m_pcImpl = new SelectionOptionsKitImpl();
	Set(cInThat);
}

void H3DF::SelectionOptionsKit::Set(SelectionOptionsKit const & cInThat)
{
	SelectionOptionsKitImpl * pcImpl = (SelectionOptionsKitImpl *)m_pcImpl;
	SelectionOptionsKitImpl * pcInThatImpl = (SelectionOptionsKitImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::operator =(SelectionOptionsKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetProximity(float fInProximity)
{
	SelectionOptionsKitImpl * pcImpl = (SelectionOptionsKitImpl *)m_pcImpl;
	pcImpl->fProximity = fInProximity;
	pcImpl->bProximity = true;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetLevel(Selection::Level eInLevel)
{
	SelectionOptionsKitImpl * pcImpl = (SelectionOptionsKitImpl *)m_pcImpl;
	pcImpl->eLevel = eInLevel;
	pcImpl->bLevel = true;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetInternalLimit(size_t nInLimit)
{
	SelectionOptionsKitImpl * pcImpl = (SelectionOptionsKitImpl *)m_pcImpl;
	pcImpl->nInternalLimit = nInLimit;
	pcImpl->bInternalLimit = true;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetRelatedLimit(size_t nInLimit)
{
	SelectionOptionsKitImpl * pcImpl = (SelectionOptionsKitImpl *)m_pcImpl;
	pcImpl->nRelatedLimit = nInLimit;
	pcImpl->bRelatedLimit = true;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetSorting(Selection::Sorting eInSorting)
{
	SelectionOptionsKitImpl * pcImpl = (SelectionOptionsKitImpl *)m_pcImpl;
	pcImpl->eSorting = eInSorting;
	pcImpl->bSorting = true;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetAlgorithm(Selection::Algorithm eInAlgorithm)
{
	SelectionOptionsKitImpl * pcImpl = (SelectionOptionsKitImpl *)m_pcImpl;
	pcImpl->eAlgorithm = eInAlgorithm;
	pcImpl->bAlgorithm = true;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetGranularity(Selection::Granularity eInGranularity)
{
	SelectionOptionsKitImpl * pcImpl = (SelectionOptionsKitImpl *)m_pcImpl;
	pcImpl->eGranularity = eInGranularity;
	pcImpl->bGranularity = true;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetBias(Selection::Bias eInBias)
{
	SelectionOptionsKitImpl * pcImpl = (SelectionOptionsKitImpl *)m_pcImpl;
	pcImpl->eBias = eInBias;
	pcImpl->bBias = true;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetScope(SegmentKey const & cInStartSegment, bool bInScopeOnly)
{
	SelectionOptionsKitImpl * pcImpl = (SelectionOptionsKitImpl *)m_pcImpl;
	pcImpl->cStartSegment = cInStartSegment;
	pcImpl->bScopeOnly = bInScopeOnly;
	return *this;
}

SelectionOptionsKit & H3DF::SelectionOptionsKit::SetScope(KeyPath const & cInStartPath, bool bInScopeOnly)
{
	SelectionOptionsKitImpl * pcImpl = (SelectionOptionsKitImpl *)m_pcImpl;
	pcImpl->cStartPath = cInStartPath;
	pcImpl->bScopeOnly = bInScopeOnly;
	return *this;
}

bool H3DF::SelectionOptionsKit::ShowProximity(float & fOutProximity) const
{
	SelectionOptionsKitImpl * pcImpl = (SelectionOptionsKitImpl *)m_pcImpl;
	if (false == pcImpl->bProximity) {
		return false;
	}

	fOutProximity = pcImpl->fProximity;
	return true;
}

bool H3DF::SelectionOptionsKit::ShowLevel(Selection::Level & eOutLevel) const
{
	SelectionOptionsKitImpl * pcImpl = (SelectionOptionsKitImpl *)m_pcImpl;
	if (false == pcImpl->bLevel) {
		return false;
	}

	eOutLevel = pcImpl->eLevel;
	return true;
}

bool H3DF::SelectionOptionsKit::ShowInternalLimit(size_t & nOutLimit) const
{
	SelectionOptionsKitImpl * pcImpl = (SelectionOptionsKitImpl *)m_pcImpl;
	if (false == pcImpl->bInternalLimit) {
		return false;
	}

	nOutLimit = pcImpl->nInternalLimit;
	return true;
}

bool H3DF::SelectionOptionsKit::ShowRelatedLimit(size_t & nOutLimit) const
{
	SelectionOptionsKitImpl * pcImpl = (SelectionOptionsKitImpl *)m_pcImpl;
	if (false == pcImpl->bRelatedLimit) {
		return false;
	}

	nOutLimit = pcImpl->nRelatedLimit;
	return true;

}

bool H3DF::SelectionOptionsKit::ShowSorting(Selection::Sorting & eOutSorting) const
{
	SelectionOptionsKitImpl * pcImpl = (SelectionOptionsKitImpl *)m_pcImpl;
	if (false == pcImpl->bSorting) {
		return false;
	}

	eOutSorting = pcImpl->eSorting;
	return true;
}

bool H3DF::SelectionOptionsKit::ShowAlgorithm(Selection::Algorithm & eOutAlgorithm) const
{
	SelectionOptionsKitImpl * pcImpl = (SelectionOptionsKitImpl *)m_pcImpl;
	if (false == pcImpl->bAlgorithm) {
		return false;
	}

	eOutAlgorithm = pcImpl->eAlgorithm;
	return true;
}

bool H3DF::SelectionOptionsKit::ShowGranularity(Selection::Granularity & eOutGranularity) const
{
	SelectionOptionsKitImpl * pcImpl = (SelectionOptionsKitImpl *)m_pcImpl;
	if (false == pcImpl->bGranularity) {
		return false;
	}

	eOutGranularity = pcImpl->eGranularity;
	return true;
}

bool H3DF::SelectionOptionsKit::ShowBias(Selection::Bias & eOutBias) const
{
	SelectionOptionsKitImpl * pcImpl = (SelectionOptionsKitImpl *)m_pcImpl;
	if (false == pcImpl->bBias) {
		return false;
	}

	eOutBias = pcImpl->eBias;
	return true;
}

bool H3DF::SelectionOptionsKit::ShowScope(SegmentKey & cOutStartSegment, bool & bOutScopeOnly) const
{
	SelectionOptionsKitImpl * pcImpl = (SelectionOptionsKitImpl *)m_pcImpl;
	if (INVALID_KEY == pcImpl->cStartSegment.KeyValue()) {
		return false;
	}

	cOutStartSegment = pcImpl->cStartSegment;
	bOutScopeOnly = pcImpl->bScopeOnly;
	
	return true;
}

bool H3DF::SelectionOptionsKit::ShowScope(KeyPath & cOutStartPath, bool & bOutScopeOnly) const
{
	SelectionOptionsKitImpl * pcImpl = (SelectionOptionsKitImpl *)m_pcImpl;
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
	SelectionOptionsControlImpl * pcImpl = new SelectionOptionsControlImpl();
	pcImpl->m_pcWindow = &cInWindow;

	m_pcImpl = pcImpl;
}

SelectionOptionsControl::SelectionOptionsControl(SelectionOptionsControl const & cInThat)
{
	m_pcImpl = new SelectionOptionsControlImpl();
	Set(cInThat);
}

SelectionOptionsControl::~SelectionOptionsControl()
{

}

void SelectionOptionsControl::Set(SelectionOptionsControl const & cInThat)
{
	SelectionOptionsControlImpl * pcImpl = (SelectionOptionsControlImpl *)m_pcImpl;
	SelectionOptionsControlImpl * pcInThatImpl = (SelectionOptionsControlImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

SelectionOptionsControl & SelectionOptionsControl::operator =(SelectionOptionsControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::SetProximity(float fInProximity)
{
	SelectionOptionsControlImpl * pcImpl = (SelectionOptionsControlImpl *)m_pcImpl;
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
	SelectionOptionsControlImpl * pcImpl = (SelectionOptionsControlImpl *)m_pcImpl;

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
	SelectionOptionsControlImpl * pcImpl = (SelectionOptionsControlImpl *)m_pcImpl;
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
	SelectionOptionsControlImpl * pcImpl = (SelectionOptionsControlImpl *)m_pcImpl;
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
	SelectionOptionsControlImpl * pcImpl = (SelectionOptionsControlImpl *)m_pcImpl;
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
	SelectionOptionsControlImpl * pcImpl = (SelectionOptionsControlImpl *)m_pcImpl;
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
	SelectionOptionsControlImpl * pcImpl = (SelectionOptionsControlImpl *)m_pcImpl;
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
	SelectionOptionsControlImpl * pcImpl = (SelectionOptionsControlImpl *)m_pcImpl;
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
	SelectionOptionsControlImpl * pcImpl = (SelectionOptionsControlImpl *)m_pcImpl;
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
	SelectionOptionsControlImpl * pcImpl = (SelectionOptionsControlImpl *)m_pcImpl;
	HC_KEY nViewKey = pcImpl->GetWindow()->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		HC_Set_Heuristics("no internal selection limit");
	} HC_Close_Segment();

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::UnsetRelatedLimit()
{
	SelectionOptionsControlImpl * pcImpl = (SelectionOptionsControlImpl *)m_pcImpl;
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
	SelectionOptionsControlImpl * pcImpl = (SelectionOptionsControlImpl *)m_pcImpl;
	HC_KEY nViewKey = pcImpl->GetWindow()->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		HC_Set_Heuristics("no] visual selection");
	} HC_Close_Segment();

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::UnsetGranularity()
{
	SelectionOptionsControlImpl * pcImpl = (SelectionOptionsControlImpl *)m_pcImpl;
	HC_KEY nViewKey = pcImpl->GetWindow()->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		HC_Set_Heuristics("no detail selection");
	} HC_Close_Segment();

	return *this;
}

SelectionOptionsControl & SelectionOptionsControl::UnsetBias()
{
	SelectionOptionsControlImpl * pcImpl = (SelectionOptionsControlImpl *)m_pcImpl;
	HC_KEY nViewKey = pcImpl->GetWindow()->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(nViewKey); {
		HC_Set_Heuristics("no selection bias");
	} HC_Close_Segment();

	return *this;
}

//== SelectionItem Class ===========================================================================
H3DF::SelectionItem::SelectionItem()
{
	m_pcImpl = new SelectionItemImpl();
}

H3DF::SelectionItem::SelectionItem(SelectionItem const & cInThat)
{
	m_pcImpl = new SelectionItemImpl();
	Set(cInThat);
}

H3DF::Type H3DF::SelectionItem::Type() const
{
	SelectionItemImpl * pcImpl = (SelectionItemImpl *)m_pcImpl;
	return pcImpl->m_cKey.Type();
}

void H3DF::SelectionItem::Set(SelectionItem const & cInThat)
{
	SelectionItemImpl * pcImpl = (SelectionItemImpl *)m_pcImpl;
	SelectionItemImpl * pcInThatImpl = (SelectionItemImpl *)cInThat.m_pcImpl;
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

	SelectionItemImpl * pcImpl = (SelectionItemImpl *)m_pcImpl;
	SelectionItemImpl * pcInThatImpl = (SelectionItemImpl *)cInThat.m_pcImpl;

	if (pcImpl->m_cKey.KeyValue() != pcInThatImpl->m_cKey.KeyValue()) {
		return false;
	}

	if (pcImpl->m_vcIncludeKeys != pcInThatImpl->m_vcIncludeKeys) {
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
	auto * pcImpl = dynamic_cast<SelectionItemImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->Reset();
}

void H3DF::SelectionItem::Reset() const
{
	auto * pcImpl = dynamic_cast<SelectionItemImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->Reset();
}

bool H3DF::SelectionItem::IsValid()
{
	auto * pcImpl = dynamic_cast<SelectionItemImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);
	return (INVALID_KEY != pcImpl->m_cKey.KeyValue()) ? true : false;
}

bool H3DF::SelectionItem::IsValid() const
{
	auto * pcImpl = dynamic_cast<SelectionItemImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);
	return (INVALID_KEY != pcImpl->m_cKey.KeyValue()) ? true : false;
}

bool H3DF::SelectionItem::ShowSelectedItem(Key & cOutSelection)
{
	if (nullptr == m_pcImpl) {
		return false;
	}

	SelectionItemImpl * pcImpl = (SelectionItemImpl *)m_pcImpl;
	cOutSelection = pcImpl->m_cKey;

	if (INVALID_KEY == cOutSelection.KeyValue()) {
		return false;
	}

	return true;
}

const bool H3DF::SelectionItem::ShowSelectedItem(Key & cOutSelection) const
{
	if (nullptr == m_pcImpl) {
		return false;
	}

	SelectionItemImpl * pcImpl = (SelectionItemImpl *)m_pcImpl;
	cOutSelection = pcImpl->m_cKey;

	if (INVALID_KEY == cOutSelection.KeyValue()) {
		return false;
	}

	return true;
}

bool H3DF::SelectionItem::ShowPath(KeyPath & cOutPath) const
{
	SelectionItemImpl * pcImpl = (SelectionItemImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	HC_KEY nKey = pcImpl->m_cKey.KeyValue();
	if(INVALID_KEY == nKey) {
		return false;
	}

	KeyArray cKeys;

	char chType[MVO_BUFFER_SIZE];
	HC_Show_Key_Type(nKey, chType);

	if (!streq(chType, "segment")) {
		nKey = HC_KShow_Owner_Original_Key(nKey);
	}

	cKeys.emplace_back(nKey);

	for (auto & vcIncludeKey : std::ranges::reverse_view(pcImpl->m_vcIncludeKeys)) {
		cKeys.emplace_back(vcIncludeKey);
	}

/*
	HC_KEY nBackKey = cKeys.back().KeyValue();
	HC_Show_Key_Type(nBackKey, chType);

	SegmentKey cSegment(nBackKey);
	CStringA strName = cSegment.Name();

	CString strName1;
	UserData::ShowSegmentName(cSegment, strName1);
*/

	HC_KEY nBackKey = cKeys[cKeys.size() - 2].KeyValue();
	cKeys.back() = HC_KShow_Owner_Original_Key(nBackKey);

/*
	for (int nIndex = 1; nIndex < pcImpl->m_nIncludeCount; ++nIndex) {
		pnPath[nIndex] = pcImpl->m_pnIncludeKeys[pcImpl->m_nIncludeCount - nIndex];
	}

	pnPath[nPathCount - 1] = HC_KShow_Owner_Original_Key(pnPath[nPathCount - 2]);
*/

	cOutPath = KeyPath(cKeys);

	return true;
}

void H3DF::SelectionItem::ShowPathString(CString & strOutPath)
{
	SelectionItemImpl * pcImpl = (SelectionItemImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	KeyPath cPath;
	if (false == ShowPath(cPath)) {
		DEBUG_STOP;
		return;
	}

	CString strText;
	char chType[MVO_BUFFER_SIZE];

	HC_KEY nKey = pcImpl->m_cKey.KeyValue();
	HC_Show_Key_Type(nKey, chType);

	CString strName;

	if (streq("segment", chType)) {
		SegmentKey cSegment(nKey);
		if (false == UserData::ShowSegmentName(cSegment, strName)) {
			strName = cSegment.Name(false);
		}

		strText.Format(L"Select Key: %d [%s], [%s, %s]", nKey, Utility::ToString(chType), strName, CString(cSegment.Name(false)));
	}
	else {
		strText.Format(L"Select Key: %d [%s]", nKey, Utility::ToString(chType));
	}


	strOutPath += strText;

	if (nKey != cPath.At(0).KeyValue()) {
		nKey = cPath.At(0).KeyValue();
		HC_Show_Key_Type(nKey, chType);

		SegmentKey cSegment(nKey);
		if (false == UserData::ShowSegmentName(cSegment, strName)) {
			strName = cSegment.Name(false);
		}

		strText.Format(L"\nOwner of select key: %d [%s], %s", nKey, Utility::ToString(chType), strName);
		strOutPath += strText;
	}

	for (int nIndex = 1; nIndex < pcImpl->m_vcIncludeKeys.size(); ++nIndex) {
		nKey = cPath.At(nIndex).KeyValue();

		H3DF::Type eType = H3DF::Utility::GetType(nKey);

		SegmentKey cSegment;

		if (H3DF::Type::IncludeKey == eType) {
			IncludeKey cInclude(nKey);
			cSegment = cInclude.GetTarget();
		}
		else {
			cSegment = SegmentKey(nKey);
		}

		if (false == UserData::ShowSegmentName(cSegment, strName)) {
			strName = cSegment.Name(false);
		}

		if (H3DF::Type::IncludeKey == eType) {
			strText.Format(L"\nInclude: %d, Segment: %d [%s, %s]", nKey, cSegment.KeyValue(), strName, CString(cSegment.Name(false)));
		}
		else {
			strText.Format(L"\nSegment: %d, [%s, %s]", nKey, strName, CString(cSegment.Name(false)));
		}

		strOutPath += strText;
	}

	nKey = cPath.Back().KeyValue();
	H3DF::Type eType = H3DF::Utility::GetType(nKey);

	SegmentKey cSegmentKey1(nKey);
	strName = cSegmentKey1.Name();

	if (false == UserData::ShowSegmentName(cSegmentKey1, strName)) {
		strName = cSegmentKey1.Name(false);
	}

	if (H3DF::Type::IncludeKey == eType) {
		strText.Format(L"\nInclude: %d, Segment: %d [%s]", nKey, nKey, strName);
	}
	else {
		strText.Format(L"\nSegment: %d [%s]", nKey, strName);
	}

	strOutPath += strText;
}

void H3DF::SelectionItem::ShowSimplePathString(CString & strOutPath)
{
	SelectionItemImpl * pcImpl = (SelectionItemImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	KeyPath cPath;
	if (false == ShowPath(cPath)) {
		DEBUG_STOP;
		return;
	}

	CString strText;
	char chType[MVO_BUFFER_SIZE];

	HC_KEY nKey = pcImpl->m_cKey.KeyValue();
	HC_Show_Key_Type(nKey, chType);

	strText.Format(L"Select Key: %d [%s]", nKey, Utility::ToString(chType));
	
	strOutPath += strText;

	for (auto nIncludeKey : pcImpl->m_vcIncludeKeys) {
		HC_Show_Key_Type(nIncludeKey, chType);
		strText.Format(L"\nPath Key: %d [%s]", nIncludeKey, Utility::ToString(chType));
		strOutPath += strText;
	}
}

bool H3DF::SelectionItem::ShowSelectionPosition(WindowPoint & cOutLocation) const
{
	if (nullptr == m_pcImpl) {
		return false;
	}

	SelectionItemImpl * pcImpl = (SelectionItemImpl *)m_pcImpl;
	cOutLocation = pcImpl->m_cWindowPoint;

	return true;
}

bool H3DF::SelectionItem::ShowSelectionPosition(WorldPoint & cOutLocation) const
{
	if (nullptr == m_pcImpl) {
		return false;
	}

	SelectionItemImpl * pcImpl = (SelectionItemImpl *)m_pcImpl;
	cOutLocation = pcImpl->m_cWorldPoint;

	return true;
}
//== SelectionResultsIterator Class ================================================================
SelectionResultsIterator::SelectionResultsIterator()
{
	m_pcImpl = new SelectionResultsIteratorImpl();
}

SelectionResultsIterator::SelectionResultsIterator(SelectionResultsIterator const & cInThat)
{
	m_pcImpl = new SelectionResultsIteratorImpl();
	Set(cInThat);
}

void SelectionResultsIterator::Set(SelectionResultsIterator const & cInThat)
{
	SelectionResultsIteratorImpl * pcImpl = (SelectionResultsIteratorImpl *)m_pcImpl;
	SelectionResultsIteratorImpl * pcInThatImpl = (SelectionResultsIteratorImpl *)cInThat.m_pcImpl;

	pcImpl->Copy(pcInThatImpl);
}

SelectionResultsIterator & SelectionResultsIterator::operator=(SelectionResultsIterator const & cInThat)
{
	Set(cInThat);
	return *this;
}

void SelectionResultsIterator::Next()
{
	SelectionResultsIteratorImpl * pcImpl = (SelectionResultsIteratorImpl *)m_pcImpl;
	++pcImpl->pcIterator;
}

SelectionResultsIterator & SelectionResultsIterator::operator++()
{
	Next();
	return *this;
}

SelectionResultsIterator & SelectionResultsIterator::operator++(int nInVal)
{
	SelectionResultsIteratorImpl * pcImpl = (SelectionResultsIteratorImpl *)m_pcImpl;
	std::advance(pcImpl->pcIterator, nInVal);
	return *this;
}

bool SelectionResultsIterator::operator == (SelectionResultsIterator const & cInSearchResultsIterator)
{
	SelectionResultsIteratorImpl * pcImpl = (SelectionResultsIteratorImpl *)m_pcImpl;
	SelectionResultsIteratorImpl * pcInThatImpl = (SelectionResultsIteratorImpl *)cInSearchResultsIterator.m_pcImpl;
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
	SelectionResultsIteratorImpl * pcImpl = (SelectionResultsIteratorImpl *)m_pcImpl;
	return pcImpl->pcIterator != pcImpl->pcEndIterator;
}

void SelectionResultsIterator::Reset()
{
	SelectionResultsIteratorImpl * pcImpl = (SelectionResultsIteratorImpl *)m_pcImpl;
	pcImpl->pcIterator = pcImpl->pcBeginIterator;
}

SelectionItem & SelectionResultsIterator::GetItem() const
{
	SelectionResultsIteratorImpl * pcImpl = (SelectionResultsIteratorImpl *)m_pcImpl;
	return *pcImpl->pcIterator;
}

SelectionItem & SelectionResultsIterator::operator * () const
{
	return GetItem();
}

//== SelectionResults Class ========================================================================
H3DF::SelectionResults::SelectionResults()
{
	m_pcImpl = new SelectionResultsImpl();
}

H3DF::SelectionResults::SelectionResults(SelectionResults const & cInThat)
{
	m_pcImpl = new SelectionResultsImpl();
	Set(cInThat);
}

H3DF::SelectionResults::~SelectionResults()
{
	Reset();
}

void H3DF::SelectionResults::Set(SelectionResults const & cInThat)
{
	// 복사하기 전에 기존의 결과값을 삭제한다.
	Reset();

	SelectionResultsImpl * pcImpl = (SelectionResultsImpl *)m_pcImpl;
	SelectionResultsImpl * pcInThatImpl = (SelectionResultsImpl *)cInThat.m_pcImpl;

	pcImpl->Copy(pcInThatImpl);
}

SelectionResults & H3DF::SelectionResults::operator=(SelectionResults const & cInThat)
{
	Set(cInThat);
	return *this;
}

bool H3DF::SelectionResults::operator==(SelectionResults const & cInThat) const
{
	SelectionResultsImpl * pcImpl = (SelectionResultsImpl *)m_pcImpl;
	SelectionResultsImpl * pcInThatImpl = (SelectionResultsImpl *)cInThat.m_pcImpl;

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

	SelectionResultsImpl * pcImpl = (SelectionResultsImpl *)m_pcImpl;
	pcImpl->Clear();
}

void H3DF::SelectionResults::Reset() const
{
	if (nullptr == m_pcImpl) {
		return;
	}

	SelectionResultsImpl * pcImpl = (SelectionResultsImpl *) m_pcImpl;
	pcImpl->Clear();
}

size_t H3DF::SelectionResults::GetCount() const
{
	if (nullptr == m_pcImpl) {
		return 0;
	}

	SelectionResultsImpl * pcImpl = (SelectionResultsImpl *)m_pcImpl;
	return  pcImpl->Size();
}

SelectionResultsIterator H3DF::SelectionResults::GetIterator() const
{
	SelectionResultsIterator cIterator;
	SelectionResultsIteratorImpl * pcIteratorImpl = (SelectionResultsIteratorImpl *)cIterator.GetImpl();

	SelectionResultsImpl * pcImpl = (SelectionResultsImpl *)m_pcImpl;
	pcIteratorImpl->pcBeginIterator = pcImpl->Begin();
	pcIteratorImpl->pcEndIterator = pcImpl->End();
	pcIteratorImpl->pcIterator = pcIteratorImpl->pcBeginIterator;

	return cIterator;
}

SelectionItem & H3DF::SelectionResults::Front()
{
	SelectionResultsImpl * pcImpl = (SelectionResultsImpl *)m_pcImpl;
	return pcImpl->Front();
}

SelectionItem & H3DF::SelectionResults::Front() const
{
	SelectionResultsImpl * pcImpl = (SelectionResultsImpl *)m_pcImpl;
	return pcImpl->Front();
}

void H3DF::SelectionResults::PushFront(SelectionItem & cInItem)
{
	SelectionResultsImpl * pcImpl = (SelectionResultsImpl *)m_pcImpl;
	pcImpl->PushFront(cInItem);
}

void H3DF::SelectionResults::PushBack(SelectionItem & cInItem)
{
	SelectionResultsImpl * pcImpl = (SelectionResultsImpl *)m_pcImpl;
	pcImpl->PushBack(cInItem);
}

bool H3DF::SelectionResults::Erase(SelectionItem & cInItem)
{
	SelectionResultsImpl * pcImpl = (SelectionResultsImpl *)m_pcImpl;
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
	SelectionResultsImpl * pcImpl = (SelectionResultsImpl *)m_pcImpl;
	SelectionResultsImpl * pcInThatImpl = (SelectionResultsImpl *)cInResults.m_pcImpl;

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

	SelectionResultsImpl * pcImpl = (SelectionResultsImpl *)m_pcImpl;
	
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
	SelectionResultsImpl * pcImpl = (SelectionResultsImpl *)m_pcImpl;
	SelectionResultsImpl * pcInThatImpl = (SelectionResultsImpl *)cInThat.m_pcImpl;

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
	SelectionResultsImpl * pcImpl = (SelectionResultsImpl *)m_pcImpl;
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
	SelectionResultsImpl * pcImpl = (SelectionResultsImpl *)m_pcImpl;
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
	SelectionResultsImpl * pcImpl = (SelectionResultsImpl *)m_pcImpl;
	return pcImpl->Sort();
}

bool H3DF::SelectionResults::IsExist(SelectionItem & cInItem)
{
	SelectionResultsImpl * pcImpl = (SelectionResultsImpl *)m_pcImpl;
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
	SelectionControlImpl * pcImpl = new SelectionControlImpl();
	pcImpl->m_pcWindow = &cInWindow;
	m_pcImpl = pcImpl;
}

H3DF::SelectionControl::SelectionControl(SelectionControl const & cInThat)
{
	m_pcImpl = new SelectionControlImpl();
	Set(cInThat);
}

H3DF::SelectionControl::SelectionControl() {}

H3DF::SelectionControl::~SelectionControl()
{
}

void H3DF::SelectionControl::Set(SelectionControl const & cInThat)
{
	SelectionControlImpl * pcImpl = (SelectionControlImpl *)m_pcImpl;
	SelectionControlImpl * pcInThatImpl = (SelectionControlImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

SelectionControl & H3DF::SelectionControl::operator =(SelectionControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

// 1. 주어진 Point와 Selection Option을 이용해서 선택 작업을 수행하고, 선택된 요소를 SelectionResults에 저장한다.
size_t H3DF::SelectionControl::SelectByPoint(Point const & cInLocation, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults) const
{
	SelectionControlImpl * pcSelCtrlImpl = (SelectionControlImpl *)m_pcImpl;

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

	// 선택된 요소가 없음
	if (0 == nResult) {
		return 0;
	}

	HC_KEY  nKey = INVALID_KEY;
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

		HC_Show_Selection_Element(&nKey, &pcItemImpl->m_nOffset1, &pcItemImpl->m_nOffset2, &pcItemImpl->m_nOffset3);
		HC_Show_Selection_Original_Key(&nKey);

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

			for (int nIndex = 0 ; nIndex < nKeyCount ; nIndex++)
			{
				HC_Show_Key_Type(pnKeys[nIndex], chKeyType);
				int i = 0;
			}

			for (int nIndex = nKeyCount - 1; nIndex >= 0; nIndex--)
			{
				HC_Show_Key_Type(pnKeys[nIndex], chKeyType);
				//TRACE(L"%d.Selection Keys Type: %s", nIndex, CString(chKeyType));

				if (streq(chKeyType, "segment")) {
					// nKey = pnKeys[nIndex];
					//nKey = HC_Show_Reference_Geometry(pnKeys[nIndex]);
				}
				else if (strstr(chKeyType, "include")) {
					pcItemImpl->m_vcIncludeKeys.emplace_back(pnKeys[nIndex]);
				}
				else if (streq(chKeyType, "reference")) {
					// nKey = pnKeys[nIndex];
					//nKey = HC_Show_Reference_Geometry(pnKeys[nIndex]);
				}
			}
		}

		HC_Show_Key_Type(nKey, chKeyType);

		if (streq(chKeyType, "line") || streq(chKeyType, "polyline") || streq(chKeyType, "circular arc") || streq(chKeyType, "elliptical arc")) {
			eSelectedType = SelectionControlImpl::SelType::Line;
			pcItemImpl->m_cKey = LineKey(nKey);
		}
		else if (streq(chKeyType, "marker")) {
			eSelectedType = SelectionControlImpl::SelType::Marker;
		}
		else if (streq(chKeyType, "text leader")) {
			eSelectedType = SelectionControlImpl::SelType::Shell;	//?
			nKey = HC_Show_Owner_Original_Key(nKey);		// move up to text;
		}
		else {
			// This may be shell, mesh, cyliner, etc...
			eSelectedType = SelectionControlImpl::SelType::Shell;
			pcItemImpl->m_cKey = ShellKey(nKey);

			// But if it really is a shell, check for regions.
			if (streq(chKeyType, "shell") && pcItemImpl->m_nOffset3 != -1) {

				int nRegion = 0;
				int nLowest = 0;
				int nHighest = 0;

				HC_Show_Region_Range(nKey, &nLowest, &nHighest);

				if ((nLowest != nHighest || nLowest > 0)) {
					// eSelectedType |= SelectionControlImpl::SelType::Region;

					HC_Open_Geometry(nKey); {
						HC_Open_Face(pcItemImpl->m_nOffset3); {
							HC_Show_Region(&nRegion);
						}HC_Close_Face();
					}HC_Close_Geometry();

					pcItemImpl->m_nRegion = nRegion;
					pcItemImpl->m_nLowest = nLowest;
					pcItemImpl->m_nHighest = nHighest;
				}
/*
				//Selection::Level cLevel;

			if(true == cInOptions.ShowLevel(cLevel)) {
				if (Selection::Level::Subentity == cLevel) {
					//if (true ==  pcSelection->GetAllowRegionSelection()) {
					int nRegion = 0;
					int nLowest = 0;
					int nHighest = 0;

					HC_Show_Region_Range(nKey, &nLowest, &nHighest);

					if ((nLowest != nHighest || nLowest > 0)) {
						eSelectedType |= SelType::Region;
						HC_Open_Geometry(nKey); {
							HC_Open_Face(nOffset3); {
								HC_Show_Region(&nRegion);
							}HC_Close_Face();
						}HC_Close_Geometry();

						pcItemImpl->nRegion = nRegion;
						pcItemImpl->nLowest = nLowest;
						pcItemImpl->nHighest = nHighest;
					}
				}
			}
*/
			}
		}

		pcResultsImpl->PushBack(cItem);

	} while (HC_Find_Related_Selection());

	return cOutResults.GetCount();
}

size_t H3DF::SelectionControl::SelectByPoint(Point const & cInLocation, SelectionResults & cOutResults) const
{
	SelectionControlImpl * pcImpl = (SelectionControlImpl *) m_pcImpl;

	SelectionOptionsKit cOptions;
	if(false == pcImpl->GetWindow()->ShowSelectionOptions(cOptions)) {
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
	SelectionControlImpl * pcSelCtrlImpl = (SelectionControlImpl *)m_pcImpl;
	SelectionOptionsKit cInOptions;

	if (nullptr == pcSelCtrlImpl->m_pcSelectionSet) {
		return HOP_NOT_HANDLED;
	}

	HPoint  new_pos;
	int		nResult = 0;
	bool	need_update = false;

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
		need_update = true;
	}

	pcSelCtrlImpl->GetBaseView()->GetConstantFrameRateObject()->SetDisableIncreaseTemp(true);

	//GetBaseView()->SetGeometryChanged();
	if (need_update) {
		pcSelCtrlImpl->GetBaseView()->Update();	// update the scene to reflect the new highlight attributes
	}

	pcSelCtrlImpl->GetBaseView()->GetConstantFrameRateObject()->SetDisableIncreaseTemp(false);

	if (pcSelCtrlImpl->GetBaseView()->GetFocusOnSelection()) {
		pcSelCtrlImpl->GetBaseView()->FocusOnSelection();
	}

	// of the selected items
	return HOP_READY;

	//pcImpl->SelectByPoint(cInLocation, nFlags, cInOptions, cOutResults);

	return 0;
}