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
	m_pcImpl = std::make_unique<ModelImpl>();
	DEBUG_VALID(m_pcImpl);

	m_pcImpl->SetType(H3DF::Type::Model);

	auto pcImpl = static_cast<ModelImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->Init();
}

SegmentKey & H3DF::Model::GetSegmentKey()
{
	auto pcImpl = static_cast<ModelImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	return pcImpl->GetSegmentKey();
}

SegmentKey const & H3DF::Model::GetSegmentKey() const
{
	auto pcImpl = static_cast<ModelImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	return pcImpl->GetSegmentKey();
}

PortfolioKey H3DF::Model::GetPortfolioKey()
{
	auto pcImpl = static_cast<ModelImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	return pcImpl->GetPortfolioKey();
}

PortfolioKey const H3DF::Model::GetPortfolioKey() const
{
	auto pcImpl = static_cast<ModelImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	return pcImpl->GetPortfolioKey();
}

void H3DF::Model::SetBRepGeometry(bool brep)
{
	auto pcImpl = static_cast<ModelImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	return pcImpl->SetBRepGeometry(brep);
}

H3DF::ModelHandedness H3DF::Model::GetModelHandedness() 
{
	auto pcImpl = static_cast<ModelImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	return pcImpl->GetModelHandedness(); 
}

void H3DF::Model::UpdateModelHandedness()
{
	auto pcImpl = static_cast<ModelImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	return pcImpl->UpdateModelHandedness();
}

SegmentKey & H3DF::Model::ModelsRoot() const
{
	auto pcImpl = static_cast<ModelImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cModelsRoot;
}

SegmentKey & H3DF::Model::MeasurementsRoot() const
{
	auto pcImpl = static_cast<ModelImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cMeasurementsRoot;
}

SegmentKey & H3DF::Model::MarkupsRoot() const
{
	auto pcImpl = static_cast<ModelImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cMarkupsRoot;
}

SegmentKey & H3DF::Model::IncludeModel() const
{
	auto pcImpl = static_cast<ModelImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cIncludeModel;
}

SegmentKey & H3DF::Model::IncludeStyles() const
{
	auto pcImpl = static_cast<ModelImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cIncludeStyles;
}