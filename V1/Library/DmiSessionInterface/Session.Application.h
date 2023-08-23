#pragma once

#include <Json.h>

#include "../Dmi3dfInterface/3DF.Component.Application.h"

namespace SESSION
{
	class Application
	{
	public:
		Application();

		void ExecuteSignal(Json::Object & cInObject);

	protected:
		H3DF::Component::Application m_cApplication;

	private:
		//Interface * m_pc3dfInterface = nullptr;

		// Pointer to the HOOPS/MVO HDB object associated with this instance of the application

		//std::map<int, H3DF::Canvas *> m_mpcCanvas;
	};
};
