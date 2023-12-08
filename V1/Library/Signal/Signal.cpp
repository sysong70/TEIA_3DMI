#include "stdafx.h"
#include "Signal.h"

#define SendActionDataOnly(action) \
Json::Object data; \
ConstructData(data, action); \
Wrapper().SendData(data);



#pragma region Application Class

void Signal::Application::ConstructData(Json::Object& data, Action action)
{
	data.SetInteger(SKW_TARGET, (int)Target::Application);
	data.SetInteger(SKW_ACTION, (int)action);
}



void Signal::Application::OnInitInstance()
{
	SendActionDataOnly(Action::OnInitInstance);
}



void Signal::Application::OnExitInstance()
{
	SendActionDataOnly(Action::OnExitInstance);
}



void Signal::Application::OnUpdatePreference(Json::Object& value)
{
	//:WARNING - append construction to value
	Json::Object data = value;
	ConstructData(data, Action::OnUpdatePreference);

	Wrapper().SendData(data);
}



void Signal::Application::OnUpdateFileOption(Json::Object& value)
{
	//:WARNING - append construction to value
	Json::Object data = value;
	ConstructData(data, Action::OnUpdateFileOption);

	Wrapper().SendData(data);
}



void Signal::Application::OnDpiAware(double scale)
{
	Json::Object data;
	ConstructData(data, Action::OnDpiAware);

	data.SetReal(SKW_DPISCALE, scale);

	Wrapper().SendData(data);
}

#pragma endregion //:REGION

#pragma region MainFrame Class

void Signal::MainFrame::ConstructData(Json::Object& data, Action action)
{
	data.SetInteger(SKW_TARGET, (int)Target::MainFrame);
	data.SetInteger(SKW_ACTION, (int)action);
}



void Signal::MainFrame::ShowNotice()
{
	DEBUG_STOP;
}



void Signal::MainFrame::ShowProgress()
{
	SendActionDataOnly(Action::ShowProgress);
}



void Signal::MainFrame::HideProgress()
{
	SendActionDataOnly(Action::HideProgress);
}

#pragma endregion //:REGION

#pragma region StatusBar Class

void Signal::StatusBar::ConstructData(Json::Object& data, Action action)
{
	data.SetInteger(SKW_TARGET, (int)Target::StatusBar);
	data.SetInteger(SKW_ACTION, (int)action);
}



void Signal::StatusBar::ShowMessage(CString message)
{
	Json::Object data;
	ConstructData(data, Action::ShowMessage);

	data.SetString(SKW_MESSAGE, message);

	Wrapper().SendData(data);
}



void Signal::StatusBar::ShowCoordinate(double x, double y)
{
	Json::Object data;
	ConstructData(data, Action::ShowCoordinate);

	data.SetReal(SKW_X, x);
	data.SetReal(SKW_Y, y);

	Wrapper().SendData(data);
}



void Signal::StatusBar::ShowCoordinate(double x, double y, double z)
{
	Json::Object data;
	ConstructData(data, Action::ShowCoordinate);

	data.SetReal(SKW_X, x);
	data.SetReal(SKW_Y, y);
	data.SetReal(SKW_Z, z);

	Wrapper().SendData(data);
}

#pragma endregion //:REGION

#pragma region Progress Class

void Signal::Progress::ConstructData(Json::Object& data, Action action)
{
	data.SetInteger(SKW_TARGET, (int)Target::Progress);
	data.SetInteger(SKW_ACTION, (int)action);
}



void Signal::Progress::SetRange(int min, int max)
{
	Json::Object data;
	ConstructData(data, Action::SetRange);

	data.SetInteger(SKW_MIN, min);
	data.SetInteger(SKW_MAX, max);

	Wrapper().SendData(data);
}



void Signal::Progress::SetPosition(int pos)
{
	Json::Object data;
	ConstructData(data, Action::SetPosition);

	data.SetInteger(SKW_POSITION, pos);

	Wrapper().SendData(data);
}



void Signal::Progress::SetMessage(CString message)
{
	Json::Object data;
	ConstructData(data, Action::SetMessage);

	data.SetString(SKW_MESSAGE, message);

	Wrapper().SendData(data);
}



void Signal::Progress::AddLog(Status status, CString title, CString description, CString tooltip)
{
	Json::Object data;
	ConstructData(data, Action::AddLog);

	data.SetInteger(SKW_STATUS, (int)status);
	data.SetString(SKW_TITLE, title);
	data.SetString(SKW_DESCRIPTION, description);
	data.SetString(SKW_TOOLTIP, tooltip);

	Wrapper().SendData(data);
}



void Signal::Progress::SetLogStatus(Status status)
{
	Json::Object data;
	ConstructData(data, Action::SetLogStatus);

	data.SetInteger(SKW_STATUS, (int)status);

	Wrapper().SendData(data);
}



void Signal::Progress::ClearLog()
{
	SendActionDataOnly(Action::ClearLog);
}

