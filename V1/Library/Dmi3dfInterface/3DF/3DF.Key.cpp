#include "StdAfx.h"

#include "3DF.Key.h"
#include "Private/3DF.KeyPrivate.h"

#include "3DF.Segment.h"

#include "3DF.Line.h"

#include <HTools.h>

USING_3DF_NAMESPACE

Key::Key(HC_KEY nInKey)
{
	KeyPrivate * pcImpl = new KeyPrivate();
	pcImpl->m_nKey = nInKey;

	m_pcImpl = pcImpl;
}

Key::Key(Key const & cInThat)
{
	m_pcImpl = new KeyPrivate();
	Set(cInThat);
}

Key::~Key()
{
}

void Key::Set(Key const & cInThat)
{
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;
	KeyPrivate * pcInThatImpl = (KeyPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

Key const & Key::operator=(Key const & cInThat)
{
	Set(cInThat);
	return *this;
}

HC_KEY Key::KeyValue() const 
{ 
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;
	return pcImpl->m_nKey;
}

void Key::SetKeyValue(HC_KEY nInKey)
{
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;
	pcImpl->m_nKey = nInKey;
}

void Key::SetKeyValue(HC_KEY nInKey) const
{
	KeyPrivate * pcImpl = (KeyPrivate *)m_pcImpl;
	pcImpl->m_nKey = nInKey;
}

void Key::Delete()
{
	assert(false);
}