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

	SegmentKey m_nOverrideKey;

	void Copy(ControlPrivate * pcInThat)
	{
		m_nOverrideKey = pcInThat->m_nOverrideKey;
		m_eType = pcInThat->Type();
	}
};

CLOSE_3DF_NAMESPACE