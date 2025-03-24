#include "stdafx.h"

#include "Signal2d.h"

#include <thread>

//**************************************************************************************************

#define theWrapper	(*(SgnDelivery2d*)WrapperPtr)

#define SendActionDataOnly(action) \
Json::Object data; \
ConstructData(data, action); \
theWrapper.SendData(data);

#define PostActionDataOnly(action) \
Json::Object data; \
ConstructData(data, action); \
theWrapper.PostData(data);

//**************************************************************************************************

#pragma region Application Class

void SgnApplication::ConstructData(Json::Object& data, Action action)
{
	data.SetInteger(SKW_TARGET, (int)Sgn::ETarget::Application);
	data.SetInteger(SKW_ACTION, (int)action);
}



void SgnApplication::OnInitInstance()
{
	SendActionDataOnly(Action::OnInitInstance);
}



void SgnApplication::OnExitInstance()
{
	SendActionDataOnly(Action::OnExitInstance);
}



void SgnApplication::OnDpiAware(double scale)
{
	Json::Object data;
	ConstructData(data, Action::OnDpiAware);

	data.SetReal(SKW_DPISCALE, scale);

	theWrapper.SendData(data);
}



void SgnApplication::OnUpdatePreference(Json::Object& value)
{
	Json::Object data;
	ConstructData(data, Action::OnUpdatePreference);

	data.SetObject(SKW_VALUE, new Json::Object(value));

	theWrapper.SendData(data);
}



void SgnApplication::OnUpdateFileOption(Json::Object& value)
{
	Json::Object data;
	ConstructData(data, Action::OnUpdateFileOption);

	data.SetObject(SKW_VALUE, new Json::Object(value));

	theWrapper.SendData(data);
}



