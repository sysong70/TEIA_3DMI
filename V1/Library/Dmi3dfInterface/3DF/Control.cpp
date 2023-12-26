#include "StdAfx.h"

#include "Control.h"
#include "Impl/ControlImpl.h"

#include "Segment.h"

#include <HTools.h>

using namespace H3DF;

H3DF::Control::Control(HC_KEY nInKey)
{
	ControlImpl * pcImpl = new ControlImpl();
	pcImpl->m_cOverrideKey = nInKey;

	m_pcImpl = pcImpl;
}

H3DF::Control::Control(Control const & cInThat)
{
	m_pcImpl = new ControlImpl();
	Set(cInThat);
}

H3DF::Control::~Control()
{
}

void H3DF::Control::Set(Control const & cInThat)
{
	ControlImpl * pcImpl = (ControlImpl *)m_pcImpl;
	ControlImpl * pcInThatImpl = (ControlImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

Control const & H3DF::Control::operator = (Control const & cInThat)
{
	Set(cInThat);
	return *this;
}

bool H3DF::Control::operator == (Control const & cInThat) const
{
	ControlImpl * pcImpl = (ControlImpl *)m_pcImpl;
	ControlImpl * pcInThatImpl = (ControlImpl *)cInThat.m_pcImpl;
	return (pcImpl->m_cOverrideKey == pcInThatImpl->m_cOverrideKey);
}