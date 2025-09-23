#pragma once

#include "AM.h"

#include "Am.Geometry.h"

namespace AM
{
	class Loop : public Geometry
	{
	public:
		Loop();

		static const AM::Type staticType = AM::Type::Loop;
		AM::Type ObjectType() const { return staticType; }

		// 복사 허용
		Loop(const Loop &) = default;
		Loop & operator=(const Loop &) = default;

		// 이동 허용
		Loop(Loop &&) noexcept = default;
		Loop & operator=(Loop &&) noexcept = default;

		Vertex insertVertex(const AM::VertexKit & kit);

	protected:
// 		Loop(Loop && that) noexcept;
// 		Loop & operator = (Loop && that) noexcept;
	};
}