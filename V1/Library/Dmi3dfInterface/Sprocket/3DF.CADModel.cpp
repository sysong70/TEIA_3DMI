#include "StdAfx.h"

#include "3DF.CADModel.h"

#include "3DF.Component.h"

#include <Common_Define.h>

using namespace H3DF;

namespace H3DF
{
	class CADModelImpl : public Impl
	{
	public:
		void Copy(CADModelImpl * pcInThat);

		Component m_cRoot;

		Component m_cModelsGroupItem;
		Component m_cMeasurementsGroupItem;
		Component m_cMarkupsGroupItem;

		DWORD m_nSolidIndex = 1;
		DWORD m_nSurfaceIndex = 1;
		DWORD m_nCurveIndex = 1;
		DWORD m_nPointIndex = 1;
	};
}

void H3DF::CADModelImpl::Copy(CADModelImpl * pcInThat)
{
	m_cRoot = pcInThat->m_cRoot;

	m_cModelsGroupItem = pcInThat->m_cModelsGroupItem;
	m_cMeasurementsGroupItem = pcInThat->m_cMeasurementsGroupItem;
	m_cMarkupsGroupItem = pcInThat->m_cMarkupsGroupItem;

	m_nSolidIndex = pcInThat->m_nSolidIndex;
	m_nSurfaceIndex = pcInThat->m_nSurfaceIndex;
	m_nCurveIndex = pcInThat->m_nCurveIndex;
	m_nPointIndex = pcInThat->m_nPointIndex;
}

//== CADModel Class =================================================================================

H3DF::CADModel::CADModel()
{
	m_pcImpl = new CADModelImpl();
	DEBUG_VALID(m_pcImpl);
}

H3DF::CADModel::CADModel(Component const & cInThat)
{
	m_pcImpl = new CADModelImpl();
	DEBUG_VALID(m_pcImpl);

	Set(cInThat);
}

void H3DF::CADModel::Set(CADModel const & cInThat)
{
	CADModelImpl * pcImpl = (CADModelImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	CADModelImpl * pcInThatImpl = (CADModelImpl *)cInThat.m_pcImpl;
	DEBUG_VALID(pcInThatImpl);

	pcImpl->Copy(pcInThatImpl);
}

CADModel & H3DF::CADModel::operator = (CADModel const & cInThat)
{
	Set(cInThat);
	return *this;
}

Component & H3DF::CADModel::Root()
{
	CADModelImpl * pcImpl = (CADModelImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cRoot;
}

Component & H3DF::CADModel::ModelsGroupComponent()
{
	CADModelImpl * pcImpl = (CADModelImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cModelsGroupItem;
}

Component & H3DF::CADModel::MeasurementsGroupComponent()
{
	CADModelImpl * pcImpl = (CADModelImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cMeasurementsGroupItem;
}

Component & H3DF::CADModel::MarkupsGroupComponent()
{
	CADModelImpl * pcImpl = (CADModelImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cMarkupsGroupItem;
}

CString H3DF::CADModel::GetName() const
{
	CADModelImpl * pcImpl = (CADModelImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cRoot.GetName();
}