/*****************************************************************************
***
***    Copyright (C) 2014-2024 IntelliCAD Technology Consortium. All Rights Reserved.
***     All rights reserved.
***
***     Use of the information contained herein, in part or in whole,
***     in/as source code and/or in/as object code, in any way by anyone
***     other than authorized employees of The IntelliCAD Technology Consortium,
***     or by anyone to whom The IntelliCAD Technology Consortium  has not
***     granted use is illegal.
***
***     Description:
***
*****************************************************************************/
#pragma once

#include "afxwin.h"
#include "afxdlgs.h"
#include "DbDimension.h"
#include "DbDimdata.h"
//**********************************************************************************************************************
//
//	Dynamic window parent
//
class IcDynamicInputUIManager;

class CDynamicInputWnd : public CWnd
{
	DECLARE_DYNAMIC(CDynamicInputWnd)

public:
	CDynamicInputWnd(CWnd *pParent);

	BOOL ShowWindow(int nCmdShow);

	// progesoft
	virtual bool movable() { return true; };

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT, CPoint);
	afx_msg void OnKeyDown(UINT, UINT, UINT);
	afx_msg void OnKeyUp(UINT, UINT, UINT);
	afx_msg void OnChar(UINT, UINT, UINT);
	afx_msg BOOL OnMouseWheel(UINT flags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT, CPoint);
	afx_msg void OnLButtonDown(UINT, CPoint);
	afx_msg void OnLButtonUp(UINT, CPoint);
	afx_msg void OnRButtonDblClk(UINT, CPoint);
	afx_msg void OnRButtonDown(UINT, CPoint);
	afx_msg void OnRButtonUp(UINT, CPoint);
	afx_msg void OnMButtonDblClk(UINT, CPoint);
	afx_msg void OnMButtonDown(UINT, CPoint);
	afx_msg void OnMButtonUp(UINT, CPoint);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg LRESULT	OnShowWindow(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP()

protected:
	bool m_bDisableMouse;
};

//**********************************************************************************************************************
//
//	Dynamic edit control
//

class CDynamicEdit : public CEdit
{
	DECLARE_DYNAMIC(CDynamicEdit);
public:
	enum State
	{
		eStateUnchanged,
		eStateEditing,
		eStateModified,
		eStateError
	};

	enum charMask
	{
		eAcceptNone = 0,	// this control accepts none
		eAcceptMinor = 1,	// this control accepts <
		eAcceptAt = 2,	// this control accepts @
		eAcceptDash = 4,	// this control accepts #
		eAcceptDot = 8,	// this contol accepts .
		eAcceptComma = 16,	// this contol accepts ,
		eAcceptNumbers = 32,	// this control accepts numbers
		eAcceptAlpha = 64,	// this control accepts alphabetical
		eAcceptSpace = 128,  // this control accepts space (space doesn't like ENTER)
		eAcceptAll = eAcceptMinor | eAcceptAt | eAcceptDash | eAcceptDot | eAcceptComma | eAcceptNumbers | eAcceptAlpha,
		eAcceptDefaultReal = eAcceptMinor | eAcceptAt | eAcceptDash | eAcceptDot | eAcceptNumbers | eAcceptComma
	};

	enum skipMask
	{
		eSkipOnTab = 1,
		eSkipOnMinor = 2,
		eSkipOnComma = 4
	};

	enum enterMask
	{
		eEnterOnEnter = 1,
		eEnterOnSpace = 2
	};

	CDynamicEdit();

	/// 0 = value is automatic , 1 = is changing this has focus and user has typed in , 2 = is changed by user and it isn't active control anymore
	///
	virtual void	setStatus( State status );

	/// 0 = value is automatic , 1 = is changing this has focus and user has typed in , 2 = is changed by user and it isn't active control anymore
	/// return current state of this controls
	virtual State	getStatus();

	/// validates edit box value
	virtual bool	validate(bool bAcceptEsnap = true);

	/// check if single char is acceptable or not
	virtual bool	isAcceptedChar(wchar_t ch);
	virtual bool	isGoToNextEditChar(wchar_t ch);
	virtual bool	isEnterChar(wchar_t ch);

	void			setAcceptMask(int value) { m_acceptMask = value; };
	int				getAcceptMask() { return m_acceptMask; };

	void			setSkipMask(int value) { m_skipMask = value; };
	int				getSkipMask() { return m_skipMask; };

	void			setEnterMask(int value) { m_enterMask = value; };
	int				getEnterMask() { return m_enterMask; };

	bool			acceptNull() { return m_bAcceptNull; };
	void			setAcceptNull(bool value) { m_bAcceptNull = value; };

	virtual void	setValue(const OdString currentText);
	virtual void	setValue(const double val);
	virtual bool	getValue(double &val);
	virtual bool	getValue(int &val);

