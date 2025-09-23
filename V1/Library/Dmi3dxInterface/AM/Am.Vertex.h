#pragma once

#include "AM.h"

#include "Am.Geometry.h"
#include "Am.Kit.h"

namespace AM
{
	class VertexKit : public Kit
	{
	public:
		VertexKit();

		VertexKit & setPosition(const H3DF::DPoint & position);
	};

	class Vertex : public Geometry
	{
	public:
		Vertex();

		static const AM::Type staticType = AM::Type::Vertex;
		AM::Type ObjectType() const { return staticType; }

		// 복사 허용
		Vertex(const Vertex &) = default;
		Vertex & operator=(const Vertex &) = default;

		// 이동 허용
		Vertex(Vertex &&) noexcept = default;
		Vertex & operator=(Vertex &&) noexcept = default;

		Vertex & set(VertexKit const & kit);

		Vertex & setPosition(const H3DF::DPoint & position);

	protected:
	};
}