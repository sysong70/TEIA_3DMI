#pragma once

#include "Component.h"

//--------------------------------------------------------------------------------------------------

namespace Component
{
	class StatusBar : public CBCGPRibbonStatusBar
	{
	public:

		StatusBar();

		virtual ~StatusBar();

		bool Initialize(CWnd* pMainFrame);

		void ReceiveSignal(Json::Object* pData);

	private:

		void ShowMessage(Json::Object& data);

		void ShowCoordinate(Json::Object& data);
	};
}
