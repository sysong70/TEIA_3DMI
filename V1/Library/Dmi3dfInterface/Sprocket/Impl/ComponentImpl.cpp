#include "StdAfx.h"

#include "ComponentImpl.h"


#include "../../3DF/Segment.h"
#include "../../3DF/3DF.Utility.h"
#include "../../3DF/KeyPath.h"
#include "../../3DF/Selection.h"
#include "../../3DF/Impl/SelectionImpl.h"

#include <Common_Define.h>

#include <ranges>

using namespace H3DF;

H3DF::ComponentImpl::ComponentImpl()
{
	m_pvSubComponents = new ComponentArray();
	m_pstrName = new CString();
}

H3DF::ComponentImpl::~ComponentImpl()
{
	if (nullptr != m_pvSubComponents) {
		for (auto * pcSubComponent : *m_pvSubComponents) {
			delete pcSubComponent;
		}
		delete m_pvSubComponents;
	}

	if (nullptr != m_pstrName) {
		delete m_pstrName;
	}
}

void H3DF::ComponentImpl::Copy(ComponentImpl * pcInThat)
{
	m_nSegmentKey = pcInThat->m_nSegmentKey;
	m_nIncludeKey = pcInThat->m_nIncludeKey;
	m_eType = pcInThat->m_eType;
	m_nStatus = pcInThat->m_nStatus;
	m_pcOwner = pcInThat->m_pcOwner;
	*m_pstrName = *pcInThat->m_pstrName;

	for (auto * pcSubComponent : *pcInThat->m_pvSubComponents) {
		Component * pcComponent = new Component(*pcSubComponent);
		m_pvSubComponents->push_back(pcComponent);
	}
}

void H3DF::ComponentImpl::SetName(CString strInName)
{
	*m_pstrName = strInName;
}

CString H3DF::ComponentImpl::TypeName()
{
	CString strTypeName;

	switch (m_eType)
	{
		case H3DF::Component::Type::ExchangePartDefinition:
			strTypeName = L"PartDefinition";
			break;

		case H3DF::Component::Type::ExchangeRIBRepModelSolid:
		case H3DF::Component::Type::ExchangeRIPolyBRepModelSolid:
			strTypeName = L"Solid";
			break;

		case H3DF::Component::Type::ExchangeRIBRepModelSurface:
		case H3DF::Component::Type::ExchangeRIPolyBRepModelSurface:
			strTypeName = L"Surface";
			break;

		case H3DF::Component::Type::ExchangeRICurve:
		case H3DF::Component::Type::ExchangeRIPolyWire:
			strTypeName = L"Curve";
			break;

		case H3DF::Component::Type::ExchangeRISet:
			strTypeName = L"Group";
			break;
		case H3DF::Component::Type::ExchangeRIPointSet:
			strTypeName = L"Point Set";
			break;

		default:
			break;
	};

	return strTypeName;
}

void H3DF::ComponentImpl::AddSubComponent(Component & cInSubComponent)
{
	m_pvSubComponents->push_back(&cInSubComponent);
}

//== Utility Functions =============================================================================

bool H3DF::ComponentImpl::SetData(Component & cInComponent, CString strInName, HC_KEY nKey, HC_KEY nIncludeKey, Component::Type eInType)
{
	ComponentImpl * pcImpl = dynamic_cast<ComponentImpl *>(cInComponent.GetImpl());
	if (nullptr == pcImpl) {
		DEBUG_STOP;
		return false;
	}

	pcImpl->SetName(strInName);
	pcImpl->m_eType = eInType;
	pcImpl->m_nSegmentKey = nKey;
	pcImpl->m_nIncludeKey = nIncludeKey;

	return true;
}

bool H3DF::ComponentImpl::SetName(Component & cInComponent, CString strInName)
{
	ComponentImpl * pcImpl = dynamic_cast<ComponentImpl *>(cInComponent.GetImpl());
	if (nullptr == pcImpl) {
		DEBUG_STOP;
		return false;
	}

	pcImpl->SetName(strInName);

	return true;
}

bool H3DF::ComponentImpl::AddSubComponent(Component & cInParentComponent, Component & cInComponent)
{
	ComponentImpl * pcParentImpl = dynamic_cast<ComponentImpl *>(cInParentComponent.GetImpl());
	if (nullptr == pcParentImpl) {
		DEBUG_STOP;
		return false;
	}

	pcParentImpl->AddSubComponent(cInComponent);

 	ComponentImpl * pcImpl = dynamic_cast<ComponentImpl *>(cInComponent.GetImpl());
 	DEBUG_VALID(pcImpl);
 
 	pcImpl->m_pcOwner = &cInParentComponent;

	return true;
}