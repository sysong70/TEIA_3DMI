#include "StdAfx.h"

#include <HTools.h>

#include "SegmentImpl.h"
#include "../Segment.h"

#include "../Bounding.h"

using namespace H3DF;

void H3DF::SegmentKeyImpl::Copy(const SegmentKeyImpl * pcInThat)
{
	KeyImpl::Copy(pcInThat);

	m_bOpen = pcInThat->m_bOpen;
	m_bForcedOpen = pcInThat->m_bForcedOpen;
	m_pcWindowKey = pcInThat->m_pcWindowKey;

	if (nullptr != pcInThat->m_pcBoundingKit) {
		m_pcBoundingKit = std::make_unique<BoundingKit>(*pcInThat->m_pcBoundingKit);
	}
}

//== Segment 관련 함수 ===============================================================================

// Local Open/Close Function 함수
void H3DF::SegmentKeyImpl::LocalOpen()
{
	if (true == m_bForcedOpen) {
		return;
	}

	if (true == m_bOpen) {
		return;
	}

	m_bOpen = true;

	assert(INVALID_KEY != KeyValue());
	HC_Open_Segment_By_Key(KeyValue());
}

void H3DF::SegmentKeyImpl::LocalOpen() const
{
	if (true == m_bForcedOpen) {
		return;
	}

	if (true == m_bOpen) {
		return;
	}

	m_bOpen = true;

	assert(INVALID_KEY != KeyValue());
	HC_Open_Segment_By_Key(KeyValue());
}

void H3DF::SegmentKeyImpl::LocalOpen(SegmentKey & cSegmentKey)
{
	SegmentKeyImpl * pcImpl = (SegmentKeyImpl *)cSegmentKey.GetImpl();
	pcImpl->LocalOpen();
}

void H3DF::SegmentKeyImpl::LocalOpen(SegmentKey const & cSegmentKey)
{
	SegmentKeyImpl * pcImpl = (SegmentKeyImpl *)cSegmentKey.GetImpl();
	pcImpl->LocalOpen();
}

void H3DF::SegmentKeyImpl::LocalClose()
{
	if (true == m_bForcedOpen) {
		return;
	}

	if (false == m_bOpen) {
		return;
	}

	m_bOpen = false;
	assert(INVALID_KEY != KeyValue());
	HC_Close_Segment();
}

void H3DF::SegmentKeyImpl::LocalClose() const
{
	if (true == m_bForcedOpen) {
		return;
	}

	if (false == m_bOpen) {
		return;
	}

	m_bOpen = false;
	assert(INVALID_KEY != KeyValue());
	HC_Close_Segment();
}

void H3DF::SegmentKeyImpl::LocalClose(SegmentKey & cSegmentKey)
{
	SegmentKeyImpl * pcImpl = (SegmentKeyImpl *)cSegmentKey.GetImpl();
	pcImpl->LocalClose();
}

void H3DF::SegmentKeyImpl::LocalClose(SegmentKey const & cSegmentKey)
{
	SegmentKeyImpl * pcImpl = (SegmentKeyImpl *)cSegmentKey.GetImpl();
	pcImpl->LocalClose();
}

// Forced Open/Close Function 함수
void H3DF::SegmentKeyImpl::ForcedOpen()
{
	if (true == m_bForcedOpen) {
		return;
	}

	if (true == m_bOpen) {
		return;
	}

	m_bOpen = true;
	m_bForcedOpen = true;

	assert(INVALID_KEY != KeyValue());
	HC_Open_Segment_By_Key(KeyValue());
}

void H3DF::SegmentKeyImpl::ForcedOpen() const
{
	if (true == m_bForcedOpen) {
		return;
	}

	if (true == m_bOpen) {
		return;
	}

	m_bOpen = true;
	m_bForcedOpen = true;

	assert(INVALID_KEY != KeyValue());
	HC_Open_Segment_By_Key(KeyValue());
}

void H3DF::SegmentKeyImpl::ForcedOpen(SegmentKey & cSegmentKey)
{
	SegmentKeyImpl * pcImpl = (SegmentKeyImpl *)cSegmentKey.GetImpl();
	pcImpl->ForcedOpen();
}

void H3DF::SegmentKeyImpl::ForcedOpen(SegmentKey const & cSegmentKey)
{
	SegmentKeyImpl * pcImpl = (SegmentKeyImpl *)cSegmentKey.GetImpl();
	pcImpl->ForcedOpen();
}

void H3DF::SegmentKeyImpl::ForcedClose()
{
	if (false == m_bForcedOpen) {
		return;
	}

	if (false == m_bOpen) {
		return;
	}

	m_bForcedOpen = false;
	m_bOpen = false;

	assert(INVALID_KEY != KeyValue());
	HC_Close_Segment();
}

void H3DF::SegmentKeyImpl::ForcedClose() const
{
	if (false == m_bForcedOpen) {
		return;
	}

	if (false == m_bOpen) {
		return;
	}

	m_bForcedOpen = false;
	m_bOpen = false;

	assert(INVALID_KEY != KeyValue());
	HC_Close_Segment();
}

