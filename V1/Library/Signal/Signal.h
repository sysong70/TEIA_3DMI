#pragma once

#include <Json.h>

/// Delivery Keywords

#define SKW_ACTION			"Action"
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

namespace Signal
{
	enum class EInputMode
	{
		Unknown = -1,

		Angle, // No negative
		Color,
		Integer,
		Length,
		Point2d,
		Point3d,
		Real,
		String,

		PlusInteger,
		PlusReal,
	};

	enum EInputControl
	{
		// Sets the "Accept Z coordinate" mode. Input is restricted to 2d input by default.
		Accept3dCoordinates = 0x0001,
		// Sets the "Use mouse up for points" mode. Mouse-up events do not register as points by default.
		AcceptMouseUpAsPoint = 0x0002,
		// Sets the "Accept non-keyword string input" mode. This mode is off by default.
		AcceptOtherInputString = 0x0004,
		// Sets the "Any blank terminates input" mode. This mode is off by default.
		AnyBlankTerminatesInput = 0x0008,
		// Sets the "Don't update last point" mode. The last point is updated by default.
		DoNotUpdateLastPoint = 0x0010,
		// Sets the "honor ORTHOMODE sysvar" bit of the user input request packet.
		// By default, this bit is not set.
		// Note that the Jig.acquirePoint(Point3d&, const Point3d& basePnt) method always honors the ORTHOMODE sysvar,
		// regardless of this bit's state.
		GovernedByOrthoMode = 0x0020,
		// Sets the "Initial blank terminates input" mode. This mode is off by default.
		InitialBlankTerminatesInput = 0x0040,
		// Sets the "Don't accept negative values" mode. Negative values are accepted by default.
		// This works for the functions acquireDist() and acquireAngle() only.
		NoNegativeResponseAccepted = 0x0080,
		// Sets the "Don't accept zero values" mode. Zero values are accepted by default.
		// This works for the functions acquireDist() and acquireAngle() only.
		NoZeroResponseAccepted = 0x0100,
		// Sets the "null input acceptable" bit of the user input request packet.
		// This bit is clear by default, and null input is not acceptable. 
		NullResponseAccepted = 0x0200,
	};

	enum class ETreeItem
	{
		Unknown = -1,

		Solid,
		Surface,
	};

	struct TreeItem
	{
		DWORD_PTR ParentKey = 0;
		DWORD_PTR Key = 0;
		CString Title;
		bool Checked = true;
		bool HasChildren = false;
		ETreeItem Type = ETreeItem::Unknown;
	};

	struct TreeItemStatus
	{
		DWORD_PTR Key = 0;
		bool Flag = true; // Multi-purpose (checked, selected, ...)
	};

	using TreeItems = std::vector<TreeItem>;
	using TreeItemStatuses = std::vector<TreeItemStatus>;
	using KeyItems = std::vector<DWORD_PTR>;

	enum class EMenuType
	{
		Unknown = -1,

		Normal,
		Seperator,
		OSnap,
		SelFilter,
	};

	struct MenuItem
	{
		EMenuType Type = EMenuType::Normal;
		int Id = -1;
		CString Title;
	};

	using MenuItems = std::vector<MenuItem>;

	using ContextItems = std::vector<int>;

	struct PropItem
	{
		CString Key;
		CString Value;
		CString Desc;

		bool UseOSnap = false;
		bool UseSelFilter = false;

		bool Enable = true;
		bool Show = true;
	};

	using PropItems = std::vector<PropItem>;
}

//--------------------------------------------------------------------------------------------------

#define CHILD_CONSTRUCTOR(className) \
friend class Delivery; \
className() {}; \
~className() {}

#define DEFINE_WRAPPER \
private: \
Delivery* m_pWrapper = nullptr; \
Delivery& Wrapper() { return *m_pWrapper; } \
public:

//--------------------------------------------------------------------------------------------------

namespace Signal
{
	enum class Target
	{
		Unknown = -1,

		Application,
		MainFrame,
		StatusBar,

		View,
		ModelPanel,
		ViewPanel,
		LayerPanel,
		ScenePanel,
		TaskBar,

		Progress,
		Command,
		UserIO,
		DebugTracer, // TEMP
	};

//--------------------------------------------------------------------------------------------------

	class Application
	{
	public:

		CHILD_CONSTRUCTOR(Application);

		enum class Action
		{
			Unknown = -1,

			OnInitInstance,
			OnExitInstance,
			OnDpiAware,
			OnUpdatePreference,
			OnUpdateFileOption,
			OnFileOptionReference,

