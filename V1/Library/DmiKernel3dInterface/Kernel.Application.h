#pragma once

#include <Json.h>

#include "Kernel.h"
#include "Object.h"

namespace KERNEL
{
	class Application : public Object
	{
	public:
		Application();

		void InitInstance();

		void ExitInstance();

		double ShowDpiScale();

		// KEN - 20240131
		void OnUpdateFileOption(Json::Object& data);
		// KEN - 20240131
		void OnFileOptionReference(Json::Object& data);

		Session * GetSession(int nId);
		void RemoveDocView(int nId);

	protected:
	};
};