	virtual bool	validReal(bool bAcceptEsnap = true);
	virtual bool	validInt(bool bAcceptEsnap = true);
	virtual bool	validAngle(bool bAcceptEsnap = true);
	virtual bool	validCommand(bool bAcceptEsnap = true);

	bool			isEmpty();
	bool			isEditable();
	void			setEditable(bool);

	bool			acceptEsnap() { return true; };
	bool			isEsnap(IcString);
	bool			isEsnap();

	bool			canSwitchToCoordinate() { return m_bCanGoToCoordinate; };
	void			setCanSwitchToCoordinate(bool value) { m_bCanGoToCoordinate = value; };

	void			setDimData(OdDbDimData* pDimData);
	OdDbDimData*	dimData() { return m_pDimData; };

	virtual CSize	getSize();

	void			Show();
	void			Hide(bool bClearState = true);

	virtual void	setManager(IcDynamicInputUIManager*);
	virtual IcDynamicInputUIManager *getManager();

	void			setIsRelative(bool value) { m_bIsRelative = value; };
	void			setIsAngular(bool value) { m_bIsAngular = value; };

	bool			isRelative() { return m_bIsRelative; };
	bool			isAngular() { return m_bIsAngular; };
	bool			isDimAngular();
	// changes mode (relative / absolute coordinates) depending from character entered
	bool			setMode(wchar_t ch);

	void			SetWindowTextW(LPCTSTR lpszString);
	BOOL			ShowWindow(int nCmdShow);

	// remap this functions to substitute sendmessage with postmessage
	BOOL			SetReadOnly(BOOL bReadOnly);
	void			SetSel(int nStartChar, int nEndChar, BOOL bNoScroll = FALSE);
	CDynamicInputWnd * GetParent();
	BOOL			EnableWindow(BOOL bEnable = 1);
	LRESULT			OnSetText(WPARAM wp, LPARAM lp);

	// mark a show required on ResizeControls show is performed after move
	void			postPoneShow();

	// start a show if required
	void			ShowIfRequired();

	// returns if show is required
	bool			ShowIsRequired();

	// return true if control modified
	bool			IsModified();
	//return true if control unchanged
	bool			IsUnchanged();

	// to remap PostMessage function to block all messages when don't need to show ( LISP, commands, scripts)
	BOOL				PostMessageW(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);

	static int		getFontSize();

	const CString&  getText() const { return m_text; }

	// progesoft
	// if this control is active use arrow cursor or maintain crosshair
	virtual bool	requiresArrowCursor() { return true; };

protected:
	afx_msg void OnKeyDown(UINT, UINT, UINT);
	afx_msg void OnKeyUp(UINT, UINT, UINT);
	afx_msg void OnChar(UINT, UINT, UINT);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT, CPoint);
	afx_msg void OnLButtonDblClk(UINT, CPoint);
	afx_msg void OnLButtonDown(UINT, CPoint);
	afx_msg void OnLButtonUp(UINT, CPoint);
	afx_msg void OnRButtonDblClk(UINT, CPoint);
	afx_msg void OnRButtonDown(UINT, CPoint);
	afx_msg void OnRButtonUp(UINT, CPoint);
	afx_msg void OnMButtonDblClk(UINT, CPoint);
	afx_msg void OnMButtonDown(UINT, CPoint);
	afx_msg void OnMButtonUp(UINT, CPoint);

	afx_msg void OnEnChange();

	afx_msg LRESULT OnEnableWindow(WPARAM wp, LPARAM lp);

	void				loadEsnapStrings();

	/// converts string to radiant calculating expression
	bool				stringToRad(IcString, double &val);

	void				setError();
	State				m_status;

	bool				m_isEditable;

	OdDbDimData*		m_pDimData;

	IcDynamicInputUIManager *m_pUiManager;		// parent ui manager

	int					m_acceptMask;			// which char is accepted
	int					m_skipMask;				// which char skip to next control
	int					m_enterMask;			// which char send command to command line

	bool				m_bCanGoToCoordinate;	// pressing ',' or '<' go to coordinate mode

	bool				m_bAcceptNull;			// accept empty string or has a default value

	bool				m_bIsRelative;			// add @ at beginning
	bool				m_bIsAngular;			// add < at beginning

	CString				m_text;

	bool				m_bSizeInvalid;			// recalculate size;
	CSize				m_size;					// size of this text

	bool				m_bPostPoneShow;
	DECLARE_MESSAGE_MAP()

public:
		// returns true if input text is an option
		bool				isKeyword(IcString text);
};

//**********************************************************************************************************************
//
//	Dynamic edit control for lockable data
//

class CDynamicLockableEdit : public CDynamicEdit
{
	DECLARE_DYNAMIC(CDynamicLockableEdit);
public:

	CDynamicLockableEdit();

protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
};

