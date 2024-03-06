#include "StdAfx.h"

#include "3DF.Model.h"
#include "Impl/ModelImpl.h"

#include "../3DF/Facility.AppOptions.h"

#include <Common_Define.h>
#include <Path.h>
#include <WStr.h>

#include "../Signal/Signal.h"

using namespace H3DF;

H3DF::Model::Model()
{
	ModelImpl * pcImpl = new ModelImpl();
	pcImpl->Init();

	m_pcImpl = pcImpl;
}

SegmentKey H3DF::Model::GetSegmentKey()
{
	ModelImpl* pcImpl = dynamic_cast<ModelImpl*>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->GetSegmentKey();
}

SegmentKey const H3DF::Model::GetSegmentKey() const
{
	ModelImpl * pcImpl = dynamic_cast<ModelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->GetSegmentKey();
}

void H3DF::Model::SetBRepGeometry(bool brep)
{
	ModelImpl * pcImpl = dynamic_cast<ModelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->SetBRepGeometry(brep);
}

H3DF::ModelHandedness H3DF::Model::GetModelHandedness() 
{
	ModelImpl * pcImpl = dynamic_cast<ModelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->GetModelHandedness(); 
}

void H3DF::Model::UpdateModelHandedness()
{
	ModelImpl * pcImpl = dynamic_cast<ModelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->UpdateModelHandedness();
}

SegmentKey & H3DF::Model::Models() const
{
	ModelImpl * pcImpl = dynamic_cast<ModelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cModels;
}

SegmentKey & H3DF::Model::Measurements() const
{
	ModelImpl * pcImpl = dynamic_cast<ModelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cMeasurements;
}

SegmentKey & H3DF::Model::Markups() const
{
	ModelImpl * pcImpl = dynamic_cast<ModelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cMarkups;
}

SegmentKey & H3DF::Model::IncludeModel() const
{
	ModelImpl * pcImpl = dynamic_cast<ModelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cIncludeModel;
}

SegmentKey & H3DF::Model::IncludeStyles() const
{
	ModelImpl * pcImpl = dynamic_cast<ModelImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cIncludeStyles;
}