#pragma once

#include "Command.Base.h"
#include <Json.h>



namespace Command
{
	class Test8 : public Base
	{
	public:

		Test8();

	public:

		void Run(Window::View* pView) override;

		bool IsRunOnlyOnce() override {
			return true;
		}
	};



	class Test9 : public Base
	{
	public:

		Test9();

	public:

		bool ReceiveSignal(Json::Object* pData) override;

		void Run(Window::View* pView) override;

		void Cancel() override;

	private:

		Json::Object m_data;
	};
}
