#include "StdAfx.h"

#include "3DF.Key.h"
#include "3DF.Segment.h"

#include <HTools.h>

USING_3DF_NAMESPACE

Key::Key(HC_KEY nInKey)
{
	m_nKey = nInKey;
}

Key::~Key()
{
	if(true == m_bForcedOpen) {
		return;
	}

	if(true == m_bOpen) {
		HC_Close_Segment();
	}
}

void Key::Open()
{
	if(true == m_bForcedOpen) {
		return;
	}

	if(true == m_bOpen) {
		return;
	}

	m_bOpen = true;
	HC_Open_Segment_By_Key(m_nKey);
}

void Key::Close()
{
	if(true == m_bForcedOpen) {
		return;
	}

	if(false == m_bOpen) {
		return;
	}

	m_bOpen = false;
	HC_Close_Segment();
}

void Key::ForcedOpen()
{
	m_bForcedOpen = true;

	if(true == m_bOpen) {
		return;
	}
	
	m_bOpen = true;
	HC_Open_Segment_By_Key(m_nKey);
}

void Key::ForcedClose()
{
	if(false == m_bOpen) {
		return;
	}

	m_bForcedOpen = false;
	m_bOpen = false;
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