#include "stdafx.h"
#include "Signal.h"

#define SendActionDataOnly(action) \
Json::Object data; \
ConstructData(data, action); \
Wrapper().SendData(data);

#define PostActionDataOnly(action) \
Json::Object data; \
ConstructData(data, action); \
Wrapper().PostData(data);

//**************************************************************************************************

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



void Signal::Application::OnDpiAware(double scale)
{
	Json::Object data;
	ConstructData(data, Action::OnDpiAware);

	data.SetReal(SKW_DPISCALE, scale);

	Wrapper().SendData(data);
}



void Signal::Application::OnUpdatePreference(Json::Object& value)
{
	Json::Object data;
	ConstructData(data, Action::OnUpdatePreference);

	data.SetObject(SKW_VALUE, new Json::Object(value));

	Wrapper().SendData(data);
}



void Signal::Application::OnUpdateFileOption(Json::Object& value)
{
	Json::Object data;
	ConstructData(data, Action::OnUpdateFileOption);

	data.SetObject(SKW_VALUE, new Json::Object(value));

	Wrapper().SendData(data);
}



void Signal::Application::OnFileOptionReference(Json::Object& value)
{
	Json::Object data;
	ConstructData(data, Action::OnFileOptionReference);

	data.SetObject(SKW_VALUE, new Json::Object(value));

	Wrapper().SendData(data);
}



void Signal::Application::AddTraceLog(CString log)
{
	Json::Object data;

	data.SetInteger(SKW_TARGET, (int)Target::DebugTracer);
	data.SetInteger(SKW_ACTION, (int)Action::AddTraceLog);
	data.SetString(SKW_VALUE, log);

	Wrapper().SendData(data);
}



void Signal::Application::AddTraceLogV(const wchar_t* pFormat, ...)
{
	CString stream;
	va_list argList;

	va_start(argList, pFormat);
	stream.FormatV(pFormat, argList);
	va_end(argList);

	AddTraceLog(stream);
}



void Signal::Application::ClearTraceLog()
{
	Json::Object data;

	data.SetInteger(SKW_TARGET, (int)Target::DebugTracer);
	data.SetInteger(SKW_ACTION, (int)Action::ClearTraceLog);

	Wrapper().SendData(data);
}



void Signal::Application::SaveTraceLog(const wchar_t* pPath, bool saveAndClear)
{
	Json::Object data;

	data.SetInteger(SKW_TARGET, (int)Target::DebugTracer);
	data.SetInteger(SKW_ACTION, (int)Action::SaveTraceLog);
	if (pPath != nullptr) {
		data.SetString(SKW_VALUE, pPath);
	}
	data.SetBoolean(SKW_CLEAR, saveAndClear);

	Wrapper().SendData(data);
}

#pragma endregion //:REGION

//**************************************************************************************************

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

//**************************************************************************************************

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

//**************************************************************************************************

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

	//:WARNING - check file path
	message.Replace(L"\\", L"/");
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

//**************************************************************************************************

#pragma region Command Class

void Signal::Command::ConstructData(Json::Object& data, Action action)
{
	data.SetInteger(SKW_TARGET, (int)Target::Command);
	data.SetInteger(SKW_ACTION, (int)action);
}



void Signal::Command::OnRequestPreference()
{
	SendActionDataOnly(Action::OnRequestPreference);
}



void Signal::Command::OnRequestFileOption()
{
	SendActionDataOnly(Action::OnRequestFileOption);
}



void Signal::Command::ResponsePreference(Json::Object& value, Json::Object& defaultValue)
{
	Json::Object data;
	ConstructData(data, Action::ResponsePreference);

	//:WARNING - Window.MainFrame
	data.SetInteger(SKW_VIEWID, -1);
	data.SetObject(SKW_VALUE, new Json::Object(value));
	data.SetObject(SKW_DEFAULTVALUE, new Json::Object(defaultValue));

	Wrapper().SendData(data);
}



