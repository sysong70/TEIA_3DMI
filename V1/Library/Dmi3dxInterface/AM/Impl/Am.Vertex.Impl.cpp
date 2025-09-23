#include "StdAfx.h"

#include "AM.Vertex.Impl.h"

using namespace AM;

//== VertexKitImpl Class ==============================================================================
AM::VertexKitImpl::VertexKitImpl()
{
	SetType(AM::Type::VertexKit);
}

std::unique_ptr<Impl> AM::VertexKitImpl::Clone() const
{
	CLONE_IMPL(VertexKitImpl);
}

//== VertexImpl Class =================================================================================
AM::VertexImpl::VertexImpl()
{
	SetType(AM::Type::Vertex);
}

std::unique_ptr<Impl> AM::VertexImpl::Clone() const
{
	CLONE_IMPL(VertexImpl);
}
