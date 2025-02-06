#pragma once

#include "Component.h"
#include "Window.h"

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

	class CommandBar : public CWnd
	{
		friend class Window::MainFrame;

		CBCGPStatic m_wndCommand;
		CBCGPStatic m_wndPrompt;
		CBCGPMultiLinkCtrl m_wndKeyword;
		CBCGPEdit m_wndInput;

		struct Data
		{
			CString Command;	// "LINE "
			CString Prompt;		// "Specify next point or "
			CString Keyword;	// [ Undo(U) Close(C) ]:
			WStringArray Keywords;

			void Clear();

			bool FindKeyword(const CString& value);
		}
		m_data;

		Window::View* m_pView = nullptr;

	public:

		CommandBar();

		~CommandBar() override;

		bool Initialize(CWnd* pParentWnd);

		void ReceiveSignal(Json::Object* pData);

		void ViewChanged(Window::View* pView);

		CSize AdjustLayout();

	protected:

		BOOL PreTranslateMessage(MSG* pMsg) override;

		afx_msg BOOL OnEraseBkgnd(CDC* pDC);

		afx_msg void OnKeyword(NMHDR* pNmhdr, LRESULT* pResult);

		DECLARE_MESSAGE_MAP()

	private:

		bool PutCommand(Json::Object& data);

		bool PutPrompt(Json::Object& data);

		bool PutEcho(Json::Object& data);

		bool PutError(Json::Object& data);

		bool StandbyCommand(Json::Object& data);;
	};
}

