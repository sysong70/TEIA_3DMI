#pragma once

#include <HBaseView.h>
#include <HUtility.h>
#include <HTools.h>
#include <HSelectionSet.h>

#include "../3DF.h"
#include "../Kit.h"
#include "../Control.h"

#include "../Segment.h"
#include "../Selection.h"
#include "../Math.h"

#include <vector>
#include <deque>

namespace H3DF
{
	class SelectionOptionsKitImpl : public Impl
	{
	public:
		void Copy(SelectionOptionsKitImpl * pcInThat) {
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

	class SelectionOptionsControlImpl : public Impl
	{
	public:
		void Copy(SelectionOptionsControlImpl * pcInThat) {
			m_pcWindow = pcInThat->m_pcWindow;
			m_pcSelectionSet = pcInThat->m_pcSelectionSet;

		}

		WindowKey * GetWindow() { return (WindowKey *) m_pcWindow; }
		const WindowKey * m_pcWindow = nullptr;

		HSelectionSet * m_pcSelectionSet = nullptr;
	};

	class SelectionItemImpl : public Impl
	{
	public:
		SelectionItemImpl() { m_eType = H3DF::Type::SelectionItem; }
		virtual ~SelectionItemImpl()
		{
			if (nullptr != pnIncludeKeys) {
				delete pnIncludeKeys;
			}
		}

		void Copy(SelectionItemImpl * pcInThat) {
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

			m_pcWindow = pcInThat->m_pcWindow;
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

		WindowKey * GetWindow() { return (WindowKey *) m_pcWindow; }
		const WindowKey * m_pcWindow = nullptr;

		bool ShowPath(KeyPath & cOutPath) const;
		bool ShowPathString(CString & strOutPath);

		void Reset();
	};

	class SelectionResultsIteratorImpl : public Impl
	{
	public:
		SelectionResultsIteratorImpl() { m_eType = H3DF::Type::SelectionResultsIterator; }

		void Copy(SelectionResultsIteratorImpl * pcInThat) {
			pcIterator = pcInThat->pcIterator;
			pcBeginIterator = pcInThat->pcBeginIterator;
			pcEndIterator = pcInThat->pcEndIterator;
		}

		std::deque<SelectionItem>::iterator pcIterator;
		std::deque<SelectionItem>::iterator pcBeginIterator;
		std::deque<SelectionItem>::iterator pcEndIterator;
	};

	class SelectionResultsImpl : public Impl
	{
	public:
		SelectionResultsImpl() { m_eType = H3DF::Type::SelectionResults; }

		void Copy(SelectionResultsImpl * pcInThat) {
			m_deItems.clear();
			for (auto cItem : pcInThat->m_deItems) {
				m_deItems.push_back(cItem);
			}
		}

		void PushFront(SelectionItem & pcInItem) { m_deItems.push_front(pcInItem); }
		void PushBack(SelectionItem & pcInItem) { m_deItems.push_back(pcInItem); }

		SelectionItem & Front() { return m_deItems.front(); }
		SelectionItem & Back() { return m_deItems.back(); }

		auto Begin() { return m_deItems.begin(); }
		auto End() { return m_deItems.end(); }

		void Clear() { m_deItems.clear(); }

		auto Erase(auto cIter) { return m_deItems.erase(cIter); }
		auto Erase(auto cBegin, auto cEnd) { return m_deItems.erase(cBegin, cEnd); }

		bool Empty() { return m_deItems.empty(); }
		size_t Size() { return m_deItems.size(); }
		void Resize(size_t nInSize) { m_deItems.resize(nInSize); }

		bool Sort();

		std::deque<SelectionItem> & GetItems() { return m_deItems; }

	private:
		std::deque<SelectionItem> m_deItems;
	};

	class SelectionControlImpl : public Impl
	{
	public:
		SelectionControlImpl() { m_eType = H3DF::Type::SelectionControl; }

		void Copy(SelectionControlImpl * pcInThat) {
			m_pcWindow = pcInThat->m_pcWindow;
			m_pcSelectionSet = pcInThat->m_pcSelectionSet;
		}

		size_t SelectByPoint(Point const & cInLocation, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults);
		int SelectByPoint(Point const & cInLocation, UINT const nFlags, SelectionOptionsKit const & cInOptions, SelectionResults & cOutResults);
		int SelectButtonDown_V1(Point const & cInLocation, UINT const nFlags, SelectionResults & cOutResults);
		void HandleSelection(UINT const nFlags, SelectionResults & cOutResults);

		static void GetSelectOption(SelectionOptionsKit const & cInOptions, char * pchOutOption);
		static void GetScope(SelectionOptionsKit const & cInOptions, char * pchOutScope);

		WindowKey * GetWindow() { return (WindowKey *) m_pcWindow; }
		const WindowKey * m_pcWindow = nullptr;

		HBaseView * GetBaseView();

		HSelectionSet * m_pcSelectionSet = nullptr;

	private:
		// & 연산을 해야하므로 enum class를 사용하지 않는다.
		enum SelType
		{
			None,
			Shell,
			Region,
			Marker,
			Line
		};
	};
}