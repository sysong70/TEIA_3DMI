#include "StdAfx.h"

#include "3DF.Component.h"
#include "Impl/ComponentImpl.h"

#include "Common_Define.h"

#include "../3DF/Segment.h"
#include "../3DF/3DF.Utility.h"
#include "../3DF/KeyPath.h"
#include "../3DF/Selection.h"
#include "../3DF/Impl/SelectionImpl.h"

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
	ComponentImpl * pcImpl = new ComponentImpl();
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;

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

Component::Type H3DF::Component::GetType() const
{
	ComponentImpl * pcImpl = (ComponentImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_eType;
}

HC_KEY H3DF::Component::GetSegmentKey() const
{
	ComponentImpl * pcImpl = (ComponentImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_nSegmentKey;
}

HC_KEY H3DF::Component::GetIncludeKey() const
{
	ComponentImpl * pcImpl = (ComponentImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_nIncludeKey;
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

	return *pcImpl->m_pvSubComponents;
}

CString H3DF::Component::GetName() const
{
	ComponentImpl * pcImpl = (ComponentImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return *pcImpl->m_pstrName;
}

DWORD H3DF::Component::GetStatus()
{
	ComponentImpl * pcImpl = (ComponentImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->Status();
}

KeyPath H3DF::Component::GetKeyPath(Component const & cInComponent)
{
	KeyArray cKeyArray;
	
	Component const * pcComponent = &cInComponent;
	while (nullptr != pcComponent) {
		cKeyArray.push_back(pcComponent->GetSegmentKey());
		pcComponent = &pcComponent->GetOwner();
	}

	KeyPath cKeyPath(cKeyArray);
	return cKeyPath;
}