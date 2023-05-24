#pragma once

#include <HBaseView.h>
#include <HUtility.h>
#include <HTools.h>
#include <HSelectionSet.h>

#include "../3DF.h"
#include "../3DF.Kit.h"
#include "../3DF.Control.h"

#include "../3DF.Segment.h"
#include "../3DF.Selection.h"

#include "../3DF.Math.h"

#include <vector>
#include <deque>

OPEN_3DF_NAMESPACE

class WindowKey;
class BaseView;

class SelectionOptionsKitPrivate : public PrivateImpl
{
public:
	void Copy(SelectionOptionsKitPrivate * pcInThat) {
		fProximity = pcInThat->fProximity;
		bProximity = pcInThat->bProximity;
		eLevel = pcInThat->eLevel;
		bLevel = pcInThat->bLevel;
		nInternalLimit = pcInThat->nInternalLimit;
		bInternalLimit = pcInThat->bInternalLimit;
		nRelatedLimit = pcInThat->nRelatedLimit;
		bRelatedLimit = pcInThat->bRelatedLimit;
		eSorting = pcInThat->eSorting;
		bSorting = pcInThat->bSorting;
		eAlgorithm = pcInThat->eAlgorithm;
		bAlgorithm = pcInThat->bAlgorithm;
		eGranularity = pcInThat->eGranularity;
		bGranularity = pcInThat->bGranularity;
		eBias = pcInThat->eBias;
		bBias = pcInThat->bBias;
		cStartSegment = pcInThat->cStartSegment;
		cStartPath = pcInThat->cStartPath;
		bScopeOnly = pcInThat->bScopeOnly;
	}

	bool bProximity = false;
	bool bLevel = false;
	bool bInternalLimit = false;
	bool bRelatedLimit = false;
	bool bSorting = false;
	bool bAlgorithm = false;
	bool bGranularity = false;
	bool bBias = false;

	float fProximity = 0.0f;
	Selection::Level eLevel = Selection::Level::Entity;
	size_t nInternalLimit = 1;
	size_t nRelatedLimit = 5;
	Selection::Sorting eSorting = Selection::Sorting::Default;
	Selection::Algorithm eAlgorithm = Selection::Algorithm::Analytic;
	Selection::Granularity eGranularity = Selection::Granularity::General;
	Selection::Bias eBias = Selection::Bias::None;

	SegmentKey cStartSegment;
	KeyPath cStartPath;
	bool bScopeOnly = false;
};

class SelectionOptionsControlPrivate : public PrivateImpl
{
public:
	void Copy(SelectionOptionsControlPrivate * pcInThat) {
		m_pcBaseView = pcInThat->m_pcBaseView;
	}

	TDF::BaseView * GetBaseView() { return (TDF::BaseView *)m_pcBaseView; }
	const TDF::BaseView * m_pcBaseView = nullptr;
};

class SelectionItemPrivate : public PrivateImpl
{
public:
	SelectionItemPrivate() { m_eType = TDF::Type::SelectionItem; }
	virtual ~SelectionItemPrivate() 
	{
		if (nullptr != pnIncludeKeys) {
			delete pnIncludeKeys;
		}
	}

	void Copy(SelectionItemPrivate * pcInThat) {
		cKey = pcInThat->cKey;

		nIncludeCount = pcInThat->nIncludeCount;
		// pnIncludeKeys값 복사
		if (0 < nIncludeCount) {
			pnIncludeKeys = new HC_KEY[nIncludeCount];
			for (int i = 0; i < nIncludeCount; i++) {
				pnIncludeKeys[i] = pcInThat->pnIncludeKeys[i];
			}
		}
		
		nOffset1 = pcInThat->nOffset1;
		nOffset2 = pcInThat->nOffset2;
		nOffset3 = pcInThat->nOffset3;

		nRegion = pcInThat->nRegion;
		nLowest = pcInThat->nLowest;
		nHighest = pcInThat->nHighest;

		cWorldPoint = pcInThat->cWorldPoint;
		cWindowPoint = pcInThat->cWindowPoint;
	}

