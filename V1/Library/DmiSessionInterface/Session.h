#pragma once

#include <Json.h>

#include "../Signal/Signal.h"

namespace KERNEL 
{
	class View;
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
		KERNEL::View * GetView();

		void MouseSignal(Json::Object & cInObject);
		void KeyboardSignal(Json::Object & cInObject);

		void ExecuteCommand(Json::Object & cInObject);
		void CancelCommands();

	private:
		int m_nSessionId = -1;

		KERNEL::View * m_pcView = nullptr;
	};
}