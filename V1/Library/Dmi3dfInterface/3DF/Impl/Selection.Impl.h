#pragma once

#include <HBaseView.h>
#include <HUtility.h>
#include <HTools.h>
#include <HSelectionSet.h>

#include "../3DF.h"
#include "../Kit.h"
#include "../Control.h"
#include "../Impl/ControlImpl.h"

#include "../Window.h"
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
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<SelectionOptionsKitImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const SelectionOptionsKitImpl * pcInThat) {
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

	class SelectionOptionsControlImpl : public ControlImpl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<SelectionOptionsControlImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const SelectionOptionsControlImpl * pcInThat) {
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
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<SelectionItemImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const SelectionItemImpl * pcInThat) {
			m_vcKeys = pcInThat->m_vcKeys;
			m_vcTypes = pcInThat->m_vcTypes;

			m_nOffset1 = pcInThat->m_nOffset1;
			m_nOffset2 = pcInThat->m_nOffset2;
			m_nOffset3 = pcInThat->m_nOffset3;

			m_nRegion = pcInThat->m_nRegion;
			m_nLowest = pcInThat->m_nLowest;
			m_nHighest = pcInThat->m_nHighest;

			m_cWorldPoint = pcInThat->m_cWorldPoint;
			m_cWindowPoint = pcInThat->m_cWindowPoint;

			m_cWindow = pcInThat->m_cWindow;
		}
		WindowKey & GetWindow() { return m_cWindow; }
		WindowKey m_cWindow;

		std::vector<HC_KEY> & Keys() { return m_vcKeys; }
		std::vector<H3DF::Type> & Types() { return m_vcTypes; }
		bool GetIncludeKeys(std::vector<HC_KEY> & vcOutIncludeKeys);

		// 실제 선택된 Key가 저장되는 변수
	protected:
		std::vector<HC_KEY> m_vcKeys;
		std::vector<H3DF::Type> m_vcTypes;

	public:
		int m_nOffset1 = 0;
		int m_nOffset2 = 0;
		int m_nOffset3 = 0;

		int m_nRegion = 0;
		int m_nLowest = 0;
		int m_nHighest = 0;

		WorldPoint m_cWorldPoint;
		WindowPoint m_cWindowPoint;
		PixelPoint m_cPixelPoint;

		void Reset();
	};

	class SelectionResultsIteratorImpl : public Impl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<SelectionResultsIteratorImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const SelectionResultsIteratorImpl * pcInThat) {
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
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<SelectionResultsImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const SelectionResultsImpl * pcInThat) {
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

		std::deque<SelectionItem> m_deItems;
	};

	class SelectionControlImpl : public ControlImpl
	{
	public:
		enum SelType
		{
			None,
			Shell,
			Region,
			Marker,
			Line
		};

		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<SelectionControlImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const SelectionControlImpl * pcInThat) {
			m_cWindow = pcInThat->m_cWindow;
			m_pcSelectionSet = pcInThat->m_pcSelectionSet;
		}

		HSelectionSet * SelectionSet();

		void HandleSelection(UINT const nFlags, SelectionResults & cOutResults);
		size_t SelectionResult(SelectionControlImpl * pcInSelCtrlImpl, SelectionResults & cOutResults);

		static void GetSelectOption(SelectionOptionsKit const & cInOptions, char * pchOutOption);
		static void GetScope(SelectionOptionsKit const & cInOptions, char * pchOutScope);

		WindowKey & GetWindow() { return m_cWindow; }
		WindowKey m_cWindow;

		HBaseView * GetBaseView();

		HSelectionSet * m_pcSelectionSet = nullptr;
	};
}