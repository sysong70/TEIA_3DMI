#include "StdAfx.h"

#include "3DF.Include.h"

#include "3DF.Segment.h"

#include <HTools.h>

USING_3DF_NAMESPACE

TDF::IncludeKey::IncludeKey(HC_KEY nInKey) :
	Key(nInKey)
{
	
}

TDF::IncludeKey::IncludeKey(IncludeKey const & cInThat):
	Key(cInThat.KeyValue())
{
}

void TDF::IncludeKey::Set(IncludeKey const & cInThat)
{
	Key::Set(cInThat);
}

IncludeKey & TDF::IncludeKey::operator = (IncludeKey const & cInThat)
{
	Key::Set(cInThat);
	return *this;
}

SegmentKey TDF::IncludeKey::GetTarget() const
{
	HC_KEY nSegment = HC_Show_Include_Segment(KeyValue(), nullptr);

	SegmentKey cSegment(nSegment);
	return cSegment;
}