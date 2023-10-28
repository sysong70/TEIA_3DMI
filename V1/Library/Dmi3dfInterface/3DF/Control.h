#pragma once

#include "3DF.h"
#include "Object.h"

namespace H3DF
{
	class API_3DF Control : public Object
	{
	public:
		Control(HC_KEY nInKey = INVALID_KEY);
		Control(Control const & cInThat);
		virtual ~Control();

		void Set(Control const & cInThat);
		Control const & operator = (Control const & cInThat);

		H3DF::Type ObjectType() const { return H3DF::Type::Control; };

		bool operator == (Control const & cInThat) const;

	};
}
