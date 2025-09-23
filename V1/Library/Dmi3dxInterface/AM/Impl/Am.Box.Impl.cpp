#include "StdAfx.h"

#include "AM.Box.Impl.h"

using namespace AM;

//== BoxKitImpl Class ==============================================================================
AM::BoxKitImpl::BoxKitImpl()
{
	SetType(AM::Type::BoxKit);
}

std::unique_ptr<Impl> AM::BoxKitImpl::Clone() const
{
	CLONE_IMPL(BoxKitImpl);
}

//== BoxImpl Class =================================================================================
AM::BoxImpl::BoxImpl()
{
	SetType(AM::Type::Box);
}

std::unique_ptr<Impl> AM::BoxImpl::Clone() const
{
	CLONE_IMPL(BoxImpl);
}
