#pragma once

#include <Json.h>

#include "../Dmi3dfInterface/Component.View.h"

namespace KERNEL
{
	class View
	{
	public:
		View();

		void ExecuteSignal(Json::Object & cInObject);

	protected:
		H3DF::Component::View m_cView;


	};
};
