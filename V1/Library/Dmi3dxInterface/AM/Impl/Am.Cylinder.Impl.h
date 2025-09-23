#pragma once

#include "../AM.h"

#include "Am.Impl.h"

namespace AM  
{
	class CylinderKitImpl : public Impl
	{
	public:
		CylinderKitImpl();

		std::unique_ptr<Impl> Clone() const override;

		H3DF::DPoint m_position;
		H3DF::DVector m_orientation;
		double m_diameter = 0;
		double m_height = 0;

		std::string_view m_productHint;
	};

	class CylinderImpl : public Impl
	{
	public:
		CylinderImpl();

		std::unique_ptr<Impl> Clone() const override;

		CylinderKitImpl m_kit;
	};
}