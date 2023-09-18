#include "StdAfx.h"

#include "Portfolio.h"

#include "Segment.h"
#include "./Private/SegmentPrivate.h"

#include "Style.h"

#include <HTools.h>

USING_3DF_NAMESPACE

PortfolioKey::PortfolioKey(CString strInName)  :
	SegmentKey(strInName)
{

}

PortfolioKey::PortfolioKey(HC_KEY nInKey) :
	SegmentKey(nInKey)
{
}

PortfolioKey::PortfolioKey(PortfolioKey const & cInThat) :
	SegmentKey(cInThat)
{
}

void PortfolioKey::Set(PortfolioKey const & cInThat)
{
	SegmentKey::Set(cInThat);
}

PortfolioKey & PortfolioKey::operator = (PortfolioKey const & cInThat)
{
	SegmentKey::Set(cInThat);
	return *this;
}

NamedStyleDefinition PortfolioKey::DefineNamedStyle(CString strInName, SegmentKey const & cInStyleSource)
{
	SegmentKeyPrivate::LocalOpen(*this);

	SegmentKey cSubSegment = Subsegment(strInName);

	SegmentKeyPrivate::LocalClose(*this);

	NamedStyleDefinition cStyle(cSubSegment.KeyValue());
	return cStyle;
}

//== PortfolioControl 관련 함수 ======================================================================

PortfolioControl::~PortfolioControl()
{
	if(nullptr != m_pdpcPortfolioDeque) {
		for(auto & pcPortfolioDeque : *m_pdpcPortfolioDeque) {
			delete pcPortfolioDeque;
		}

		m_pdpcPortfolioDeque->clear();
	}
}

size_t PortfolioControl::GetCount() const
{
	if(nullptr == m_pdpcPortfolioDeque) {
		return 0;
	}

	return m_pdpcPortfolioDeque->size();
}

PortfolioControl & PortfolioControl::Push(PortfolioKey const & cInPortfolio)
{
	if(nullptr == m_pdpcPortfolioDeque) {
		m_pdpcPortfolioDeque = new PortfolioDeque();
	}

	PortfolioKey * pcPortfolioKey = new PortfolioKey();
	*pcPortfolioKey = cInPortfolio;
	m_pdpcPortfolioDeque->push_front(pcPortfolioKey);
	return *this;
}

bool PortfolioControl::Pop()
{
	if(nullptr == m_pdpcPortfolioDeque) {
		return false;
	}

	if(true == m_pdpcPortfolioDeque->empty()) {
		return false;
	}

	PortfolioKey * pcPortfolioKey = m_pdpcPortfolioDeque->back();
	delete pcPortfolioKey;

	m_pdpcPortfolioDeque->pop_back();

	return true;
}

bool PortfolioControl::ShowTop(PortfolioKey & cOutPortfolio) const
{
	if(nullptr == m_pdpcPortfolioDeque) {
		return false;
	}

	if(true == m_pdpcPortfolioDeque->empty()) {
		return false;
	}

	cOutPortfolio = *m_pdpcPortfolioDeque->front();
	return true;
}