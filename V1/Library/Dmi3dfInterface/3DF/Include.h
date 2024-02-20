#pragma once

#include "3DF.h"
#include "Key.h"

OPEN_3DF_NAMESPACE

class API_3DF IncludeKey : public Key
{
public:
	IncludeKey();
	IncludeKey(HC_KEY nInKey);
	IncludeKey(IncludeKey const & cInThat);

	void Set(IncludeKey const & cInThat);
	IncludeKey & operator = (IncludeKey const & cInThat);

	H3DF::Type ObjectType() const { return H3DF::Type::IncludeKey; };

	SegmentKey GetTarget() const;

	IncludeKey & SetConditionalExpression(ConditionalExpression const & cInConditional);
	IncludeKey & UnsetConditionalExpression();
	bool ShowConditionalExpression(ConditionalExpression & cOutConditional) const;
};

CLOSE_3DF_NAMESPACE