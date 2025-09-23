#include "StdAfx.h"

#include "Am.Vertex.h"

#include "./Impl/Am.Impl.h"
#include "./Impl/Am.Vertex.Impl.h"

using namespace AM;

AM::VertexKit::VertexKit() = default;

VertexKit & AM::VertexKit::setPosition(const H3DF::DPoint & position)
{
	auto * impl = ENSURE_IMPL(VertexKit);
	if (nullptr != impl) {
		impl->m_position = position;
	}

	return *this;
}

//== Vertex class ==================================================================================
AM::Vertex::Vertex() = default;

Vertex & AM::Vertex::set(VertexKit const & kit)
{
	auto * impl = ENSURE_IMPL(Vertex);
	if (nullptr != impl) {
		auto * kitImpl = GET_IMPL(kit, VertexKit);
		if (nullptr != kitImpl) {
			impl->m_kit = *kitImpl;
		}
	}

	return *this;
}

Vertex & AM::Vertex::setPosition(const H3DF::DPoint & position)
{
	auto * impl = ENSURE_IMPL(Vertex);
	if (nullptr != impl) {
		impl->m_kit.m_position = position;
	}

	return *this;
}