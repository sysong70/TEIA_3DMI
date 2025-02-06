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
		// Receive from kernel
		void ShowCoordinate(Json::Object& data);

	public:

		void ShowCoordinate(const CString& value);
		// TEST - StatusBar::ShowCoordinate()
		void ShowCoordinate(double x, double y);
		void ShowCoordinate(double x, double y, double z);
	};
}