#pragma endregion //:REGION

#pragma region View Class

void Signal::View::ConstructData(Json::Object& data, Action action)
{
	data.SetInteger(SKW_TARGET, (int)Target::View);
	data.SetInteger(SKW_ACTION, (int)action);
	data.SetInteger(SKW_VIEWID, Wrapper().ViewId);
}



void Signal::View::ConstructMouseData(Json::Object& data, Action action, UINT flags, int x, int y)
{
	ConstructData(data, action);

	data.SetInteger(SKW_FLAG, flags);
	data.SetInteger(SKW_X, x);
	data.SetInteger(SKW_Y, y);
}



void Signal::View::ConstructWheelData(Json::Object& data, UINT flags, short delta, int x, int y)
{
	ConstructMouseData(data, Action::OnMouseWheel, flags, x, y);

	data.SetInteger(SKW_DELTA, delta);
}



void Signal::View::ConstructKeyData(Json::Object& data, Action action, UINT chr, UINT repeat, UINT flags)
{
	ConstructData(data, action);

	data.SetInteger(SKW_CHAR, chr);
	data.SetInteger(SKW_FLAG, flags);
}



void Signal::View::OnConstruct()
{
	SendActionDataOnly(Action::OnConstruct);
}



void Signal::View::OnDestruct()
{
	SendActionDataOnly(Action::OnDestruct);
}



void Signal::View::OnInitialize(DWORD_PTR hWnd, CString path)
{
	Json::Object data;
	ConstructData(data, Action::OnInitialize);

	data.SetDwordPtr(SKW_HWND, hWnd);
	data.SetString(SKW_FILEPATH, path);

	Wrapper().SendData(data);
}



void Signal::View::OnCommand(UINT id)
{
	Json::Object data;
	ConstructData(data, Action::OnCommand);

	data.SetInteger(SKW_ID, id);

	Wrapper().SendData(data);
}



void Signal::View::OnCancel()
{
	SendActionDataOnly(Action::OnCancel);
}

#define SendMouseData(action) \
Json::Object data; \
ConstructMouseData(data, action, flags, x, y); \
Wrapper().SendData(data);

void Signal::View::OnMouseMove(UINT flags, int x, int y)
{
	SendMouseData(Action::OnMouseMove);
}



void Signal::View::OnLButtonDown(UINT flags, int x, int y, int osnapId)
{
	Json::Object data;
	ConstructMouseData(data, Action::OnLButtonDown, flags, x, y);

	data.SetInteger(SKW_OSNAPID, osnapId);

	Wrapper().SendData(data);
}



void Signal::View::OnLButtonUp(UINT flags, int x, int y)
{
	SendMouseData(Action::OnLButtonUp);
}



void Signal::View::OnMButtonDown(UINT flags, int x, int y)
{
	SendMouseData(Action::OnMButtonDown);
}



void Signal::View::OnMButtonUp(UINT flags, int x, int y)
{
	SendMouseData(Action::OnMButtonUp);
}



void Signal::View::OnRButtonDown(UINT flags, int x, int y)
{
	SendMouseData(Action::OnRButtonDown);
}



void Signal::View::OnRButtonUp(UINT flags, int x, int y)
{
	SendMouseData(Action::OnRButtonUp);
}



void Signal::View::OnMouseWheel(UINT flags, short delta, int x, int y)
{
	Json::Object data;
	ConstructWheelData(data, flags, delta, x, y);

	Wrapper().SendData(data);
}



void Signal::View::OnMouseWheel(UINT flags, short delta, int x, int y, int left, int top, int right, int bottom)
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

void Signal::View::OnPaint()
{
	SendActionDataOnly(Action::OnPaint);
}



void Signal::View::OnPaint(int left, int top, int right, int bottom)
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



void Signal::View::OnResize(int x, int y)
{
	Json::Object data;
	ConstructData(data, Action::OnResize);

	data.SetInteger(SKW_X, x);
	data.SetInteger(SKW_Y, y);

	Wrapper().SendData(data);
}



void Signal::View::OnInput(CString value, int row, int column)
{
	Json::Object data;
	ConstructData(data, Action::OnInput);

	data.SetString(SKW_VALUE, value);
	data.SetInteger(SKW_ROW, row);
	data.SetInteger(SKW_COLUMN, column);

	Wrapper().SendData(data);
}

#define SendKeyData(action) \
Json::Object data; \
ConstructKeyData(data, action, chr, repeat, flags); \
Wrapper().SendData(data);

void Signal::View::OnChar(UINT chr, UINT repeat, UINT flags)
{
	SendKeyData(Action::OnChar);
}



void Signal::View::OnKeyDown(UINT chr, UINT repeat, UINT flags)
{
	SendKeyData(Action::OnKeyDown);
}



void Signal::View::OnKeyUp(UINT chr, UINT repeat, UINT flags)
{
	SendKeyData(Action::OnKeyUp);
}

#undef SendKeyData

