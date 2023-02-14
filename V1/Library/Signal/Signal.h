#pragma once

#include "Json.h"

/// Delivery Keywords

#define SKW_ACTION			"Action"
#define SKW_DATA			"Data"
#define SKW_Delivery		"Delivery"
#define SKW_DELTA			"Delta"
#define SKW_DESCRIPTION		"Description"
#define SKW_DOCID			"DocId"
#define SKW_EVENT			"Event"
#define SKW_FILEPATH		"FilePath"
#define SKW_FLAG			"Flag"
#define SKW_GLOBALNAME		"GlobalName"
#define SKW_GROUPNAME		"GroupName"
#define SKW_HASCHILDREN		"HasChildren"
#define SKW_HEIGHT			"Height"
#define SKW_HWND			"HWND"
#define SKW_ID				"Id"
#define SKW_INITIALIZE		"Initialize"
#define SKW_MESSAGE			"Message"
#define SKW_OPTION			"Option"
#define SKW_POSITION		"Position"
#define SKW_RECT			"Rect"
#define SKW_STATUS			"Status"
#define SKW_TARGET			"Target"
#define SKW_TITLE			"Title"
#define SKW_TOOLTIP			"Tooltip"
#define SKW_TYPE			"Type"
#define SKW_VALID			"Valid"
#define SKW_VIEWID			"ViewId"
#define SKW_WIDTH			"Width"
#define SKW_X				"x"
#define SKW_Y				"y"
#define SKW_Z				"z"



#define CHILD_CONSTRUCTOR(className) \
friend class Delivery; \
className() {}; \
~className() {}

#define DEFINE_WRAPPER \
private: \
Delivery* m_pWrapper = nullptr; \
Delivery& Wrapper() { return *m_pWrapper; } \
public:

namespace Signal
{
	class Application;
	class MainFrame;
	class Progress;
	class View;
	class ModelPanel;

	enum class Target
	{
		Unknown = -1,

		Application,
		MainFrame,
		StatusBar,
		View,

		ViewPanel,
		LayerPanel,
		ScenePanel,
		ModelPanel,
		TaskBar,

		Progress,
	};



	class Application
	{
	public:

		CHILD_CONSTRUCTOR(Application);

		enum class Action
		{
			Unknown = -1,

			OnInitInstance,
			OnExitInstance,
		};

		DEFINE_WRAPPER;

		void ConstructData(Json::Object& data, Action action);

	public:

		void OnInitInstance();

		void OnExitInstance();
	};



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

		//:TODO - message box
		void ShowNotice();

		void ShowProgress();

		void HideProgress();
	};

	//:TODO

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



	class Progress
	{
	public:

		CHILD_CONSTRUCTOR(Progress);

		enum class Action
		{
			Unknown = -1,

			StartMarquee,
			SetMessage,
			AddLog,
			SetLogStatus,
			StopMarquee,
		};

		enum class Status
		{
			Unknown = -1,

			Succeed,  // black
			Fail,     // red
		};

		DEFINE_WRAPPER;

		void ConstructData(Json::Object& data, Action action);

	public:

		void StartMarquee();

		void SetMessage(CString message);

		void AddLog(Status status, CString title, CString description = L"", CString tooltip = L"");
		// change last item color by status
		void SetLogStatus(Status status);

		void StopMarquee();
	};



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
			OnText,

			SetValidation, // complete opening file
		};

		DEFINE_WRAPPER;

		void ConstructData(Json::Object& data, Action action);

		void ConstructMouseData(Json::Object& data, Action action, UINT flags, int x, int y);

		void ConstructWheelData(Json::Object& data, UINT flags, short delta, int x, int y);

	public:

		void OnConstruct();

		void OnDestruct();

		void OnInitialize(DWORD_PTR hWnd, CString path = L"");
		// id: enum Command
		void OnCommand(UINT id);

		void OnMouseMove(UINT flags, int x, int y);
		void OnLButtonDown(UINT flags, int x, int y);
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

		void OnText(UINT flags, int x, int y);

	public:

		void SetValidation(bool success = true);
	};



	class ModelPanel
	{
	public:

		CHILD_CONSTRUCTOR(ModelPanel);

		enum class Action
		{
			Unknown = -1,

			OnBeginDrag,
			OnBeginLabelEdit,
			OnClick,
			OnDblClick,
			OnDeleteItem,
			OnEndLabelEdit,
			OnItemExpanded,
			OnItemExpanding,
			OnRClick,
			OnRDbClick,
			OnSelChanged,
			OnSelChanging,
			OnSetFocus,

			AddItem,
		};

		DEFINE_WRAPPER;

		void ConstructData(Json::Object& data, Action action);

	public:

		//:TODO
		void OnSelChanged();
		//:TODO
		void OnItemExpanded();

	public:

		void AddItem(...);
	};



	class Delivery
	{
	public:

		friend class Application;
		friend class MainFrame;
		friend class Progress;
		friend class View;
		friend class ModelPanel;

		Delivery();

		Delivery(int viewId, void (*sender)(const wchar_t*));

		~Delivery() {}

	public:

		int ViewId = -1;

		Application application;
		MainFrame mainFrame;
		StatusBar statusBar;
		Progress progress;
		View view;
		ModelPanel modelPanel;

		void (*SendSignal)(const wchar_t*) = nullptr;

		void SetSender(void (*func)(const wchar_t*));

		void SendData(Json::Object& data);
	};
};

#undef DEFINE_WRAPPER
#undef CHILD_CONSTRUCTOR
