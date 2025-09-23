#include "StdAfx.h"

#include "Am.Loop.h"

#include "./Impl/Am.Loop.Impl.h"

#include "Am.Vertex.h"
#include "./Impl/Am.Vertex.Impl.h"

using namespace AM;

AM::Loop::Loop() = default;

Vertex AM::Loop::insertVertex(const AM::VertexKit & kit)
{
	Vertex vertex;
	auto * impl = ENSURE_IMPL(Loop);
	if (nullptr != impl) {

		auto * kitImpl = GET_IMPL(kit, VertexKit);
		if(nullptr != kitImpl) {
			vertex.set(kit);
			impl->m_kit.m_vertices.push_back(vertex);
		}
	}

	return vertex;
}