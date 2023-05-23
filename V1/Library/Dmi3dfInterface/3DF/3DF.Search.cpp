#include "StdAfx.h"

#include "3DF.Search.h"

#include "./Private/3DF.SearchPrivate.h"

USING_3DF_NAMESPACE

TDF::SearchOptionsKit::SearchOptionsKit()
{
	m_pcImpl = new SearchOptionsKitPrivate();
}

TDF::SearchOptionsKit::SearchOptionsKit(SearchOptionsKit const & cInKit)
{
	m_pcImpl = new SearchOptionsKitPrivate();
	Set(cInKit);
}

void TDF::SearchOptionsKit::Set(SearchOptionsKit const & cInThat)
{
	SearchOptionsKitPrivate * pcImpl = (SearchOptionsKitPrivate *)m_pcImpl;
	SearchOptionsKitPrivate * pcInThatImpl = (SearchOptionsKitPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

SearchOptionsKit & TDF::SearchOptionsKit::operator=(SearchOptionsKit && cInThat)
{
	Set(cInThat);
	return *this;
}