void Signal::Command::ResponseFileOption(Json::Object& value, Json::Object& defaultValue)
{
	Json::Object data;
	ConstructData(data, Action::ResponseFileOption);

	//:WARNING - Window.MainFrame
	data.SetInteger(SKW_VIEWID, -1);
	data.SetObject(SKW_VALUE, new Json::Object(value));
	data.SetObject(SKW_DEFAULTVALUE, new Json::Object(defaultValue));

	Wrapper().SendData(data);
}

#pragma endregion //:REGION

//**************************************************************************************************

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
	//Wrapper().PostData(data);
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

	Json::Array& rect = data.CreateArray(SKW_RECT);
	rect.AddInteger(left);
	rect.AddInteger(top);
	rect.AddInteger(right);
	rect.AddInteger(bottom);

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

	Json::Array& rect = data.CreateArray(SKW_RECT);
	rect.AddInteger(left);
	rect.AddInteger(top);
	rect.AddInteger(right);
	rect.AddInteger(bottom);

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

//**************************************************************************************************

#pragma region ModelPanel Class

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

void Signal::ModelPanel::OnItemChecked(DWORD_PTR key, bool checked)
{
	Json::Object data;
	ConstructData(data, Action::OnItemChecked);

	data.SetDwordPtr(SKW_KEY, key);
	data.SetBoolean(SKW_CHECKED, checked);

	Wrapper().SendData(data);
}



void Signal::ModelPanel::OnItemDblClicked(DWORD_PTR key)
{
	SendKeyData(Action::OnItemDblClicked);
}



void Signal::ModelPanel::OnItemDeleted(DWORD_PTR key)
{
	SendKeyData(Action::OnItemDeleted);
}



void Signal::ModelPanel::OnItemExpanded(DWORD_PTR key)
{
	SendKeyData(Action::OnItemExpanded);
}



void Signal::ModelPanel::OnItemSelected(DWORD_PTR key)
{
	SendKeyData(Action::OnItemSelected);
}



void Signal::ModelPanel::OnItemShow(DWORD_PTR key, bool show)
{
	Json::Object data;
	ConstructData(data, Action::OnItemShow);

	data.SetDwordPtr(SKW_KEY, key);
	data.SetBoolean(SKW_CHECKED, show);

	Wrapper().SendData(data);
}



void Signal::ModelPanel::RedrawTree(bool value)
{
	Json::Object data;
	ConstructData(data, Action::RedrawTree);

	data.SetBoolean(SKW_FLAG, value);

	Wrapper().SendData(data);
}



void Signal::ModelPanel::AddItem(TreeItem& item)
{
	Json::Object data;
	ConstructData(data, Action::AddItem);

	data.SetDwordPtr(SKW_PARENT, item.ParentKey);
	data.SetDwordPtr(SKW_KEY, item.Key);
	data.SetString(SKW_TITLE, item.Title);
	data.SetBoolean(SKW_CHECKED, item.Checked);
	data.SetInteger(SKW_TYPE, (int)item.Type);

	Wrapper().SendData(data);
}



void Signal::ModelPanel::AddChildren(DWORD_PTR parentKey, TreeItems& items, bool expand)
{
	Json::Object data;
	ConstructData(data, Action::AddChildren);

	data.SetDwordPtr(SKW_PARENT, parentKey);
	data.SetBoolean(SKW_EXPAND, expand);

	Json::Array& nodes = data.CreateArray(SKW_CHILDREN);
	for (auto& item : items) {
		Json::Object& child = nodes.AddObject();

		// ignore TreeItem.Parent
		child.SetDwordPtr(SKW_KEY, item.Key);
		child.SetString(SKW_TITLE, item.Title);
		child.SetBoolean(SKW_CHECKED, item.Checked);
		child.SetInteger(SKW_TYPE, (int)item.Type);
	}

	Wrapper().SendData(data);
}



void Signal::ModelPanel::CheckItem(DWORD_PTR key, bool checked)
{
	Json::Object data;
	ConstructData(data, Action::CheckItem);

	data.SetDwordPtr(SKW_KEY, key);
	data.SetBoolean(SKW_CHECKED, checked);

	Wrapper().SendData(data);
}



