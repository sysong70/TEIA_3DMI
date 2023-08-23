#include "StdAfx.h"

#include "3DF.Include.h"

#include "3DF.Segment.h"

#include <HTools.h>

USING_3DF_NAMESPACE

H3DF::IncludeKey::IncludeKey(HC_KEY nInKey) :
	Key(nInKey)
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