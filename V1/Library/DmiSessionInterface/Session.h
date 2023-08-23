#pragma once

#include <Json.h>

#include "Session.Application.h"
#include "Session.View.h"

namespace SESSION
{
	class Session
	{
	public:
		Session();

		void ExecuteSignal(Json::Object & cInObject);

	protected:
		SESSION::Application m_cApplication;
		SESSION::View m_cView;

	private:
		//Interface * m_pc3dfInterface = nullptr;

		// Pointer to the HOOPS/MVO HDB object associated with this instance of the application

		//std::map<int, H3DF::Canvas *> m_mpcCanvas;
	};
};
