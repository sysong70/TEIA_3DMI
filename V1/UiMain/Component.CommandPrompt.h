#pragma once

#include "Component.h"
#include "Window.h"

namespace Window
{
	class View;
}

//--------------------------------------------------------------------------------------------------

namespace Component
{
	// REF - https://help.autodesk.com/view/ACDLT/2025/ENU/?guid=GUID-D991386C-FBAA-4094-9FCB-AADD98ACD3EF
	/*
	* ^C – Represents pressing the Esc key.
	* . (period) – Instructs AutoCAD to use the standard definition of a command.
	*	The behavior of commands can be altered with programming languages like AutoLISP and ObjectARX.
	* _ (underscore) – Lets AutoCAD know the command or option provided is the global/English name.
	*	This is required for macros to work correctly across multiple languages.
	* ; (semi-colon) – Represents pressing the Enter key.
	* \ (backslash) – Represents a pause for user input;
	*	allows the user to enter a value, specify a point, or select objects.
	* (space) – Represents pressing the Spacebar key.
	*
	* command prompt: LINE Specify next point or [ Undo(U) Close(C) ]
	*/

	class DynamicInput : public CBCGPEdit
	{
		using Predefined = CStringArray;

		static Predefined Commands;
		static Predefined Keywords;

	public:

		friend class CommandPomppt;

		enum class InputMode
		{
			Command = 0x00000001,
			Keyword = 0x00000002,
			Point = 0x00000004, // Point value or OSnap overrides
			Select = 0x00000008,
			Integer = 0x00000010,
			Double = 0x00000020,
		};

		DynamicInput();

		~DynamicInput();

	private:

		int m_nViewId;
		InputMode m_eType = InputMode::Command;
		Predefined m_keywords;
		CString m_buffer;

	public:

		bool Initialize(CWnd* pParentWnd, UINT id = WM_USER, const RECT& rect = {});

		void StandBy(InputMode eType);

	public:

		void SetPrompt(const CString& value);

		void SetPrompt(double x, double y);

		void SetPrompt(double x, double y, double z);

	protected:

		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

		DECLARE_MESSAGE_MAP()

	protected:

		bool OnProcess();

		bool OnCommand();

		bool OnKeyword();

		bool OnPoint();

		bool OnSelect();

		bool OnNumberic();
	};



	class CommandPrompt : public CWnd
	{
		CBCGPStatic m_wndCommand;
		CBCGPStatic m_wndPrompt;
		CBCGPMultiLinkCtrl m_wndKeyword;
		CBCGPEdit m_wndInput;

		CString m_command;  // "LINE "
		CString m_prompt;	// "Specify next point or "
		// param - Undo(U)|취소(C) Close(U)|닫기(C)
		CString m_keyword;	// [ Undo(U) Close(C) ]:
		CString m_input; // or macro

	public:

		enum class InputMode
		{
			Any,
			Point,
			Length,
			Angle,
		};

		CommandPrompt();

		~CommandPrompt() override;

		bool Initialize(CWnd* pParentWnd);

		CSize AdjustLayout();

	public:

		// Run macro, from Ribbon UI, Context Menu, ...
		void Activate(CString macro);
		// from Renderer
		void Activate(Json::Object* pData);

		void Activate(CString command, CString prompt, CString keyword);
		// continue command and next step
		void Activate(CString prompt, CString keyword);

		void Cancel();

		bool Parse();
		// Type a command
		void Wait();

	protected:

		afx_msg BOOL OnEraseBkgnd(CDC* pDC);

		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

		DECLARE_MESSAGE_MAP()

	private:

		void ClearInput(bool wait = false);

		bool SetCommand(const CString& value);

		bool SetPrompt(const CString& value);

		bool SetKeyword(const CString& value);

	private:

		bool SetMacro(const CString& value);
	};
}

