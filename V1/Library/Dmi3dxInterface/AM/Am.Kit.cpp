#include "StdAfx.h"

#include "Am.Kit.h"

using namespace AM;

AM::Kit::Kit() = default;

AM::Kit::Kit(Kit && cInThat) noexcept :
	Object(std::move(cInThat))
{
}

Kit & AM::Kit::operator = (Kit && cInThat) noexcept
{
	this->Object::operator = (std::move(cInThat));
	return *this;
}