	// LineKey 등이 저장되는 변수
	Key cKey;

	int nIncludeCount = 0;
	HC_KEY * pnIncludeKeys = nullptr;

	int nOffset1 = 0;
	int nOffset2 = 0;
	int nOffset3 = 0;

	int nRegion = 0;
	int nLowest = 0;
	int nHighest = 0;

	WorldPoint cWorldPoint;
	WindowPoint cWindowPoint;
};

class SelectionResultsIteratorPrivate : public PrivateImpl
{
public:
	SelectionResultsIteratorPrivate() { m_eType = TDF::Type::SelectionResultsIterator; }

	void Copy(SelectionResultsIteratorPrivate * pcInThat) {
		pcIterator = pcInThat->pcIterator;
		pcBeginIterator = pcInThat->pcBeginIterator;
		pcEndIterator = pcInThat->pcEndIterator;
	}

	std::deque<SelectionItem *>::iterator pcIterator;
	std::deque<SelectionItem *>::iterator pcBeginIterator;
	std::deque<SelectionItem *>::iterator pcEndIterator;
};

class SelectionResultsPrivate : public PrivateImpl
{
public:
	SelectionResultsPrivate() { m_eType = TDF::Type::SelectionResults; }	

	void Copy(SelectionResultsPrivate * pcInThat) {
		deItems.clear();
		for (auto pcItem : pcInThat->deItems) {
			SelectionItem * pcNewItem = new SelectionItem(*pcItem);
			deItems.push_back(pcNewItem);
		}
	}

	void PushFront(SelectionItem *& pcInItem) { deItems.push_front(pcInItem); }
	void PushBack(SelectionItem *& pcInItem) { deItems.push_back(pcInItem); }

	SelectionItem * Front() { return deItems.front(); }
	SelectionItem * Back() { return deItems.back(); }

	auto Begin() { return deItems.begin(); }
	auto End() { return deItems.end(); }

	void Clear() { deItems.clear(); }

	auto Erase(auto cIter) { return deItems.erase(cIter); }
	auto Erase(auto cBegin, auto cEnd) { return deItems.erase(cBegin, cEnd); }

	bool Empty() { return deItems.empty(); }
	size_t Size() { return deItems.size(); }
	void Resize(size_t nInSize) { deItems.resize(nInSize); }

	bool Sort();

	std::deque<SelectionItem *> & GetItems() { return deItems; }

private:
	std::deque<SelectionItem *> deItems;
};

class SelectionControlPrivate : public PrivateImpl
{
public:
	SelectionControlPrivate() { m_eType = TDF::Type::SelectionControl; }	

	void Copy(SelectionControlPrivate * pcInThat) {
		m_pcWindow = pcInThat->m_pcWindow;
	}

	size_t SelectByPoint(Point const & cInLocation, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults);
	int SelectByPoint(Point const & cInLocation, UINT const nFlags, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults);
	int SelectButtonDown_V1(Point const & cInLocation, UINT const nFlags, SelectionResults & cOutResults);
	void HandleSelection(UINT const nFlags, SelectionResults & cOutResults);

	void GetSelectOption(SelectionOptionsKit const & cInOptions, char * pchOutOption);
	void GetScope(SelectionOptionsKit const & cInOptions, char * pchOutScope);

	WindowKey * GetWindow() { return (WindowKey *)m_pcWindow; }
	const WindowKey * m_pcWindow = nullptr;

	HBaseView * GetBaseView();

private:
	// & 연산을 해야하므로 enum class를 사용하지 않는다.
	enum SelType {
		None,
		Shell,
		Region,
		Marker,
		Line
	};

	static bool SorterFunction(const void * pcArg1, const void * pcArg2);
};

CLOSE_3DF_NAMESPACE