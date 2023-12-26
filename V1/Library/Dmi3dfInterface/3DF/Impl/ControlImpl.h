#pragma once

#include "../3DF.h"
#include "../Object.h"
#include "../Segment.h"

OPEN_3DF_NAMESPACE

class ControlImpl : public H3DF::Impl
{
public:
	ControlImpl() { m_eType = Type::Key; }
	~ControlImpl() {}

	SegmentKey m_cOverrideKey;

	void Copy(ControlImpl * pcInThat)
	{
		m_cOverrideKey = pcInThat->m_cOverrideKey;
		m_eType = pcInThat->Type();
	}
};

CLOSE_3DF_NAMESPACE