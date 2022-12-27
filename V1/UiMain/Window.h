#pragma once

namespace Window
{
	class Application;
	class ChildFrame;
	class Document;
	class MainFrame;
	class View;



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