#pragma once

#include "3DF.h"
#include "Object.h"

namespace H3DF
{
	class API_3DF Control : public Object
	{
	public:
		static const H3DF::Type staticType = H3DF::Type::Control;
		H3DF::Type ObjectType() const { return staticType; };

		Control(Control const & cInThat) = default;

	protected:
		Control() = default;
		Control(Control && cInThat) noexcept;
		Control & operator = (Control && cInThat) noexcept;
	};
}
