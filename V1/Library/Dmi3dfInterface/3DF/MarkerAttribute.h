#pragma once

#include "3DF.h"
#include "Control.h"

#include "Marker.h"

OPEN_3DF_NAMESPACE

class API_3DF MarkerAttributeControl : public Control
{
public:
	MarkerAttributeControl(HC_KEY nInKey);
	MarkerAttributeControl & SetSize(float fInSize, Marker::SizeUnits nInUnits = Marker::SizeUnits::ScaleFactor);
};

CLOSE_3DF_NAMESPACE