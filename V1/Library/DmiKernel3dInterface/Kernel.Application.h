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
		//:Ken - 20240131
		void OnUpdateFileOption(Json::Object& data);
		//:Ken - 20240131
		void OnFileOptionReference(Json::Object& data);

	protected:

		H3DF::Application m_cApplication;
	};
};