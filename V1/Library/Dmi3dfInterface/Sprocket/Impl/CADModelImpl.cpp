#include "StdAfx.h"

#include "CADModelImpl.h"

#include "../3DF.CADModel.h"

#include <Common_Define.h>

using namespace H3DF;

H3DF::CADModelImpl::CADModelImpl()
{
	m_pmComponentMap = new CAtlMap<HC_KEY, Component *>;

	m_pmEntityMap = new CAtlMap<DWORD_PTR, A3DEntity *>;
}

H3DF::CADModelImpl::~CADModelImpl()
{
	if (nullptr != m_pmComponentMap) {
		m_pmComponentMap->RemoveAll();
		delete m_pmComponentMap;
	}

	if(nullptr != m_pmEntityMap) {
		m_pmEntityMap->RemoveAll();
		delete m_pmEntityMap;
	}
}

void H3DF::CADModelImpl::Copy(const CADModelImpl * pcInThat)
{
	m_pcModels = pcInThat->m_pcModels;
	m_pcMeasurements = pcInThat->m_pcMeasurements;
	m_pcMarkups = pcInThat->m_pcMarkups;

	m_nProductOccurrenceIndex = pcInThat->m_nProductOccurrenceIndex;
	m_nSolidIndex = pcInThat->m_nSolidIndex;
	m_nSurfaceIndex = pcInThat->m_nSurfaceIndex;
	m_nCurveIndex = pcInThat->m_nCurveIndex;
	m_nPointIndex = pcInThat->m_nPointIndex;
}

Component & H3DF::CADModelImpl::ModelComponent()
{
	return *m_pcModels;
}

void H3DF::CADModelImpl::MapSetAt(HC_KEY nInKey, Component * pcInComponent)
{
	m_pmComponentMap->SetAt(nInKey, pcInComponent);
}

//== CADModelUtility Class =========================================================================

void H3DF::CADModelUtility::MapSetAt(CADModel * pcInCadModel, HC_KEY nInKey, Component * pcInComponent)
{
	CADModelImpl * pcImpl = (CADModelImpl *)pcInCadModel->GetImpl();
	DEBUG_VALID(pcImpl);

	pcImpl->MapSetAt(nInKey, pcInComponent);
}

CString H3DF::CADModelUtility::TypeName(CADModel * pcInCadModel, const Component & cInComponent)
{
	CADModelImpl * pcCadModelImpl = (CADModelImpl *)pcInCadModel->GetImpl();
	DEBUG_VALID(pcCadModelImpl);

	ComponentImpl * pcImpl = (ComponentImpl *)cInComponent.GetImpl();
	DEBUG_VALID(pcImpl);

	CString strTypeName = pcImpl->TypeName();

	switch (pcImpl->m_eType)
	{
		case Component::Type::ExchangeProductOccurrence:
			strTypeName.Format(L"%s %d", strTypeName, pcCadModelImpl->m_nProductOccurrenceIndex++);
			break;

		case Component::Type::ExchangeRICurve:
		case Component::Type::ExchangeRIPolyWire:
			strTypeName.Format(L"%s %d", strTypeName, pcCadModelImpl->m_nCurveIndex++);
			break;

		case Component::Type::ExchangeRIBRepModelSolid:
		case Component::Type::ExchangeRIPolyBRepModelSolid:
			strTypeName.Format(L"%s %d", strTypeName, pcCadModelImpl->m_nSolidIndex++);
			break;

		case Component::Type::ExchangeRIBRepModelSurface:
		case Component::Type::ExchangeRIPolyBRepModelSurface:
			strTypeName.Format(L"%s %d", strTypeName, pcCadModelImpl->m_nSurfaceIndex++);
			break;

		case Component::Type::ExchangeRISet:
			strTypeName.Format(L"%s %d", strTypeName, pcCadModelImpl->m_nGroupIndex++);
			break;

		case Component::Type::ExchangeRIPointSet:
			strTypeName.Format(L"%s %d", strTypeName, pcCadModelImpl->m_nPointSetIndex++);
			break;

		default:
			break;
	}

	return strTypeName;
}