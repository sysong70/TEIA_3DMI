#include "StdAfx.h"

#include "Search.h"
#include "./Impl/3DF.Impl.h"
#include "./Impl/SearchImpl.h"

using namespace H3DF;

H3DF::SearchOptionsKit::SearchOptionsKit()
{
	m_pcImpl = std::make_unique<SearchOptionsKitImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::SearchOptionsKit::SearchOptionsKit(SearchOptionsKit const & cInKit)
{
	m_pcImpl = (nullptr != cInKit.GetImpl()) ? cInKit.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

SearchOptionsKit & H3DF::SearchOptionsKit::operator = (SearchOptionsKit const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

//== Search Results Iterator Class =================================================================
H3DF::SearchResultsIterator::SearchResultsIterator()
{
	m_pcImpl = std::make_unique<SearchResultsIteratorImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::SearchResultsIterator::SearchResultsIterator(SearchResultsIterator const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

SearchResultsIterator & H3DF::SearchResultsIterator::operator=(SearchResultsIterator const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

void H3DF::SearchResultsIterator::Next()
{
	auto pcImpl = static_cast<SearchResultsIteratorImpl *>(m_pcImpl.get());
	++pcImpl->pcIterator;
}

SearchResultsIterator & H3DF::SearchResultsIterator::operator++()
{
	auto pcImpl = static_cast<SearchResultsIteratorImpl *>(m_pcImpl.get());
	++pcImpl->pcIterator;
	return *this;
}

SearchResultsIterator H3DF::SearchResultsIterator::operator++(int nInValue)
{
	auto pcImpl = static_cast<SearchResultsIteratorImpl *>(m_pcImpl.get());
	std::advance(pcImpl->pcIterator, nInValue);
	return *this;
}

bool H3DF::SearchResultsIterator::operator == (SearchResultsIterator const & cInSearchResultsIterator)
{
	auto pcImpl = static_cast<SearchResultsIteratorImpl *>(m_pcImpl.get());
	auto pcInThatImpl = static_cast<SearchResultsIteratorImpl *>(cInSearchResultsIterator.m_pcImpl.get());

	if (pcImpl->pcIterator != pcInThatImpl->pcIterator) {
		return false;
	}

	if (pcImpl->pcBeginIterator != pcInThatImpl->pcBeginIterator) {
		return false;
	}

	if (pcImpl->pcEndIterator != pcInThatImpl->pcEndIterator) {
		return false;
	}

	return true;
}

bool H3DF::SearchResultsIterator::operator != (SearchResultsIterator const & cInSearchResultsIterator)
{
	return !(*this == cInSearchResultsIterator);
}

bool H3DF::SearchResultsIterator::IsValid() const
{
	auto pcImpl = static_cast<SearchResultsIteratorImpl *>(m_pcImpl.get());
	return pcImpl->pcIterator != pcImpl->pcEndIterator;
}

void H3DF::SearchResultsIterator::Reset()
{
	auto pcImpl = static_cast<SearchResultsIteratorImpl *>(m_pcImpl.get());
	pcImpl->pcIterator = pcImpl->pcBeginIterator;
}

Key H3DF::SearchResultsIterator::GetItem() const
{
	auto pcImpl = static_cast<SearchResultsIteratorImpl *>(m_pcImpl.get());
	return *pcImpl->pcIterator;
}

Key H3DF::SearchResultsIterator::operator * () const
{
	return GetItem();
}

//== Search Results Class ==========================================================================
H3DF::SearchResults::SearchResults()
{
	m_pcImpl = std::make_unique<SearchResultsImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::SearchResults::SearchResults(SearchResults const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

H3DF::SearchResults::~SearchResults()
{

}

void H3DF::SearchResults::Set(SearchResults const & cInThat)
{
	auto pcImpl = static_cast<SearchResultsImpl *>(m_pcImpl.get());
	auto pcInThatImpl = static_cast<SearchResultsImpl *>(cInThat.m_pcImpl.get());
	pcImpl->Copy(pcInThatImpl);
}

SearchResults & H3DF::SearchResults::operator=(SearchResults const & cInThat)
{
	Set(cInThat);
	return *this;
}

void H3DF::SearchResults::Reset()
{
	auto pcImpl = static_cast<SearchResultsImpl *>(m_pcImpl.get());
	pcImpl->m_deKeys.clear();
}

size_t H3DF::SearchResults::GetCount() const
{
	auto pcImpl = static_cast<SearchResultsImpl *>(m_pcImpl.get());
	return pcImpl->m_deKeys.size();
}

SearchResultsIterator H3DF::SearchResults::GetIterator() const
{
	auto pcImpl = static_cast<SearchResultsImpl *>(m_pcImpl.get());
	SearchResultsIterator cIterator;
	SearchResultsIteratorImpl * pcIteratorImpl = (SearchResultsIteratorImpl *)cIterator.GetImpl();
	pcIteratorImpl->pcIterator = pcImpl->m_deKeys.begin();
	pcIteratorImpl->pcBeginIterator = pcImpl->m_deKeys.begin();
	pcIteratorImpl->pcEndIterator = pcImpl->m_deKeys.end();
	return cIterator;
}