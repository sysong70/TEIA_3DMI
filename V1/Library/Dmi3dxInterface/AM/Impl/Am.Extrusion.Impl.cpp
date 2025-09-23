#include "StdAfx.h"

#include "AM.Extrusion.Impl.h"

using namespace AM;

//== ExtrusionKitImpl Class ==============================================================================
AM::ExtrusionKitImpl::ExtrusionKitImpl()
{
	SetType(AM::Type::ExtrusionKit);
}

std::unique_ptr<Impl> AM::ExtrusionKitImpl::Clone() const
{
	CLONE_IMPL(ExtrusionKitImpl);
}

//== ExtrusionImpl Class =================================================================================
AM::ExtrusionImpl::ExtrusionImpl()
{
	SetType(AM::Type::Extrusion);
}

std::unique_ptr<Impl> AM::ExtrusionImpl::Clone() const
{
	CLONE_IMPL(ExtrusionImpl);
}
