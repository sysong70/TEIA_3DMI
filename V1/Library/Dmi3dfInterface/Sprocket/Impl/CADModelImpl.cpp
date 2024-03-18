#include "StdAfx.h"

#include "CADModelImpl.h"

#include <Common_Define.h>

using namespace H3DF;

H3DF::CADModelImpl::CADModelImpl()
{
	m_pmComponentMap = new CAtlMap<HC_KEY, Component *>;
}

H3DF::CADModelImpl::~CADModelImpl()
{
	if (nullptr != m_pmComponentMap) {

// 		POSITION pcPosition = m_pmComponentMap->GetStartPosition();
// 
// 		while (nullptr != pcPosition)
// 		{
// 			Component * pcComponent = m_pmComponentMap->GetNextValue(pcPosition);
// 			delete pcComponent;
// 		}

		m_pmComponentMap->RemoveAll();
		delete m_pmComponentMap;
	}
}

void H3DF::CADModelImpl::Copy(CADModelImpl * pcInThat)
{
	m_pcModels = pcInThat->m_pcModels;
	m_pcMeasurements = pcInThat->m_pcMeasurements;
	m_pcMarkups = pcInThat->m_pcMarkups;

	m_nSolidIndex = pcInThat->m_nSolidIndex;
	m_nSurfaceIndex = pcInThat->m_nSurfaceIndex;
	m_nCurveIndex = pcInThat->m_nCurveIndex;
	m_nPointIndex = pcInThat->m_nPointIndex;
}

CString H3DF::CADModelImpl::TypeName(const Component & cInComponent)
{
	ComponentImpl * pcImpl = (ComponentImpl *)cInComponent.GetImpl();
	DEBUG_VALID(pcImpl);

	CString strTypeName = pcImpl->TypeName();

	switch (pcImpl->m_eType)
	{
		case Component::Type::ExchangeRICurve:
		case Component::Type::ExchangeRIPolyWire:
			strTypeName.Format(L"%s %d", strTypeName, m_nCurveIndex++);
			break;

		case Component::Type::ExchangeRIBRepModelSolid:
		case Component::Type::ExchangeRIPolyBRepModelSolid:
			strTypeName.Format(L"%s %d", strTypeName, m_nSolidIndex++);
			break;

		case Component::Type::ExchangeRIBRepModelSurface:
		case Component::Type::ExchangeRIPolyBRepModelSurface:
			strTypeName.Format(L"%s %d", strTypeName, m_nSurfaceIndex++);
			break;

		case Component::Type::ExchangeRISet:
			strTypeName.Format(L"%s %d", strTypeName, m_nGroupIndex++);
			break;

		case Component::Type::ExchangeRIPointSet:
			strTypeName.Format(L"%s %d", strTypeName, m_nPointSetIndex
				++);
			break;

		default:
			break;
	}
	
	return strTypeName;
}

Component & H3DF::CADModelImpl::ModelComponent()
{
	return *m_pcModels;
}

void H3DF::CADModelImpl::MapSetAt(HC_KEY nInKey, Component * pcInComponent)
{
	m_pmComponentMap->SetAt(nInKey, pcInComponent);
}