void Signal::ModelPanel::CheckItems(const KeyItems& items, bool checked)
{
	Json::Object data;
	ConstructData(data, Action::CheckItems);

	data.SetBoolean(SKW_CHECKED, checked);
	
	Json::Array& children = data.CreateArray(SKW_ITEMS);
	for (auto item : items) {
		children.AddDwordPtr(item);
	}

	Wrapper().SendData(data);
}



void Signal::ModelPanel::DeleteItem(DWORD_PTR key)
{
	SendKeyData(Action::DeleteItem);
}



void Signal::ModelPanel::ExpandItem(DWORD_PTR key, bool expand)
{
	Json::Object data;
	ConstructData(data, Action::ExpandItem);

	data.SetDwordPtr(SKW_KEY, key);
	data.SetBoolean(SKW_EXPAND, expand);
}



void Signal::ModelPanel::ExpandParent(DWORD_PTR key)
{
	SendKeyData(Action::ExpandParent);
}



void Signal::ModelPanel::InverseCheckedStatus()
{
	SendActionDataOnly(Action::InverseCheckedStatus);
}



void Signal::ModelPanel::SelectItem(DWORD_PTR key, bool select)
{
	Json::Object data;
	ConstructData(data, Action::SelectItem);

	data.SetDwordPtr(SKW_KEY, key);
	data.SetBoolean(SKW_FLAG, select);

	Wrapper().SendData(data);
}

#undef SendKeyData

#pragma endregion //:REGION

//**************************************************************************************************

#pragma region TaskBar Class

void Signal::TaskBar::ConstructData(Json::Object& data, Action action)
{
	data.SetInteger(SKW_TARGET, (int)Target::TaskBar);
	data.SetInteger(SKW_ACTION, (int)action);
	data.SetInteger(SKW_VIEWID, Wrapper().ViewId);
}



void Signal::TaskBar::OnRequestValue(UINT commandId)
{
	Json::Object data;
	ConstructData(data, Action::OnRequestValue);

	data.SetInteger(SKW_ID, commandId);

	Wrapper().SendData(data);
}



void Signal::TaskBar::OnChangedValue(UINT commandId, Json::Object& value)
{
	Json::Object data;
	ConstructData(data, Action::OnChangedValue);

	data.SetInteger(SKW_ID, commandId);
	data.SetObject(SKW_VALUE, new Json::Object(value));

	Wrapper().SendData(data);
}



void Signal::TaskBar::ResponseValue(UINT commandId, Json::Object& value, Json::Object& defaultValue)
{
	Json::Object data;
	ConstructData(data, Action::ResponseValue);

	data.SetInteger(SKW_ID, commandId);
	data.SetObject(SKW_VALUE, new Json::Object(value));
	data.SetObject(SKW_DEFAULTVALUE, new Json::Object(defaultValue));

	Wrapper().SendData(data);
}

#pragma endregion //:REGION

//**************************************************************************************************

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
	SetWrapper(taskBar);
	SetWrapper(command);

#undef SetWrapper
}



Signal::Delivery::Delivery(int viewId, void(*sender)(const wchar_t*))
	: ViewId(viewId)
	, SendSignal(sender)
{
}



Signal::Delivery::~Delivery()
{
}



void Signal::Delivery::SetSender(void (*func)(const wchar_t*))
{
	SendSignal = func;
}



void Signal::Delivery::SendData(Json::Object& data)
{
	DEBUG_VALID(SendSignal);
	if (SendSignal != nullptr) {
		SendSignal(data.ToString().GetBuffer());
	}
}

#include <thread>
#pragma warning(disable : 4996)

void Signal::Delivery::PostData(Json::Object& data)
{
	CString stream = data.ToString();
	const wchar_t* pStream = stream.GetBuffer();

	size_t length = ::wcslen(pStream);
	wchar_t* pData = new wchar_t[length + 1];
	::wcsncpy(pData, pStream, length);
	pData[length] = 0;

	std::thread([this, pData]() {
		if (SendSignal != nullptr) {
			SendSignal(pData);
		}

		delete [] pData;
	}).detach();
}

#pragma endregion //:REGION

#undef SendActionDataOnly
