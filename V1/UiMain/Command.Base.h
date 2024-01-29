#pragma once

#include "Command.h"
#include <Json.h>

#include "Command.Resource.h"
#include "Component.TaskBar.h"
#include "Control.TaskPanel.h"
#include "Facility.AppResources.h"
#include "Facility.CommandIndexer.h"
#include "Window.Application.h"
#include "Window.MainFrame.h"
#include "Signal.h"



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
		// run and close immediately
		virtual bool IsRunOnlyOnce() {
			return false;
		}

	protected:

		Window::View* m_pView = nullptr;
	};
}
