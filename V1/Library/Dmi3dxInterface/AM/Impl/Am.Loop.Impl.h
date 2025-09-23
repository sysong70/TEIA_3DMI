#pragma once

#include "../AM.h"

#include "Am.Impl.h"

#include "../Am.Vertex.h"

#include <vector>

namespace AM  
{
	class LoopKitImpl : public Impl
	{
	public:
		LoopKitImpl();

		std::unique_ptr<Impl> Clone() const override;

		std::vector<Vertex> m_vertices;
	};

	class LoopImpl : public Impl
	{
	public:
		LoopImpl();

		std::unique_ptr<Impl> Clone() const override;

		LoopKitImpl m_kit;
	};
}