#pragma once

#include "../AM.h"

#include "Am.Impl.h"

#include "../Am.Loop.h"

namespace AM  
{
	class ExtrusionKitImpl : public Impl
	{
	public:
		ExtrusionKitImpl();

		std::unique_ptr<Impl> Clone() const override;

		H3DF::DPoint m_position;
		H3DF::DVector m_orientation;
		double m_height;

		std::vector<Loop> m_loops;
	};

	class ExtrusionImpl : public Impl
	{
	public:
		ExtrusionImpl();

		std::unique_ptr<Impl> Clone() const override;

		ExtrusionKitImpl m_kit;
	};
}