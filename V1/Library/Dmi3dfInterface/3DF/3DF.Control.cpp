#include "StdAfx.h"

#include "3DF.Control.h"
#include "3DF.Segment.h"

#include <HTools.h>

namespace TDF {
	class ControlPrivate : public PrivateImpl
	{
		public:
		ControlPrivate() { m_eType = TDF::Type::Control; }
		void Copy(ControlPrivate * pcInThat) {
			m_nKey = pcInThat->m_nKey;
			m_nOwnerKey = pcInThat->m_nOwnerKey;
		}
		// Key
		HC_KEY m_nKey = INVALID_KEY;
		// Owner Key
		HC_KEY m_nOwnerKey = INVALID_KEY;
	};
}

USING_3DF_NAMESPACE

Control::Control(HC_KEY nKey)
{
	m_nKey = nKey;
}

void Control::Open() const
{
	HC_Open_Segment_By_Key(m_nKey);
}

void Control::Close() const
{
	HC_Close_Segment();
}

bool Control::HasOwner() const
{
	return (INVALID_KEY != m_nOwnerKey) ? true : false;
}

void Control::SetOwerKey(HC_KEY nInKey)
{
	m_nOwnerKey = nInKey;
}

SegmentKey Control::Owner() const
{
	if(INVALID_KEY == m_nOwnerKey) {
		assert(false);
	}

	SegmentKey cOwner(m_nOwnerKey);
	return cOwner;
}

void Control::Delete()
{
	assert(false);
}