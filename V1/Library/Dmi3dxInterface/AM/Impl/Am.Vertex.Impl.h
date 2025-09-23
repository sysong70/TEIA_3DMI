#pragma once

#include "../AM.h"

#include "Am.Impl.h"

namespace AM  
{
	class VertexKitImpl : public Impl
	{
	public:
		VertexKitImpl();

		std::unique_ptr<Impl> Clone() const override;

		H3DF::DPoint m_position;
	};

	class VertexImpl : public Impl
	{
	public:
		VertexImpl();

		std::unique_ptr<Impl> Clone() const override;

		VertexKitImpl m_kit;
	};
}