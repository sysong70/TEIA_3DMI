#pragma once

#include "3DF.h"
#include "Key.h"

namespace H3DF
{
	class API_3DF IncludeKey : public Key
	{
	public:
		IncludeKey();
		IncludeKey(HC_KEY nInKey);
		IncludeKey(IncludeKey const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::IncludeKey;
		H3DF::Type ObjectType() const { return staticType; };

		IncludeKey & operator = (IncludeKey const & cInThat);

		SegmentKey GetTarget() const;

		IncludeKey & SetConditionalExpression(ConditionalExpression const & cInConditional);
		IncludeKey & UnsetConditionalExpression();
		bool ShowConditionalExpression(ConditionalExpression & cOutConditional) const;
	};
};