#pragma once

#include <Json.h>

#include "../Dmi3dfInterface/3DF.Component.View.h"

namespace KERNEL
{
	class View
	{
	public:
		View();

		void ExecuteSignal(Json::Object & cInObject);

	protected:
		H3DF::Component::View m_cView;

	private:
		//Interface * m_pc3dfInterface = nullptr;

		// Pointer to the HOOPS/MVO HDB object associated with this instance of the application

		//std::map<int, H3DF::Canvas *> m_mpcCanvas;
	};
};
