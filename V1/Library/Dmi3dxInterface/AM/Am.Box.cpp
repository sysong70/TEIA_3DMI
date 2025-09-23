#include "StdAfx.h"

#include "Am.Box.h"

#include "./Impl/Am.Impl.h"
#include "./Impl/Am.Box.Impl.h"

using namespace AM;

AM::Box::Box() = default;

Box & AM::Box::setPosition(const H3DF::DPoint & position) noexcept
{
	auto * impl = ENSURE_IMPL(Box);
	if (nullptr != impl) {
		impl->m_kit.m_position = position;
	}

	return *this;
	
}

Box & AM::Box::setOrientation(const H3DF::DVector & orientation) noexcept
{
	auto * impl = ENSURE_IMPL(Box);
	if (nullptr != impl) {
		impl->m_kit.m_orientation = orientation;
	}

	return *this;
}

Box & AM::Box::setSize(const H3DF::DVector & size) noexcept
{
	auto * impl = ENSURE_IMPL(Box);
	if (nullptr != impl) {
		impl->m_kit.m_size = size;
	}

	return *this;
}
