#pragma once

#include "AM.h"

#include "Am.Geometry.h"
#include "Am.Kit.h"

namespace AM
{
	class CylinderKit : public Kit
	{
	public:
		CylinderKit();

		CylinderKit & setPosition(const H3DF::DPoint & position);
		CylinderKit & setOrientation(const H3DF::DVector & orientation);
		CylinderKit & setDiameter(const double diameter);
		CylinderKit & setHeight(double height);

		CylinderKit & setProductHint(std::string_view productHint);
	};


	class Cylinder : public Geometry
	{
	public:
		Cylinder();

		static const AM::Type staticType = AM::Type::Cylinder;
		AM::Type ObjectType() const { return staticType; }

		// 복사 허용
		Cylinder(const Cylinder &) = default;
		Cylinder & operator=(const Cylinder &) = default;

		// 이동 허용
		Cylinder(Cylinder &&) noexcept = default;
		Cylinder & operator=(Cylinder &&) noexcept = default;

		Result writeDatal(std::ostream * os) noexcept override;

		Cylinder & set(CylinderKit const & kit);

		Cylinder & setPosition(const H3DF::DPoint & position);
		Cylinder & setOrientation(const H3DF::DVector & orientation);
		Cylinder & setDiameter(const double diameter);
		Cylinder & setHeight(double height);

		Cylinder & setProductHint(std::string_view productHint);
	};
}