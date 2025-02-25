#include "StdAfx.h"

#include "3DF.Model.h"
#include "Impl/ModelImpl.h"

//#include "../3DF/Facility.AppOptions.h"

// #include <Common_Define.h>
// #include <Path.h>
// #include <WStr.h>

//#include "../Signal/Signal.h"

using namespace H3DF;

H3DF::Model::Model()
{
	ModelImpl * pcImpl = new ModelImpl();
	pcImpl->Init();

	m_pcImpl = pcImpl;
}

SegmentKey H3DF::Model::GetSegmentKey()
{
	ModelImpl* pcImpl = static_cast<ModelImpl*>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->GetSegmentKey();
}

SegmentKey const H3DF::Model::GetSegmentKey() const
{
	ModelImpl * pcImpl = static_cast<ModelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->GetSegmentKey();
}

PortfolioKey H3DF::Model::GetPortfolioKey()
{
	ModelImpl * pcImpl = static_cast<ModelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->GetPortfolioKey();
}

PortfolioKey const H3DF::Model::GetPortfolioKey() const
{
	ModelImpl * pcImpl = static_cast<ModelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->GetPortfolioKey();
}

void H3DF::Model::SetBRepGeometry(bool brep)
{
	ModelImpl * pcImpl = static_cast<ModelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->SetBRepGeometry(brep);
}

H3DF::ModelHandedness H3DF::Model::GetModelHandedness() 
{
	ModelImpl * pcImpl = static_cast<ModelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->GetModelHandedness(); 
}

void H3DF::Model::UpdateModelHandedness()
{
	ModelImpl * pcImpl = static_cast<ModelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->UpdateModelHandedness();
}

SegmentKey & H3DF::Model::ModelsRoot() const
{
	ModelImpl * pcImpl = static_cast<ModelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cModelsRoot;
}

SegmentKey & H3DF::Model::MeasurementsRoot() const
{
	ModelImpl * pcImpl = static_cast<ModelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cMeasurementsRoot;
}

SegmentKey & H3DF::Model::MarkupsRoot() const
{
	ModelImpl * pcImpl = static_cast<ModelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cMarkupsRoot;
}

SegmentKey & H3DF::Model::IncludeModel() const
{
	ModelImpl * pcImpl = static_cast<ModelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cIncludeModel;
}

SegmentKey & H3DF::Model::IncludeStyles() const
{
	ModelImpl * pcImpl = static_cast<ModelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cIncludeStyles;
}