#include "StdAfx.h"

#include "Am.Cylinder.h"

#include "./Impl/Am.Impl.h"
#include "./Impl/Am.Cylinder.Impl.h"

using namespace AM;
//== CylinderKit class =============================================================================
AM::CylinderKit::CylinderKit() = default;

CylinderKit & AM::CylinderKit::setPosition(const H3DF::DPoint & position)
{
	auto * impl = ENSURE_IMPL(CylinderKit);
	if (nullptr != impl) {
		impl->m_position = position;
	}

	return *this;
}

CylinderKit & AM::CylinderKit::setOrientation(const H3DF::DVector & orientation)
{
	auto * impl = ENSURE_IMPL(CylinderKit);
	if (nullptr != impl) {
		impl->m_orientation = orientation;
	}

	return *this;
}

CylinderKit & AM::CylinderKit::setDiameter(double diameter)
{
	auto * impl = ENSURE_IMPL(CylinderKit);
	if (nullptr != impl) {
		impl->m_diameter = diameter;
	}

	return *this;
}

CylinderKit & AM::CylinderKit::setHeight(double height)
{
	auto * impl = ENSURE_IMPL(CylinderKit);
	if (nullptr != impl) {
		impl->m_height = height;
	}

	return *this;
}

CylinderKit & AM::CylinderKit::setProductHint(std::string_view productHint)
{
	auto * impl = ENSURE_IMPL(CylinderKit);
	if (nullptr != impl) {
		impl->m_productHint = productHint;
	}

	return *this;
}


//== Cylinder class ================================================================================
AM::Cylinder::Cylinder() = default;

Cylinder & AM::Cylinder::set(CylinderKit const & kit)
{
	auto * impl = ENSURE_IMPL(Cylinder);
	if (nullptr != impl) {
		auto * kitImpl = GET_IMPL(kit, CylinderKit);
		if (nullptr != kitImpl) {
			impl->m_kit = *kitImpl;
		}
	}

	return *this;
}

Cylinder & AM::Cylinder::setPosition(const H3DF::DPoint & position)
{
	auto * impl = ENSURE_IMPL(Cylinder);
	if (nullptr != impl) {
		impl->m_kit.m_position = position;
	}

	return *this;
}

Cylinder & AM::Cylinder::setOrientation(const H3DF::DVector & orientation)
{
	auto * impl = ENSURE_IMPL(Cylinder);
	if (nullptr != impl) {
		impl->m_kit.m_orientation = orientation;
	}

	return *this;
}

Cylinder & AM::Cylinder::setDiameter(double diameter)
{
	auto * impl = ENSURE_IMPL(Cylinder);
	if (nullptr != impl) {
		impl->m_kit.m_diameter = diameter;
	}

	return *this;
}

Cylinder & AM::Cylinder::setHeight(double height)
{
	auto * impl = ENSURE_IMPL(Cylinder);
	if (nullptr != impl) {
		impl->m_kit.m_height = height;
	}

	return *this;
}

Cylinder & AM::Cylinder::setProductHint(std::string_view productHint)
{
	auto * impl = ENSURE_IMPL(Cylinder);
	if (nullptr != impl) {
		impl->m_kit.m_productHint = productHint;
	}

	return *this;
}