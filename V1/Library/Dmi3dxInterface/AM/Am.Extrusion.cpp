#include "StdAfx.h"

#include "Am.Extrusion.h"

#include "./Impl/Am.Impl.h"
#include "./Impl/Am.Extrusion.Impl.h"

#include "Am.Loop.h"

using namespace AM;

AM::Extrusion::Extrusion() = default;

Extrusion & AM::Extrusion::setPosition(const H3DF::DPoint & position)
{
	auto * impl = ENSURE_IMPL(Extrusion);
	if (nullptr != impl) {
		impl->m_kit.m_position = position;
	}

	return *this;
}

Extrusion & AM::Extrusion::setOrientation(const H3DF::DVector & orientation)
{
	auto * impl = ENSURE_IMPL(Extrusion);
	if (nullptr != impl) {
		impl->m_kit.m_orientation = orientation;
	}

	return *this;
}

Extrusion & AM::Extrusion::setHeight(double height)
{
	auto * impl = ENSURE_IMPL(Extrusion);
	if (nullptr != impl) {
		impl->m_kit.m_height = height;
	}

	return *this;
}

Extrusion & AM::Extrusion::addLoop(const AM::Loop & loop)
{
	auto * impl = ENSURE_IMPL(Extrusion);
	if (nullptr != impl) {
		impl->m_kit.m_loops.push_back(loop);
	}

	return *this;
}