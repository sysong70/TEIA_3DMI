#include "StdAfx.h"

#include "3DF.Key.h"
#include "3DF.Segment.h"

#include <HTools.h>

USING_3DF_NAMESPACE

Key::Key(HC_KEY nInKey)
{
	m_nKey = nInKey;
}

void Key::Open() const
{
	HC_Open_Segment_By_Key(m_nKey);
}

void Key::Close() const
{
	HC_Close_Segment();
}

bool Key::HasOwner() const
{ 
	return (INVALID_KEY != m_nOwnerKey) ? true : false;
}

void Key::SetOwerKey(HC_KEY nInKey)
{
	m_nOwnerKey = nInKey;
}

SegmentKey Key::Owner()
{
	if(INVALID_KEY == m_nKey && INVALID_KEY == m_nOwnerKey) {
		assert(false);
	}

	if(INVALID_KEY == m_nOwnerKey) {
		Open();
		m_nOwnerKey = HC_Show_Owner_Original_Key(m_nKey);
		Close();
	}

	SegmentKey cOwner(m_nOwnerKey);
	return cOwner;
}

void Key::Delete()
{
	assert(false);
}