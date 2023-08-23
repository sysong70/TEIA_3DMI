#include "StdAfx.h"

#include "3DF.Search.h"

#include "./Private/3DF.SearchPrivate.h"

USING_3DF_NAMESPACE

H3DF::SearchOptionsKit::SearchOptionsKit()
{
	m_pcImpl = new SearchOptionsKitPrivate();
}

H3DF::SearchOptionsKit::SearchOptionsKit(SearchOptionsKit const & cInKit)
{
	m_pcImpl = new SearchOptionsKitPrivate();
	Set(cInKit);
}

void H3DF::SearchOptionsKit::Set(SearchOptionsKit const & cInThat)
{
	SearchOptionsKitPrivate * pcImpl = (SearchOptionsKitPrivate *)m_pcImpl;
	SearchOptionsKitPrivate * pcInThatImpl = (SearchOptionsKitPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

SearchOptionsKit & H3DF::SearchOptionsKit::operator=(SearchOptionsKit && cInThat)
{
	Set(cInThat);
	return *this;
}
