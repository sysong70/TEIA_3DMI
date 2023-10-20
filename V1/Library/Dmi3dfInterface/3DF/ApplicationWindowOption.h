#pragma once

#include "3DF.h"

#include "Kit.h"

#include "Window.h"

namespace H3DF
{
	class API_3DF ApplicationWindowOptionsKit : public Kit
	{
	public:
		ApplicationWindowOptionsKit();
		ApplicationWindowOptionsKit(ApplicationWindowOptionsKit const & cInThat);

		void Set(ApplicationWindowOptionsKit const & cInThat);
		ApplicationWindowOptionsKit const & operator = (ApplicationWindowOptionsKit const & cInThat);

		H3DF::Type ObjectType() const { return H3DF::Type::ApplicationWindowOptionsKit; };

		void Show(ApplicationWindowOptionsKit & cOutKit) const;

		ApplicationWindowOptionsKit & SetDriver(Window::Driver eInDriver);

		ApplicationWindowOptionsKit & UnsetDriver();
	};
}