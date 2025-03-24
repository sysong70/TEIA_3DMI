#pragma once

#include <Json.h>
#include <vector>

/// Delivery Keywords

#define SKW_ACTION			"Action"
#define SKW_ANGLE			"Angle"
#define SKW_CHAR			"Char"
#define SKW_CHECKED			"checked" // lower case
#define SKW_CHILDREN		"Children"
#define SKW_CLEAR			"Clear"
#define SKW_COLUMN			"Column"
#define SKW_COMMAND			"Command"
#define SKW_DEFAULTVALUE	"Default"
#define SKW_DELTA			"Delta"
#define SKW_DESCRIPTION		"Description"
#define SKW_DPISCALE		"DpiScale"
#define SKW_ECHO			"Echo"
#define SKW_ERROR			"Error"
#define SKW_ERRORMESSAGE	"ErrorMessage"
#define SKW_EXPAND			"Expand"
#define SKW_FILEPATH		"FilePath"
#define SKW_FLAG			"Flag"
#define SKW_GLOBALNAME		"GlobalName"
#define SKW_GROUPNAME		"GroupName"
#define SKW_HASCHILDREN		"HasChildren"
#define SKW_HWND			"HWND"
#define SKW_ID				"Id"
#define SKW_ITEMS			"Items"
#define SKW_KEY				"Key"
#define SKW_KEYWORD			"Keyword"
#define SKW_LENGTH			"Length"
#define SKW_MAX				"Max"
#define SKW_MESSAGE			"Message"
#define SKW_MIN				"Min"
#define SKW_MODE			"Mode"
#define SKW_OPTIONS			"Options"
#define SKW_OSNAPID			"OsnapId"
#define SKW_PARENT			"Parent"
#define SKW_POSITION		"Position"
#define SKW_PROMPT			"Prompt"
#define SKW_RECT			"Rect"
#define SKW_REPCNT			"RepCnt"
#define SKW_ROW				"Row"
#define SKW_SHOW			"Show"
#define SKW_STATUS			"Status"
#define SKW_TARGET			"Target"
#define SKW_TITLE			"Title"
#define SKW_TOOLTIP			"Tooltip"
#define SKW_TYPE			"Type"
#define SKW_VALID			"Valid"
#define SKW_VALUE			"Value"
#define SKW_VIEWID			"ViewId"
#define SKW_X				"x"  // lower case
#define SKW_Y				"y"  // lower case
#define SKW_Z				"z"  // lower case

//--------------------------------------------------------------------------------------------------
// UserIO - use upper case

#define KEY_CANCEL		L"^C"
#define PRE_KEYWORD		L'&'
#define PRE_OSNAP		L'_'
#define PRE_RELATIVE	L'#'

#define PRE_FILTERANGLE		L'<'
#define PRE_FILTERLENGTH	L'@'
#define PRE_FILTERX			L".x"
#define PRE_FILTERY			L".y"

//--------------------------------------------------------------------------------------------------

class SgnApplication;
class SgnMainFrame;
class SgnView;
class SgnTaskBar;
class SgnCommand;

namespace Sgn
{
	enum class ETarget
	{
		Unknown = -1,

		Application,
		Command,
		MainFrame,
			DebugTracer,
			ProgressBar,
			StatusBar,
		TaskBar,
		UserIO,
		View,
	};

	enum class EStatus
	{
		Unknown = -1,

		Succeed,  // LightGray
		Info,     // Green
		Warning,  // Yellow
		Fail,     // Red
	};
}

//--------------------------------------------------------------------------------------------------

class SgnDelivery2d;

#define DefineConstructor(className) \
friend class Delivery; \
Sgn##className() {}; \
~Sgn##className() {}; \
SgnDelivery2d* WrapperPtr = nullptr

//--------------------------------------------------------------------------------------------------

class SgnApplication
{
public:

	DefineConstructor(Application);

	enum class Action
	{
		Unknown = -1,

		OnInitInstance,
		OnExitInstance,
		OnDpiAware,
		OnUpdatePreference,
		OnUpdateFileOption,
		OnFileOptionReference,
	};

