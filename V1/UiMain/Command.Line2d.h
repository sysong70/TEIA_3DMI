#pragma once

#include "Command.Base.h"

//--------------------------------------------------------------------------------------------------

namespace Command
{
	namespace Line2d
	{
		class TwoPoints : public Base
		{
		public:

			TwoPoints() {}

		public:

			bool ReceiveSignal(Json::Object* pData) override;

			void Run(Window::View* pView) override;
		};

//--------------------------------------------------------------------------------------------------

		class Polyline : public Base
		{
		public:

			Polyline() {}

		public:

			bool ReceiveSignal(Json::Object* pData) override;

			void Run(Window::View* pView) override;
		};

//--------------------------------------------------------------------------------------------------

		class Polygon : public Base
		{
		public:

			Polygon() {}

		public:

			bool ReceiveSignal(Json::Object* pData) override;

			void Run(Window::View* pView) override;
		};

//--------------------------------------------------------------------------------------------------

		class Rectangle : public Base
		{
		public:

			Rectangle() {}

		public:

			bool ReceiveSignal(Json::Object* pData) override;

			void Run(Window::View* pView) override;
		};
	}
}
