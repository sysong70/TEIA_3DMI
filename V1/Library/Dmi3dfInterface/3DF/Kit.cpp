#include "StdAfx.h"

#include "Kit.h"

#include "Math.h"

using namespace H3DF;

H3DF::Kit::Kit()
{
}

H3DF::Kit::Kit(Kit && cInThat) noexcept :
	Object(std::move(cInThat))
{
}

Kit & H3DF::Kit::operator = (Kit && cInThat) noexcept
{
	this->Object::operator = (std::move(cInThat));
	return *this;
}