	void ConstructData(Json::Object& data, Action action);

public:

	void OnInitInstance();

	void OnExitInstance();

	void OnDpiAware(double scale);

	void OnUpdatePreference(Json::Object& value);

	void OnUpdateFileOption(Json::Object& value);

	void OnFileOptionReference(Json::Object& value);
};

//--------------------------------------------------------------------------------------------------

class SgnMainFrame
{
public:

	DefineConstructor(MainFrame);

	enum class Action
	{
		Unknown = -1,

		AddProgressLog,
		ClearProgressLog,
		HideProgressBar,
		SetProgressMessage,
		SetProgressPosition,
		SetProgressRange,
		SetProgressStatus,
		ShowProgressBar,

		AddTraceLog,
		ClearTraceLog,
		SaveTraceLog,

		ShowStatusCoordinate,
		ShowStatusMessage,

		PaintIo
	};

	void ConstructData(Json::Object& data, Action action);

	void ConstructData(Json::Object& data, Sgn::ETarget target, Action action);

public:

	void AddProgressLog(Sgn::EStatus status, CString title, CString description = L"", CString tooltip = L"");

	void ClearProgressLog();

	void HideProgressBar();

	void SetProgressMessage(CString message);

	void SetProgressPosition(int pos);

	void SetProgressRange(int min = 0, int max = 100);
	// change last item color by status
	void SetProgressStatus(Sgn::EStatus status);

	void ShowProgressBar();

public:

	// Single log
	void AddTraceLog(CString log);
	// formatted log (like TRACE)
	void AddTraceLogV(const wchar_t* pFormat, ...);

	void ClearTraceLog();
	// pPath or NULL(show file dialog)
	void SaveTraceLog(const wchar_t* pPath, bool saveAndClear = true);

public:

	void ShowStatusMessage(CString message);

	void ShowStatusCoordinate(double x, double y);

	void ShowStatusCoordinate(double x, double y, double z);
};

//--------------------------------------------------------------------------------------------------

class SgnCommand
{
public:

	DefineConstructor(Command);

	enum class Action
	{
		Unknown = -1,

		OnRequestPreference,
		OnRequestFileOption,

		ResponsePreference,
		ResponseFileOption,
	};

	void ConstructData(Json::Object& data, Action action);

public:

	void OnRequestPreference();

	void OnRequestFileOption();

public:

	void ResponsePreference(Json::Object& value, Json::Object& defaultValue);

	void ResponseFileOption(Json::Object& value, Json::Object& defaultValue);
};

//--------------------------------------------------------------------------------------------------

class SgnView
{
public:

	DefineConstructor(View);

	enum class Action
	{
		Unknown = -1,

		OnConstruct,  // CreateHpsView
		OnDestruct,   // ResetModelHandler, DeleteHpsView, DeleteModel
		OnInitialize, // CreateModelHandler, InitialUpdateHpsView

		OnCommand,
		OnContextCommand,
		OnCancel, // cancel command

		OnMouseMove,
		OnLButtonDown,
		OnLButtonUp,
		OnMButtonDown,
		OnMButtonUp,
		OnRButtonDown,
		OnRButtonUp,
		OnMouseWheel,
		OnPaint,
		OnResize,
		OnInput,

		OnChar,
		OnKeyDown,
		OnKeyUp,

		CancelCommand,
		CompleteCommand,

		PaintOverlap,	// complete OnPaint
		SetValidation,	// complete opening file
	};

	void ConstructData(Json::Object& data, Action action);

	void ConstructMouseData(Json::Object& data, Action action, UINT flags, int x, int y);

	void ConstructWheelData(Json::Object& data, UINT flags, short delta, int x, int y);

	void ConstructKeyData(Json::Object& data, Action action, UINT chr, UINT repeat, UINT flags);

public:

	void OnConstruct();

	void OnDestruct();

	void OnInitialize(DWORD_PTR hWnd, CString path = L"");

	// Command

	// id: enum Command
	void OnCommand(UINT id);

