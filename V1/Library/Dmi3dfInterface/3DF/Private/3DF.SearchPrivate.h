#pragma once

#include "../3DF.h"
#include "../3DF.Kit.h"
#include "../3DF.Control.h"
#include "../3DF.Search.h"

OPEN_3DF_NAMESPACE

class SearchPrivate : public PrivateImpl
{
public:
	void Copy(SearchPrivate * pcInThat) {
	}

	static CString GetSearchTypeString(Search::Type eInType);

	static CString GetSearchSpaceString(Search::Space eInSpace);
};

class SearchOptionsKitPrivate : public PrivateImpl
{
public:
	void Copy(SearchOptionsKitPrivate * pcInThat) {
	}

};

CLOSE_3DF_NAMESPACE