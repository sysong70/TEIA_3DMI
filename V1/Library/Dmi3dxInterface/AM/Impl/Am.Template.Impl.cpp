#include "StdAfx.h"

#include "AM.Template.Impl.h"

#include "../Am.Template.h"

using namespace AM;

//== TemplateKitImpl Class =========================================================================
AM::TemplateKitImpl::TemplateKitImpl()
{
	SetType(AM::Type::TemplateKit);
}

std::unique_ptr<Impl> AM::TemplateKitImpl::Clone() const
{
	CLONE_IMPL(TemplateKitImpl);
}

//== TemplateImpl Class ============================================================================
AM::TemplateImpl::TemplateImpl()
{
	SetType(AM::Type::Template);
}

std::unique_ptr<Impl> AM::TemplateImpl::Clone() const
{
	CLONE_IMPL(TemplateImpl);
}