void Signal::View::SetValidation(bool success)
{
	Json::Object data;
	ConstructData(data, Action::SetValidation);

	data.SetBoolean(SKW_VALID, success);

	Wrapper().SendData(data);
}



void Signal::View::PaintOverlap()
{
	SendActionDataOnly(Action::PaintOverlap);
}



void Signal::View::SetInputMode(EInputMode mode)
{
	Json::Object data;
	ConstructData(data, Action::SetInputMode);

	data.SetInteger(SKW_MODE, (int)mode);

	Wrapper().SendData(data);
}

#pragma endregion //:REGION

#pragma region ModelPanel

void Signal::ModelPanel::ConstructData(Json::Object& data, Action action)
{
	data.SetInteger(SKW_TARGET, (int)Target::ModelPanel);
	data.SetInteger(SKW_ACTION, (int)action);
	data.SetInteger(SKW_VIEWID, Wrapper().ViewId);
}

#define SendKeyData(action) \
Json::Object data; \
ConstructData(data, action); \
data.SetDwordPtr(SKW_KEY, key); \
Wrapper().SendData(data);

void Signal::ModelPanel::OnDeleteItem(DWORD_PTR key)
{
	SendKeyData(Action::OnDeleteItem);
}



void Signal::ModelPanel::OnItemExpanded(DWORD_PTR key)
{
	SendKeyData(Action::OnItemExpanded);
}



void Signal::ModelPanel::OnSelChanged(DWORD_PTR key)
{
	SendKeyData(Action::OnSelChanged);
}

#undef SendKeyData

void Signal::ModelPanel::AddItems(TreeItems& items)
{
	Json::Object data;
	ConstructData(data, Action::AddItems);

	Json::Array& nodes = data.CreateArray(SKW_ITEMS);
	for (auto& item : items) {
		Json::Object* pChild = new Json::Object();

		pChild->SetDwordPtr(SKW_PARENT, item.ParentKey);
		pChild->SetDwordPtr(SKW_KEY, item.Key);
		pChild->SetString(SKW_TITLE, item.Title);
		pChild->SetBoolean(SKW_HASCHILDREN, item.HasChildren);
		pChild->SetInteger(SKW_TYPE, (int)item.Type);

		nodes.AddObject(*pChild);
	}

	Wrapper().SendData(data);
}



void Signal::ModelPanel::AddChildren(DWORD_PTR parentKey, TreeItems& items)
{
	Json::Object data;
	ConstructData(data, Action::AddChildren);

	data.SetDwordPtr(SKW_PARENT, parentKey);

	Json::Array& nodes = data.CreateArray(SKW_CHILDREN);
	for (auto& item : items) {
		Json::Object* pChild = new Json::Object();

		// ignore TreeItem.Parent
		pChild->SetDwordPtr(SKW_KEY, item.Key);
		pChild->SetString(SKW_TITLE, item.Title);
		pChild->SetBoolean(SKW_HASCHILDREN, item.HasChildren);
		pChild->SetInteger(SKW_TYPE, (int)item.Type);

		nodes.AddObject(*pChild);
	}
}

#pragma endregion //:REGION

#pragma region InteractiveCommand Class

void Signal::InteractiveCommand::ConstructData(Json::Object& data, Action action)
{
	data.SetInteger(SKW_TARGET, (int)Target::Command);
	data.SetInteger(SKW_ACTION, (int)action);
}



void Signal::InteractiveCommand::OnRequestValue(UINT commandId)
{
	Json::Object data;
	ConstructData(data, Action::OnRequestValue);

	data.SetInteger(SKW_ID, commandId);
}



void Signal::InteractiveCommand::ResponseValue(UINT commandId, Json::Object& value, Json::Object& defaultValue)
{
	Json::Object data;
	ConstructData(data, Action::ResponseValue);

	data.SetInteger(SKW_ID, commandId);
	data.SetObject(SKW_VALUE, new Json::Object(value));
	data.SetObject(SKW_DEFAULTVALUE, new Json::Object(defaultValue));

	Wrapper().SendData(data);
}

#pragma endregion //:REGION

#pragma region Delivery Class

Signal::Delivery::Delivery()
{
#define SetWrapper(className) className.m_pWrapper = this

	SetWrapper(application);
	SetWrapper(mainFrame);
	SetWrapper(statusBar);
	SetWrapper(progress);
	SetWrapper(view);
	SetWrapper(modelPanel);
	SetWrapper(command);

#undef SetWrapper
}



Signal::Delivery::Delivery(int viewId, void(*sender)(const wchar_t*))
	: ViewId(viewId)
	, SendSignal(sender)
{
}



void Signal::Delivery::SetSender(void (*func)(const wchar_t*))
{
	SendSignal = func;
}



void Signal::Delivery::SendData(Json::Object& data)
{
	if (SendSignal != nullptr) {
		SendSignal(data.ToString().GetBuffer());
	}
}

#pragma endregion //:REGION

#undef SendActionDataOnly