//**********************************************************************************************************************
//
//	Dynamic edit control for real data
//

class CDynamicRealEdit : public CDynamicLockableEdit
{
	DECLARE_DYNAMIC(CDynamicRealEdit);
public:

	CDynamicRealEdit();
	bool	validate(bool bAcceptEsnap = true) override;

protected:

	DECLARE_MESSAGE_MAP()
};

//**********************************************************************************************************************
//
//	Dynamic edit control for int data
//

class CDynamicIntEdit : public CDynamicLockableEdit
{
	DECLARE_DYNAMIC(CDynamicIntEdit);
public:

	CDynamicIntEdit();
	bool	validate(bool bAcceptEsnap = true) override;

protected:

	DECLARE_MESSAGE_MAP()
};
//**********************************************************************************************************************
//
//	Dynamic edit control for angle data
//

class CDynamicAngleEdit : public CDynamicLockableEdit
{
	DECLARE_DYNAMIC(CDynamicAngleEdit);
public:

	CDynamicAngleEdit();
	bool	validate(bool bAcceptEsnap = true) override;
	void	setValue(const double val) override;

protected:

	DECLARE_MESSAGE_MAP()
};

//**********************************************************************************************************************
//
//	Dynamic edit control for string data
//

class CDynamicStringEdit : public CDynamicEdit
{
	DECLARE_DYNAMIC(CDynamicStringEdit);
public:

	CDynamicStringEdit();
	bool	validate(bool bAcceptEsnap = true) override;
	CSize	getSize() override;
	bool	validCommand(bool bAcceptEsnap = true) override;
protected:

	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
};

//**********************************************************************************************************************
//
//	Dynamic edit control for dim data
//

class CDynamicDimEdit : public CDynamicLockableEdit
{
	DECLARE_DYNAMIC(CDynamicDimEdit);
public:

	CDynamicDimEdit();
	bool	validate(bool bAcceptEsnap = true) override;

protected:

	DECLARE_MESSAGE_MAP()
};
//**********************************************************************************************************************
//
//	Dynamic edit control for string data
//

class CDynamicStringPromptEdit : public CDynamicEdit
{
	DECLARE_DYNAMIC(CDynamicStringPromptEdit);
public:
	CDynamicStringPromptEdit();
	virtual bool	validate(bool bAcceptEsnap = true);
	void			setValue(const OdString val) override;
	virtual CSize	getSize();

    bool            getDisplayDownArrow() const { return m_bDisplayDownArrow; }
	void			setDisplayDownArrow(bool value) { m_bDisplayDownArrow = value; };

protected:
	afx_msg void OnPaint(); // draw also arrow down icon

	bool			m_bDisplayDownArrow;
	DECLARE_MESSAGE_MAP()
};

class CDynamicPrompt : public CDynamicStringPromptEdit
{
	DECLARE_DYNAMIC(CDynamicPrompt);
public:
	static const UINT kTimerId = 1002;
	static const UINT kDefaultValueOffset = 60;
	CDynamicPrompt();
	void			setValue(const OdString val) override;
	afx_msg			void OnTimer(UINT_PTR timerId);
	void			SetWindowTextW(LPCTSTR lpszString);
protected:

	bool			m_bNeedUpdatePrompt;
	DECLARE_MESSAGE_MAP()
};
//**********************************************************************************************************************
//
//	Dynamic window parent for option list box
//

class CDynamicListBoxInputWnd : public CDynamicInputWnd
{
	DECLARE_DYNAMIC(CDynamicListBoxInputWnd)

public:

	CDynamicListBoxInputWnd(CWnd *pParent);

	BOOL ShowWindow(int nCmdShow);
protected:
	afx_msg void OnKeyDown(UINT, UINT, UINT);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()

};

//**********************************************************************************************************************
//
//	Dynamic option list control
//
class CDynamicOptionListBox : public CListCtrl
{
	DECLARE_DYNAMIC(CDynamicOptionListBox);
	static const UINT kTimerId = 1000;
	static const UINT kDefaultValueOffset = 15;

public:
	CDynamicOptionListBox();

	virtual IcDynamicInputUIManager *getManager() { return m_pUiManager; };
	virtual void	setManager(IcDynamicInputUIManager * pManager) { m_pUiManager = pManager; };

	// set flag to display a mandatory list of options
	void	setMandatoryOption(bool mandatoryOption) { m_optionMandatory = mandatoryOption; };
	void	setDefaultIndex(int index) { m_defaultIndex = index; }

	void	Show();
	void	Hide();
	CSize	getSize();

	// clistbox wrapper functions
	int		GetCurSel();
	void	SetCurSel(int index);
	int		SelectString(const int startindex, const CString textToSelect);
	void	ResetContent();
	int		AddString(CString str);
	int		AddString(CString str, CString strGlobal);
	BOOL	ShowWindow(int nCmdShow);

