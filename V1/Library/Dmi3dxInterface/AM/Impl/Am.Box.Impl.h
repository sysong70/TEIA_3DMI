#pragma once

#include "../AM.h"

#include "Am.Impl.h"

namespace AM  
{
	class BoxKitImpl : public Impl
	{
	public:
		BoxKitImpl();

		std::unique_ptr<Impl> Clone() const override;

		H3DF::DPoint m_position;
		H3DF::DVector m_orientation;
		H3DF::DVector m_size;
	};

	class BoxImpl : public Impl
	{
	public:
		BoxImpl();

		std::unique_ptr<Impl> Clone() const override;

		BoxKitImpl m_kit;
	};
}