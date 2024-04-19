#pragma once

#include "../3DF/3DF.h"

#include "3DF.Component.h"

namespace H3DF
{
    class API_3DF ComponentMarkupView : public Component
	{
	public:
		ComponentMarkupView();
		ComponentMarkupView(ComponentMarkupView const & cInThat);

		void Set(ComponentMarkupView const & cInThat);
		ComponentMarkupView & operator = (ComponentMarkupView const & cInThat);

        H3DF::Type ObjectType() const { return H3DF::Type::ComponentMarkupView; };

        bool Equals(ComponentMarkupView const & cInThat) const;
	};
}
