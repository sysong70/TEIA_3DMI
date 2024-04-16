#pragma once

#include "../../3DF/3DF.h"
#include "../../3DF/Object.h"

#include "../3DF.Component.h"

#include "ComponentImpl.h"


namespace H3DF
{
	class API_3DF ComponentMarkupViewImpl : public ComponentImpl
	{
	public:
		ComponentMarkupViewImpl();
		~ComponentMarkupViewImpl();

		void Copy(ComponentMarkupViewImpl * pcInThat);
	};
}