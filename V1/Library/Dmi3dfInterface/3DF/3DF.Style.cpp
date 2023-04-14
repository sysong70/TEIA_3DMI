#include "StdAfx.h"

#include "3DF.Style.h"
#include "3DF.Segment.h"
#include "3DF.Portfolio.h"

#include <hc.h>
#include <Htools.h>

USING_3DF_NAMESPACE

//== NamedStyleDefinition Function =================================================================

NamedStyleDefinition::NamedStyleDefinition(SegmentKey cInSource)
{
	m_nSourceSegmentKey = cInSource.KeyValue();;

}

NamedStyleDefinition::NamedStyleDefinition(NamedStyleDefinition const & cInThat)
{
	//m_strName = cInThat.Name();
	m_nSourceSegmentKey = cInThat.GetSource().KeyValue();
	m_nOwnerPortfolioKey = cInThat.Owner().KeyValue();
}

NamedStyleDefinition & NamedStyleDefinition::operator = (NamedStyleDefinition const & cInThat)
{
	//m_strName = cInThat.Name();
	m_nSourceSegmentKey = cInThat.GetSource().KeyValue();
	m_nOwnerPortfolioKey = cInThat.Owner().KeyValue();

	return *this;
}

SegmentKey NamedStyleDefinition::GetSource() const
{
	SegmentKey cSegment(m_nSourceSegmentKey);
	return cSegment;
}

PortfolioKey NamedStyleDefinition::Owner() const
{
	PortfolioKey cPortfolioKey(m_nOwnerPortfolioKey);
	return cPortfolioKey;
}

//== StyleKey Function =============================================================================
StyleKey::StyleKey(HC_KEY nInKey) :
	Key(nInKey)
{
}

StyleKey::StyleKey(StyleKey const & cInThat) :
	Key(cInThat)
{
}

void StyleKey::Set(StyleKey const & cInThat)
{
	Key::Set(cInThat);
}

StyleKey & StyleKey::operator = (StyleKey const & cInThat)
{
	Set(cInThat);
	return *this;
}

//== StyleControl Function =========================================================================

StyleControl::StyleControl(SegmentKey & cInSegment)
{
	m_nKey = cInSegment.KeyValue();
}

StyleControl::StyleControl(StyleControl const & cInThat)
{
	m_nKey = cInThat.KeyValue();
}

StyleControl & StyleControl::operator = (StyleControl const & cInThat)
{
	m_nKey = cInThat.KeyValue();
	return *this;
}

/*
StyleKey StyleControl::PushNamed(CString & strInStyleName)
{
	Open();
	HC_KEY nStyleKey = HC_Style_Segment(H_ASCII_TEXT(strInStyleName));
	Close();

	StyleKey cStyle(nStyleKey);
	return cStyle;
}
*/

StyleKey StyleControl::PushSegment(SegmentKey const & cInStyleSource)
{
	Open();
	HC_KEY nStyleKey = HC_Style_Segment_By_Key(cInStyleSource.KeyValue());
	Close();

	StyleKey cStyle(nStyleKey);
	return cStyle;
}

