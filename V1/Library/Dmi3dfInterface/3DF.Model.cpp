#include "StdAfx.h"

#include "3DF.Model.h"
#include "Private/Model.Private.h"

#include "3DF/Facility.AppOptions.h"

#include <Common_Define.h>
#include <Path.h>
#include <WStr.h>

#include "../Signal/Signal.h"

using namespace H3DF;

H3DF::Model::Model()
{
	ModelPrivate * pcImpl = new ModelPrivate();
	pcImpl->Init();

	m_pcImpl = pcImpl;
}

SegmentKey H3DF::Model::GetSegmentKey()
{
	ModelPrivate* pcImpl = static_cast<ModelPrivate*>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->GetSegmentKey();
}

SegmentKey const H3DF::Model::GetSegmentKey() const
{
	ModelPrivate * pcImpl = static_cast<ModelPrivate *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->GetSegmentKey();
}

void H3DF::Model::SetBRepGeometry(bool brep)
{
	ModelPrivate * pcImpl = static_cast<ModelPrivate *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->SetBRepGeometry(brep);
}

H3DF::ModelHandedness H3DF::Model::GetModelHandedness() 
{
	ModelPrivate * pcImpl = static_cast<ModelPrivate *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->GetModelHandedness(); 
}

void H3DF::Model::UpdateModelHandedness()
{
	ModelPrivate * pcImpl = static_cast<ModelPrivate *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->UpdateModelHandedness();
}