void H3DF::SegmentKeyImpl::ForcedClose(SegmentKey & cSegmentKey)
{
	SegmentKeyImpl * pcImpl = (SegmentKeyImpl *)cSegmentKey.GetImpl();
	pcImpl->ForcedClose();
}

void H3DF::SegmentKeyImpl::ForcedClose(SegmentKey const & cSegmentKey)
{
	SegmentKeyImpl * pcImpl = (SegmentKeyImpl *)cSegmentKey.GetImpl();
	pcImpl->ForcedClose();
}

// Open/Close Function 함수
void H3DF::SegmentKeyImpl::Open()
{
	m_bForcedOpen = true;

	if (true == m_bOpen) {
		return;
	}

	m_bOpen = true;
	HC_Open_Segment_By_Key(KeyValue());
}

void H3DF::SegmentKeyImpl::Close()
{
	if (false == m_bOpen) {
		return;
	}

	m_bForcedOpen = false;
	m_bOpen = false;
	HC_Close_Segment();
}

// 관련 함수
bool H3DF::SegmentKeyImpl::IsLocalOpen() const
{
	return m_bOpen;
}

bool H3DF::SegmentKeyImpl::IsForcedOpen() const
{
	return m_bForcedOpen;
}

void H3DF::SegmentKeyImpl::SetColor(CStringA strInGeometryName, RGBAColor cInColor)
{
	CStringA strColorText;
	if (1.0f == cInColor.alpha) {
		strColorText.Format("%s = (diffuse = (r=%f g=%f b=%f))", strInGeometryName, cInColor.red, cInColor.green, cInColor.blue);
	}
	else {
		float fTransparency = 1.0f - cInColor.alpha;
		strColorText.Format("%s = (diffuse = (r=%f g=%f b=%f), transmission = (r=%f g=%f b=%f))", strInGeometryName, cInColor.red, cInColor.green, cInColor.blue, fTransparency, fTransparency, fTransparency);
	}

	HC_Set_Color(strColorText);
}

WindowKey * H3DF::SegmentKeyImpl::GetWindow() const
{
	DEBUG_VALID(m_pcWindowKey);
	return m_pcWindowKey;
}

void H3DF::SegmentKeyImpl::SetWindow(WindowKey * pcInWindow)
{
	m_pcWindowKey = pcInWindow;
}

// 찾는 요소가 찾아지면 바로 멈춤. 제한적으로 사용해야 함.
bool H3DF::SegmentKeyImpl::FindUp(SegmentKey & cInSegment, H3DF::Type eInType, SegmentKey & cOutFindSegment)
{
	SegmentKey cOwner = cInSegment.Owner();

	bool bResult = false;
	SegmentKey cFindSegment;
	while (true == cOwner.IsValidate()) {
		if (eInType == cOwner.Type()) {
			cFindSegment = cOwner;
			bResult = true;
			break;
		}

		cOwner = cOwner.Owner();
	}

	return bResult;
}

void H3DF::SegmentKeyUtility::LocalOpen(SegmentKey & cSegmentKey)
{
	SegmentKeyImpl * pcImpl = (SegmentKeyImpl *) cSegmentKey.GetImpl();
	pcImpl->LocalOpen();
}

void H3DF::SegmentKeyUtility::LocalOpen(SegmentKey const & cSegmentKey)
{
	SegmentKeyImpl * pcImpl = (SegmentKeyImpl *) cSegmentKey.GetImpl();
	pcImpl->LocalOpen();
}

void H3DF::SegmentKeyUtility::LocalClose(SegmentKey & cSegmentKey)
{
	SegmentKeyImpl * pcImpl = (SegmentKeyImpl *) cSegmentKey.GetImpl();
	pcImpl->LocalClose();

}

void H3DF::SegmentKeyUtility::LocalClose(SegmentKey const & cSegmentKey)
{
	SegmentKeyImpl * pcImpl = (SegmentKeyImpl *) cSegmentKey.GetImpl();
	pcImpl->LocalClose();
}

void H3DF::SegmentKeyUtility::ForcedOpen(SegmentKey & cSegmentKey)
{
	SegmentKeyImpl * pcImpl = (SegmentKeyImpl *) cSegmentKey.GetImpl();
	pcImpl->ForcedOpen();
}

void H3DF::SegmentKeyUtility::ForcedOpen(SegmentKey const & cSegmentKey)
{
	SegmentKeyImpl * pcImpl = (SegmentKeyImpl *) cSegmentKey.GetImpl();
	pcImpl->ForcedOpen();
}

void H3DF::SegmentKeyUtility::ForcedClose(SegmentKey & cSegmentKey)
{
	SegmentKeyImpl * pcImpl = (SegmentKeyImpl *) cSegmentKey.GetImpl();
	pcImpl->ForcedClose();
}

void H3DF::SegmentKeyUtility::ForcedClose(SegmentKey const & cSegmentKey)
{
	SegmentKeyImpl * pcImpl = (SegmentKeyImpl *) cSegmentKey.GetImpl();
	pcImpl->ForcedClose();
}
