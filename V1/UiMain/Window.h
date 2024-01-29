#pragma once



namespace Window
{
	// Main
	class Application;	// CBCGPWinApp
	class ChildFrame;	// CBCGPMDIChildWnd
	class MainFrame;	// CBCGPMDIFrameWnd
	// View
	class Document;		// CDocument
	class View;			// CView
	class View2d;		// Window::View
	class View3d;		// Window::View



	enum class EAppMode
	{
		All = -1,
		Lite,
		Standard,
		Pro,
	};



	enum class EStatusArea
	{
		Message = 0,
		Progress,
		Coordinate,
	};



	enum class EUserMessage
	{
		Unknown = -1,
		OnSignal = WM_USER,
		OnNextFileOpen,
	};
}