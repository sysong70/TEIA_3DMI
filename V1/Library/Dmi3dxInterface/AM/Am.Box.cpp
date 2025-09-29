#include "StdAfx.h"

#include "Am.Box.h"

#include "./Impl/Am.Impl.h"
#include "./Impl/Am.Box.Impl.h"

using namespace AM;

//== BoxKit Class ==================================================================================

AM::BoxKit::BoxKit() = default;

BoxKit & AM::BoxKit::setPosition(const H3DF::DPoint & position)
{
	auto * impl = ENSURE_IMPL(BoxKit);
	if (nullptr != impl) {
		impl->m_position = position;
	}

	return *this;
}

BoxKit & AM::BoxKit::setOrientation(const H3DF::DVector & orientation)
{
	auto * impl = ENSURE_IMPL(BoxKit);
	if (nullptr != impl) {
		impl->m_orientation = orientation;
	}

	return *this;
}

BoxKit & AM::BoxKit::setSize(const H3DF::DVector & size) noexcept
{
	auto * impl = ENSURE_IMPL(BoxKit);
	if (nullptr != impl) {
		impl->m_size = size;
	}

	return *this;
}

//== Box Class =====================================================================================

AM::Box::Box() = default;

Result AM::Box::writeDatal(std::ostream * os) noexcept
{
	auto * impl = ENSURE_IMPL(Box);
	if (nullptr == impl) {
		return Result::Fail(Error::NotInitialized, "Box implementation is not valid.");
	}

	return impl->writeDatal(os);
}

Box & AM::Box::set(BoxKit const & kit)
{
	auto * impl = ENSURE_IMPL(Box);
	if (nullptr != impl) {
		auto * kitImpl = GET_IMPL(kit, BoxKit);
		if (nullptr != kitImpl) {
			impl->m_kit = *kitImpl;
		}
	}

	return *this;
}

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
