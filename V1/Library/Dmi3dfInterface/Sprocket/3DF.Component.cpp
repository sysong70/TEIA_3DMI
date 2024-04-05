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

	// Component의 소유자를 설정한다.
	if (nullptr != pcImpl->m_pvSubComponents) {
		for (auto * pcComponent : *pcImpl->m_pvSubComponents) {
			ComponentImpl * pcSubImpl = (ComponentImpl *)pcComponent->GetImpl();
			DEBUG_VALID(pcImpl);
			pcSubImpl->m_pcOwner = this;
		}
	}
}

Component & H3DF::Component::operator = (Component const & cInThat)
{
	Set(cInThat);
	return *this;
}

bool H3DF::Component::Equals(Component const & cInThat) const
{
	ComponentImpl * pcImpl = (ComponentImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	
	ComponentImpl * pcInThatImpl = (ComponentImpl *)cInThat.m_pcImpl;
	DEBUG_VALID(pcInThatImpl);

	if (pcImpl->m_nSegmentKey != pcInThatImpl->m_nSegmentKey) {
		return false;
	}

	if (pcImpl->m_nIncludeKey != pcInThatImpl->m_nIncludeKey) {
		return false;
	}



	return (pcImpl->m_nSegmentKey == pcInThatImpl->m_nSegmentKey);
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

ComponentArray & H3DF::Component::GetSubComponents() const
{
	ComponentImpl * pcImpl = (ComponentImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return *pcImpl->m_pvSubComponents;
}

size_t H3DF::Component::GetAllSubComponentCount() const
{
	ComponentImpl * pcImpl = (ComponentImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	size_t nCount = GetSubComponents().size();

	for (auto * pcComponent : GetSubComponents()) {
		nCount += pcComponent->GetAllSubComponentCount();
	}

	return nCount;
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

	return pcImpl->m_nStatus;
}

DWORD H3DF::Component::AddStatus(Component::Status eStatus)
{
	ComponentImpl * pcImpl = (ComponentImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_nStatus |= eStatus;
	return pcImpl->m_nStatus;
}

DWORD H3DF::Component::RemoveStatus(Component::Status eStatus)
{
	ComponentImpl * pcImpl = (ComponentImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_nStatus &= ~eStatus;
	return pcImpl->m_nStatus;
}

bool H3DF::Component::IsRepresentationItem()
{
	ComponentImpl * pcImpl = (ComponentImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	switch (pcImpl->m_eType)
	{
		case H3DF::Component::Type::ExchangeRIPointSet:
		case H3DF::Component::Type::ExchangeRICurve:
		case H3DF::Component::Type::ExchangeRIPolyWire:
		case H3DF::Component::Type::ExchangeRIBRepModelSolid:
		case H3DF::Component::Type::ExchangeRIPolyBRepModelSolid:
		case H3DF::Component::Type::ExchangeRIBRepModelSurface:
		case H3DF::Component::Type::ExchangeRIPolyBRepModelSurface:
			return true;
			break;
	}

	return false;
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