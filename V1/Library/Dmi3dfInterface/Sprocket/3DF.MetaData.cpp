#include "StdAfx.h"

#include "3DF.MetaData.h"
#include "Impl/MetaDataImpl.h"

#include "Common_Define.h"

#include "../3DF/Segment.h"
#include "../3DF/3DF.Utility.h"
#include "../3DF/KeyPath.h"
#include "../3DF/Selection.h"
#include "../3DF/Impl/SelectionImpl.h"

#include <ranges>

using namespace H3DF;

//== MetaData Class ================================================================================

H3DF::MetaData::MetaData()
{
	m_pcImpl = new MetaDataImpl();
	DEBUG_VALID(m_pcImpl);
}

H3DF::MetaData::MetaData(MetaData const & cInThat)
{
	MetaDataImpl * pcImpl = new MetaDataImpl();
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;

	Set(cInThat);
}

H3DF::MetaData::MetaData(MetaDataIndex nInIndex)
{
	MetaDataImpl * pcImpl = new MetaDataImpl();
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;

	pcImpl->m_eIndex = nInIndex;
}

void H3DF::MetaData::Set(MetaData const & cInThat)
{
	MetaDataImpl * pcImpl = (MetaDataImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	MetaDataImpl * pcInThatImpl = (MetaDataImpl *)cInThat.m_pcImpl;
	DEBUG_VALID(pcInThatImpl);

	pcImpl->Copy(pcInThatImpl);
}

H3DF::MetaData & H3DF::MetaData::operator = (MetaData const & cInThat)
{
	Set(cInThat);
	return *this;
}

bool H3DF::MetaData::Equals(MetaData const & cInThat) const
{
	MetaDataImpl * pcImpl = (MetaDataImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	MetaDataImpl * pcInThatImpl = (MetaDataImpl *)cInThat.m_pcImpl;
	DEBUG_VALID(pcInThatImpl);

	if (pcImpl->m_eIndex != pcInThatImpl->m_eIndex) {
		return false;
	}

	return true;
}

bool H3DF::MetaData::operator != (MetaData const & cInThat) const
{
	return !Equals(cInThat);
}

bool H3DF::MetaData::operator == (MetaData const & cInThat) const
{
	return Equals(cInThat);
}

MetaDataIndex H3DF::MetaData::GetIndex() const
{
	MetaDataImpl * pcImpl = (MetaDataImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_eIndex;
}

void H3DF::MetaData::SetIndex(MetaDataIndex nInIndex)
{
	MetaDataImpl * pcImpl = (MetaDataImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_eIndex = nInIndex;
}

//== StringMetaData Class ==========================================================================

H3DF::StringMetaData::StringMetaData()
{
	if (nullptr != m_pcImpl) {
		REMOVE_POINTER(m_pcImpl);
	}

	m_pcImpl = new StringMetaDataImpl();
	DEBUG_VALID(m_pcImpl);
}

H3DF::StringMetaData::StringMetaData(MetaData const & cInThat)
{
	if (nullptr != m_pcImpl) {
		REMOVE_POINTER(m_pcImpl);
	}

	StringMetaDataImpl * pcImpl = new StringMetaDataImpl();
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;

	if (H3DF::Type::StringMetaData == cInThat.Type()) {
		Set(cInThat);
	}
	else {
		MetaData::Set(cInThat);
	}
}

H3DF::StringMetaData::StringMetaData(StringMetaData const & cInThat)
{
	if (nullptr != m_pcImpl) {
		REMOVE_POINTER(m_pcImpl);
	}

	StringMetaDataImpl * pcImpl = new StringMetaDataImpl();
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;

	Set(cInThat);
}

H3DF::StringMetaData::StringMetaData(MetaDataIndex nInIndex, CString strInValue)
{
	if (nullptr != m_pcImpl) {
		REMOVE_POINTER(m_pcImpl);
	}

	StringMetaDataImpl * pcImpl = new StringMetaDataImpl();
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;

	pcImpl->m_eIndex = nInIndex;
	pcImpl->m_strValue = strInValue;
}

void H3DF::StringMetaData::Set(StringMetaData const & cInThat)
{
	StringMetaDataImpl * pcImpl = (StringMetaDataImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	StringMetaDataImpl * pcInThatImpl = (StringMetaDataImpl *)cInThat.m_pcImpl;
	DEBUG_VALID(pcInThatImpl);

	pcImpl->Copy(pcInThatImpl);
}

StringMetaData & H3DF::StringMetaData::operator = (StringMetaData const & cInThat)
{
	Set(cInThat);
	return *this;
}

bool H3DF::StringMetaData::Equals(StringMetaData const & cInThat) const
{
	StringMetaDataImpl * pcImpl = (StringMetaDataImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	StringMetaDataImpl * pcInThatImpl = (StringMetaDataImpl *)cInThat.m_pcImpl;
	DEBUG_VALID(pcInThatImpl);

	if (pcImpl->m_eIndex != pcInThatImpl->m_eIndex) {
		return false;
	}

	if (pcImpl->m_strValue != pcInThatImpl->m_strValue) {
		return false;
	}

	return true;
}

bool H3DF::StringMetaData::operator != (StringMetaData const & cInThat) const
{
	return !Equals(cInThat);
}

bool H3DF::StringMetaData::operator == (StringMetaData const & cInThat) const
{
	return Equals(cInThat);
}

CString H3DF::StringMetaData::GetValue() const
{
	StringMetaDataImpl * pcImpl = (StringMetaDataImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_strValue;
}

void H3DF::StringMetaData::SetValue(CString strInValue)
{
	StringMetaDataImpl * pcImpl = (StringMetaDataImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_strValue = strInValue;
}

//== DwordMetaData Class ===========================================================================
H3DF::DwordPtrMetaData::DwordPtrMetaData()
{
	if (nullptr != m_pcImpl) {
		REMOVE_POINTER(m_pcImpl);
	}

	m_pcImpl = new DwordPtrMetaDataImpl();
	DEBUG_VALID(m_pcImpl);
}

H3DF::DwordPtrMetaData::DwordPtrMetaData(MetaData const & cInThat)
{
	if (nullptr != m_pcImpl) {
		REMOVE_POINTER(m_pcImpl);
	}

	DwordPtrMetaDataImpl * pcImpl = new DwordPtrMetaDataImpl();
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;

	if (H3DF::Type::DwordPtrMetaData == cInThat.Type()) {
		Set(cInThat);
	}
	else {
		MetaData::Set(cInThat);
	}
}

H3DF::DwordPtrMetaData::DwordPtrMetaData(DwordPtrMetaData const & cInThat)
{
	if (nullptr != m_pcImpl) {
		REMOVE_POINTER(m_pcImpl);
	}

	DwordPtrMetaDataImpl * pcImpl = new DwordPtrMetaDataImpl();
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;

	Set(cInThat);
}

H3DF::DwordPtrMetaData::DwordPtrMetaData(MetaDataIndex nInIndex, DWORD_PTR nInValue)
{
	if (nullptr != m_pcImpl) {
		REMOVE_POINTER(m_pcImpl);
	}

	DwordPtrMetaDataImpl * pcImpl = new DwordPtrMetaDataImpl();
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;

	pcImpl->m_eIndex = nInIndex;
	pcImpl->m_nValue = nInValue;
}

void H3DF::DwordPtrMetaData::Set(DwordPtrMetaData const & cInThat)
{
	DwordPtrMetaDataImpl * pcImpl = (DwordPtrMetaDataImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	DwordPtrMetaDataImpl * pcInThatImpl = (DwordPtrMetaDataImpl *)cInThat.m_pcImpl;
	DEBUG_VALID(pcInThatImpl);

	pcImpl->Copy(pcInThatImpl);
}

DwordPtrMetaData & H3DF::DwordPtrMetaData::operator = (DwordPtrMetaData const & cInThat)
{
	Set(cInThat);
	return *this;
}

bool H3DF::DwordPtrMetaData::Equals(DwordPtrMetaData const & cInThat) const
{
	DwordPtrMetaDataImpl * pcImpl = (DwordPtrMetaDataImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	DwordPtrMetaDataImpl * pcInThatImpl = (DwordPtrMetaDataImpl *)cInThat.m_pcImpl;
	DEBUG_VALID(pcInThatImpl);

	if (pcImpl->m_eIndex != pcInThatImpl->m_eIndex) {
		return false;
	}

	if (pcImpl->m_nValue != pcInThatImpl->m_nValue) {
		return false;
	}

	return true;
}

bool H3DF::DwordPtrMetaData::operator != (DwordPtrMetaData const & cInThat) const
{
	return !Equals(cInThat);
}

bool H3DF::DwordPtrMetaData::operator == (DwordPtrMetaData const & cInThat) const
{
	return Equals(cInThat);
}

DWORD_PTR H3DF::DwordPtrMetaData::GetValue() const
{
	DwordPtrMetaDataImpl * pcImpl = (DwordPtrMetaDataImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_nValue;
}

void H3DF::DwordPtrMetaData::SetValue(DWORD_PTR nInValue)
{
	DwordPtrMetaDataImpl * pcImpl = (DwordPtrMetaDataImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_nValue = nInValue;
}