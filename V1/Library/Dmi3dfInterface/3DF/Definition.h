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

		static const H3DF::Type staticType = H3DF::Type::Definition;
		H3DF::Type ObjectType() const { return staticType; };

		Definition(Definition && cInThat) noexcept;
		Definition & operator = (Definition && cInThat) noexcept;

		Definition const & operator = (Definition const & cInThat);

		PortfolioKey Owner() const;
		CStringA Name() const;

		bool operator == (Definition const & cInThat) const;
		bool operator != (Definition const & cInThat) const;
	};
}