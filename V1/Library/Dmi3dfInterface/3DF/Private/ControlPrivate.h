#pragma once

#include "../3DF.h"
#include "../Object.h"
#include "../Segment.h"

OPEN_3DF_NAMESPACE

class ControlPrivate : public H3DF::PrivateImpl
{
public:
	ControlPrivate() { m_eType = Type::Key; }
	~ControlPrivate() {}

	SegmentKey m_cOverrideKey;

	void Copy(ControlPrivate * pcInThat)
	{
		m_cOverrideKey = pcInThat->m_cOverrideKey;
		m_eType = pcInThat->Type();
	}
};

CLOSE_3DF_NAMESPACE