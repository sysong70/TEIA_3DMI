#pragma once

#include "AM.h"

#include "Am.Geometry.h"

namespace AM
{
	class Extrusion : public Geometry
	{
	public:
		Extrusion();

		static const AM::Type staticType = AM::Type::Extrusion;
		AM::Type ObjectType() const { return staticType; }

		// 복사 허용
		Extrusion(const Extrusion &) = default;
		Extrusion & operator=(const Extrusion &) = default;

		// 이동 허용
		Extrusion(Extrusion &&) noexcept = default;
		Extrusion & operator=(Extrusion &&) noexcept = default;

		Extrusion & setPosition(const H3DF::DPoint & position);
		Extrusion & setOrientation(const H3DF::DVector & orientation);
		Extrusion & setHeight(double height);

		Extrusion & addLoop(const AM::Loop & loop);
	};
}