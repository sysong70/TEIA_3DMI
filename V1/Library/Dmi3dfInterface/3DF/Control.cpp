#include "StdAfx.h"

#include "Control.h"
#include "Private/ControlPrivate.h"

#include "Segment.h"

#include <HTools.h>

using namespace H3DF;

H3DF::Control::Control(HC_KEY nInKey)
{
	ControlPrivate * pcImpl = new ControlPrivate();
	pcImpl->m_nOverrideKey = nInKey;

	m_pcImpl = pcImpl;
}

H3DF::Control::Control(Control const & cInThat)
{
	m_pcImpl = new ControlPrivate();
	Set(cInThat);
}

H3DF::Control::~Control()
{
}

void H3DF::Control::Set(Control const & cInThat)
{
	ControlPrivate * pcImpl = (ControlPrivate *)m_pcImpl;
	ControlPrivate * pcInThatImpl = (ControlPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

Control const & H3DF::Control::operator = (Control const & cInThat)
{
	Set(cInThat);
	return *this;
}

bool H3DF::Control::operator == (Control const & cInThat) const
{
	ControlPrivate * pcImpl = (ControlPrivate *)m_pcImpl;
	ControlPrivate * pcInThatImpl = (ControlPrivate *)cInThat.m_pcImpl;
	return (pcImpl->m_nOverrideKey == pcInThatImpl->m_nOverrideKey);
}