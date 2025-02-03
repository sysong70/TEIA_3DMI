#pragma once

#include "Command.h"
#include "Command.Resource.h"
#include "Facility.AppResources.h"
#include "Facility.CommandIndexer.h"
#include "Window.Application.h"
#include "Window.View.h"

//--------------------------------------------------------------------------------------------------

namespace Command
{
	class Base
	{
	public:

		Base();

		~Base();

	public:

		virtual bool ReceiveSignal(Json::Object* pData);

		virtual void Run(Window::View* pView = nullptr);

		virtual void Cancel();

		virtual void Complete();
		// run and close immediately
		virtual bool IsRunOnlyOnce() {
			return false;
		}

	protected:

		Window::View* m_pView = nullptr;
		Control::TaskPanel* m_pPanel = nullptr;
	};
}
