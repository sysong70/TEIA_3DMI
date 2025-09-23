#include "StdAfx.h"

#include "Am.Equipment.h"

#include "./Impl/Am.Impl.h"
#include "./Impl/Am.Equipment.Impl.h"

#include "Am.Template.h"

using namespace AM;

AM::Equipment::Equipment()
{
}

AM::Equipment::~Equipment()
{

}

Equipment & AM::Equipment::setTitile(std::string_view title)
{
	auto * impl = ENSURE_IMPL(Equipment);
	if (nullptr != impl) {
		impl->m_title = title;
	}

	return *this;
}

Equipment & AM::Equipment::setBuiltIn(bool builtIn)
{
	auto * impl = ENSURE_IMPL(Equipment);
	if (nullptr != impl) {
		impl->m_builtIn = builtIn;
	}

	return *this;
}

AM::Template AM::Equipment::insertTemplate(TemplateKit const & kit)
{
	AM::Template templ;

	auto * impl = ENSURE_IMPL(Equipment);
	if (nullptr == impl) {
		return templ;
	}

	templ.set(kit);
	
	impl->m_templates.push_back(templ);

	return templ;
}