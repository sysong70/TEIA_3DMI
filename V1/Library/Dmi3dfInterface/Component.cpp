#include "StdAfx.h"

#include "Component.h"
#include "Impl/ComponentImpl.h"

#include "Common_Define.h"

#include "3DF/Segment.h"
#include "3DF/3DF.Utility.h"
#include "3DF/KeyPath.h"
#include "3DF/Selection.h"
#include "3DF/Impl/SelectionImpl.h"

#include <ranges>

using namespace H3DF;

//== Component Class ===============================================================================
H3DF::Component::Component()
{
	m_pcImpl = new ComponentImpl();
	DEBUG_VALID(m_pcImpl);
}

H3DF::Component::Component(Component const & cInThat)
{
	m_pcImpl = new ComponentImpl();
	DEBUG_VALID(m_pcImpl);
	Set(cInThat);
}

void H3DF::Component::Set(Component const & cInThat)
{
	ComponentImpl * pcImpl = (ComponentImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	ComponentImpl * pcInThatImpl = (ComponentImpl *)cInThat.m_pcImpl;
	DEBUG_VALID(pcInThatImpl);
	pcImpl->Copy(pcInThatImpl);
}

Component & H3DF::Component::operator = (Component const & cInThat)
{
	Set(cInThat);
	return *this;
}

Component::ComponentType H3DF::Component::GetComponentType() const
{
	ComponentImpl * pcImpl = (ComponentImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_eType;
}

bool H3DF::Component::HasComponentType(ComponentType eInMask) const
{
	ComponentImpl * pcImpl = (ComponentImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return (pcImpl->m_eType == eInMask);
}

Key H3DF::Component::GetKey() const
{
	ComponentImpl * pcImpl = (ComponentImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_nKey;
}

Component & H3DF::Component::GetOwner() const
{
	ComponentImpl * pcImpl = (ComponentImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return *pcImpl->m_pcOwner;
}

ComponentArray & H3DF::Component::GetSubcomponents() const
{
	ComponentImpl * pcImpl = (ComponentImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return *pcImpl->m_vpnSubcomponents;
}

KeyPath H3DF::Component::GetKeyPath(Component const & cInComponent)
{
	KeyArray cKeyArray;
	
	Component const * pcComponent = &cInComponent;
	while (nullptr != pcComponent) {
		cKeyArray.push_back(pcComponent->GetKey());
		pcComponent = &pcComponent->GetOwner();
	}

	KeyPath cKeyPath(cKeyArray);
	return cKeyPath;
}