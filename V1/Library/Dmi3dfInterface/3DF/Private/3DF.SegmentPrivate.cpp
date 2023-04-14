#include "StdAfx.h"

#include <HTools.h>

#include "3DF.SegmentPrivate.h"

USING_3DF_NAMESPACE

void SegmentKeyPrivate::Copy(SegmentKeyPrivate * pcInThat)
{
	m_bOpen = pcInThat->m_bOpen;
	m_bForcedOpen = pcInThat->m_bForcedOpen;
}

//== Segment 관련 함수 ===============================================================================
void SegmentKeyPrivate::Open()
{
	if (true == m_bForcedOpen) {
		return;
	}

	if (true == m_bOpen) {
		return;
	}

	m_bOpen = true;

	assert(INVALID_KEY != m_nKey);
	HC_Open_Segment_By_Key(m_nKey);
}

void SegmentKeyPrivate::Open() const
{
	if (true == m_bForcedOpen) {
		return;
	}

	if (true == m_bOpen) {
		return;
	}

	m_bOpen = true;

	assert(INVALID_KEY != m_nKey);
	HC_Open_Segment_By_Key(m_nKey);
}

void SegmentKeyPrivate::Close()
{
	if (true == m_bForcedOpen) {
		return;
	}

	if (false == m_bOpen) {
		return;
	}

	m_bOpen = false;
	assert(INVALID_KEY != m_nKey);
	HC_Close_Segment();
}

void SegmentKeyPrivate::Close() const
{
	if (true == m_bForcedOpen) {
		return;
	}

	if (false == m_bOpen) {
		return;
	}

	m_bOpen = false;
	assert(INVALID_KEY != m_nKey);
	HC_Close_Segment();
}

void SegmentKeyPrivate::ForcedOpen()
{
	m_bForcedOpen = true;

	if (true == m_bOpen) {
		return;
	}

	m_bOpen = true;
	HC_Open_Segment_By_Key(m_nKey);
}

void SegmentKeyPrivate::ForcedClose()
{
	if (false == m_bOpen) {
		return;
	}

	m_bForcedOpen = false;
	m_bOpen = false;
	HC_Close_Segment();
}

bool SegmentKeyPrivate::IsOpen() const
{
	return m_bOpen;
}

bool SegmentKeyPrivate::IsForcedOpen() const
{
	return m_bForcedOpen;
}

HC_KEY const SegmentKeyPrivate::KeyValue() const
{
	return m_nKey;
}

void SegmentKeyPrivate::SetKeyValue(HC_KEY nInKey)
{
	m_nKey = nInKey;
}

