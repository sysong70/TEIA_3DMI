#include "StdAfx.h"

#include "DefinitionImpl.h"

#include <HTools.h>

using namespace H3DF;

HC_KEY const H3DF::DefinitionImpl::KeyValue() const
{
	return m_nKey;
}

void H3DF::DefinitionImpl::SetKeyValue(HC_KEY nInKey)
{
	m_nKey = nInKey;
}