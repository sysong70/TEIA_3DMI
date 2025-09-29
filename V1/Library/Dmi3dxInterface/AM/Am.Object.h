#pragma once

#include "AM.h"
#include "../3DX.h"

#include "../Dmi3dx.h"

#include <3DF/Math.h>
#include <Common_Define.h>

#include <memory>
#include <filesystem>
#include <cassert>
#include <algorithm>
#include <cmath>

namespace AM
{
	class Object
	{
	public:
		Object();
		Object(Object const & that);
		virtual ~Object();

		static const AM::Type staticType = AM::Type::None;
		AM::Type ObjectType() const { return staticType; }
		virtual AM::Type Type() const;

		Object const & operator = (Object const & that);

		Object(Object && that) noexcept;
		Object & operator=(Object && that) noexcept;

		virtual Result writeDatal(std::ostream * os) noexcept;

	private:
		friend class Impl;
		std::unique_ptr<Impl> m_impl;
	};
}