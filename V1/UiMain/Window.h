#pragma once

//--------------------------------------------------------------------------------------------------

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

	enum class EDocType
	{
		Unknown = -1,
		Model,
		Drawing,
		Image,
		PDF,
	};

	enum class EViewType
	{
		Unknown = -1,
		View3d,
		View2d,
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



	bool IsAllowedFile(const wchar_t* pFilePath);

	bool IsAllowed3d(const wchar_t* pFilePath);

	bool IsAllowed2d(const wchar_t* pFilePath);
}