			AddTraceLog,
			ClearTraceLog,
			SaveTraceLog,
		};

		DEFINE_WRAPPER;

		void ConstructData(Json::Object& data, Action action);

	public:

		void OnInitInstance();

		void OnExitInstance();

		void OnDpiAware(double scale);

		void OnUpdatePreference(Json::Object& value);

		void OnUpdateFileOption(Json::Object& value);

		void OnFileOptionReference(Json::Object& value);

	public:

		// Single log
		void AddTraceLog(CString log);
		// formatted log (like TRACE)
		void AddTraceLogV(const wchar_t* pFormat, ...);

		void ClearTraceLog();
		// pPath or NULL(show file dialog)
		void SaveTraceLog(const wchar_t* pPath, bool saveAndClear = true);
	};

//--------------------------------------------------------------------------------------------------

	class MainFrame
	{
	public:

		CHILD_CONSTRUCTOR(MainFrame);

		enum class Action
		{
			Unknown = -1,

			ShowNotice,
			ShowProgress,
			HideProgress,
		};

		DEFINE_WRAPPER;

		void ConstructData(Json::Object& data, Action action);

	public:

		// TODO
		void ShowNotice();

		void ShowProgress();

		void HideProgress();
	};

// TODO
//--------------------------------------------------------------------------------------------------

	class StatusBar
	{
	public:

		CHILD_CONSTRUCTOR(StatusBar);

		enum class Action
		{
			Unknown = -1,

			ShowMessage,
			ShowCoordinate,
		};

		DEFINE_WRAPPER;

		void ConstructData(Json::Object& data, Action action);

	public:

		void ShowMessage(CString message);

		void ShowCoordinate(double x, double y);

		void ShowCoordinate(double x, double y, double z);
	};

//--------------------------------------------------------------------------------------------------
	/*
		Delivery delivery;
		delivery.mainFrame.ShowProgress();
		delivery.progress.SetRange(0, 3);
		delivery.progress.SetMessage(L"FileName")

		delivery.progress.SetPosition(1);
		delivery.progress.AddLog(Signal::Progress::Status::Succeed, L"Start reading file");
		// ...
		delivery.progress.SetPosition(2);
		delivery.progress.AddLog(Signal::Progress::Status::Succeed, L"Create rendering device");
		// ...

		if (success) {
			delivery.mainFrame.HideProgress();
			delivery.view.SetValidation();
		}
		else {
			delivery.progress.ClearLog();
			delivery.progress.AddLog(Signal::Progress::Status::Fail, L"Error message");
		}
	*/

	class Progress
	{
	public:

		CHILD_CONSTRUCTOR(Progress);

		enum class Action
		{
			Unknown = -1,

			SetRange,
			SetPosition,
			SetMessage,
			AddLog,
			SetLogStatus,
			ClearLog,
		};

		enum class Status
		{
			Unknown = -1,

			Succeed,  // LightGray
			Info,     // Green
			Warning,  // Yellow
			Fail,     // Red
		};

		DEFINE_WRAPPER;

		void ConstructData(Json::Object& data, Action action);

	public:

		void SetRange(int min = 0, int max = 100);

		void SetPosition(int pos);

		void SetMessage(CString message);

		void AddLog(Status status, CString title, CString description = L"", CString tooltip = L"");
		// change last item color by status
		void SetLogStatus(Status status);

		void ClearLog();
	};

//--------------------------------------------------------------------------------------------------

	class Command
	{
	public:

		CHILD_CONSTRUCTOR(Command);

		enum class Action
		{
			Unknown = -1,

			OnRequestPreference,
			OnRequestFileOption,

			ResponsePreference,
			ResponseFileOption,
		};

		DEFINE_WRAPPER;

		void ConstructData(Json::Object& data, Action action);

	public:

		void OnRequestPreference();

		void OnRequestFileOption();

	public:

		void ResponsePreference(Json::Object& value, Json::Object& defaultValue);

		void ResponseFileOption(Json::Object& value, Json::Object& defaultValue);
	};

//--------------------------------------------------------------------------------------------------

	class View
	{
	public:

		CHILD_CONSTRUCTOR(View);

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

			SetValidation, // complete opening file
			PaintOverlap, // complete OnPaint
			SetInputMode,
			SetContextMenu,
			ShowContextMenu,
			ShowInputBox,
		};

		DEFINE_WRAPPER;

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
		// TEMP:
		void PaintOverlap();

		void SetInputMode(EInputMode mode);

		void SetContextMenu(MenuItems& menus, bool show = false);

		void ShowContextMenu(ContextItems& ids);

