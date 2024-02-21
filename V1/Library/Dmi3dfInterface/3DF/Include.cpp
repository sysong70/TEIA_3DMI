#include "StdAfx.h"

#include "Include.h"

#include "Segment.h"

#include "Condition.h"

#include <HTools.h>

using namespace H3DF;

H3DF::IncludeKey::IncludeKey() : Key()
{
	
}

H3DF::IncludeKey::IncludeKey(HC_KEY nInKey) : Key(nInKey)
{
	
}

H3DF::IncludeKey::IncludeKey(IncludeKey const & cInThat):
	Key(cInThat.KeyValue())
{
}

void H3DF::IncludeKey::Set(IncludeKey const & cInThat)
{
	Key::Set(cInThat);
}

IncludeKey & H3DF::IncludeKey::operator = (IncludeKey const & cInThat)
{
	Key::Set(cInThat);
	return *this;
}

SegmentKey H3DF::IncludeKey::GetTarget() const
{
	HC_KEY nSegment = HC_Show_Include_Segment(KeyValue(), nullptr);

	SegmentKey cSegment(nSegment);
	return cSegment;
}

IncludeKey & H3DF::IncludeKey::SetConditionalExpression(ConditionalExpression const & cInConditional)
{
	CStringA strCondition;
	cInConditional.ShowCondition(strCondition);
	
	HC_Conditional_Include_By_Key(KeyValue(), strCondition);

	return *this;
}

IncludeKey & H3DF::IncludeKey::UnsetConditionalExpression()
{
	return *this;
}

bool H3DF::IncludeKey::ShowConditionalExpression(ConditionalExpression & cOutConditional) const
{
	return true;
}
