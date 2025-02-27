#pragma once

#include "3DF.h"
#include "Object.h"

namespace H3DF
{
	class API_3DF Control : public Object
	{
	public:
		H3DF::Type Type() const override { return ObjectType(); }
		H3DF::Type ObjectType() const override { return H3DF::Type::Control; }

		Control(Control const & cInThat) = default;

	protected:
		Control() = default;
		Control(Control && cInThat) noexcept;
		Control & operator = (Control && cInThat) noexcept;
	};
}
