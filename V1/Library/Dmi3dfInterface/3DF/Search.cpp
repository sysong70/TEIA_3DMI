#include "StdAfx.h"

#include "Search.h"

#include "./Impl/SearchImpl.h"

using namespace H3DF;

H3DF::SearchOptionsKit::SearchOptionsKit()
{
	m_pcImpl = new SearchOptionsKitImpl();
}

H3DF::SearchOptionsKit::SearchOptionsKit(SearchOptionsKit const & cInKit)
{
	m_pcImpl = new SearchOptionsKitImpl();
	Set(cInKit);
}

void H3DF::SearchOptionsKit::Set(SearchOptionsKit const & cInThat)
{
	SearchOptionsKitImpl * pcImpl = (SearchOptionsKitImpl *)m_pcImpl;
	SearchOptionsKitImpl * pcInThatImpl = (SearchOptionsKitImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

SearchOptionsKit & H3DF::SearchOptionsKit::operator=(SearchOptionsKit && cInThat)
{
	Set(cInThat);
	return *this;
}

//== Search Results Iterator Class =================================================================
H3DF::SearchResultsIterator::SearchResultsIterator()
{
	m_pcImpl = new SearchResultsIteratorImpl();
}

H3DF::SearchResultsIterator::SearchResultsIterator(SearchResultsIterator const & cInThat)
{
	m_pcImpl = new SearchResultsIteratorImpl();
	Set(cInThat);
}

void H3DF::SearchResultsIterator::Set(SearchResultsIterator const & cInThat)
{
	SearchResultsIteratorImpl * pcImpl = (SearchResultsIteratorImpl *)m_pcImpl;
	SearchResultsIteratorImpl * pcInThatImpl = (SearchResultsIteratorImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

SearchResultsIterator & H3DF::SearchResultsIterator::operator=(SearchResultsIterator const & cInThat)
{
	Set(cInThat);
	return *this;
}

void H3DF::SearchResultsIterator::Next()
{
	SearchResultsIteratorImpl * pcImpl = (SearchResultsIteratorImpl *)m_pcImpl;
	++pcImpl->pcIterator;
}

SearchResultsIterator & H3DF::SearchResultsIterator::operator++()
{
	SearchResultsIteratorImpl * pcImpl = (SearchResultsIteratorImpl *)m_pcImpl;
	++pcImpl->pcIterator;
	return *this;
}

SearchResultsIterator H3DF::SearchResultsIterator::operator++(int nInValue)
{
	SearchResultsIteratorImpl * pcImpl = (SearchResultsIteratorImpl *)m_pcImpl;
	std::advance(pcImpl->pcIterator, nInValue);
	return *this;
}

bool H3DF::SearchResultsIterator::operator == (SearchResultsIterator const & cInSearchResultsIterator)
{
	SearchResultsIteratorImpl * pcImpl = (SearchResultsIteratorImpl *)m_pcImpl;
	SearchResultsIteratorImpl * pcInThatImpl = (SearchResultsIteratorImpl *)cInSearchResultsIterator.m_pcImpl;

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
	SearchResultsIteratorImpl * pcImpl = (SearchResultsIteratorImpl *)m_pcImpl;
	return pcImpl->pcIterator != pcImpl->pcEndIterator;
}

void H3DF::SearchResultsIterator::Reset()
{
	SearchResultsIteratorImpl * pcImpl = (SearchResultsIteratorImpl *)m_pcImpl;
	pcImpl->pcIterator = pcImpl->pcBeginIterator;
}

Key H3DF::SearchResultsIterator::GetItem() const
{
	SearchResultsIteratorImpl * pcImpl = (SearchResultsIteratorImpl *)m_pcImpl;
	return *pcImpl->pcIterator;
}

Key H3DF::SearchResultsIterator::operator * () const
{
	return GetItem();
}

//== Search Results Class ==========================================================================
H3DF::SearchResults::SearchResults()
{
	m_pcImpl = new SearchResultsImpl();
}

H3DF::SearchResults::SearchResults(SearchResults const & cInThat)
{
	m_pcImpl = new SearchResultsImpl();
	Set(cInThat);
}

H3DF::SearchResults::~SearchResults()
{

}

void H3DF::SearchResults::Set(SearchResults const & cInThat)
{
	SearchResultsImpl * pcImpl = (SearchResultsImpl *)m_pcImpl;
	SearchResultsImpl * pcInThatImpl = (SearchResultsImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

SearchResults & H3DF::SearchResults::operator=(SearchResults const & cInThat)
{
	Set(cInThat);
	return *this;
}

void H3DF::SearchResults::Reset()
{
	SearchResultsImpl * pcImpl = (SearchResultsImpl *)m_pcImpl;
	pcImpl->m_deKeys.clear();
}

size_t H3DF::SearchResults::GetCount() const
{
	SearchResultsImpl * pcImpl = (SearchResultsImpl *)m_pcImpl;
	return pcImpl->m_deKeys.size();
}

SearchResultsIterator H3DF::SearchResults::GetIterator() const
{
	SearchResultsImpl * pcImpl = (SearchResultsImpl *)m_pcImpl;
	SearchResultsIterator cIterator;
	SearchResultsIteratorImpl * pcIteratorImpl = (SearchResultsIteratorImpl *)cIterator.GetImpl();
	pcIteratorImpl->pcIterator = pcImpl->m_deKeys.begin();
	pcIteratorImpl->pcBeginIterator = pcImpl->m_deKeys.begin();
	pcIteratorImpl->pcEndIterator = pcImpl->m_deKeys.end();
	return cIterator;
}