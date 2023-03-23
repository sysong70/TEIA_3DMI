#pragma once

#include <HBaseView.h>
#include <HUtility.h>
#include <HTools.h>
#include <HSelectionSet.h>

#include "../3DF.h"
#include "../3DF.Kit.h"
#include "../3DF.Control.h"

#include "../3DF.Selection.h"

#include "../3DF.Math.h"

#include <vector>

OPEN_3DF_NAMESPACE

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
};

class SelectionOptionsControlPrivate : public PrivateImpl
{
public:
	void Copy(SelectionOptionsControlPrivate * pcInThat) {
		m_pcBaseView = pcInThat->m_pcBaseView;
	}

	HBaseView * GetBaseView() { return (HBaseView *)m_pcBaseView; }
	const HBaseView * m_pcBaseView = nullptr;
};

class SelectionItemPrivate : public PrivateImpl
{
public:
	~SelectionItemPrivate() {
		if (nullptr != pcKey) {
			delete pcKey;
		}

		if (nullptr != pnKeys) {
			delete pnKeys;
		}

		if (nullptr != pnIncludeKeys) {
			delete pnIncludeKeys;
		}
	}

	_3DF::Type Type() const override { return _3DF::Type::SelectionItem; }

	void Copy(SelectionItemPrivate * pcInThat) {
		pcKey = pcInThat->pcKey;

		nKeyCount = pcInThat->nKeyCount;
		pnKeys = pcInThat->pnKeys;

		nIncludeCount = pcInThat->nIncludeCount;
		pnIncludeKeys = pcInThat->pnIncludeKeys;

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
	Key * pcKey = nullptr;

	int nKeyCount = 0;
	HC_KEY * pnKeys = nullptr;

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

class SelectionControlPrivate : public PrivateImpl
{
public:
	_3DF::Type Type() const override { return _3DF::Type::SelectionControl; }

	void Copy(SelectionControlPrivate * pcInThat) {
		m_pcBaseView = pcInThat->m_pcBaseView;
		m_pcWindow = pcInThat->m_pcWindow;
	}

	int SelectByPoint(Point const & cInLocation, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults);
	int SelectByPoint(Point const & cInLocation, UINT const nFlags, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults);
	int SelectButtonDown_V1(Point const & cInLocation, UINT const nFlags, SelectionResults & cOutResults);
	void HandleSelection(UINT const nFlags, SelectionResults & cOutResults);

	void GetSelectOption(SelectionOptionsKit const & cInOptions, char * pchOutOption);

	WindowKey * GetWindow() { return (WindowKey *)m_pcWindow; }
	const WindowKey * m_pcWindow = nullptr;

	HBaseView * GetBaseView() { return (HBaseView *)m_pcBaseView; }
	const HBaseView * m_pcBaseView = nullptr;

private:
	// & 연산을 해야하므로 enum class를 사용하지 않는다.
	enum SelType {
		None,
		Shell,
		Region,
		Marker,
		Line
	};
};

CLOSE_3DF_NAMESPACE