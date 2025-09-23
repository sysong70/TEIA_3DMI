#pragma once

#include "AM.h"

#include "Am.Geometry.h"

namespace AM
{
	class Box : public Geometry
	{
	public:
		Box();

		static const AM::Type staticType = AM::Type::Box;
		AM::Type ObjectType() const { return staticType; }

		// 복사 허용
		Box(const Box &) = default;
		Box & operator=(const Box &) = default;

		// 이동 허용
		Box(Box &&) noexcept = default;
		Box & operator=(Box &&) noexcept = default;

		Box & setPosition(const H3DF::DPoint & position) noexcept;
		Box & setOrientation(const H3DF::DVector & orientation) noexcept;
		Box & setSize(const H3DF::DVector & size) noexcept;
	};
}