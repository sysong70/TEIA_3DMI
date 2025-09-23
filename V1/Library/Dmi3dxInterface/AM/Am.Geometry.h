#pragma once

#include "AM.h"

#include "Am.Object.h"

namespace AM
{
	class Geometry : public Object
	{
	public:
		static const AM::Type staticType = AM::Type::Geometry;
		AM::Type ObjectType() const { return staticType; }

		// 복사 허용
		Geometry(const Geometry &) = default;
		Geometry & operator=(const Geometry &) = default;

		// 이동 허용
		Geometry(Geometry &&) noexcept = default;
		Geometry & operator = (Geometry &&) noexcept = default;

	protected:
		Geometry() = default;
	};
}