#include "stdafx.h"
#include "Signal.h"



#define SendActionDataOnly(action) \
Json::Object data; \
ConstructData(data, action); \
Wrapper().SendData(data);



namespace Signal
{
#pragma region Application Class

	void Application::ConstructData(Json::Object& data, Action action)
	{
		data.SetInteger(SKW_TARGET, (int)Target::Application);
		data.SetInteger(SKW_ACTION, (int)action);
	}



	void Application::OnInitInstance()
	{
		SendActionDataOnly(Action::OnInitInstance);
	}



	void Application::OnExitInstance()
	{
		SendActionDataOnly(Action::OnExitInstance);
	}

#pragma endregion //:REGION

#pragma region MainFrame Class

	void MainFrame::ConstructData(Json::Object& data, Action action)
	{
		data.SetInteger(SKW_TARGET, (int)Target::MainFrame);
		data.SetInteger(SKW_ACTION, (int)action);
	}



	void MainFrame::ShowNotice()
	{
		DEBUG_STOP;
	}



	void MainFrame::ShowProgress()
	{
		SendActionDataOnly(Action::ShowProgress);
	}



	void MainFrame::HideProgress()
	{
		SendActionDataOnly(Action::HideProgress);
	}

#pragma endregion //:REGION

#pragma region StatusBar Class

	void StatusBar::ConstructData(Json::Object& data, Action action)
	{
		data.SetInteger(SKW_TARGET, (int)Target::StatusBar);
		data.SetInteger(SKW_ACTION, (int)action);
	}



	void StatusBar::ShowMessage(CString message)
	{
		Json::Object data;
		ConstructData(data, Action::ShowMessage);

		data.SetString(SKW_MESSAGE, message);
	}



	void StatusBar::ShowCoordinate(double x, double y)
	{
		Json::Object data;
		ConstructData(data, Action::ShowMessage);

		data.SetReal(SKW_X, x);
		data.SetReal(SKW_Y, y);
	}



	void StatusBar::ShowCoordinate(double x, double y, double z)
	{
		Json::Object data;
		ConstructData(data, Action::ShowMessage);

		data.SetReal(SKW_X, x);
		data.SetReal(SKW_Y, y);
		data.SetReal(SKW_Z, z);
	}

#pragma endregion //:REGION

#pragma region Progress Class

	void Progress::ConstructData(Json::Object& data, Action action)
	{
		data.SetInteger(SKW_TARGET, (int)Target::Progress);
		data.SetInteger(SKW_ACTION, (int)action);
	}



	void Progress::StartMarquee()
	{
		SendActionDataOnly(Action::StartMarquee);
	}



	void Progress::SetMessage(CString message)
	{
		Json::Object data;
		ConstructData(data, Action::StartMarquee);

		data.SetString(SKW_MESSAGE, message);

		Wrapper().SendData(data);
	}



	void Progress::AddLog(Status status, CString title, CString description, CString tooltip)
	{
		Json::Object data;
		ConstructData(data, Action::StartMarquee);

		data.SetInteger(SKW_STATUS, (int)status);
		data.SetString(SKW_TITLE, title);
		data.SetString(SKW_DESCRIPTION, description);
		data.SetString(SKW_TOOLTIP, tooltip);

		Wrapper().SendData(data);
	}



	void Progress::SetLogStatus(Status status)
	{
		Json::Object data;
		ConstructData(data, Action::SetLogStatus);

		data.SetInteger(SKW_STATUS, (int)status);

		Wrapper().SendData(data);
	}



	void Progress::StopMarquee()
	{
		SendActionDataOnly(Action::StopMarquee);
	}

#pragma endregion //:REGION

#pragma region View Class

	void View::ConstructData(Json::Object& data, Action action)
	{
		data.SetInteger(SKW_TARGET, (int)Target::View);
		data.SetInteger(SKW_ACTION, (int)action);
		data.SetInteger(SKW_VIEWID, Wrapper().ViewId);
	}



	void View::ConstructMouseData(Json::Object& data, Action action, UINT flags, int x, int y)
	{
		ConstructData(data, action);

		data.SetInteger(SKW_FLAG, flags);
		data.SetInteger(SKW_X, x);
		data.SetInteger(SKW_Y, y);
	}



