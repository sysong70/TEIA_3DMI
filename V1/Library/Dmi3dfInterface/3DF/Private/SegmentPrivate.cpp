#include "StdAfx.h"

#include <HTools.h>

#include "SegmentPrivate.h"
#include "../Segment.h"

using namespace H3DF;

void H3DF::SegmentKeyPrivate::Copy(SegmentKeyPrivate * pcInThat)
{
	KeyPrivate::Copy(pcInThat);

	m_bOpen = pcInThat->m_bOpen;
	m_bForcedOpen = pcInThat->m_bForcedOpen;
	m_pcBaseView = pcInThat->m_pcBaseView;
}

//== Segment 관련 함수 ===============================================================================

// Local Open/Close Function 함수
void H3DF::SegmentKeyPrivate::LocalOpen()
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

void H3DF::SegmentKeyPrivate::LocalOpen() const
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

void H3DF::SegmentKeyPrivate::LocalOpen(SegmentKey & cSegmentKey)
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)cSegmentKey.GetImpl();
	pcImpl->LocalOpen();
}

void H3DF::SegmentKeyPrivate::LocalOpen(SegmentKey const & cSegmentKey)
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)cSegmentKey.GetImpl();
	pcImpl->LocalOpen();
}

void H3DF::SegmentKeyPrivate::LocalClose()
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

void H3DF::SegmentKeyPrivate::LocalClose() const
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

void H3DF::SegmentKeyPrivate::LocalClose(SegmentKey & cSegmentKey)
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)cSegmentKey.GetImpl();
	pcImpl->LocalClose();
}

void H3DF::SegmentKeyPrivate::LocalClose(SegmentKey const & cSegmentKey)
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *)cSegmentKey.GetImpl();
	pcImpl->LocalClose();
}

// Forced Open/Close Function 함수
void H3DF::SegmentKeyPrivate::ForcedOpen()
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

void H3DF::SegmentKeyPrivate::ForcedOpen() const
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

void H3DF::SegmentKeyPrivate::ForcedOpen(SegmentKey & cSegmentKey)
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *) cSegmentKey.GetImpl();
	pcImpl->ForcedOpen();

}

void H3DF::SegmentKeyPrivate::ForcedOpen(SegmentKey const & cSegmentKey)
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *) cSegmentKey.GetImpl();
	pcImpl->ForcedOpen();
}

void H3DF::SegmentKeyPrivate::ForcedClose()
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

void H3DF::SegmentKeyPrivate::ForcedClose() const
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

void H3DF::SegmentKeyPrivate::ForcedClose(SegmentKey & cSegmentKey)
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *) cSegmentKey.GetImpl();
	pcImpl->ForcedClose();
}

void H3DF::SegmentKeyPrivate::ForcedClose(SegmentKey const & cSegmentKey)
{
	SegmentKeyPrivate * pcImpl = (SegmentKeyPrivate *) cSegmentKey.GetImpl();
	pcImpl->ForcedClose();
}

// Open/Close Function 함수
void H3DF::SegmentKeyPrivate::Open()
{
	m_bForcedOpen = true;

	if (true == m_bOpen) {
		return;
	}

	m_bOpen = true;
	HC_Open_Segment_By_Key(m_nKey);
}

void H3DF::SegmentKeyPrivate::Close()
{
	if (false == m_bOpen) {
		return;
	}

	m_bForcedOpen = false;
	m_bOpen = false;
	HC_Close_Segment();
}

// 관련 함수
bool H3DF::SegmentKeyPrivate::IsLocalOpen() const
{
	return m_bOpen;
}

bool H3DF::SegmentKeyPrivate::IsForcedOpen() const
{
	return m_bForcedOpen;
}

HC_KEY const H3DF::SegmentKeyPrivate::KeyValue() const
{
	return m_nKey;
}

void H3DF::SegmentKeyPrivate::SetKeyValue(HC_KEY nInKey)
{
	m_nKey = nInKey;
}

void H3DF::SegmentKeyPrivate::SetColor(CString strInGeometryName, RGBAColor cInColor)
{
	CString strColorText;
	if (1.0f == cInColor.alpha) {
		strColorText.Format(L"%s = (diffuse = (r=%f g=%f b=%f))", strInGeometryName, cInColor.red, cInColor.green, cInColor.blue);
	}
	else {
		float fTransparency = 1.0f - cInColor.alpha;
		strColorText.Format(L"%s = (diffuse = (r=%f g=%f b=%f), transmission = (r=%f g=%f b=%f))", strInGeometryName, cInColor.red, cInColor.green, cInColor.blue, fTransparency, fTransparency, fTransparency);
	}

	HC_Set_Color(Utility::ToChar(strColorText));
}

BaseView * H3DF::SegmentKeyPrivate::GetBaseView() const
{
	return m_pcBaseView;
}

void H3DF::SegmentKeyPrivate::SetBaseView(BaseView * pcInBaseView)
{
	m_pcBaseView = pcInBaseView;
}