#include "StdAfx.h"

#include "Style.h"
#include "Segment.h"
#include "Impl/SegmentImpl.h"
#include "Portfolio.h"

#include "Impl/ControlImpl.h"

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

H3DF::StyleKey::StyleKey() : Key()
{
}

H3DF::StyleKey::StyleKey(HC_KEY nInKey) : Key(nInKey)
{
}

H3DF::StyleKey::StyleKey(StyleKey const & cInThat) : Key(cInThat)
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

class StyleControlImpl : public ControlImpl
{
public:
	StyleControlImpl() { m_eType = H3DF::Type::StyleControl; }

	void Copy(StyleControlImpl * pcInThat) {
		ControlImpl::Copy(pcInThat);
	}
};

H3DF::StyleControl::StyleControl(SegmentKey & cInSegment) 
{
	StyleControlImpl * pcImpl = new StyleControlImpl();
	DEBUG_VALID(pcImpl);

	pcImpl->m_cOverrideKey = cInSegment;
	m_pcImpl = pcImpl;
}

H3DF::StyleControl::StyleControl(StyleControl const & cInThat)
{
	m_pcImpl = new StyleControlImpl();
	DEBUG_VALID(m_pcImpl);

	Set(cInThat);
}

void H3DF::StyleControl::Set(StyleControl const & cInThat)
{
	StyleControlImpl * pcImpl = (StyleControlImpl *)m_pcImpl;
	StyleControlImpl * pcInThatImpl = (StyleControlImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

StyleControl & H3DF::StyleControl::operator = (StyleControl const & cInThat)
{
	Set(cInThat);
	return *this;
}


StyleKey H3DF::StyleControl::PushNamed(CString & strInStyleName)
{
	StyleControlImpl * pcImpl = (StyleControlImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey);
	HC_KEY nStyleKey = HC_Style_Segment(Utility::ToChar(strInStyleName));
	SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	StyleKey cStyle(nStyleKey);
	return cStyle;
}

StyleKey H3DF::StyleControl::PushSegment(SegmentKey const & cInStyleSource)
{
	StyleControlImpl * pcImpl = (StyleControlImpl *)m_pcImpl;
	if (nullptr == pcImpl) { assert(false); }

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey);
	
	HC_KEY nStyleKey = HC_Style_Segment_By_Key(cInStyleSource.KeyValue());
	
	SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	StyleKey cStyle(nStyleKey);
	return cStyle;
}

