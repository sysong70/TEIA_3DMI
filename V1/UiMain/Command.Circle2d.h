#pragma once

#include "Command.Base.h"

//--------------------------------------------------------------------------------------------------

namespace Command
{
	namespace Circle2d
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

		class ThreePoints : public Base
		{
		public:

			ThreePoints() {}

		public:

			bool ReceiveSignal(Json::Object* pData) override;

			void Run(Window::View* pView) override;
		};

//--------------------------------------------------------------------------------------------------

		class CenterRadius : public Base
		{
		public:

			CenterRadius() {}

		public:

			bool ReceiveSignal(Json::Object* pData) override;

			void Run(Window::View* pView) override;
		};

//--------------------------------------------------------------------------------------------------

		class TwoTangentsRadius : public Base
		{
		public:

			TwoTangentsRadius() {}

		public:

			bool ReceiveSignal(Json::Object* pData) override;

			void Run(Window::View* pView) override;
		};
	}
}
