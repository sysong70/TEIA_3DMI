#pragma once

#include "AM.h"

#include "Am.Object.h"

namespace AM
{
	class Kit : public Object
	{
	public:
		Kit();

		static const AM::Type staticType = AM::Type::Kit;
		AM::Type ObjectType() const { return staticType; }

	protected:
		Kit(Kit && that) noexcept;
		Kit & operator = (Kit && that) noexcept;
	};
}