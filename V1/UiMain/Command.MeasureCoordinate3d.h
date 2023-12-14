#pragma once

#include "Command.Base.h"
#include <Json.h>



namespace Command
{
	class MeasureCoordinate3d : public Base
	{
	public:

		MeasureCoordinate3d();

	public:

		bool ReceiveSignal(Json::Object* pData) override;

		void Run(Window::View* pView) override;

		void Cancel() override;

	private:

		Json::Object m_data;
	};
}
