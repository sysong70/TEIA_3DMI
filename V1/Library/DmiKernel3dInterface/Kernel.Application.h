#pragma once

#include <Json.h>

#include "Kernel.h"
#include "Object.h"

namespace KERNEL
{
	class API_KERNEL Application : public Object
	{
	public:
		Application();

		void InitInstance();

		void ExitInstance();
		//:Ken - 20240131
		void OnUpdateFileOption(Json::Object& data);
		//:Ken - 20240131
		void OnFileOptionReference(Json::Object& data);

		DocView * GetDocView(int nId);
		void RemoveDocView(int nId);

	protected:
	};
};