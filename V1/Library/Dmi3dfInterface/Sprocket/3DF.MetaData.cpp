#include "StdAfx.h"

#include "3DF.MetaData.h"
#include "Impl/MetaDataImpl.h"

#include "Common_Define.h"

#include "../3DF/Segment.h"
#include "../3DF/3DF.Utility.h"
#include "../3DF/KeyPath.h"
#include "../3DF/Selection.h"
#include "../3DF/Impl/Selection.Impl.h"

#include <ranges>

using namespace H3DF;

//== MetaData Class ================================================================================

H3DF::MetaData::MetaData()
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<MetaDataImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::MetaData::MetaData(MetaData const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

H3DF::MetaData::MetaData(MetaDataIndex nInIndex)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<MetaDataImpl>();
	DEBUG_VALID(m_pcImpl);

	auto pcImpl = static_cast<MetaDataImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_eIndex = nInIndex;
}

H3DF::MetaData & H3DF::MetaData::operator = (MetaData const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

bool H3DF::MetaData::Equals(MetaData const & cInThat) const
{
	auto pcImpl = static_cast<MetaDataImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	auto pcInThatImpl = static_cast<MetaDataImpl *>(cInThat.m_pcImpl.get());
	DEBUG_VALID(pcInThatImpl);

	return pcImpl->Equals(pcInThatImpl);
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
	auto pcImpl = static_cast<MetaDataImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	return pcImpl->m_eIndex;
}

void H3DF::MetaData::SetIndex(MetaDataIndex nInIndex)
{
	auto pcImpl = static_cast<MetaDataImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_eIndex = nInIndex;
}

//== StringMetaData Class ==========================================================================

H3DF::StringMetaData::StringMetaData()
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<StringMetaDataImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::StringMetaData::StringMetaData(MetaData const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	// PolygonShapeElementImpl 생성
	m_pcImpl = std::make_unique<StringMetaDataImpl>();
	auto pcImpl = static_cast<StringMetaDataImpl *>(m_pcImpl.get());

	auto pcInThatImpl = static_cast<const MetaDataImpl *>(cInThat.GetImpl());

	if (nullptr != pcImpl && nullptr != pcInThatImpl) {
		pcImpl->MetaDataImpl::Copy(pcInThatImpl);
	}
	else {
		DEBUG_STOP;
	}
}

H3DF::StringMetaData::StringMetaData(StringMetaData const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

H3DF::StringMetaData::StringMetaData(MetaDataIndex nInIndex, CString strInValue)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<StringMetaDataImpl>();
	DEBUG_VALID(m_pcImpl);

	auto pcImpl = static_cast<StringMetaDataImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_eIndex = nInIndex;
	pcImpl->m_strValue = strInValue;
}

StringMetaData & H3DF::StringMetaData::operator = (StringMetaData const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

bool H3DF::StringMetaData::operator != (StringMetaData const & cInThat) const
{
	return !(*this == cInThat);
}

bool H3DF::StringMetaData::operator == (StringMetaData const & cInThat) const
{
	auto pcImpl = static_cast<StringMetaDataImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	auto pcInThatImpl = static_cast<StringMetaDataImpl *>(cInThat.m_pcImpl.get());
	DEBUG_VALID(pcInThatImpl);

	return pcImpl->Equals(pcInThatImpl);
}

CString H3DF::StringMetaData::GetValue() const
{
	auto pcImpl = static_cast<StringMetaDataImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	return pcImpl->m_strValue;
}

void H3DF::StringMetaData::SetValue(CString strInValue)
{
	auto pcImpl = static_cast<StringMetaDataImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_strValue = strInValue;
}

//== DwordMetaData Class ===========================================================================
H3DF::DwordPtrMetaData::DwordPtrMetaData()
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<DwordPtrMetaDataImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::DwordPtrMetaData::DwordPtrMetaData(MetaData const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	// PolygonShapeElementImpl 생성
	m_pcImpl = std::make_unique<DwordPtrMetaDataImpl>();
	auto pcImpl = static_cast<DwordPtrMetaDataImpl *>(m_pcImpl.get());

	auto pcInThatImpl = static_cast<const MetaDataImpl *>(cInThat.GetImpl());

	if (nullptr != pcImpl && nullptr != pcInThatImpl) {
		pcImpl->MetaDataImpl::Copy(pcInThatImpl);
	}
	else {
		DEBUG_STOP;
	}
}

H3DF::DwordPtrMetaData::DwordPtrMetaData(DwordPtrMetaData const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

H3DF::DwordPtrMetaData::DwordPtrMetaData(MetaDataIndex nInIndex, DWORD_PTR nInValue)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<DwordPtrMetaDataImpl>();
	DEBUG_VALID(m_pcImpl);

	auto pcImpl = static_cast<DwordPtrMetaDataImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_eIndex = nInIndex;
	pcImpl->m_nValue = nInValue;
}

DwordPtrMetaData & H3DF::DwordPtrMetaData::operator = (DwordPtrMetaData const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

bool H3DF::DwordPtrMetaData::operator != (DwordPtrMetaData const & cInThat) const
{
	return !(*this == cInThat);
}

bool H3DF::DwordPtrMetaData::operator == (DwordPtrMetaData const & cInThat) const
{
	auto pcImpl = static_cast<DwordPtrMetaDataImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	auto pcInThatImpl = static_cast<DwordPtrMetaDataImpl *>(cInThat.m_pcImpl.get());
	DEBUG_VALID(pcInThatImpl);


	return pcImpl->Equals(pcInThatImpl);
}

DWORD_PTR H3DF::DwordPtrMetaData::GetValue() const
{
	auto pcImpl = static_cast<DwordPtrMetaDataImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	return pcImpl->m_nValue;
}

void H3DF::DwordPtrMetaData::SetValue(DWORD_PTR nInValue)
{
	auto pcImpl = static_cast<DwordPtrMetaDataImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_nValue = nInValue;
}