#include "StdAfx.h"

#include "AM.Loop.Impl.h"

using namespace AM;

//== LoopKitImpl Class ==============================================================================
AM::LoopKitImpl::LoopKitImpl()
{
	SetType(AM::Type::LoopKit);
}

std::unique_ptr<Impl> AM::LoopKitImpl::Clone() const
{
	CLONE_IMPL(LoopKitImpl);
}

//== LoopImpl Class =================================================================================
AM::LoopImpl::LoopImpl()
{
	SetType(AM::Type::Loop);
}

std::unique_ptr<Impl> AM::LoopImpl::Clone() const
{
	CLONE_IMPL(LoopImpl);
}
