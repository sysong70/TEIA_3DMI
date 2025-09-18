#include "StdAfx.h"

#include "Am.Template.h"

#include "./Impl/Am.Impl.h"
#include "./Impl/Am.Template.Impl.h"

using namespace AM;

TemplateKit::TemplateKit()
{
	SET_IMPL(TemplateKit);
}

AM::Template::Template()
{
	SET_IMPL(Template);
}

AM::Template::~Template()
{

}

void AM::Template::setDescription(std::string_view description) noexcept
{
	IMPL(Template);
	impl->m_description = description;
}

void AM::Template::setProperty(std::string_view property) noexcept
{
	IMPL(Template);
	impl->m_property = property;
}