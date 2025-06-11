#pragma once

#include "../3DF.h"
#include "../Kit.h"
#include "../Control.h"
#include "../Search.h"

#include <deque>

namespace H3DF
{
	class SearchImpl : public Impl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<SearchImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const SearchImpl * pcInThat) {
		}

		static CString GetSearchTypeString(Search::Type eInType);

		static CString GetSearchSpaceString(Search::Space eInSpace);
	};

	class SearchOptionsKitImpl : public Impl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<SearchOptionsKitImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const SearchOptionsKitImpl * pcInThat) {
		}

	};

	
	class SearchResultsIteratorImpl : public Impl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<SearchResultsIteratorImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const SearchResultsIteratorImpl * pcInThat) {
			pcIterator = pcInThat->pcIterator;
			pcBeginIterator = pcInThat->pcBeginIterator;
			pcEndIterator = pcInThat->pcEndIterator;
		}

		std::deque<Key>::iterator pcIterator;
		std::deque<Key>::iterator pcBeginIterator;
		std::deque<Key>::iterator pcEndIterator;
	};


	class SearchResultsImpl : public Impl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<SearchResultsImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const SearchResultsImpl * pcInThat) {
			m_deKeys.clear();
			for (const auto & cKey : pcInThat->m_deKeys) {
				m_deKeys.push_back(cKey);
			}
		}

		void PushFront(Key pcInItem) { m_deKeys.push_front(pcInItem); }
		void PushBack(Key pcInItem) { m_deKeys.push_back(pcInItem); }

		Key Front() { return m_deKeys.front(); }
		Key Back() { return m_deKeys.back(); }

		auto Begin() { return m_deKeys.begin(); }
		auto End() { return m_deKeys.end(); }

		void Clear() { m_deKeys.clear(); }

		auto Erase(auto cIter) { return m_deKeys.erase(cIter); }
		auto Erase(auto cBegin, auto cEnd) { return m_deKeys.erase(cBegin, cEnd); }

		bool Empty() { return m_deKeys.empty(); }
		size_t Size() { return m_deKeys.size(); }
		void Resize(size_t nInSize) { m_deKeys.resize(nInSize); }

		std::deque<Key> & GetKeys() { return m_deKeys; }
		std::deque<Key> m_deKeys;

		//== Utility Functions =====================================================================
		static Key GetKey(CStringA strType, HC_KEY nInKey);
	};
}