#include "StdAfx.h"

#include "AM.Cylinder.Impl.h"

using namespace AM;

//== CylinderKitImpl Class =========================================================================
AM::CylinderKitImpl::CylinderKitImpl()
{
	SetType(AM::Type::BoxKit);
}

std::unique_ptr<Impl> AM::CylinderKitImpl::Clone() const
{
	CLONE_IMPL(CylinderKitImpl);
}

//== CylinderImpl Class ============================================================================
AM::CylinderImpl::CylinderImpl()
{
	SetType(AM::Type::Box);
}

std::unique_ptr<Impl> AM::CylinderImpl::Clone() const
{
	CLONE_IMPL(CylinderImpl);
}
