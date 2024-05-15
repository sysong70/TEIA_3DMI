#pragma once

#include "Command.h"
#include "Signal.Connector.h"

namespace KERNEL
{
	namespace Command
	{
		class API_KERNEL Manager : public CommandBase
		{
		public:
			Manager();

			void ExecuteApplicationSignal(Json::Object & cInObject);

			void ExecuteViewSignal(Json::Object & cInObject);

			void ViewInitialize(Json::Object & cInObject, Signal::Delivery & cInstance);
			void ThreadFileOpen();
			void SetThreadFileOpenViewId(int nId);

			void RemoveDocView(int nId);

			void CommandRequest(Json::Object & cInObject, int nViewId);
			void CommandChange(Json::Object & cInObject, int nViewId);
			void ModelPanelSignal(Json::Object & cInObject, int nViewId);

		protected:
			void ViewPaint(Json::Object & cInObject, int nViewId);
			void ViewResize(Json::Object & cInObject, int nViewId);
		};
	}
}