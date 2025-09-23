#include "StdAfx.h"

#include "Am.Template.h"

#include "./Impl/Am.Template.Impl.h"

using namespace AM;

//== TemplateKit class =============================================================================
AM::TemplateKit::TemplateKit() = default;

TemplateKit & AM::TemplateKit::setDescription(std::string_view description)
{
	auto * impl = ENSURE_IMPL(TemplateKit);
	if(nullptr != impl) {
		impl->m_description = description;
	}

	return *this;
}

TemplateKit & AM::TemplateKit::setProperty(std::string_view property)
{
	auto * impl = ENSURE_IMPL(TemplateKit);
	if (nullptr != impl) {
		impl->m_property = property;
	}

	return *this;
}

//== Template class ================================================================================

AM::Template::Template() = default;

AM::Template::~Template() = default;

Template & AM::Template::set(TemplateKit const & kit)
{
	auto * impl = ENSURE_IMPL(Template);
	if (nullptr != impl) {
		auto * kitImpl = GET_IMPL(kit, TemplateKit);
		if (nullptr != kitImpl) {
			impl->m_kit = std::move(*kitImpl);
		}
	}

	return *this;
}

Template & AM::Template::setDescription(std::string_view description)
{
	auto * impl = ENSURE_IMPL(Template);
	if (nullptr != impl) {
		impl->m_kit.m_description = description;
	}

	return *this;
}

Template & AM::Template::setProperty(std::string_view property)
{
	auto * impl = ENSURE_IMPL(Template);
	if (nullptr != impl) {
		impl->m_kit.m_property = property;
	}

	return *this;
}

Cylinder AM::Template::insertCylinder(CylinderKit const & kit)
{
	auto * impl = ENSURE_IMPL(Template);
	if (nullptr == impl) {
		return {};
	}

	auto cylinder = std::make_unique<Cylinder>();
	cylinder->set(kit);

	Cylinder * returnCylinder = cylinder.get();

	impl->m_kit.m_geometries.emplace_back(std::move(cylinder));

	return *returnCylinder;
}