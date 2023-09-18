#pragma once

#include "../3DF.h"
#include "../Kit.h"
#include "../Control.h"
#include "../Search.h"

namespace H3DF
{
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
}