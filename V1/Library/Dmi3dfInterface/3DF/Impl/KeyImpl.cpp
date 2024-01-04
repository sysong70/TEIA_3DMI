#include "StdAfx.h"

#include "KeyImpl.h"

#include <HTools.h>

using namespace H3DF;

HC_KEY const H3DF::KeyImpl::KeyValue() const
{
	return m_nKey;
}

void H3DF::KeyImpl::SetKeyValue(HC_KEY nInKey)
{
	m_nKey = nInKey;
}