	void OnContextCommand(UINT id);

	void OnCancel();

	// Mouse

	void OnMouseMove(UINT flags, int x, int y);

	void OnLButtonDown(UINT flags, int x, int y, int osnapId = -1);

	void OnLButtonUp(UINT flags, int x, int y);

	void OnMButtonDown(UINT flags, int x, int y);

	void OnMButtonUp(UINT flags, int x, int y);

	void OnRButtonDown(UINT flags, int x, int y);

	void OnRButtonUp(UINT flags, int x, int y);

	void OnMouseWheel(UINT flags, short delta, int x, int y);

	void OnMouseWheel(UINT flags, short delta, int x, int y, int left, int top, int right, int bottom);

	void OnPaint();

	void OnPaint(int left, int top, int right, int bottom);

	void OnResize(int x, int y);

	void OnInput(CString value, int row, int column);

	// Keyboard

	void OnChar(UINT chr, UINT repeat, UINT flags);

	void OnKeyDown(UINT chr, UINT repeat, UINT flags);

	void OnKeyUp(UINT chr, UINT repeat, UINT flags);

public:

	void CancelCommand(UINT id);

	void CompleteCommand(UINT id);

	void SetValidation(bool success = true);

	void PaintOverlap();
};

//--------------------------------------------------------------------------------------------------

class SgnTaskBar
{
public:

	DefineConstructor(TaskBar);

	enum class Action
	{
		Unknown = -1,

		OnCancel,
		OnComplete,

		OnChangedValue,
		OnClickedValue,
		OnRequestValue,

		CompleteCommand,

		EnableValue,
		ResponseValue,
	};

	void ConstructData(Json::Object& data, Action action);

public:

	void OnCancel(UINT commandId);

	void OnComplete(UINT commandId, CString key);

public:

	// all data of property grid
	void OnChangedValue(UINT coomandId, Json::Object& value);
	// single data of property item
	void OnChangedValue(UINT commandId, CString key, Json::Value& value);

	void OnClickedValue(UINT commandId, CString key);

	void OnRequestValue(UINT commandId);

public:

	void EnableValue(UINT commandId, const std::vector<CString>& items, bool enable = true);

	void ResponseValue(UINT commandId, Json::Object& value, Json::Object& defaultValue);

	void SetValue(UINT commandId, Json::Object& value);
};

//--------------------------------------------------------------------------------------------------

class SgnUserIO
{
public:

	DefineConstructor(UserIO);

	enum class Action
	{
		Unknown = -1,

		OnInput,
		OnContextMenu, //:TODO

		PaintOverlap,
		PutCommand,
		PutPrompt, // and keyword
		PutEcho,
		PutError,
		SetDynamicInput,
		StandbyCommand,
	};

	void ConstructData(Json::Object& data, Action action);

public:

	void OnInput(const CString& value);

public:

	void PutCommand(CString value);

	void PutPrompt(CString prompt, CString keyword, int options);

	void PutError(const CString& value);

	void PutEcho(const CString& value);

	void SetDynamicInput(const Json::Object& value);

	void StandbyCommand(CString prompt);
};

//--------------------------------------------------------------------------------------------------

class SgnDelivery2d
{
public:

	friend class SgnApplication;
	friend class SgnMainFrame;
	friend class SgnView;
	friend class SgnTaskBar;
	friend class SgnCommand;

	SgnDelivery2d();

	SgnDelivery2d(int viewId, void (*sender)(const wchar_t*));

	virtual ~SgnDelivery2d();

public:

	int ViewId = -1;

	SgnApplication	Application;
	SgnMainFrame	MainFrame;
	SgnView			View;
	SgnTaskBar		TaskBar;
	SgnCommand		Command;
	SgnUserIO		UserIO;

	void (*SendSignal)(const wchar_t*) = nullptr;

	void SetSender(void (*func)(const wchar_t*));

	void SendData(Json::Object& data);

	void PostData(Json::Object& data);
};

#undef DefineWrapper
#undef DefineConstructor
