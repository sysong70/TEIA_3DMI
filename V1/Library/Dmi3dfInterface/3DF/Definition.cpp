#include "StdAfx.h"

#include "Definition.h"
#include "Impl/DefinitionImpl.h"

#include "Portfolio.h"

#include "Math.h"

using namespace H3DF;

H3DF::Definition::Definition()
{
	auto * pcImpl = new DefinitionImpl();
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;
}

H3DF::Definition::Definition(HC_KEY nInKey)
{
	if (INVALID_KEY == nInKey) {
		return;
	}

	auto * pcImpl = new DefinitionImpl();
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;
}

H3DF::Definition::Definition(Definition const & cInThat)
{
	DefinitionImpl * pcInThatImpl = dynamic_cast<DefinitionImpl *>(cInThat.m_pcImpl);
	DEBUG_VALID(pcInThatImpl);
	if (INVALID_KEY == pcInThatImpl->KeyValue()) {
		return;
	}

	auto * pcImpl = new DefinitionImpl();
	DEBUG_VALID(pcImpl);
	m_pcImpl = pcImpl;

	Set(cInThat);
}

void H3DF::Definition::Set(Definition const & cInThat)
{
	DefinitionImpl * pcImpl = dynamic_cast<DefinitionImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);
	DefinitionImpl * pcInThatImpl = dynamic_cast<DefinitionImpl *>(cInThat.m_pcImpl);
	DEBUG_VALID(pcInThatImpl);

	pcImpl->Set(pcInThatImpl);
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
	Set(cInThat);
	return *this;
}

PortfolioKey H3DF::Definition::Owner() const
{
	DefinitionImpl * pcImpl = dynamic_cast<DefinitionImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cOwnerPortfolio;

// 	SegmentKey cKey(pcImpl->KeyValue());
// 	PortfolioKey cOwner(cKey.Owner());
// 	return cOwner;
}

CStringA H3DF::Definition::Name() const
{
	DefinitionImpl * pcImpl = dynamic_cast<DefinitionImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->m_strName;

// 	SegmentKey cKey(pcImpl->KeyValue());
// 	return cKey.Name();
}


bool H3DF::Definition::operator == (Definition const & cInThat) const
{
	DefinitionImpl * pcImpl = dynamic_cast<DefinitionImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);
	DefinitionImpl * pcInThatImpl = dynamic_cast<DefinitionImpl *>(cInThat.m_pcImpl);
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