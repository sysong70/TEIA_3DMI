#pragma once

namespace Window
{
	class Application; // CBCGPWinApp
	class ChildFrame; // CBCGPMDIChildWnd
	class Document; // CDocument
	class MainFrame; // CBCGPMDIFrameWnd
	class View; // CView



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