void SgnApplication::OnFileOptionReference(Json::Object& value)
{
	Json::Object data;
	ConstructData(data, Action::OnFileOptionReference);

	data.SetObject(SKW_VALUE, new Json::Object(value));

	theWrapper.SendData(data);
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region MainFrame Class

void SgnMainFrame::ConstructData(Json::Object& data, Action action)
{
	data.SetInteger(SKW_TARGET, (int)Sgn::ETarget::MainFrame);
	data.SetInteger(SKW_ACTION, (int)action);
}



void SgnMainFrame::ConstructData(Json::Object& data, Sgn::ETarget target, Action action)
{
	data.SetInteger(SKW_TARGET, (int)target);
	data.SetInteger(SKW_ACTION, (int)action);
}

//--------------------------------------------------------------------------------------------------

void SgnMainFrame::AddProgressLog(Sgn::EStatus status, CString title, CString description, CString tooltip)
{
	Json::Object data;
	ConstructData(data, Sgn::ETarget::ProgressBar, Action::AddProgressLog);

	data.SetInteger(SKW_STATUS, (int)status);
	data.SetString(SKW_TITLE, title);
	data.SetString(SKW_DESCRIPTION, description);
	data.SetString(SKW_TOOLTIP, tooltip);

	theWrapper.SendData(data);
}



void SgnMainFrame::ClearProgressLog()
{
	Json::Object data;
	ConstructData(data, Sgn::ETarget::ProgressBar, Action::ClearProgressLog);

	theWrapper.SendData(data);
}



void SgnMainFrame::HideProgressBar()
{
	SendActionDataOnly(Action::HideProgressBar);
}



void SgnMainFrame::SetProgressMessage(CString message)
{
	Json::Object data;
	ConstructData(data, Sgn::ETarget::ProgressBar, Action::SetProgressMessage);

	// WARNING - check file path
	message.Replace(L"\\", L"/");
	data.SetString(SKW_MESSAGE, message);

	theWrapper.SendData(data);
}



void SgnMainFrame::SetProgressPosition(int pos)
{
	Json::Object data;
	ConstructData(data, Sgn::ETarget::ProgressBar, Action::SetProgressPosition);

	data.SetInteger(SKW_POSITION, pos);

	theWrapper.SendData(data);
}



void SgnMainFrame::SetProgressRange(int min, int max)
{
	Json::Object data;
	ConstructData(data, Sgn::ETarget::ProgressBar, Action::SetProgressRange);

	data.SetInteger(SKW_MIN, min);
	data.SetInteger(SKW_MAX, max);

	theWrapper.SendData(data);
}



void SgnMainFrame::SetProgressStatus(Sgn::EStatus status)
{
	Json::Object data;
	ConstructData(data, Sgn::ETarget::ProgressBar, Action::SetProgressStatus);

	data.SetInteger(SKW_STATUS, (int)status);

	theWrapper.SendData(data);
}



void SgnMainFrame::ShowProgressBar()
{
	SendActionDataOnly(Action::ShowProgressBar);
}

//--------------------------------------------------------------------------------------------------

void SgnMainFrame::AddTraceLog(CString log)
{
	Json::Object data;
	ConstructData(data, Sgn::ETarget::DebugTracer, Action::AddTraceLog);

	data.SetString(SKW_VALUE, log);

	theWrapper.SendData(data);
}



void SgnMainFrame::AddTraceLogV(const wchar_t* pFormat, ...)
{
	CString stream;
	va_list argList;

	va_start(argList, pFormat);
	stream.FormatV(pFormat, argList);
	va_end(argList);

	AddTraceLog(stream);
}



void SgnMainFrame::ClearTraceLog()
{
	Json::Object data;
	ConstructData(data, Sgn::ETarget::DebugTracer, Action::ClearTraceLog);

	theWrapper.SendData(data);
}



void SgnMainFrame::SaveTraceLog(const wchar_t* pPath, bool saveAndClear)
{
	Json::Object data;
	ConstructData(data, Sgn::ETarget::DebugTracer, Action::SaveTraceLog);

	if (pPath != nullptr) {
		data.SetString(SKW_VALUE, pPath);
	}
	data.SetBoolean(SKW_CLEAR, saveAndClear);

	theWrapper.SendData(data);
}

//--------------------------------------------------------------------------------------------------

void SgnMainFrame::ShowStatusMessage(CString message)
{
	Json::Object data;
	ConstructData(data, Sgn::ETarget::StatusBar, Action::ShowStatusMessage);

	data.SetString(SKW_MESSAGE, message);

	theWrapper.SendData(data);
}



void SgnMainFrame::ShowStatusCoordinate(double x, double y)
{
	Json::Object data;
	ConstructData(data, Sgn::ETarget::StatusBar, Action::ShowStatusCoordinate);

	data.SetReal(SKW_X, x);
	data.SetReal(SKW_Y, y);
	//:WARNING - post
	theWrapper.PostData(data);
}



void SgnMainFrame::ShowStatusCoordinate(double x, double y, double z)
{
	Json::Object data;
	ConstructData(data, Sgn::ETarget::StatusBar, Action::ShowStatusCoordinate);

	data.SetReal(SKW_X, x);
	data.SetReal(SKW_Y, y);
	data.SetReal(SKW_Z, z);
	//:WARNING - post
	theWrapper.PostData(data);
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region Command Class

void SgnCommand::ConstructData(Json::Object& data, Action action)
{
	data.SetInteger(SKW_TARGET, (int)Sgn::ETarget::Command);
	data.SetInteger(SKW_ACTION, (int)action);
}



void SgnCommand::OnRequestPreference()
{
	SendActionDataOnly(Action::OnRequestPreference);
}



void SgnCommand::OnRequestFileOption()
{
	SendActionDataOnly(Action::OnRequestFileOption);
}



void SgnCommand::ResponsePreference(Json::Object& value, Json::Object& defaultValue)
{
	Json::Object data;
	ConstructData(data, Action::ResponsePreference);

	//:WARNING
	data.SetInteger(SKW_VIEWID, -1);
	data.SetObject(SKW_VALUE, new Json::Object(value));
	data.SetObject(SKW_DEFAULTVALUE, new Json::Object(defaultValue));

	theWrapper.SendData(data);
}



void SgnCommand::ResponseFileOption(Json::Object& value, Json::Object& defaultValue)
{
	Json::Object data;
	ConstructData(data, Action::ResponseFileOption);

	//:WARNING
	data.SetInteger(SKW_VIEWID, -1);
	data.SetObject(SKW_VALUE, new Json::Object(value));
	data.SetObject(SKW_DEFAULTVALUE, new Json::Object(defaultValue));

	theWrapper.SendData(data);
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region View Class

void SgnView::ConstructData(Json::Object& data, Action action)
{
	data.SetInteger(SKW_TARGET, (int)Sgn::ETarget::View);
	data.SetInteger(SKW_ACTION, (int)action);
	data.SetInteger(SKW_VIEWID, theWrapper.ViewId);
}



void SgnView::ConstructMouseData(Json::Object& data, Action action, UINT flags, int x, int y)
{
	ConstructData(data, action);

	data.SetInteger(SKW_FLAG, flags);
	data.SetInteger(SKW_X, x);
	data.SetInteger(SKW_Y, y);
}



void SgnView::ConstructWheelData(Json::Object& data, UINT flags, short delta, int x, int y)
{
	ConstructMouseData(data, Action::OnMouseWheel, flags, x, y);

	data.SetInteger(SKW_DELTA, delta);
}



void SgnView::ConstructKeyData(Json::Object& data, Action action, UINT chr, UINT repeat, UINT flags)
{
	ConstructData(data, action);

	data.SetInteger(SKW_CHAR, chr);
	data.SetInteger(SKW_FLAG, flags);
}



void SgnView::OnConstruct()
{
	SendActionDataOnly(Action::OnConstruct);
}



void SgnView::OnDestruct()
{
	SendActionDataOnly(Action::OnDestruct);
}



void SgnView::OnInitialize(DWORD_PTR hWnd, CString path)
{
	Json::Object data;
	ConstructData(data, Action::OnInitialize);

	data.SetDwordPtr(SKW_HWND, hWnd);
	data.SetString(SKW_FILEPATH, path);

	theWrapper.SendData(data);
}



void SgnView::OnCommand(UINT id)
{
	Json::Object data;
	ConstructData(data, Action::OnCommand);

	data.SetInteger(SKW_ID, id);

	theWrapper.SendData(data);
}



void SgnView::OnContextCommand(UINT id)
{
	Json::Object data;
	ConstructData(data, Action::OnContextCommand);

	data.SetInteger(SKW_ID, id);

	theWrapper.SendData(data);
}



void SgnView::OnCancel()
{
	SendActionDataOnly(Action::OnCancel);
}

#define SendMouseData(action) \
Json::Object data; \
ConstructMouseData(data, action, flags, x, y); \
theWrapper.SendData(data);

void SgnView::OnMouseMove(UINT flags, int x, int y)
{
	SendMouseData(Action::OnMouseMove);
}



void SgnView::OnLButtonDown(UINT flags, int x, int y, int osnapId)
{
	Json::Object data;
	ConstructMouseData(data, Action::OnLButtonDown, flags, x, y);

	data.SetInteger(SKW_OSNAPID, osnapId);

	theWrapper.SendData(data);
}



void SgnView::OnLButtonUp(UINT flags, int x, int y)
{
	SendMouseData(Action::OnLButtonUp);
}



void SgnView::OnMButtonDown(UINT flags, int x, int y)
{
	SendMouseData(Action::OnMButtonDown);
}



void SgnView::OnMButtonUp(UINT flags, int x, int y)
{
	SendMouseData(Action::OnMButtonUp);
}



void SgnView::OnRButtonDown(UINT flags, int x, int y)
{
	SendMouseData(Action::OnRButtonDown);
}



void SgnView::OnRButtonUp(UINT flags, int x, int y)
{
	SendMouseData(Action::OnRButtonUp);
}



void SgnView::OnMouseWheel(UINT flags, short delta, int x, int y)
{
	Json::Object data;
	ConstructWheelData(data, flags, delta, x, y);

	theWrapper.SendData(data);
}



void SgnView::OnMouseWheel(UINT flags, short delta, int x, int y, int left, int top, int right, int bottom)
{
	Json::Object data;
	ConstructWheelData(data, flags, delta, x, y);

	Json::Array& rect = data.CreateArray(SKW_RECT);
	rect.AddInteger(left);
	rect.AddInteger(top);
	rect.AddInteger(right);
	rect.AddInteger(bottom);

	theWrapper.SendData(data);
}

#undef SendMouseData

void SgnView::OnPaint()
{
	SendActionDataOnly(Action::OnPaint);
}



void SgnView::OnPaint(int left, int top, int right, int bottom)
{
	Json::Object data;
	ConstructData(data, Action::OnPaint);

	Json::Array& rect = data.CreateArray(SKW_RECT);
	rect.AddInteger(left);
	rect.AddInteger(top);
	rect.AddInteger(right);
	rect.AddInteger(bottom);

	theWrapper.SendData(data);
}



void SgnView::OnResize(int x, int y)
{
	Json::Object data;
	ConstructData(data, Action::OnResize);

	data.SetInteger(SKW_X, x);
	data.SetInteger(SKW_Y, y);

	theWrapper.SendData(data);
}



void SgnView::OnInput(CString value, int row, int column)
{
	Json::Object data;
	ConstructData(data, Action::OnInput);

	data.SetString(SKW_VALUE, value);
	data.SetInteger(SKW_ROW, row);
	data.SetInteger(SKW_COLUMN, column);

	theWrapper.SendData(data);
}

#define SendKeyData(action) \
Json::Object data; \
ConstructKeyData(data, action, chr, repeat, flags); \
theWrapper.SendData(data);

void SgnView::OnChar(UINT chr, UINT repeat, UINT flags)
{
	SendKeyData(Action::OnChar);
}



void SgnView::OnKeyDown(UINT chr, UINT repeat, UINT flags)
{
	SendKeyData(Action::OnKeyDown);
}



void SgnView::OnKeyUp(UINT chr, UINT repeat, UINT flags)
{
	SendKeyData(Action::OnKeyUp);
}

#undef SendKeyData

void SgnView::CancelCommand(UINT id)
{
	Json::Object data;
	ConstructData(data, Action::CancelCommand);

	data.SetInteger(SKW_ID, id);

	theWrapper.SendData(data);
}



void SgnView::CompleteCommand(UINT id)
{
	Json::Object data;
	ConstructData(data, Action::CompleteCommand);

	data.SetInteger(SKW_ID, id);

	theWrapper.SendData(data);
}



void SgnView::SetValidation(bool success)
{
	Json::Object data;
	ConstructData(data, Action::SetValidation);

	data.SetBoolean(SKW_VALID, success);

	theWrapper.SendData(data);
}



void SgnView::PaintOverlap()
{
	//:WARNING - post
	//SendActionDataOnly(Action::PaintOverlap);
	PostActionDataOnly(Action::PaintOverlap);
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region TaskBar Class

void SgnTaskBar::ConstructData(Json::Object& data, Action action)
{
	data.SetInteger(SKW_TARGET, (int)Sgn::ETarget::TaskBar);
	data.SetInteger(SKW_ACTION, (int)action);
	data.SetInteger(SKW_VIEWID, theWrapper.ViewId);
}



void SgnTaskBar::OnChangedValue(UINT commandId, Json::Object& value)
{
	Json::Object data;
	ConstructData(data, Action::OnChangedValue);

	data.SetInteger(SKW_ID, commandId);
	data.SetObject(SKW_VALUE, new Json::Object(value));

	theWrapper.SendData(data);
}

void SgnTaskBar::OnChangedValue(UINT commandId, CString key, Json::Value& value)
{
	Json::Object data;
	ConstructData(data, Action::OnChangedValue);

	data.SetInteger(SKW_ID, commandId);
	data.SetString(SKW_KEY, key);
	data.SetValue(SKW_VALUE, new Json::Value(value));

	theWrapper.SendData(data);
}



void SgnTaskBar::OnClickedValue(UINT commandId, CString key)
{
	Json::Object data;
	ConstructData(data, Action::OnChangedValue);

	data.SetInteger(SKW_ID, commandId);
	data.SetString(SKW_KEY, key);

	theWrapper.SendData(data);
}



void SgnTaskBar::OnRequestValue(UINT commandId)
{
	Json::Object data;
	ConstructData(data, Action::OnRequestValue);

	data.SetInteger(SKW_ID, commandId);

	theWrapper.SendData(data);
}



void SgnTaskBar::ResponseValue(UINT commandId, Json::Object& value, Json::Object& defaultValue)
{
	Json::Object data;
	ConstructData(data, Action::ResponseValue);

	data.SetInteger(SKW_ID, commandId);
	data.SetObject(SKW_VALUE, new Json::Object(value));
	data.SetObject(SKW_DEFAULTVALUE, new Json::Object(defaultValue));

	theWrapper.SendData(data);
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region UserIO Class

#define SendStringValue(action) \
Json::Object data; \
ConstructData(data, action); \
data.SetString(SKW_VALUE, value); \
theWrapper.SendData(data)

void SgnUserIO::ConstructData(Json::Object& data, Action action)
{
	data.SetInteger(SKW_TARGET, (int)Sgn::ETarget::UserIO);
	data.SetInteger(SKW_ACTION, (int)action);
	data.SetInteger(SKW_VIEWID, theWrapper.ViewId);
}



void SgnUserIO::OnInput(const CString& value)
{
	SendStringValue(Action::OnInput);
}



void SgnUserIO::SetDynamicInput(const Json::Object& value)
{
	Json::Object data;
	ConstructData(data, Action::SetDynamicInput);

	data.SetObject(SKW_OPTIONS, new Json::Object(value));
	//:WARNING - post
	theWrapper.PostData(data);
}



void SgnUserIO::StandbyCommand(CString prompt)
{
	ASSERT(prompt.IsEmpty() == false);

	Json::Object data;
	ConstructData(data, Action::StandbyCommand);

	data.SetString(SKW_PROMPT, prompt);

	theWrapper.SendData(data);
}



void SgnUserIO::PutCommand(CString value)
{
	ASSERT(value.IsEmpty() == false);

	Json::Object data;
	ConstructData(data, Action::PutCommand);

	data.SetString(SKW_COMMAND, value);

	theWrapper.SendData(data);
}



void SgnUserIO::PutPrompt(CString prompt, CString keyword, int options)
{
	ASSERT(prompt.IsEmpty() == false);

	Json::Object data;
	ConstructData(data, Action::PutPrompt);

	data.SetString(SKW_PROMPT, prompt);
	data.SetString(SKW_KEYWORD, keyword);
	data.SetInteger(SKW_OPTIONS, options);

	theWrapper.SendData(data);
}



void SgnUserIO::PutError(const CString& value)
{
	ASSERT(value.IsEmpty() == false);

	Json::Object data;
	ConstructData(data, Action::PutError);

	data.SetString(SKW_VALUE, value);

	theWrapper.SendData(data);
}



void SgnUserIO::PutEcho(const CString& value)
{
	ASSERT(value.IsEmpty() == false);

	Json::Object data;
	ConstructData(data, Action::PutEcho);

	data.SetString(SKW_VALUE, value);

	theWrapper.SendData(data);
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region Delivery Class

SgnDelivery2d::SgnDelivery2d()
{
#define SetWrapper(className) className.WrapperPtr = this

	SetWrapper(Application);
	SetWrapper(MainFrame);
	SetWrapper(View);
	SetWrapper(TaskBar);
	SetWrapper(Command);
	SetWrapper(UserIO);

#undef SetWrapper
}



SgnDelivery2d::SgnDelivery2d(int viewId, void(*sender)(const wchar_t*))
	: ViewId(viewId)
	, SendSignal(sender)
{
}



SgnDelivery2d::~SgnDelivery2d()
{
}



void SgnDelivery2d::SetSender(void (*func)(const wchar_t*))
{
	SendSignal = func;
}



void SgnDelivery2d::SendData(Json::Object& data)
{
	if (SendSignal != nullptr) {
		SendSignal(data.ToString().GetBuffer());
	}
	else {
		DEBUG_STOP;
	}
}

//**************************************************************************************************

#pragma warning(disable : 4996)

void SgnDelivery2d::PostData(Json::Object& data)
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

	//:WAIT
	//theThreadPool.Enqueue([this, pData] {
	//	if (SendSignal != nullptr) {
	//		SendSignal(pData);
	//	}

	//	delete[] pData;
	//});
}

#pragma endregion // REGION

#undef SendActionDataOnly
#undef PostActionDataOnly
#undef theWrapper
