#pragma once

#include "3DF.h"
#include "3DF.Control.h"

#include "3DF.Marker.h"

OPEN_3DF_NAMESPACE

class API_3DF MarkerAttributeControl : public Control
{
public:
	MarkerAttributeControl(HC_KEY nInKey);
	MarkerAttributeControl & SetSize(float fInSize, Marker::SizeUnits nInUnits = Marker::SizeUnits::ScaleFactor);
};

CLOSE_3DF_NAMESPACE