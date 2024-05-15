#pragma once

#include <Json.h>

#include "../Signal/Signal.h"

#include "../DmiKernel3dInterface/Command.Manager.h"

namespace SESSION
{
	// Session에 각족 manager와 필요한 내용을 저장하도록 한다.
	class Session
	{
	public:
		Session(KERNEL::Command::Manager * pcCommandManager, int nId);
		//Session(KERNEL::Application & cApplication, int nId);
		~Session();

		int Id() const;

		void ExecuteViewSignal(Json::Object & cInObject);

		//== View 관련 함수 ==========================================================================
		void ViewInitialize(Json::Object & cInObject, int nViewId);
		static UINT ThreadFileOpen(LPVOID pcParam);

/*
		void ViewExecuteCommand(Json::Object & cInObject);
		void ViewCancelCommands();
*/
		//== Command 관련 함수 =======================================================================
		void CommandRequest(Json::Object & cInObject, int nViewId);
		void CommandChange(Json::Object & cInObject, int nViewId);

		//== Panel 관련 함수 ========================================================================
		void ModelPanelSignal(Json::Object & cInObject, int nViewId);

	private:
		int m_nSessionId = -1;
		KERNEL::Command::Manager * m_pcCommandManager = nullptr;
		KERNEL::DocView * m_pcDocView = nullptr;
	};
}