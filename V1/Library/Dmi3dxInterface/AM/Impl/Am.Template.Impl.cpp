#include "StdAfx.h"

#include "AM.Template.Impl.h"

using namespace AM;

AM::TemplateKitImpl::TemplateKitImpl()
{
	SetType(AM::Type::TemplateKit);
}

AM::TemplateImpl::TemplateImpl()
{
	SetType(AM::Type::Template);
}

std::unique_ptr<Impl> AM::TemplateImpl::Clone() const
{
	auto impl = std::make_unique<TemplateImpl>(*this);
	DEBUG_VALID(impl.get());

	return impl;
}
