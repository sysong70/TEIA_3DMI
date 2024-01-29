#pragma once

#include <Json.h>

#include "../Signal/Signal.h"

namespace KERNEL 
{
	class DocView;
}

namespace SESSION
{
	// Session에 각족 manager와 필요한 내용을 저장하도록 한다.
	class Session
	{
	public:
		Session();
		~Session();

		int SessionId() const;
		void SessionId(int nSessionId);

		//== View 관련 함수 ==========================================================================
		void ViewInitialize(Json::Object & cInObject, Signal::Delivery & cInstance);
		void ViewDestruct();
		void ViewPaint(Json::Object & cInObject);
		void ViewResize(Json::Object & cInObject);
		KERNEL::DocView * GetView();

		void ViewMouseSignal(Json::Object & cInObject);
		void ViewKeyboardSignal(Json::Object & cInObject);

		void ViewExecuteCommand(Json::Object & cInObject);
		void ViewCancelCommands();

		//== Command 관련 함수 =======================================================================
		void CommandRequest(Json::Object & cInObject);
		void CommandChange(Json::Object & cInObject);

		//== Panel 관련 함수 ========================================================================
		void ModelPanelSignal(Json::Object & cInObject);
	
	private:
		int m_nSessionId = -1;

		KERNEL::DocView * m_pcDocView = nullptr;
	};
}