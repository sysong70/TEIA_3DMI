#include "StdAfx.h"

#include "KeyPrivate.h"
#include "../Key.h"

#include <HTools.h>

using namespace H3DF;

// Local Open/Close Function 함수
void H3DF::KeyPrivate::LocalOpen()
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

void H3DF::KeyPrivate::LocalOpen() const
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

void H3DF::KeyPrivate::LocalOpen(Key & cKey)
{
	KeyPrivate * pcImpl = (KeyPrivate *)cKey.GetImpl();
	pcImpl->LocalOpen();
}

void H3DF::KeyPrivate::LocalOpen(Key const & cKey)
{
	KeyPrivate * pcImpl = (KeyPrivate *)cKey.GetImpl();
	pcImpl->LocalOpen();
}

void H3DF::KeyPrivate::LocalClose()
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

void H3DF::KeyPrivate::LocalClose() const
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

void H3DF::KeyPrivate::LocalClose(Key & cKey)
{
	KeyPrivate * pcImpl = (KeyPrivate *)cKey.GetImpl();
	pcImpl->LocalClose();
}

void H3DF::KeyPrivate::LocalClose(Key const & cKey)
{
	KeyPrivate * pcImpl = (KeyPrivate *)cKey.GetImpl();
	pcImpl->LocalClose();
}

// Forced Open/Close Function 함수
void H3DF::KeyPrivate::ForcedOpen()
{
	if (true == m_bForcedOpen) {
		return;
	}

	if (true == m_bOpen) {
		return;
	}

	m_bOpen = true;
	m_bForcedOpen = true;

	assert(INVALID_KEY != m_nKey);
	HC_Open_Segment_By_Key(m_nKey);
}

void H3DF::KeyPrivate::ForcedOpen() const
{
	if (true == m_bForcedOpen) {
		return;
	}

	if (true == m_bOpen) {
		return;
	}

	m_bOpen = true;
	m_bForcedOpen = true;

	assert(INVALID_KEY != m_nKey);
	HC_Open_Segment_By_Key(m_nKey);
}

void H3DF::KeyPrivate::ForcedOpen(Key & cKey)
{
	KeyPrivate * pcImpl = (KeyPrivate *)cKey.GetImpl();
	pcImpl->ForcedOpen();

}

void H3DF::KeyPrivate::ForcedOpen(Key const & cKey)
{
	KeyPrivate * pcImpl = (KeyPrivate *)cKey.GetImpl();
	pcImpl->ForcedOpen();
}

void H3DF::KeyPrivate::ForcedClose()
{
	if (false == m_bForcedOpen) {
		return;
	}

	if (false == m_bOpen) {
		return;
	}

	m_bForcedOpen = false;
	m_bOpen = false;

	assert(INVALID_KEY != m_nKey);
	HC_Close_Segment();
}

void H3DF::KeyPrivate::ForcedClose() const
{
	if (false == m_bForcedOpen) {
		return;
	}

	if (false == m_bOpen) {
		return;
	}

	m_bForcedOpen = false;
	m_bOpen = false;

	assert(INVALID_KEY != m_nKey);
	HC_Close_Segment();
}

void H3DF::KeyPrivate::ForcedClose(Key & cKey)
{
	KeyPrivate * pcImpl = (KeyPrivate *)cKey.GetImpl();
	pcImpl->ForcedClose();
}

void H3DF::KeyPrivate::ForcedClose(Key const & cKey)
{
	KeyPrivate * pcImpl = (KeyPrivate *)cKey.GetImpl();
	pcImpl->ForcedClose();
}

// Open/Close Function 함수
void H3DF::KeyPrivate::Open()
{
	m_bForcedOpen = true;

	if (true == m_bOpen) {
		return;
	}

	m_bOpen = true;
	HC_Open_Segment_By_Key(m_nKey);
}

void H3DF::KeyPrivate::Close()
{
	if (false == m_bOpen) {
		return;
	}

	m_bForcedOpen = false;
	m_bOpen = false;
	HC_Close_Segment();
}

// 관련 함수
bool H3DF::KeyPrivate::IsLocalOpen() const
{
	return m_bOpen;
}

bool H3DF::KeyPrivate::IsForcedOpen() const
{
	return m_bForcedOpen;
}

HC_KEY const H3DF::KeyPrivate::KeyValue() const
{
	return m_nKey;
}

void H3DF::KeyPrivate::SetKeyValue(HC_KEY nInKey)
{
	m_nKey = nInKey;
}