	bool	MessageHandler(UINT message, WPARAM wParam, LPARAM lParam);

protected:

	DECLARE_MESSAGE_MAP()

	afx_msg void OnKillFocus(CWnd *pNewWnd);
	afx_msg void OnLButtonDown(UINT flags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRep, UINT flag);
	afx_msg void OnChar(UINT nChar, UINT nRep, UINT flag);
	afx_msg void OnKeyUp(UINT nChar, UINT nRep, UINT flag);
	afx_msg void OnMouseMove(UINT flags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg LRESULT OnLBFindString(WPARAM, LPARAM);
	afx_msg LRESULT OnLBFindStringExact(WPARAM, LPARAM);
	afx_msg void OnTimer(UINT_PTR);

	int FindString(int iStart, LPCSTR lpszString, BOOL bExact);

	IcDynamicInputUIManager *m_pUiManager;		// parent ui manager
	bool					m_optionMandatory;
	CString					m_pressedChars;
	int						m_defaultIndex;
	int						m_curIndex;
	bool					isChanged;
	CStringArray			m_globalOptions;
};

//**********************************************************************************************************************
//
//	Dynamic edit control for command string data
//

class CDynamicCommandStringEdit : public CDynamicEdit
{
	DECLARE_DYNAMIC(CDynamicCommandStringEdit);
public:

	CDynamicCommandStringEdit();
	bool	validate(bool bAcceptEsnap = true) override;
	void AutocompleteTextHighlighted(CString text);

	// progesoft
	// if this control is active use arrow cursor or maintain crosshair
	virtual bool	requiresArrowCursor() { return false; };

	// progesoft
	// same of parent class but without setfocus
	virtual void	setStatus(State status);
protected:

	DECLARE_MESSAGE_MAP()

	// progesoft [-
	afx_msg void OnMouseMove(UINT flags, CPoint pt);
	afx_msg BOOL OnMouseWheel(UINT flags, short zDelta, CPoint pt);
	// progesoft -]
};

//**********************************************************************************************************************
//
//	Dynamic window parent for command list box
//

class CDynamicCommandListBoxInputWnd : public CDynamicInputWnd
{
	DECLARE_DYNAMIC(CDynamicCommandListBoxInputWnd)

public:

	CDynamicCommandListBoxInputWnd(CWnd *pParent);

	BOOL ShowWindow(int nCmdShow);
protected:
	afx_msg void OnKeyDown(UINT, UINT, UINT);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
};

//**********************************************************************************************************************
//
//	Dynamic command list control
//
class CDynamicCommandListBox : public CListCtrl
{
	DECLARE_DYNAMIC(CDynamicCommandListBox);
	static const UINT kTimerId = 1000;

	// progesoft
	static const UINT kFillList = 1001;
public:
	CDynamicCommandListBox();

	virtual IcDynamicInputUIManager *getManager() { return m_pUiManager; };
	virtual void	setManager(IcDynamicInputUIManager * pManager) { m_pUiManager = pManager; };

	// set flag to display a mandatory list of options
	void	setMandatoryOption(bool mandatoryOption) { m_optionMandatory = mandatoryOption; };

	void	Show();
	void	Hide();
	CSize	getSize();

	// clistbox wrapper functions
	int		GetCurSel();
	void	SetCurSel(int index);
	int		SelectString(const int startindex, const CString textToSelect);
	void	ResetContent();
	int		AddString(CString str);
	BOOL	ShowWindow(int nCmdShow);
	// mark a show required on ResizeControls show is performed after move
	void			postPoneShow();
	// start a show if required
	void			ShowIfRequired();
	// returns if show is required
	bool			ShowIsRequired();

	bool	MessageHandler(UINT message, WPARAM wParam, LPARAM lParam);
protected:

	DECLARE_MESSAGE_MAP()

	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnKillFocus(CWnd *pNewWnd);
	afx_msg void OnLButtonDown(UINT flags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRep, UINT flag);
	afx_msg void OnChar(UINT nChar, UINT nRep, UINT flag);
	afx_msg void OnKeyUp(UINT nChar, UINT nRep, UINT flag);
	afx_msg void OnMouseMove(UINT flags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg LRESULT OnLBFindString(WPARAM, LPARAM);
	afx_msg LRESULT OnLBFindStringExact(WPARAM, LPARAM);
	afx_msg void OnTimer(UINT_PTR);
	bool					m_bPostPoneShow;

	int FindString(int iStart, LPCSTR lpszString, BOOL bExact);
	int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	constexpr int GetIconIndexByType(bool isSysvar);

	IcDynamicInputUIManager *m_pUiManager;		// parent ui manager
	bool					m_optionMandatory;
	CString					m_pressedChars;
	CMFCToolBarImages		m_icons;
	int						m_helpIconId;
};