		void ShowInputBox(EInputMode mode, CString prompt, CString errorMessage);
	};

//--------------------------------------------------------------------------------------------------

	class ModelPanel
	{
	public:

		CHILD_CONSTRUCTOR(ModelPanel);

		enum class Action
		{
			Unknown = -1,

			//OnBeginDrag,
			//OnBeginLabelEdit,
			//OnEndLabelEdit,
			//OnClick
			OnItemDblClicked,	// OnDblClick,
			OnItemDeleted,		// OnDeleteItem,
			OnItemChecked,
			OnItemExpanded,
			OnItemExpanding,
			OnItemRClicked,		// OnRClick,
			OnItemRDbCliced,	// OnRDbClick
			//OnSetFocus,
			OnItemSelected,		// OnSelChanged
			OnItemShow,			// OnClick
			//OnSelChanging,

			RedrawTree,

			AddItem,
			AddChildren,
			CheckItem,
			CheckItems,
			CollapseItem,
			DeleteItem,
			ExpandItem,
			ExpandParent,
			InverseCheckedStatus,
			SelectItem,
			SelectItems,
			ViewItem,
		};

		DEFINE_WRAPPER;

		void ConstructData(Json::Object& data, Action action);

	public:

		// select item?
		void OnItemChecked(DWORD_PTR key, bool checked);

		void OnItemDblClicked(DWORD_PTR key);

		void OnItemDeleted(DWORD_PTR key);

		void OnItemExpanded(DWORD_PTR key);

		void OnItemSelected(DWORD_PTR key);

		void OnItemShow(DWORD_PTR key, bool show);

	public:

		void RedrawTree(bool value);

	public:

		void AddItem(TreeItem& item);
		// ignore TreeItem.Parent
		void AddChildren(DWORD_PTR parentKey, TreeItems& items, bool expand = true);

		void CheckItem(DWORD_PTR key, bool checked);

		void CheckItems(const KeyItems& items, bool checked);

		void CheckItems(const TreeItemStatuses& items);

		void DeleteItem(DWORD_PTR key);
		// expand or collapse
		void ExpandItem(DWORD_PTR key, bool expand = true);
		// expand from root to item
		void ExpandParent(DWORD_PTR key);

		void InverseCheckedStatus();

		void SelectItem(DWORD_PTR key, bool select = true);

		void SelectItems(const KeyItems& items, bool select = true);
		// ensure visiable
		void ViewItem(DWORD_PTR key);
	};

//--------------------------------------------------------------------------------------------------

	class TaskBar
	{
	public:

		CHILD_CONSTRUCTOR(TaskBar);

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
			SetValueState,
			ShowValue,
			UpdateValue,
		};

		DEFINE_WRAPPER;

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

		void SetValueState(UINT commandId, const PropItems& items);

		void ShowValue(UINT commandId, const std::vector<CString>& items, bool show = true);

		void UpdateValue(UINT commandId, Json::Object& value);

		void UpdateValue(UINT commandId, CString key, Json::Value& value);
	};

//--------------------------------------------------------------------------------------------------

	class UserIO
	{
	public:

		CHILD_CONSTRUCTOR(UserIO);

		enum class Action
		{
			Unknown = -1,

			OnInput,
			OnContextMenu,

			PutCommand,
			PutPrompt, // and keyword
			InputError,
			InputEcho,
		};

		DEFINE_WRAPPER;

		void ConstructData(Json::Object& data, Action action);

	public:

		void OnInput(const CString& value);

	public:

		void PutCommand(CString command, CString prompt, CString keyword = L"");

		void PutPrompt(CString prompt, CString keyword = L"");

		void InputError(const CString& value);

		void InputEcho(const CString& value);
	};

//--------------------------------------------------------------------------------------------------

	class Delivery
	{
	public:

		friend class Application;
		friend class MainFrame;
		friend class Progress;
		friend class View;
		friend class ModelPanel;
		friend class TaskBar;
		friend class Command;

		Delivery();

		Delivery(int viewId, void (*sender)(const wchar_t*));

		virtual ~Delivery();

	public:

		int ViewId = -1;

		Application application;
		MainFrame mainFrame;
		StatusBar statusBar;
		Progress progress;
		View view;
		ModelPanel modelPanel;
		TaskBar taskBar;
		Command command;
		UserIO userIO;

		void (*SendSignal)(const wchar_t*) = nullptr;

		void SetSender(void (*func)(const wchar_t*));

		void SendData(Json::Object& data);

		void PostData(Json::Object& data);
	};
};

#undef DEFINE_WRAPPER
#undef CHILD_CONSTRUCTOR
