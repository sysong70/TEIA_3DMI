#pragma once

#include "../3DF.h"
#include "../Kit.h"
#include "../Control.h"
#include "../Search.h"

namespace H3DF
{
	class SearchImpl : public Impl
	{
	public:
		void Copy(SearchImpl * pcInThat) {
		}

		static CString GetSearchTypeString(Search::Type eInType);

		static CString GetSearchSpaceString(Search::Space eInSpace);
	};

	class SearchOptionsKitImpl : public Impl
	{
	public:
		void Copy(SearchOptionsKitImpl * pcInThat) {
		}

	};
}