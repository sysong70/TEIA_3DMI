#include "StdAfx.h"

#include "Style.h"
#include "Segment.h"
#include "Private/SegmentPrivate.h"
#include "Portfolio.h"

#include <hc.h>
#include <Htools.h>

using namespace H3DF;

//== NamedStyleDefinition Function =================================================================

H3DF::NamedStyleDefinition::NamedStyleDefinition(SegmentKey cInSource)
{
	m_nSourceSegmentKey = cInSource.KeyValue();;

}

H3DF::NamedStyleDefinition::NamedStyleDefinition(NamedStyleDefinition const & cInThat)
{
	//m_strName = cInThat.Name();
	m_nSourceSegmentKey = cInThat.GetSource().KeyValue();
	m_nOwnerPortfolioKey = cInThat.Owner().KeyValue();
}

NamedStyleDefinition & H3DF::NamedStyleDefinition::operator = (NamedStyleDefinition const & cInThat)
{
	//m_strName = cInThat.Name();
	m_nSourceSegmentKey = cInThat.GetSource().KeyValue();
	m_nOwnerPortfolioKey = cInThat.Owner().KeyValue();

	return *this;
}

SegmentKey H3DF::NamedStyleDefinition::GetSource() const
{
	SegmentKey cSegment(m_nSourceSegmentKey);
	return cSegment;
}

PortfolioKey H3DF::NamedStyleDefinition::Owner() const
{
	PortfolioKey cPortfolioKey(m_nOwnerPortfolioKey);
	return cPortfolioKey;
}

//== StyleKey Function =============================================================================
H3DF::StyleKey::StyleKey(HC_KEY nInKey) :
	Key(nInKey)
{
}

H3DF::StyleKey::StyleKey(StyleKey const & cInThat) :
	Key(cInThat)
{
}

void H3DF::StyleKey::Set(StyleKey const & cInThat)
{
	Key::Set(cInThat);
}

StyleKey & H3DF::StyleKey::operator = (StyleKey const & cInThat)
{
	Set(cInThat);
	return *this;
}

//== StyleControl Function =========================================================================

class StyleControlPrivate : public PrivateImpl
{
public:
	StyleControlPrivate() { m_eType = H3DF::Type::StyleControl; }

	void Copy(StyleControlPrivate * pcInThat) {
		m_cKey = pcInThat->GetSegmentKey();
	}

	SegmentKey & GetSegmentKey() { return m_cKey; }

public:
	SegmentKey m_cKey;
};

H3DF::StyleControl::StyleControl(SegmentKey & cInSegment)
{
	StyleControlPrivate * pcImpl = new StyleControlPrivate();
	if (nullptr == pcImpl) { assert(false); }

	m_pcImpl = pcImpl;

	pcImpl->m_cKey = cInSegment;
}

H3DF::StyleControl::StyleControl(StyleControl const & cInThat)
{
	m_pcImpl = new StyleControlPrivate();
	if (nullptr == m_pcImpl) { assert(false); }

	Set(cInThat);
}

void H3DF::StyleControl::Set(StyleControl const & cInThat)
{
	StyleControlPrivate * pcImpl = (StyleControlPrivate *)m_pcImpl;
	if (nullptr == pcImpl) { assert(false); }
	StyleControlPrivate * pcInThatImpl = (StyleControlPrivate *)cInThat.m_pcImpl;
	if (nullptr == pcInThatImpl) { assert(false); }

	pcImpl->Copy(pcInThatImpl);
}

StyleControl & H3DF::StyleControl::operator = (StyleControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

/*
StyleKey H3DF::StyleControl::PushNamed(CString & strInStyleName)
{
	Open();
	HC_KEY nStyleKey = HC_Style_Segment(Utility::ToChar(strInStyleName));
	Close();

	StyleKey cStyle(nStyleKey);
	return cStyle;
}
*/

StyleKey H3DF::StyleControl::PushSegment(SegmentKey const & cInStyleSource)
{
	StyleControlPrivate * pcImpl = (StyleControlPrivate *)m_pcImpl;
	if (nullptr == pcImpl) { assert(false); }

	SegmentKeyPrivate::LocalOpen(pcImpl->GetSegmentKey());
	
	HC_KEY nStyleKey = HC_Style_Segment_By_Key(cInStyleSource.KeyValue());
	
	SegmentKeyPrivate::LocalClose(pcImpl->GetSegmentKey());

	StyleKey cStyle(nStyleKey);
	return cStyle;
}

