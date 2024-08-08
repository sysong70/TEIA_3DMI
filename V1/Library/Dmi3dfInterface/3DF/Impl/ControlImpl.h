#pragma once

#include "../3DF.h"
#include "../Object.h"
#include "../Segment.h"

OPEN_3DF_NAMESPACE

class ControlImpl : public H3DF::Impl
{
public:
	SegmentKey m_cOverrideKey;

	void Copy(ControlImpl * pcInThat)
	{
		m_cOverrideKey = pcInThat->m_cOverrideKey;
	}
};

CLOSE_3DF_NAMESPACE