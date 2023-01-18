#include "StdAfx.h"

#include "3DF.MarkerAttribute.h"

#include <hc.h>

USING_3DF_NAMESPACE

MarkerAttributeControl::MarkerAttributeControl(HC_KEY nInKey) :
	Control(nInKey)
{
}

MarkerAttributeControl & MarkerAttributeControl::SetSize(float fInSize, Marker::SizeUnits nInUnits)
{
	Open();
	HC_Set_Marker_Size(fInSize);
	Close();

	return *this;
}