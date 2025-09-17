#include "StdAfx.h"

#include "Definition.h"
#include "Impl/DefinitionImpl.h"

#include "Portfolio.h"

#include "Math.h"

using namespace H3DF;

H3DF::Definition::Definition()
{
}

H3DF::Definition::Definition(HC_KEY nInKey)
{
	if (INVALID_KEY == nInKey) {
		return;
	}

	m_pcImpl = std::make_unique<DefinitionImpl>();
	static_cast<DefinitionImpl *>(m_pcImpl.get())->SetKeyValue(nInKey);
}

H3DF::Definition::Definition(Definition const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
}

H3DF::Definition::Definition(Definition && cInThat) noexcept :
	Object(std::move(cInThat)) {}

Definition & H3DF::Definition::operator=(Definition && cInThat) noexcept
{
	this->Object::operator = (std::move(cInThat));
	return *this;
}

Definition const & H3DF::Definition::operator = (Definition const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

PortfolioKey H3DF::Definition::Owner() const
{
	auto pcImpl = dynamic_cast<DefinitionImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cOwnerPortfolio;

// 	SegmentKey cKey(pcImpl->KeyValue());
// 	PortfolioKey cOwner(cKey.Owner());
// 	return cOwner;
}

CStringA H3DF::Definition::Name() const
{
	auto pcImpl = dynamic_cast<DefinitionImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	return pcImpl->m_strName;

// 	SegmentKey cKey(pcImpl->KeyValue());
// 	return cKey.Name();
}


bool H3DF::Definition::operator == (Definition const & cInThat) const
{
	auto pcImpl = dynamic_cast<DefinitionImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	auto pcInThatImpl = dynamic_cast<const DefinitionImpl *>(cInThat.GetImpl());
	DEBUG_VALID(pcInThatImpl);

	if (pcImpl->KeyValue() != pcInThatImpl->KeyValue()) {
		return false;
	}

	if (pcImpl->m_cOwnerPortfolio != pcInThatImpl->m_cOwnerPortfolio) {
		return false;
	}

	if (pcImpl->m_strName != pcInThatImpl->m_strName) {
		return false;
	}

	return true;
}

bool H3DF::Definition::operator != (Definition const & cInThat) const
{
	return !(*this == cInThat);
}