#include "StdAfx.h"

#include "GeometryImpl.h"

#include "../Geometry.h"

#include "KeyImpl.h"

#include "Math.h"

#include <HTools.h>

using namespace H3DF;

//== GeometryKeyImpl 관련 함수 =======================================================================


void H3DF::GeometryKeyImpl::Copy(GeometryKeyImpl * pcInThat)
{
	KeyImpl::Copy(pcInThat);

	m_bOpen = pcInThat->m_bOpen;
	m_bForcedOpen = pcInThat->m_bForcedOpen;
}

// Local Open/Close Function 함수
void H3DF::GeometryKeyImpl::LocalOpen()
{
	if (true == m_bForcedOpen) {
		return;
	}

	if (true == m_bOpen) {
		return;
	}

	m_bOpen = true;

	assert(INVALID_KEY != KeyValue());
	HC_Open_Geometry(KeyValue());
}

void H3DF::GeometryKeyImpl::LocalOpen() const
{
	if (true == m_bForcedOpen) {
		return;
	}

	if (true == m_bOpen) {
		return;
	}

	m_bOpen = true;

	assert(INVALID_KEY != KeyValue());
	HC_Open_Geometry(KeyValue());
}

void H3DF::GeometryKeyImpl::LocalClose()
{
	if (true == m_bForcedOpen) {
		return;
	}

	if (false == m_bOpen) {
		return;
	}

	m_bOpen = false;
	assert(INVALID_KEY != KeyValue());
	HC_Close_Geometry();
}

void H3DF::GeometryKeyImpl::LocalClose() const
{
	if (true == m_bForcedOpen) {
		return;
	}

	if (false == m_bOpen) {
		return;
	}

	m_bOpen = false;
	assert(INVALID_KEY != KeyValue());
	HC_Close_Geometry();
}

void H3DF::GeometryKeyImpl::LocalOpen(GeometryKey & cGeometryKey)
{
	GeometryKeyImpl * pcImpl = dynamic_cast<GeometryKeyImpl *>(cGeometryKey.GetImpl());
	DEBUG_VALID(pcImpl);

	pcImpl->LocalOpen();
}

void H3DF::GeometryKeyImpl::LocalOpen(GeometryKey const & cGeometryKey)
{
	const GeometryKeyImpl * pcImpl = dynamic_cast<const GeometryKeyImpl *>(cGeometryKey.GetImpl());
	DEBUG_VALID(pcImpl);

	pcImpl->LocalOpen();
}

void H3DF::GeometryKeyImpl::LocalClose(GeometryKey & cGeometryKey)
{
	GeometryKeyImpl * pcImpl = dynamic_cast<GeometryKeyImpl *>(cGeometryKey.GetImpl());
	DEBUG_VALID(pcImpl);

	pcImpl->LocalClose();
}

void H3DF::GeometryKeyImpl::LocalClose(GeometryKey const & cGeometryKey)
{
	const GeometryKeyImpl * pcImpl = dynamic_cast<const GeometryKeyImpl *>(cGeometryKey.GetImpl());
	DEBUG_VALID(pcImpl);

	pcImpl->LocalClose();
}

// Forced Open/Close Function 함수
void H3DF::GeometryKeyImpl::ForcedOpen()
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
	HC_Open_Geometry(KeyValue());
}

void H3DF::GeometryKeyImpl::ForcedOpen() const
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
	HC_Open_Geometry(KeyValue());
}

void H3DF::GeometryKeyImpl::ForcedOpen(GeometryKey & cGeometryKey)
{
	GeometryKeyImpl * pcImpl = (GeometryKeyImpl *)cGeometryKey.GetImpl();
	pcImpl->ForcedOpen();

}

void H3DF::GeometryKeyImpl::ForcedOpen(GeometryKey const & cGeometryKey)
{
	GeometryKeyImpl * pcImpl = (GeometryKeyImpl *)cGeometryKey.GetImpl();
	pcImpl->ForcedOpen();
}

void H3DF::GeometryKeyImpl::ForcedClose()
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
	HC_Close_Geometry();
}

void H3DF::GeometryKeyImpl::ForcedClose() const
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
	HC_Close_Geometry();
}

void H3DF::GeometryKeyImpl::ForcedClose(GeometryKey & cGeometryKey)
{
	GeometryKeyImpl * pcImpl = (GeometryKeyImpl *)cGeometryKey.GetImpl();
	pcImpl->ForcedClose();
}

void H3DF::GeometryKeyImpl::ForcedClose(GeometryKey const & cGeometryKey)
{
	GeometryKeyImpl * pcImpl = (GeometryKeyImpl *)cGeometryKey.GetImpl();
	pcImpl->ForcedClose();
}

// Open/Close Function 함수
void H3DF::GeometryKeyImpl::Open()
{
	m_bForcedOpen = true;

	if (true == m_bOpen) {
		return;
	}

	m_bOpen = true;
	HC_Open_Geometry(KeyValue());
}

void H3DF::GeometryKeyImpl::Close()
{
	if (false == m_bOpen) {
		return;
	}

	m_bForcedOpen = false;
	m_bOpen = false;
	HC_Close_Geometry();
}

// 관련 함수
bool H3DF::GeometryKeyImpl::IsLocalOpen() const
{
	return m_bOpen;
}

bool H3DF::GeometryKeyImpl::IsForcedOpen() const
{
	return m_bForcedOpen;
}