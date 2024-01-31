#pragma once

#include <Json.h>

#include "Kernel.h"

#include "../Dmi3dfInterface/3DF.Application.h"

namespace KERNEL
{
	class API_KERNEL Application
	{
	public:
		Application();

		void InitInstance();

		void ExitInstance();

	protected:
		H3DF::Application m_cApplication;
	};
};