#include "StdAfx.h"

#include "3DF.Bounding.h"

USING_3DF_NAMESPACE

BoundingKit & BoundingKit::SetExclusion(bool bInExclude) 
{ 
	m_bExclude = bInExclude; 
	return *this;
}

bool BoundingKit::ShowExclusion(bool & bOutEexclusion) const
{
	bOutEexclusion = m_bExclude;
	return true;
}