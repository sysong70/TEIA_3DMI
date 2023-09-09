#pragma once

#include <Json.h>

#include "Application.h"
#include "View.h"

namespace KERNEL
{
	class Kernel
	{
	public:
		Kernel();

		void ExecuteSignal(Json::Object & cInObject);

	protected:
		KERNEL::Application m_cApplication;
		KERNEL::View m_cView;

	private:
		//Interface * m_pc3dfInterface = nullptr;

		// Pointer to the HOOPS/MVO HDB object associated with this instance of the application

		//std::map<int, H3DF::Canvas *> m_mpcCanvas;
	};
};
