#include "StdAfx.h"

#include "Control.h"
#include "Impl/ControlImpl.h"

#include "Segment.h"

#include <HTools.h>

using namespace H3DF;

H3DF::Control::Control(Control && cInThat)  noexcept : 
	Object(std::move(cInThat)) 
{
}

Control & H3DF::Control::operator = (Control && cInThat) noexcept
{
	this->Object::operator = (std::move(cInThat));
	return *this;
}