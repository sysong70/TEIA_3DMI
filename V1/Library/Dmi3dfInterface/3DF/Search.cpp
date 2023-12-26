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
