#pragma once

#include "Ctl.h"

class ClsUserIoManager;
class WndView;

//--------------------------------------------------------------------------------------------------

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

class CtlCommandBar : public CWnd
{
public:

	CBCGPStatic CommandCtl;
	CBCGPStatic PromptCtl;
	CBCGPMultiLinkCtrl KeywordCtl;
	CBCGPEdit InputCtl;

	ClsUserIoManager& UioManager;

public:

	CtlCommandBar(ClsUserIoManager& manager)
		: UioManager(manager)
	{}

	~CtlCommandBar() override { DestroyWindow(); }

public:

	bool Initialize(CWnd* pParentWnd);

	void AdjustLayout();

public:

	// Not window event
	bool OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	// Not window event
	bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	BOOL PreTranslateMessage(MSG* pMsg) override;

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	// From CBCGPMultiLinkCtrl
	afx_msg void OnKeyword(NMHDR* pNmhdr, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

public:

	bool SetCommand();

	bool SetPrompt();

	bool SetEcho(const CString& value);

	bool SetError(const CString& value);

	bool StandbyCommand(const CString& prompt);
};
