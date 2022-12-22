#pragma once

namespace Window
{
	class Application;
	class ChildFrame;
	class Document;
	class MainFrame;
	class View;



	enum class AppMode
	{
		All = -1,
		Lite,
		Standard,
		Pro,
	};



	enum class StatusArea
	{
		Message = 0,
		Progress,
		Coordinate,
	};



	enum class UserMessage
	{
		Unknown = -1,
		OnSignal = WM_USER,
		OnNextFileOpen,
	};
}