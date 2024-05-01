#pragma once

#include "3DF.h"
#include "Key.h"

namespace H3DF
{
	class API_3DF Definition : public Object
	{
	public:
		Definition();
		Definition(HC_KEY nInKey);
		Definition(Definition const & cInThat);

		void Set(Definition const & cInThat);
		Definition const & operator = (Definition const & cInThat);

		PortfolioKey Owner() const;
		CStringA Name() const;

		bool operator == (Definition const & cInThat) const;
		bool operator != (Definition const & cInThat) const;
	};
}