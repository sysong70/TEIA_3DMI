#pragma once

#include "Command.h"
#include <Json.h>



namespace Window
{
	class View;
}



namespace Command
{
	class Base
	{
	public:

		Base();

		~Base();

	public:

		virtual bool ReceiveSignal(Json::Object* pData);

		virtual void Run(Window::View* pView);

		virtual void Cancel();

	protected:

		Window::View* m_pView = nullptr;
	};
}