	void View::ConstructWheelData(Json::Object& data, UINT flags, short delta, int x, int y)
	{
		ConstructMouseData(data, Action::OnMouseWheel, flags, x, y);
		data.SetInteger(SKW_DELTA, delta);
	}



	void View::OnConstruct()
	{
		SendActionDataOnly(Action::OnConstruct);
	}



	void View::OnDestruct()
	{
		SendActionDataOnly(Action::OnDestruct);
	}



	void View::OnInitialize(DWORD_PTR hWnd, CString path)
	{
		Json::Object data;
		ConstructData(data, Action::OnInitialize);
		data.SetDwordPtr(SKW_HWND, hWnd);
		data.SetString(SKW_FILEPATH, path);

		Wrapper().SendData(data);
	}



	void View::OnCommand(UINT id)
	{
		Json::Object data;
		ConstructData(data, Action::OnCommand);
		data.SetInteger(SKW_ID, id);

		Wrapper().SendData(data);
	}

#define SendMouseData(action) \
Json::Object data; \
ConstructMouseData(data, action, flags, x, y); \
Wrapper().SendData(data);

	void View::OnMouseMove(UINT flags, int x, int y)
	{
		SendMouseData(Action::OnMouseMove);
	}

	void View::OnLButtonDown(UINT flags, int x, int y)
	{
		SendMouseData(Action::OnLButtonDown);
	}

	void View::OnLButtonUp(UINT flags, int x, int y)
	{
		SendMouseData(Action::OnLButtonUp);
	}

	void View::OnMButtonDown(UINT flags, int x, int y)
	{
		SendMouseData(Action::OnMButtonDown);
	}

	void View::OnMButtonUp(UINT flags, int x, int y)
	{
		SendMouseData(Action::OnMButtonUp);
	}

	void View::OnRButtonDown(UINT flags, int x, int y)
	{
		SendMouseData(Action::OnRButtonDown);
	}

	void View::OnRButtonUp(UINT flags, int x, int y)
	{
		SendMouseData(Action::OnRButtonUp);
	}

	void View::OnMouseWheel(UINT flags, short delta, int x, int y, int left, int top, int right, int bottom)
	{
		Json::Object data;
		ConstructWheelData(data, flags, delta, x, y);

		Json::Array* rectArray = new Json::Array();
		rectArray->AddInteger(left);
		rectArray->AddInteger(top);
		rectArray->AddInteger(right);
		rectArray->AddInteger(bottom);

		data.SetArray(SKW_RECT, rectArray);

		Wrapper().SendData(data);
	}

#undef SendMouseData

	void View::OnPaint(int left, int top, int right, int bottom)
	{
		Json::Object data;
		ConstructData(data, Action::OnPaint);

		Json::Array* rectArray = new Json::Array();
		rectArray->AddInteger(left);
		rectArray->AddInteger(top);
		rectArray->AddInteger(right);
		rectArray->AddInteger(bottom);

		data.SetArray(SKW_RECT, rectArray);

		Wrapper().SendData(data);
	}



	void View::OnResize(int x, int y)
	{
		Json::Object data;
		ConstructData(data, Action::OnResize);

		data.SetInteger(SKW_X, x);
		data.SetInteger(SKW_Y, y);

		Wrapper().SendData(data);
	}



	void View::OnText(UINT flags, int x, int y)
	{
		DEBUG_STOP;
	}



	void View::SetValidation(bool success)
	{
		Json::Object data;
		ConstructData(data, Action::SetValidation);

		data.SetBoolean(SKW_VALID, success);

		Wrapper().SendData(data);
	}

#pragma endregion //:REGION

#pragma region Delivery Class

	Delivery::Delivery()
	{
	#define SetWrapper(className) className.m_pWrapper = this

		SetWrapper(application);
		SetWrapper(mainFrame);
		SetWrapper(statusBar);
		SetWrapper(progress);
		SetWrapper(view);
		SetWrapper(modelPanel);

	#undef SetWrapper
	}



	Delivery::Delivery(int viewId, void(*sender)(const wchar_t*))
		: ViewId(viewId)
		, SendSignal(sender)
	{
	}



	void Delivery::SetSender(void (*func)(const wchar_t*))
	{
		SendSignal = func;
	}



	void Delivery::SendData(Json::Object& data)
	{
		if (SendSignal != nullptr) {
			SendSignal(data.ToString().GetBuffer());
		}
	}

#pragma endregion //:REGION
};

#undef SendActionDataOnly
