#include "StdAfx.h"

#include "Portfolio.h"

#include "Segment.h"
#include "./Impl/SegmentImpl.h"

#include "./Impl/ControlImpl.h"

#include "Style.h"

#include <HTools.h>

using namespace H3DF;

using PortfolioDeque = std::deque<PortfolioKey *>;

H3DF::PortfolioKey::PortfolioKey()
{
}

H3DF::PortfolioKey::PortfolioKey(HC_KEY nInKey) : Key(nInKey)
{
}

H3DF::PortfolioKey::PortfolioKey(Key const & cInThat) : Key(cInThat) 
{
}

H3DF::PortfolioKey::PortfolioKey(PortfolioKey const & cInThat) : Key(cInThat)
{
}

void H3DF::PortfolioKey::Set(PortfolioKey const & cInThat)
{
	Key::Set(cInThat);
}

PortfolioKey & H3DF::PortfolioKey::operator = (PortfolioKey const & cInThat)
{
	Key::Set(cInThat);
	return *this;
}

NamedStyleDefinition H3DF::PortfolioKey::DefineNamedStyle(CStringA strInName, SegmentKey const & cInStyleSource)
{
	if (INVALID_KEY == KeyValue()) {
		DEBUG_STOP;
	}

	SegmentKey cPortfolio(KeyValue());

	SegmentKey cStyleSegment = cPortfolio.Subsegment(strInName);

	cStyleSegment.Open();
	HC_KEY nStyleKey = HC_Style_Segment_By_Key(cInStyleSource.KeyValue());
	cStyleSegment.Close();

	NamedStyleDefinition cStyle(nStyleKey);
	return cStyle;
}

//== PortfolioControlImpl 관련 함수 ==================================================================

namespace H3DF
{
	class PortfolioControlImpl : public ControlImpl
	{
	public:
		PortfolioControlImpl();
		virtual ~PortfolioControlImpl();

		void Copy(PortfolioControlImpl * pcInThat) {
			ControlImpl::Copy(pcInThat);
		}

		PortfolioDeque * m_pdpcPortfolioDeque = nullptr;
	};
}

H3DF::PortfolioControlImpl::PortfolioControlImpl()
{
	m_pdpcPortfolioDeque = new PortfolioDeque();
	m_eType = H3DF::Type::PortfolioControl;
}

H3DF::PortfolioControlImpl::~PortfolioControlImpl()
{
	if (nullptr != m_pdpcPortfolioDeque) {
		for (auto & pcPortfolioDeque : *m_pdpcPortfolioDeque) {
			delete pcPortfolioDeque;
		}

		m_pdpcPortfolioDeque->clear();
	}
}

//== PortfolioControl 관련 함수 ======================================================================

H3DF::PortfolioControl::PortfolioControl(SegmentKey & cInSegmentKey)
{
	PortfolioControlImpl * pcImpl = new PortfolioControlImpl();
	pcImpl->m_cOverrideKey = cInSegmentKey;

	m_pcImpl = pcImpl;
}

H3DF::PortfolioControl::PortfolioControl(PortfolioControl const & cInThat)
{
	m_pcImpl = new PortfolioControlImpl();
	Set(cInThat);
}

void H3DF::PortfolioControl::Set(PortfolioControl const & cInThat)
{
	PortfolioControlImpl * pcImpl = (PortfolioControlImpl *)m_pcImpl;
	PortfolioControlImpl * pcInThatImpl = (PortfolioControlImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

PortfolioControl & H3DF::PortfolioControl::operator = (PortfolioControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

size_t H3DF::PortfolioControl::GetCount() const
{
	PortfolioControlImpl * pcImpl = (PortfolioControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_pdpcPortfolioDeque->size();
}

PortfolioControl & H3DF::PortfolioControl::Push(PortfolioKey const & cInPortfolio)
{
	PortfolioControlImpl * pcImpl = (PortfolioControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	PortfolioKey * pcPortfolioKey = new PortfolioKey();
	*pcPortfolioKey = cInPortfolio;
	pcImpl->m_pdpcPortfolioDeque->push_front(pcPortfolioKey);
	return *this;
}

bool H3DF::PortfolioControl::Pop()
{
	PortfolioControlImpl * pcImpl = (PortfolioControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	if(true == pcImpl->m_pdpcPortfolioDeque->empty()) {
		return false;
	}

	PortfolioKey * pcPortfolioKey = pcImpl->m_pdpcPortfolioDeque->back();
	delete pcPortfolioKey;

	pcImpl->m_pdpcPortfolioDeque->pop_back();

	return true;
}

bool H3DF::PortfolioControl::ShowTop(PortfolioKey & cOutPortfolio) const
{
	PortfolioControlImpl * pcImpl = (PortfolioControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	if(true == pcImpl->m_pdpcPortfolioDeque->empty()) {
		return false;
	}

	cOutPortfolio = *pcImpl->m_pdpcPortfolioDeque->front();
	return true;
}