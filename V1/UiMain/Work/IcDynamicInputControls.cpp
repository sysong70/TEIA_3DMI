/***************************************************************************
**
***     Copyright (C) 2012-2019 ProgeCAD srl. All rights reserved.
***     Derivative Portions Copyright (C) 2019-2024 IntelliCAD Technology Consortium. All Rights Reserved.
***
***     Use of the information contained herein, in part or in whole,
***     in/as source code and/or in/as object code, in any way by
***     anyone other than authorized employees of ProgeCAD srl. or
***     The IntelliCAD Technology Consortium, or by anyone to whom ProgeCAD srl.
***     or The IntelliCAD Technology Consortium has not granted
***     use is illegal.
***
*****************************************************************************/

#include "stdafx.h"
#include "IcDynamicInputUIManager.h"
#include "IcadCommon\Interfaces\IIcadIOManager.h"
#include "IcadCommon\utilities\IcadInterfaceUtils.h"
#include "IcadCommon\IcadCommonUtils.h"
#include "IcadCommon\Interfaces\IIcadServicesManager.h"
#include "IntelliCAD\IcadCommon\IcadSharedGlobals.h"
#include "CommandGUI/DynamicInput/IcDynamicInputUIManager.h"
#include "IcadCommon/utilities/ResourceStringClass.h"

#include "IcadCommon/Interfaces/IIcOsnapManager.h"

#include "IcadCommon\utilities\IcCharType.h"
#include "IcadCore/Engine/IcKeywords.h"
#include "IcadCore/Engine/IcadHelp.h"
#include "IcadCommon/IcadHelpData.h"

#define WM_CUSTOMSHOWWINDOW		WM_USER + 1
#define WM_CUSTOMENABLEWINDOW	WM_USER + 2
#define WM_CUSTOMSETTEXT		WM_USER + 3
//**********************************************************************************************************************
//
//	Dynamic window parent
//

void SetTrasparentWindow(CWnd *pWindow, int perc)
{
	typedef BOOL(CALLBACK* SetLayeredWindowAttributesProc)(HWND, COLORREF, BYTE, DWORD);
	HMODULE hLBLib = LoadLibrary(L"user32.dll");

	if (pWindow && pWindow->m_hWnd && NULL != hLBLib)
	{
		SetLayeredWindowAttributesProc SetLayeredWindowAttributes = (SetLayeredWindowAttributesProc)GetProcAddress(hLBLib, "SetLayeredWindowAttributes");
		LONG oldStyle = GetWindowLong(pWindow->m_hWnd, GWL_EXSTYLE);
		LONG ret = SetWindowLong(pWindow->m_hWnd, GWL_EXSTYLE, oldStyle | WS_EX_LAYERED);
		DWORD err = GetLastError();
		ret = SetLayeredWindowAttributes(pWindow->m_hWnd, 0, (BYTE)(((100 - perc) * 25) / 10), LWA_ALPHA);
		FreeLibrary(hLBLib);
	}
	FreeLibrary(hLBLib);
}

CString getCapital(const CString &szString, bool isOption)
{
	CString command;
	CString selectedItem = szString;
    bool isUpperStarted = false;
	for (int i = 0; i != selectedItem.GetLength(); i++)
	{
		if (iswupper(selectedItem[i]))
        {
			command += selectedItem[i];
            isUpperStarted = true;
        }
        else if (isUpperStarted && (islower(selectedItem[i]) || selectedItem[i] == L')'))
            break;
		else if (iswdigit(selectedItem[i]) || selectedItem[i] == L'.')
			command += selectedItem[i];
		else if (!command.IsEmpty() && selectedItem[i] == L'-' && isOption)
            break;
		else if (command.IsEmpty() && ((selectedItem[i] == L'+') || (selectedItem[i] == L'-')))
			command += selectedItem[i];
		else if (selectedItem[i] == L'_')
			command += selectedItem[i];
		else if (selectedItem[i] == L'.')
			command += selectedItem[i];
	}
	return command;
}

// extract from option string upper case letters and similar to send them to command line
CString getCommandToSend(const CString &szString, bool isOption)
{
	CString selectedItem = szString;

	if (!isOption)
	{
		int pos = selectedItem.Find(L' ');
		if (pos > 0)
		{
			selectedItem = selectedItem.Left(pos);
		}
		return selectedItem;
	}

	CString command;
	if (selectedItem.Find(L"|")>-1)
		selectedItem = selectedItem.Mid(selectedItem.Find(L"|") + 1);

	// in russian translation I need to ignore cyrillic capital letter and use only latin capital in ()
	if (selectedItem.Find(L"(")>-1)
	{
		CString latin = selectedItem.Mid(selectedItem.Find(L"(") + 1);
		command = getCapital(latin, isOption);
		if (!command.IsEmpty())
			return command;
	}

	while (selectedItem.Find(L"<") != -1 && selectedItem.Find(L">") != -1 && selectedItem.Find(L"<") < selectedItem.Find(L">")) {
		selectedItem.Delete(selectedItem.Find(L"<"), (selectedItem.Find(L">") - selectedItem.Find(L"<")) + 1);
	}

	command = getCapital(selectedItem, isOption);
	if (command.IsEmpty() && selectedItem.Find(L'?')>-1)
	{
		command = L"?";
	}
	else if (command.IsEmpty() && selectedItem.Find(L'&')>-1)
	{
		command = L"&";
	}
	else if (command.IsEmpty() && selectedItem.Find(L'*')>-1)
	{
		command = L"*";
	}
	else if (command.IsEmpty())
	{
		command = selectedItem;
	}
	return command;
}

OdArray<IcString> g_arrayEsnapString;

CDynamicInputWnd::CDynamicInputWnd(CWnd *pParent)
{
	// option list box disables mouse message forward
	m_bDisableMouse = false;
}

IMPLEMENT_DYNAMIC(CDynamicInputWnd, CWnd)

BEGIN_MESSAGE_MAP(CDynamicInputWnd, CWnd)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CHAR()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_SIZE()
	ON_MESSAGE(WM_CUSTOMSHOWWINDOW, OnShowWindow)
END_MESSAGE_MAP()

BOOL CDynamicInputWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}
void CDynamicInputWnd::OnMouseMove(UINT flags, CPoint pt)
{
	ClientToScreen(&pt);
	CWnd *currentView = icedGetIcadDwgView();
	if (currentView && !m_bDisableMouse)
	{
		currentView->ScreenToClient(&pt);
		currentView->SendMessage(WM_MOUSEMOVE, flags, MAKELPARAM(pt.x, pt.y));
	}
}
BOOL CDynamicInputWnd::OnMouseWheel(UINT flags, short zDelta, CPoint pt)
{
	//ClientToScreen( &pt );
	CWnd *currentView = icedGetIcadDwgView();
	if (currentView && !m_bDisableMouse)
	{
		//currentView->ScreenToClient( &pt );
		currentView->SendMessage(WM_MOUSEWHEEL, MAKEWPARAM(flags, zDelta), MAKELPARAM(pt.x, pt.y));
	}
	return FALSE;
}

void CDynamicInputWnd::OnKeyUp(UINT nChar, UINT nRep, UINT flag)
{
	//CWnd *currentView = icedGetIcadFrame();
	//if (currentView)
	//	currentView->SendMessage( WM_KEYUP, nChar, MAKELPARAM( nRep, flag ) );
}


extern UINT vkToChar(UINT code, UINT flags);

void CDynamicInputWnd::OnKeyDown(UINT nChar, UINT nRep, UINT flag)
{
	if (!m_hWnd)
		return;
	HWND hChild = ::GetWindow(m_hWnd, GW_CHILD);
	if (hChild && IsWindow(hChild) && ::IsWindowVisible(hChild))
	{
		CWnd *currentWnd = CWnd::FromHandle(hChild);
		CDynamicEdit *pWnd = (CDynamicEdit *)currentWnd;
		if (pWnd && pWnd->getManager())
			pWnd->getManager()->sendKey(nChar, MAKELPARAM(nRep, flag));
	}
}

void CDynamicInputWnd::OnChar(UINT nChar, UINT nRep, UINT flag)
{
	/*CWnd *currentView = icedGetIcadFrame();
	if (currentView)
	currentView->SendMessage( WM_CHAR, nChar, MAKELPARAM( nRep, flag ) );*/
}

void CDynamicInputWnd::OnLButtonDblClk(UINT flags, CPoint pt)
{
	// forward mousemove message to icad
	ClientToScreen(&pt);
	CWnd *currentView = icedGetIcadDwgView();
	if (currentView && !m_bDisableMouse)
	{
		currentView->ScreenToClient(&pt);
		currentView->SendMessage(WM_LBUTTONDBLCLK, flags, MAKELPARAM(pt.x, pt.y));
	}
}
void CDynamicInputWnd::OnLButtonDown(UINT flags, CPoint pt)
{
	// forward mousemove message to icad
	ClientToScreen(&pt);
	CWnd *currentView = icedGetIcadDwgView();
	if (currentView && !m_bDisableMouse)
	{
		currentView->ScreenToClient(&pt);
		currentView->SendMessage(WM_LBUTTONDOWN, flags, MAKELPARAM(pt.x, pt.y));
	}
}
void CDynamicInputWnd::OnLButtonUp(UINT flags, CPoint pt)
{
	// forward mousemove message to icad
	ClientToScreen(&pt);
	CWnd *currentView = icedGetIcadDwgView();
	if (currentView && !m_bDisableMouse)
	{
		currentView->ScreenToClient(&pt);
		currentView->SendMessage(WM_LBUTTONUP, flags, MAKELPARAM(pt.x, pt.y));
	}
}
void CDynamicInputWnd::OnRButtonDblClk(UINT flags, CPoint pt)
{
	// forward mousemove message to icad
	ClientToScreen(&pt);
	CWnd *currentView = icedGetIcadDwgView();
	if (currentView && !m_bDisableMouse)
	{
		currentView->ScreenToClient(&pt);
		currentView->SendMessage(WM_RBUTTONDBLCLK, flags, MAKELPARAM(pt.x, pt.y));
	}
}
void CDynamicInputWnd::OnRButtonDown(UINT flags, CPoint pt)
{
	// forward mousemove message to icad
	ClientToScreen(&pt);
	CWnd *currentView = icedGetIcadDwgView();
	if (currentView && !m_bDisableMouse)
	{
		currentView->ScreenToClient(&pt);
		currentView->SendMessage(WM_RBUTTONDOWN, flags, MAKELPARAM(pt.x, pt.y));
	}
}
void CDynamicInputWnd::OnRButtonUp(UINT flags, CPoint pt)
{
	// forward mousemove message to icad
	ClientToScreen(&pt);
	CWnd *currentView = icedGetIcadDwgView();
	if (currentView && !m_bDisableMouse)
	{
		currentView->ScreenToClient(&pt);
		currentView->SendMessage(WM_RBUTTONUP, flags, MAKELPARAM(pt.x, pt.y));
	}
}
void CDynamicInputWnd::OnMButtonDblClk(UINT flags, CPoint pt)
{
	// forward mousemove message to icad
	ClientToScreen(&pt);
	CWnd *currentView = icedGetIcadDwgView();
	if (currentView && !m_bDisableMouse)
	{
		currentView->ScreenToClient(&pt);
		currentView->SendMessage(WM_MBUTTONDBLCLK, flags, MAKELPARAM(pt.x, pt.y));
	}
}
void CDynamicInputWnd::OnMButtonDown(UINT flags, CPoint pt)
{
	// forward mousemove message to icad
	ClientToScreen(&pt);
	CWnd *currentView = icedGetIcadDwgView();
	if (currentView && !m_bDisableMouse)
	{
		currentView->ScreenToClient(&pt);
		currentView->SendMessage(WM_MBUTTONDOWN, flags, MAKELPARAM(pt.x, pt.y));
	}
}
void CDynamicInputWnd::OnMButtonUp(UINT flags, CPoint pt)
{
	// forward mousemove message to icad
	ClientToScreen(&pt);
	CWnd *currentView = icedGetIcadDwgView();
	if (currentView && !m_bDisableMouse)
	{
		currentView->ScreenToClient(&pt);
		currentView->SendMessage(WM_MBUTTONUP, flags, MAKELPARAM(pt.x, pt.y));
	}
}

void CDynamicInputWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (!m_hWnd)
		return;
	HWND hChild = ::GetWindow(m_hWnd, GW_CHILD);
	if (hChild && IsWindow(hChild) /*&& ::IsWindowVisible(hChild)*/)
	{
		CWnd *currentWnd = CWnd::FromHandle(hChild);

		CRect cr;
		GetClientRect(cr);
		if (currentWnd)
		{
			currentWnd->MoveWindow(0, 0, cr.right, cr.bottom);
			currentWnd->UpdateWindow();
		}

	}
}

BOOL CDynamicInputWnd::ShowWindow(int nCmdShow)
{
	if (IcadSharedGlobals::IsMainThreadCurrent())
		CWnd::ShowWindow(nCmdShow);
	else
		PostMessage(WM_CUSTOMSHOWWINDOW, (WPARAM)nCmdShow == SW_SHOW ? TRUE : FALSE, 0);
	return TRUE;
}

// used to manage hide/show message from other thread than main
LRESULT CDynamicInputWnd::OnShowWindow(WPARAM wp, LPARAM lp)
{
	CWnd::ShowWindow(wp == TRUE ? SW_SHOW : SW_HIDE);
	return 0;
}
//**********************************************************************************************************************
//
//	Dynamic edit control
//

CDynamicEdit::CDynamicEdit()
{
	m_status = eStateUnchanged;
	m_isEditable = true;
	m_pDimData = NULL;
	m_pUiManager = NULL;

	m_acceptMask = eAcceptNone;
	m_enterMask = eEnterOnEnter | eEnterOnSpace;
	m_skipMask = eSkipOnTab | eSkipOnMinor | eSkipOnComma;
	m_bCanGoToCoordinate = true; // default all controls can switch to coordinate mode
	m_bAcceptNull = true; // default all controls accept null value (empty string or default)
	m_bIsRelative = false;
	m_bIsAngular = false;
	m_bSizeInvalid = true;
	m_bPostPoneShow = false; // mark a show required
}

IMPLEMENT_DYNAMIC(CDynamicEdit, CEdit)

BEGIN_MESSAGE_MAP(CDynamicEdit, CEdit)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CHAR()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_CONTROL_REFLECT(EN_UPDATE, OnEnChange)
	ON_MESSAGE(WM_CUSTOMENABLEWINDOW, OnEnableWindow)
	ON_MESSAGE(WM_CUSTOMSETTEXT, OnSetText)
END_MESSAGE_MAP()


void CDynamicEdit::setManager(IcDynamicInputUIManager *pMan)
{
	m_pUiManager = pMan;
}

IcDynamicInputUIManager *CDynamicEdit::getManager()
{
	return m_pUiManager;
}

BOOL CDynamicEdit::PostMessageW(UINT message, WPARAM wParam, LPARAM lParam)
{
	IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
	ASSERT(pISysvar != NULL);

	int dynMode;
	int retVal = pISysvar->getVar(L"DYNMODE", dynMode);

	if (dynMode <= DynInputValues::KPointerAndDimInputOff)
		return TRUE;


	return CWnd::PostMessageW(message, wParam, lParam);
}

void CDynamicEdit::OnEnChange()
{
	m_bSizeInvalid = true;
	CString stValue;
	GetWindowText(stValue);

	if (m_pUiManager && !stValue.IsEmpty())
		m_pUiManager->resizeControls();

	if (m_pDimData && m_pDimData->customStringFunc())
	{
		DimDataSetCustomStringFuncPtr pFunc = m_pDimData->customStringFunc();

		OdString odValue = (LPCTSTR)stValue;
		OdGeVector3d vector;
		pFunc(m_pDimData, NULL, odValue, vector);
	}
	//  	if (m_pUiManager && !stValue.IsEmpty() )
	//  		m_pUiManager->updateGraphicSurface();

}

void CDynamicEdit::OnKeyUp(UINT nChar, UINT nRep, UINT flag)
{
	CEdit::OnKeyUp(nChar, nRep, flag);
}

void CDynamicEdit::OnKeyDown(UINT nChar, UINT nRep, UINT flag)
{
	wchar_t ch = vkToChar(nChar, MAKELPARAM(nRep, flag));
	if (isGoToNextEditChar(ch))
	{
		getManager()->onKeyboardMessage(GetSafeHwnd(), WM_KEYDOWN, nChar, MAKELPARAM(nRep, flag));
	    if (m_status == State::eStateEditing && ::GetFocus() == m_hWnd)
	    {
		    CWnd *currentView = icedGetIcadDwgView();
		    if (currentView)
		    {
			    currentView->SetFocus();
		    }
        }
	}
	else
	{
		CEdit::OnKeyDown(nChar, nRep, flag);
	}

	GetParent()->Invalidate(TRUE);
}

void CDynamicEdit::OnChar(UINT nChar, UINT nRep, UINT flag)
{
	if (nChar == VK_ESCAPE || nChar == VK_RETURN )
	{
		CWnd *currentView = icedGetIcadDwgView();
		if (currentView)
		{
			currentView->SetFocus();
			currentView->SendMessage(WM_CHAR, nChar, MAKELPARAM(nRep, flag));
			return;
		}
	}
	CEdit::OnChar(nChar, nRep, flag);
}

bool CDynamicEdit::isEmpty()
{
	CString currentString;
	GetWindowTextW(currentString);
	return currentString.IsEmpty();
}

bool CDynamicEdit::isEditable()
{
	return m_isEditable;
}

BOOL CDynamicEdit::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CDynamicEdit::setEditable(bool val)
{
	m_isEditable = val;
}

void CDynamicEdit::setError()
{
	setStatus(CDynamicEdit::eStateError);
	Invalidate();
	SetSel(0, -1);
}

void CDynamicEdit::loadEsnapStrings()
{
	g_arrayEsnapString.append(ResourceString(IDC_IC_GET_NON_5, "NON"));
	g_arrayEsnapString.append(L"NON");
	g_arrayEsnapString.append(L"NONE");
	g_arrayEsnapString.append(ResourceString(CMDSTR_FROM, "FROM"));
	g_arrayEsnapString.append(L"FROM");
	g_arrayEsnapString.append(ResourceString(CMDSTR_TT, "TT"));
	g_arrayEsnapString.append(L"TT");
	g_arrayEsnapString.append(ResourceString(IDS_GET_M2P, "M2P"));
	g_arrayEsnapString.append(L"M2P");
	g_arrayEsnapString.append(ResourceString(IDS_GET_MTP, "MTP"));
	g_arrayEsnapString.append(L"MTP");
	g_arrayEsnapString.append(L".X");
	g_arrayEsnapString.append(L".Y");
	g_arrayEsnapString.append(L".Z");
	g_arrayEsnapString.append(L".XY");
	g_arrayEsnapString.append(L".XZ");
	g_arrayEsnapString.append(L".YZ");
}
bool CDynamicEdit::isEsnap(IcString text)
{
	if (g_arrayEsnapString.size() == 0)
		loadEsnapStrings();

	text.trimWhiteSpace(L"be");
	IcString textNoUnderScore = text;
	if (textNoUnderScore[0] == L'_')
		textNoUnderScore = textNoUnderScore.mid(1);

	for (int i = 0; i != g_arrayEsnapString.size(); i++)
	{
		if (textNoUnderScore.iCompare(g_arrayEsnapString[i]) == 0)
			return true;
	}
	IcDbOsnapGlyphBasePtr pGlyph;
	IIcOsnapManager* pOsnapManager = IcadSharedGlobals::GetCMainWindow()->getIcadOsnapManager();
	return (pOsnapManager->findOSnap(text, pGlyph) == RTNORM);
}

bool CDynamicEdit::isKeyword(IcString text)
{
	int result = false;

	IIcadIOManager* pIO = getIOManagerInterface(getIcadInterfaces());
	IcString tmp = pIO->getKeywords();
	int flags = pIO->getUserInputControls();
	pIO->setKeywords(0, m_pUiManager->getKeywords());
	result = (IcCoreExternal::setKeyWord(text.WChar_str()) == RTKWORD);
	pIO->setKeywords(flags, tmp);

	return result;
}

bool CDynamicEdit::isEsnap()
{
	IcString currentText;
	GetWindowText((LPTSTR)currentText.getBuffer(1024), 1024);
	currentText.releaseBuffer();
	currentText.trimWhiteSpace(L"be");
	return isEsnap(currentText);
}

void CDynamicEdit::setStatus(State status)
{
	if (m_status == status)
		return;

	if (m_status == State::eStateEditing && ::GetFocus() == m_hWnd)
	{
		if (CWnd *currentView = icedGetIcadDwgView())
		{
			currentView->SetFocus();
		}
	}

	m_status = status;
	m_bSizeInvalid = true;

	if (m_status == State::eStateEditing && ::GetFocus() != m_hWnd)
	{
		SetFocus();
	}
}

CDynamicEdit::State CDynamicEdit::getStatus()
{
	return m_status;
}

bool CDynamicEdit::stringToRad(IcString text, double &value)
{
	double val;
	bool ret = false;
	int aunits = 0;
	IIcadServicesManager* pServMgr = getServicesManagerInterface(getIcadInterfaces());
	icedGetVar(L"AUNITS", aunits);
	if (pServMgr->stringToAngle(7, text, -1, val) == RTNORM)
	{
		value = val;
		ret = true;
	}
	return ret;
}

bool CDynamicEdit::validReal(bool bAcceptEsnap)
{
	if (!m_hWnd)
	{
		return false;
	}


	IcString currentText;
	GetWindowText((LPTSTR)currentText.getBuffer(1024), 1024);
	currentText.releaseBuffer();
	currentText.trimWhiteSpace(L"be");

	if (currentText.isEmpty())
	{
		return m_bAcceptNull;
	}

	// esnap are always accepted ? only command line can determine it
	if (bAcceptEsnap && acceptEsnap() && isEsnap(currentText))
	{
		return true;
	}

	IIcadServicesManager* pServMgr = getServicesManagerInterface(getIcadInterfaces());
	if (currentText[0] == L'@')
	{
		if (getAcceptMask() & eAcceptAt)
		{
			currentText = currentText.mid(1);
		}
		else
		{
			setError();
			return false;
		}
	}
	if (currentText[0] == L'#')
	{
		if (getAcceptMask() & eAcceptDash)
		{
			currentText = currentText.mid(1);
		}
		else
		{
			setError();
			return false;
		}
	}

	if (isAngular())
	{
		if (getAcceptMask() & eAcceptMinor)
		{
			currentText.trimWhiteSpace(L"<");
		}
		else
		{
			setError();
			return false;
		}

		double val = 0.0;
		if (pServMgr->stringToAngle( 7, currentText, -1, val) == RTNORM)
		{
			return true;
		}


		if (iswalpha(currentText[0]) && !(getAcceptMask() & CDynamicEdit::eAcceptAlpha))
		{
			setError();
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		double val = 0.0;
		if (pServMgr->stringToDistance(currentText, -1, val) == RTNORM)
			return true;

		if (!(getAcceptMask() & CDynamicEdit::eAcceptAlpha))
		{
			setError();
			return false;
		}
		else
		{
			return true;
		}
	}
}

bool CDynamicEdit::validInt(bool bAcceptEsnap)
{
	IcString currentText;
	GetWindowText((LPTSTR)currentText.getBuffer(1024), 1024);
	currentText.releaseBuffer();
	currentText.trimWhiteSpace(L"be");

	// esnap are always accepted ? only command line can determine it
	if (bAcceptEsnap && acceptEsnap() && isEsnap(currentText))
	{
		return true;
	}

	if (currentText.isEmpty())
	{
		return m_bAcceptNull;
	}

	if (currentText.iCompare(L"_ON") == 0 ||
		currentText.iCompare(L"_OFF") == 0 ||
		currentText.iCompare(ResourceString(IDC_MSG_ON, "ON")) == 0 ||
		currentText.iCompare(ResourceString(IDC_MSG_OFF, "Off")) == 0)
	{
		return true;
	}

	IIcadServicesManager* pServMgr = getServicesManagerInterface(getIcadInterfaces());
	double val = 0.0;
	if ((pServMgr->stringToDistance(currentText, -1, val) != RTNORM || wcschr(currentText, L'.')) &&
		!(getAcceptMask() & CDynamicEdit::eAcceptAlpha))
	{
		setError();
		return false;
	}
	return true;
}

bool CDynamicEdit::validAngle(bool bAcceptEsnap)
{
	IcString currentText;
	GetWindowText((LPTSTR)currentText.getBuffer(1024), 1024);
	currentText.releaseBuffer();
	currentText.trimWhiteSpace(L"be");

	if (currentText.isEmpty())
	{
		return m_bAcceptNull;
	}

	// esnap are always accepted ? only command line can determine it
	if (bAcceptEsnap && acceptEsnap() && isEsnap(currentText))
	{
		return true;
	}

	IIcadServicesManager* pServMgr = getServicesManagerInterface(getIcadInterfaces());
	double val = 0.0;

	if (pServMgr->stringToAngle(7, currentText, -1, val) != RTNORM &&
		!(getAcceptMask() & CDynamicEdit::eAcceptAlpha))
	{
		setError();
		return false;
	}
	return true;
}

bool CDynamicEdit::validCommand(bool bAcceptEsnap)
{
	if (validate(bAcceptEsnap))
	{
		return true;
	}

	IIcadIOManager* pIO = getIOManagerInterface(getIcadInterfaces());
	int ui = pIO->getUserInputControls();

	IcString currentText;
	GetWindowText((LPTSTR)currentText.getBuffer(1024), 1024);
	currentText.releaseBuffer();
	currentText.trimWhiteSpace(L"be");

	if (currentText.isEmpty())
	{
		return (ui & IcUserInputControls::kNullResponseIsNotAccepted) == 0;
	}

	// esnap are always accepted ? only command line can determine it
	if (bAcceptEsnap && acceptEsnap() && isEsnap(currentText))
	{
		return true;
	}

	// transparent command
	if (currentText[0] == '\'' ||
		currentText[0] == '('  ||
		currentText[0] == '!' ||
		isKeyword(currentText) ||
		ui & IcUserInputControls::kAcceptOtherInputString)
	{
		return true;
	}

	if (!currentText.right(1).iCompare(L"x") || !currentText.right(2).iCompare(L"xp"))
	{
		return true;
	}

	setError();
	return false;
}

void CDynamicEdit::setValue(const OdString currentText)
{
	const wchar_t *pString = (LPCTSTR)currentText;
	if (pString[0] == L'\n')
		pString++;
	SetWindowTextW(pString);
}

void CDynamicEdit::setValue(const double val)
{
	IcString currentText;
	IIcadServicesManager* pServMgr = getServicesManagerInterface(getIcadInterfaces());
	if (isAngular() || isDimAngular())
	{
		int aunits;
		icedGetVar(L"AUNITS", aunits);
		//avoid surveyor units
		if (aunits == 4)
			aunits = 0;
		pServMgr->angleToString(1, val, aunits, -1, currentText);
	}
	else
	{
		pServMgr->realToString(val, -1, -1, currentText);
	}

	setValue(currentText);
}

bool CDynamicEdit::getValue(double &value)
{
	IcString currentText;
	GetWindowText((LPTSTR)currentText.getBuffer(1024), 1024);
	currentText.releaseBuffer();
	currentText.trimWhiteSpace(L"be");

	IIcadServicesManager* pServMgr = getServicesManagerInterface(getIcadInterfaces());
	bool ret = false;
	double val = 0.0;
	if (currentText[0] == L'@' || currentText[0] == L'#')
		currentText = currentText.mid(1);

	//if (currentText[0] == L'<')
	if (isAngular() || isDimAngular())
	{
		//currentText = currentText.mid(1);

		if (stringToRad(currentText, val))
		{
			value = val;
			ret = true;
		}
	}
	else
	{
		IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
		int lunits = 2;
		int retVal = pISysvar->getVar(L"LUNITS", lunits);

		if (pServMgr->stringToDistance(currentText, -1, val) == RTNORM)
		{
			value = val;
			ret = true;
		}
	}
	return ret;
}

bool CDynamicEdit::getValue(int &value)
{
	IcString currentText;
	GetWindowText((LPTSTR)currentText.getBuffer(1024), 1024);
	currentText.releaseBuffer();
	currentText.trimWhiteSpace(L"be");

	IIcadServicesManager* pServMgr = getServicesManagerInterface(getIcadInterfaces());
	bool ret = false;
	double val = 0.0;
	OdString onString = ResourceString(IDC_MSG_ON, "ON");
	OdString offString = ResourceString(IDC_MSG_OFF, "Off");
	if (currentText.iCompare(L"_ON") == 0 || currentText.iCompare(onString) == 0)
	{
		value = 1;
		return true;
	}
	if (currentText.iCompare(L"_OFF") == 0 || currentText.iCompare(offString) == 0)
	{
		value = 0;
		return true;
	}
	if (pServMgr->stringToDistance(currentText, -1, val) == RTNORM)
	{
		value = (int)val;
		ret = true;
	}
	return ret;
}

void CDynamicEdit::postPoneShow()
{
	m_bPostPoneShow = true;
}

void CDynamicEdit::ShowIfRequired()
{
	if (m_bPostPoneShow)
		Show();
	m_bPostPoneShow = false;
}

bool CDynamicEdit::ShowIsRequired()
{
	return m_bPostPoneShow;
}

void CDynamicEdit::Show()
{

	if (GetParent() && GetParent()->m_hWnd && !GetParent()->IsWindowVisible())
	{
		int perc = 0;
		icedGetVar(L"TOOLTIPTRANSPARENCY", perc);
		GetParent()->ShowWindow(SW_SHOWNOACTIVATE);
		SetTrasparentWindow(GetParent(), perc);
		if (m_hWnd && !IsWindowVisible())
		{
			ShowWindow(SW_SHOWNOACTIVATE);
		}
		SetTrasparentWindow(this, perc);
	}

	// this is an hack to move control to right position and avoid bad displacement of controls
	// sometimes happen that controls is moving from 0,0
	RECT position;
	GetWindowRect(&position);
	GetParent()->ScreenToClient(&position);
	if (position.left != 0 || position.top != 0)
		SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOZORDER);
}

void CDynamicEdit::Hide(bool bClearState)
{
	EnableWindow(false);
	SetReadOnly(true);

	if (GetParent() && GetParent()->m_hWnd && GetParent()->IsWindowVisible())
	{
		GetParent()->ShowWindow(SW_HIDE);
		if (m_hWnd && IsWindowVisible())
		{
			ShowWindow(SW_HIDE);
		}
	}

	if(bClearState)
	{
		m_status = eStateUnchanged;
		m_isEditable = true;

		//m_acceptMask = eAcceptNone;
		m_enterMask = eEnterOnEnter | eEnterOnSpace;
		m_skipMask = eSkipOnTab | eSkipOnMinor | eSkipOnComma;
		m_bCanGoToCoordinate = true; // default all controls can switch to coordinate mode
		m_bAcceptNull = true; // default all controls accept null value (empty string or default)
		m_bIsRelative = false;
		m_bIsAngular = false;
		m_bSizeInvalid = true;
		m_bPostPoneShow = false; // mark a show required
	}
}

void CDynamicEdit::OnPaint()
{
	int textStartX = 5;
	int textStartY = 3;

	CRect rectUpdate;
	if (GetUpdateRect(rectUpdate) != 0)
	{
		OdCmColor colText;
		OdCmColor colBkg;
		OdCmColor commonBkgColor;
		int tilemode, cvport;
		icedGetVar(L"TILEMODE", tilemode);
		icedGetVar(L"CVPORT", cvport);
		if (tilemode)
		{
			icedGetVar(L"BKGCOLOR", commonBkgColor);
		}
		else
		{
			icedGetVar(L"PSPACEPCOLOR", commonBkgColor);
		}

		if (!tilemode && cvport == 1)
		{
			icedGetVar(L"DYNTOOLTIPCOLORLAYOUTTEXT", colText);
			icedGetVar(L"DYNTOOLTIPCOLORLAYOUTBKG", colBkg);
		}
		else
		{
			icedGetVar(L"DYNTOOLTIPCOLORMODELTEXT", colText);
			icedGetVar(L"DYNTOOLTIPCOLORMODELBKG", colBkg);
		}
		//colText.setColor( iText );
		//colBkg.setColor( iBkg );
		if (m_pDimData && !m_pDimData->isDimEditable())
		{
			colBkg = commonBkgColor;
			int red = (int)commonBkgColor.red();
			int green = (int)commonBkgColor.green();
			int blue = (int)commonBkgColor.blue();
			// standart linear grayscale conversion
			int grayIndex = (int)(0.2126 * red + 0.7152 * green + 0.0722 * blue);
			if (grayIndex > 128)
			{
				// black text
				colText.setRGB(0, 0, 0);
			}
			else
			{
				// white text
				colText.setRGB(255, 255, 255);
			}
		}

		CPaintDC dc(this);
		CRect srect;
		GetClientRect(srect);
		CMemDC memDC(dc, srect);

		// to make textout function write only inside text box
		CRgn region;
		region.CreateRectRgnIndirect(rectUpdate);
		memDC.GetDC().SelectClipRgn(&region);

		if (getStatus() == CDynamicEdit::eStateError)
		{
			CBrush errorBrush;
			DWORD errorColor = RGB(255, 0, 0);
			errorBrush.CreateSolidBrush(errorColor);
			memDC.GetDC().SelectObject(&errorBrush);
			memDC.GetDC().Rectangle(rectUpdate);
		}
		else
		{
			CBrush blackBrush;
			DWORD blackColor = 0;
			blackBrush.CreateSolidBrush(blackColor);
			memDC.GetDC().SelectObject(&blackBrush);
			memDC.GetDC().Rectangle(rectUpdate);
		}
		DWORD BackColor;
		if (IsWindowEnabled())
			BackColor = GetSysColor(COLOR_WINDOW);
		else
			BackColor = RGB(colBkg.red(), colBkg.green(), colBkg.blue());// GetSysColor( COLOR_INACTIVECAPTION );

																		 // Apply the device context to the client area of the edit control
		int margin = 3;
		if (m_pDimData && !m_pDimData->isDimEditable())
		{
			margin = 0;
		}
		else if (getStatus() != CDynamicEdit::eStateError)
		{
			margin = 1;
		}

		memDC.GetDC().FillSolidRect(rectUpdate.left + margin, rectUpdate.top + margin, rectUpdate.Width() - 2 * margin, rectUpdate.Height() - 2 * margin, BackColor);

		// Rewrite the text since the backcolor paint
		// overwrote the existing text
		CString Text;
		GetWindowText(Text);
		int startChar, endChar;
		GetSel(startChar, endChar);
		if (IsWindowEnabled())
			memDC.GetDC().SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		else
			memDC.GetDC().SetTextColor(RGB(colText.red(), colText.green(), colText.blue())); // GetSysColor(COLOR_INACTIVECAPTIONTEXT));

		CFont *pFont = GetFont();

		LOGFONT lf;
		if (pFont)
			pFont->GetLogFont(&lf);
		lf.lfHeight = CDynamicEdit::getFontSize();
		CFont newFont;
		newFont.CreateFontIndirectW(&lf);

		CFont *oldFont = NULL;
		if (pFont)
			oldFont = memDC.GetDC().SelectObject(&newFont);

		if (isAngular())
		{
			Text = L"< " + Text;
			startChar += 2;
			endChar += 2;
		}
		textStartX = rectUpdate.left + textStartX;
		textStartY = rectUpdate.top + textStartY;
		if (!IsWindowEnabled() || startChar == endChar)
		{
			memDC.GetDC().TextOut(textStartX, textStartY, Text.GetBuffer(Text.GetLength()));
		}
		else
		{
			CString notSelectedStartText = Text.Left(startChar);
			CString selectedText = Text.Mid(startChar, endChar - startChar);
			CString notSelectedEndText = Text.Right(Text.GetLength() - endChar);

			memDC.GetDC().TextOut(4, textStartY, notSelectedStartText);
			CSize textSize = memDC.GetDC().GetTextExtent(notSelectedStartText);

			memDC.GetDC().SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
			memDC.GetDC().SetBkColor(GetSysColor(COLOR_HIGHLIGHT));

			memDC.GetDC().TextOut(textStartX + textSize.cx + 1, textStartY, (LPCTSTR)selectedText, selectedText.GetLength());

			CSize selectedTextSize = memDC.GetDC().GetTextExtent(selectedText);

			memDC.GetDC().SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
			memDC.GetDC().SetBkColor(BackColor);

			memDC.GetDC().TextOut(textStartX + textSize.cx + 1 + selectedTextSize.cx, textStartY, (LPCTSTR)notSelectedEndText, notSelectedEndText.GetLength());
		}
		// draw caret
		if (IsWindowEnabled() && startChar == endChar)
		{
			CSize sizeOfTextBeforeCursor;
			if (startChar == 0)
			{
				sizeOfTextBeforeCursor = memDC.GetDC().GetTextExtent(L"O", 1);
				sizeOfTextBeforeCursor.cx = 0;
			}
			else
			{
				sizeOfTextBeforeCursor = memDC.GetDC().GetTextExtent(Text, startChar);
			}
			CEdit::SetCaretPos(CPoint(textStartX + sizeOfTextBeforeCursor.cx, textStartY - 1));
		}
		if (IsModified())
		{
			CSize fullTextSize = memDC.GetDC().GetTextExtent(Text);
			//HICON hIcon = LoadIcon(IcadSharedGlobals::GetIcadResourceInstance(),MAKEINTRESOURCE(IDI_LOCKER));
			HICON hIcon = (HICON)::LoadImage(IcadSharedGlobals::GetIcadResourceInstance(), MAKEINTRESOURCE(IDI_LOCKER), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
			int x = fullTextSize.cx + 10;
			int y = textStartY;
			DrawIconEx(memDC.GetDC().GetSafeHdc(), x, y, hIcon, IcCuiSizes::Scale(16), IcCuiSizes::Scale(16), 0, NULL, DI_NORMAL | DI_COMPAT);
			DestroyIcon(hIcon);
		}

		if (pFont)
			memDC.GetDC().SelectObject(oldFont);
	}
}

bool CDynamicEdit::IsModified()
{
	bool result = getStatus() == CDynamicEdit::eStateModified;

	/*if (getManager()->getMode() == IIcDynamicInputManager::eModeDimensions &&m_pDimData)
	{
	result = m_pDimData->isConstrained();
	}*/
	return result;
}

bool CDynamicEdit::IsUnchanged()
{
	bool result = getStatus() == CDynamicEdit::eStateUnchanged;
	/*if (m_pDimData)
	{
	result = !m_pDimData->isConstrained();
	}*/
	return result;
}

void CDynamicEdit::OnMouseMove(UINT flags, CPoint pt)
{
	// forward mousemove message to icad
	ClientToScreen(&pt);
	CWnd *currentView = icedGetIcadDwgView();
	currentView->ScreenToClient(&pt);
	if (m_isEditable)
	{
		CEdit::OnMouseMove(flags, pt);
	}
	else
	{
		currentView->SendMessage(WM_MOUSEMOVE, flags, MAKELPARAM(pt.x, pt.y));
	}
}

void CDynamicEdit::OnLButtonDblClk(UINT flags, CPoint pt)
{
	// forward mousemove message to icad
	ClientToScreen(&pt);
	CWnd *currentView = icedGetIcadDwgView();
	currentView->ScreenToClient(&pt);
	currentView->SendMessage(WM_LBUTTONDBLCLK, flags, MAKELPARAM(pt.x, pt.y));
}

void CDynamicEdit::OnLButtonDown(UINT flags, CPoint pt)
{
	// forward mousemove message to icad
	ClientToScreen(&pt);
	CWnd *currentView = icedGetIcadDwgView();
	currentView->ScreenToClient(&pt);

	if (m_isEditable)
	{
		CEdit::OnLButtonDown(flags, pt);
	}
	else
	{
		currentView->SendMessage(WM_LBUTTONDOWN, flags, MAKELPARAM(pt.x, pt.y));
	}
}

void CDynamicEdit::OnLButtonUp(UINT flags, CPoint pt)
{
	// forward mousemove message to icad
	ClientToScreen(&pt);
	CWnd *currentView = icedGetIcadDwgView();
	currentView->ScreenToClient(&pt);
	if (m_isEditable)
	{
		CEdit::OnLButtonUp(flags, pt);
	}
	else
	{
		currentView->SendMessage(WM_LBUTTONUP, flags, MAKELPARAM(pt.x, pt.y));
	}
}

void CDynamicEdit::OnRButtonDblClk(UINT flags, CPoint pt)
{
	// forward mousemove message to icad
	ClientToScreen(&pt);
	CWnd *currentView = icedGetIcadDwgView();
	currentView->ScreenToClient(&pt);
	currentView->SendMessage(WM_RBUTTONDBLCLK, flags, MAKELPARAM(pt.x, pt.y));
}

void CDynamicEdit::OnRButtonDown(UINT flags, CPoint pt)
{
	// forward mousemove message to icad
	ClientToScreen(&pt);
	CWnd *currentView = icedGetIcadDwgView();
	currentView->ScreenToClient(&pt);
	currentView->SendMessage(WM_RBUTTONDOWN, flags, MAKELPARAM(pt.x, pt.y));
}

void CDynamicEdit::OnRButtonUp(UINT flags, CPoint pt)
{
	// forward mousemove message to icad
	ClientToScreen(&pt);
	CWnd *currentView = icedGetIcadDwgView();
	currentView->ScreenToClient(&pt);
	currentView->SendMessage(WM_RBUTTONUP, flags, MAKELPARAM(pt.x, pt.y));
}

void CDynamicEdit::OnMButtonDblClk(UINT flags, CPoint pt)
{
	// forward mousemove message to icad
	ClientToScreen(&pt);
	CWnd *currentView = icedGetIcadDwgView();
	currentView->ScreenToClient(&pt);
	currentView->SendMessage(WM_MBUTTONDBLCLK, flags, MAKELPARAM(pt.x, pt.y));
}

void CDynamicEdit::OnMButtonDown(UINT flags, CPoint pt)
{
	// forward mousemove message to icad
	ClientToScreen(&pt);
	CWnd *currentView = icedGetIcadDwgView();
	currentView->ScreenToClient(&pt);
	currentView->SendMessage(WM_MBUTTONDOWN, flags, MAKELPARAM(pt.x, pt.y));
}

void CDynamicEdit::OnMButtonUp(UINT flags, CPoint pt)
{
	// forward mousemove message to icad
	ClientToScreen(&pt);
	CWnd *currentView = icedGetIcadDwgView();
	currentView->ScreenToClient(&pt);
	currentView->SendMessage(WM_MBUTTONUP, flags, MAKELPARAM(pt.x, pt.y));
}

bool CDynamicEdit::validate(bool bAcceptEsnap)
{
	return true;
}

// *******************************************************************************************************************
// returns true
//
bool CDynamicEdit::isAcceptedChar(wchar_t ch)
{
	if (ch == L'<' && !(m_acceptMask & eAcceptMinor))
		return false;
	else if (ch == L'@' && !(m_acceptMask & eAcceptAt))
		return false;
	else if (ch == L'#' && !(m_acceptMask & eAcceptDash))
		return false;
	else if (ch == L'.' && !(m_acceptMask & eAcceptDot))
		return false;
	else if (ch == L',' && !(m_acceptMask & eAcceptComma))
		return false;
	// todo
	// accept also 1e30 form
	else if (iswdigit(ch) && !(m_acceptMask & eAcceptNumbers))
		return false;
	else if (iswalpha(ch) && !(m_acceptMask & eAcceptAlpha))
		return false;
	else if (ch == L' ' && !(m_acceptMask & eAcceptSpace))
		return false;
	return true;
}

bool CDynamicEdit::isGoToNextEditChar(wchar_t ch)
{
	if (ch == L'<' && (m_skipMask & eSkipOnMinor))
		return true;
	if (ch == L',' && (m_skipMask & eSkipOnComma))
		return true;
	if (ch == L'\t' && (m_skipMask & eSkipOnTab))
		return true;
	return false;
}

bool CDynamicEdit::isEnterChar(wchar_t ch)
{
	if (ch == L' ' && (m_enterMask & eEnterOnSpace))
	{
		CString text;
		GetWindowText(text);
		text.Trim();
		if (text[0] == '(')
			return false;
		else
			return true;
	}
	if (ch == VK_RETURN && (m_enterMask & eEnterOnEnter))
		return true;

	return false;
}

void CDynamicEdit::setDimData(OdDbDimData* pDimData)
{
	m_pDimData = pDimData;
	if (pDimData)
	{
		if (pDimData->isCustomString())
		{
			// TEXT command jig accepts space
			setEnterMask(CDynamicEdit::eEnterOnEnter);
			setCanSwitchToCoordinate(false);
		}
		else if (pDimData->isDimResultantAngle() || pDimData->isDimDeltaAngle())
		{
			m_acceptMask = eAcceptNumbers | eAcceptDot;
		}
		else if (pDimData->isDimRadius() ||
			pDimData->isDimResultantLength() ||
			pDimData->isDimDeltaLength() ||
			pDimData->isCustomDimValue())
		{
			m_acceptMask = eAcceptMinor | eAcceptAt | eAcceptDash | eAcceptDot | eAcceptNumbers;
		}
	}
}

CSize CDynamicEdit::getSize()
{
	if (m_bSizeInvalid)
	{
		CSize size;
		CString currentString;
		GetWindowTextW(currentString);
		CDC *pDC = GetDC();
		CFont *pFont = GetFont();
		if (!pFont)
			pFont = pDC->GetCurrentFont();
		ASSERT(pFont);
		if (!pFont)
		{
			if (pDC)
			{
				ReleaseDC(pDC);
			}
			return size;
		}
		int tooltipsize = 0;
		icedGetVar(L"TOOLTIPSIZE", tooltipsize);
		LOGFONT lf;
		pFont->GetLogFont(&lf);
		lf.lfHeight = CDynamicEdit::getFontSize();
		CFont newFont;
		newFont.CreateFontIndirectW(&lf);
		if (currentString.IsEmpty())
			currentString = L" ";
		if (isAngular())
			currentString = L"< " + currentString;
		if (pDC)
		{
			CFont *pOldFont = pDC->SelectObject(&newFont);
			size = pDC->GetTextExtent(currentString);
			pDC->SelectObject(pOldFont);
			size.cx += 12;
			size.cy += 7;
			ReleaseDC(pDC);
			if (IsModified())
				size.cx += IcCuiSizes::Scale(16) + 3;
			m_size = size;
			m_bSizeInvalid = false;
		}
		else
			ASSERT(FALSE);
	}
	return m_size;
}

bool CDynamicEdit::isDimAngular()
{
	return m_pDimData && (m_pDimData->isDimDeltaAngle() || m_pDimData->isDimResultantAngle());
}

bool CDynamicEdit::setMode(wchar_t ch)
{
	if (!isAcceptedChar(ch))
		return false;
	if (ch == L'#')
	{
		m_bIsRelative = false;
		return true;
	}
	if (ch == L'@')
	{
		m_bIsRelative = true;
		return true;
	}
	return false;
}

void CDynamicEdit::SetWindowTextW(LPCTSTR lpszString)
{
	m_text = lpszString;
	if (IcadSharedGlobals::IsMainThreadCurrent())
		CEdit::SetWindowText(lpszString);
	else
		PostMessage(WM_CUSTOMSETTEXT, 0, (LPARAM)(LPCTSTR)m_text);

	m_bSizeInvalid = true;
}

BOOL CDynamicEdit::ShowWindow(int nCmdShow)
{
	if (IcadSharedGlobals::IsMainThreadCurrent())
	{
		if (nCmdShow == SW_SHOW && !IsWindowVisible() || nCmdShow == SW_HIDE && IsWindowVisible())
		{
			CEdit::ShowWindow(nCmdShow);
			if (nCmdShow == SW_SHOW)
				SetWindowTextW(m_text);
		}
	}
	else
	{
		PostMessage(WM_SHOWWINDOW, (WPARAM)nCmdShow == SW_SHOW ? TRUE : FALSE, 0);
	}

	m_bSizeInvalid = true;
	return TRUE;
}

BOOL CDynamicEdit::SetReadOnly(BOOL bReadOnly)
{
	ASSERT(::IsWindow(m_hWnd));
	if (IcadSharedGlobals::IsMainThreadCurrent())
		return CEdit::SetReadOnly(bReadOnly);
	else
		return (BOOL)PostMessage(EM_SETREADONLY, bReadOnly, 0L);
}

void CDynamicEdit::SetSel(int nStartChar, int nEndChar, BOOL bNoScroll)
{
	if (IcadSharedGlobals::IsMainThreadCurrent())
	{
		CEdit::SetSel(nStartChar, nEndChar, bNoScroll);
	}
	else
	{
		ASSERT(::IsWindow(m_hWnd));
		PostMessage(EM_SETSEL, nStartChar, nEndChar);
		if (!bNoScroll)
			PostMessage(EM_SCROLLCARET, 0, 0);
	}
}

CDynamicInputWnd* CDynamicEdit::GetParent()
{
	CDynamicInputWnd* parent = nullptr;

	if (m_hWnd)
	{
		parent = (CDynamicInputWnd*)CWnd::GetParent();
	}

	return parent;
}

BOOL CDynamicEdit::EnableWindow(BOOL bEnable)
{
	if (IcadSharedGlobals::IsMainThreadCurrent())
		return CWnd::EnableWindow(bEnable);
	else
		PostMessage(WM_CUSTOMENABLEWINDOW, (WPARAM)bEnable, 0);
	return TRUE;
}

LRESULT CDynamicEdit::OnEnableWindow(WPARAM wp, LPARAM lp)
{
	CWnd::EnableWindow((BOOL)wp);
	return 0;
}

LRESULT CDynamicEdit::OnSetText(WPARAM wp, LPARAM lp)
{
	CWnd::SetWindowTextW((const wchar_t *)m_text);
	return 0;
}

static constexpr auto fontSizeToHeightRatio = 1.33;
int CDynamicEdit::getFontSize()
{
	int tooltipsize = 0;
	icedGetVar(L"TOOLTIPSIZE", tooltipsize);
	int cmdbarFontSz = 0;
	icedGetVar(L"CMDBARFONTSIZE", cmdbarFontSz);
	return (int)IcCuiSizes::Scale(cmdbarFontSz * fontSizeToHeightRatio + tooltipsize);
}

//**********************************************************************************************************************
//
//	Dynamic edit control for lockable data
//
CDynamicLockableEdit::CDynamicLockableEdit() : CDynamicEdit()
{

}

IMPLEMENT_DYNAMIC(CDynamicLockableEdit, CDynamicEdit)

BEGIN_MESSAGE_MAP(CDynamicLockableEdit, CDynamicEdit)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CDynamicLockableEdit::OnPaint()
{
	CDynamicEdit::OnPaint();

}
//**********************************************************************************************************************
//
//	Dynamic edit control for real data
//

CDynamicRealEdit::CDynamicRealEdit() : CDynamicLockableEdit()
{
	m_acceptMask = eAcceptMinor | eAcceptAt | eAcceptDash | eAcceptDot | eAcceptNumbers;
}

IMPLEMENT_DYNAMIC(CDynamicRealEdit, CDynamicLockableEdit)

BEGIN_MESSAGE_MAP(CDynamicRealEdit, CDynamicLockableEdit)
END_MESSAGE_MAP()

bool CDynamicRealEdit::validate(bool bAcceptEsnap)
{
	return validReal(bAcceptEsnap);
}

//**********************************************************************************************************************
//
//	Dynamic edit control for int data
//

CDynamicIntEdit::CDynamicIntEdit() : CDynamicLockableEdit()
{
	m_acceptMask = eAcceptNumbers;
}

IMPLEMENT_DYNAMIC(CDynamicIntEdit, CDynamicLockableEdit)

BEGIN_MESSAGE_MAP(CDynamicIntEdit, CDynamicLockableEdit)
END_MESSAGE_MAP()

bool CDynamicIntEdit::validate(bool bAcceptEsnap)
{
	return validInt(bAcceptEsnap);
}

//**********************************************************************************************************************
//
//	Dynamic edit control for angle data
//

CDynamicAngleEdit::CDynamicAngleEdit() : CDynamicLockableEdit()
{
	m_acceptMask = eAcceptNumbers | eAcceptDot;
}

IMPLEMENT_DYNAMIC(CDynamicAngleEdit, CDynamicLockableEdit)

BEGIN_MESSAGE_MAP(CDynamicAngleEdit, CDynamicLockableEdit)
END_MESSAGE_MAP()

bool CDynamicAngleEdit::validate(bool bAcceptEsnap)
{
	return validAngle(bAcceptEsnap);
}

void CDynamicAngleEdit::setValue(const double val)
{
	IcString currentText;
	IIcadServicesManager* pServMgr = getServicesManagerInterface(getIcadInterfaces());
	int aunits;
	icedGetVar(L"AUNITS", aunits);
	//avoid surveyor units
	if (aunits == 4)
		aunits = 0;
	pServMgr->angleToString(1, val, aunits, -1, currentText);

	CDynamicEdit::setValue(currentText);
}

//**********************************************************************************************************************
//
//	Dynamic edit control for string data
//

CDynamicStringEdit::CDynamicStringEdit() : CDynamicEdit()
{
	m_acceptMask = eAcceptAll;
	m_skipMask = eSkipOnTab;
	m_bCanGoToCoordinate = false;
}

IMPLEMENT_DYNAMIC(CDynamicStringEdit, CDynamicEdit)

BEGIN_MESSAGE_MAP(CDynamicStringEdit, CDynamicEdit)
	ON_WM_PAINT()
END_MESSAGE_MAP()

bool CDynamicStringEdit::validate(bool bAcceptEsnap)
{
	return true;
}

CSize CDynamicStringEdit::getSize()
{
	if (isEmpty())
	{
		return CSize(0, CDynamicEdit::getSize().cy);
	}
	return CDynamicEdit::getSize();
}

bool CDynamicStringEdit::validCommand(bool bAcceptEsnap)
{
	IIcadIOManager* pIO = getIOManagerInterface(getIcadInterfaces());
	int ui = pIO->getUserInputControls();

	IcString currentText;
	GetWindowText((LPTSTR)currentText.getBuffer(1024), 1024);
	currentText.releaseBuffer();
	currentText.trimWhiteSpace(L"be");

	if (currentText.isEmpty())
	{
		return (ui & IcUserInputControls::kNullResponseIsNotAccepted) == 0;
	}
	return true;
}

void CDynamicStringEdit::OnPaint()
{
	if (isEmpty())
	{
		CWnd::OnPaint();
	}
	else
	{
		CDynamicEdit::OnPaint();
	}
}

//**********************************************************************************************************************
//
//	Dynamic edit control for dim data
//

CDynamicDimEdit::CDynamicDimEdit() : CDynamicLockableEdit()
{
	m_acceptMask = eAcceptNumbers | eAcceptDot;
}

IMPLEMENT_DYNAMIC(CDynamicDimEdit, CDynamicLockableEdit)

BEGIN_MESSAGE_MAP(CDynamicDimEdit, CDynamicLockableEdit)
END_MESSAGE_MAP()

bool CDynamicDimEdit::validate(bool bAcceptEsnap)
{
	if (m_pDimData)
	{
		if (m_pDimData->isDimRadius() ||
			m_pDimData->isDimResultantLength() ||
			m_pDimData->isDimDeltaLength())
		{
			return validReal(bAcceptEsnap);
		}
		else if (m_pDimData->isDimResultantAngle() || m_pDimData->isDimDeltaAngle())
		{
			return validAngle(bAcceptEsnap);
		}
		else if (m_pDimData->isCustomDimValue())
		{
			return validReal(bAcceptEsnap);
		}
	}

	return CDynamicLockableEdit::validate(bAcceptEsnap);
}


//**********************************************************************************************************************
//
//	Dynamic edit control for prompt message
//

CDynamicStringPromptEdit::CDynamicStringPromptEdit() : CDynamicEdit()
{
	m_acceptMask = eAcceptNone;
	m_bDisplayDownArrow = true;
}

IMPLEMENT_DYNAMIC(CDynamicStringPromptEdit, CDynamicEdit)

BEGIN_MESSAGE_MAP(CDynamicStringPromptEdit, CDynamicEdit)
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()

bool CDynamicStringPromptEdit::validate(bool bAcceptEsnap)
{
	return true;
}

void CDynamicStringPromptEdit::setValue(const OdString val)
{
	const wchar_t* pString = (LPCTSTR)val;
	if (val[0] == L'\n')
		pString++;
	SetWindowTextW((LPCTSTR)pString);
}

CSize CDynamicStringPromptEdit::getSize()
{
	//CSize ret = CDynamicEdit::getSize();
	if (m_bSizeInvalid)
	{
		CSize size;
		CString currentString;
		GetWindowTextW(currentString);
		CDC* pDC = GetDC();
		CFont* pFont = GetFont();
		if (!pFont)
			pFont = pDC->GetCurrentFont();
		ASSERT(pFont);
		if (!pFont)
		{
			if (pDC)
			{
				ReleaseDC(pDC);
			}
			return size;
		}
		int tooltipsize = 0;
		icedGetVar(L"TOOLTIPSIZE", tooltipsize);
		LOGFONT lf;
		pFont->GetLogFont(&lf);
		lf.lfHeight = CDynamicEdit::getFontSize();
		CFont newFont;
		newFont.CreateFontIndirectW(&lf);
		if (currentString.IsEmpty())
			currentString = L" ";
		if (pDC)
		{
			CFont* pOldFont = pDC->SelectObject(&newFont);
			size = pDC->GetTextExtent(currentString);
			pDC->SelectObject(pOldFont);
			size.cx += 12;
			size.cy += 7;
			ReleaseDC(pDC);
			m_size = size;
			m_bSizeInvalid = false;
		}
		else
			ASSERT(FALSE);

		if (m_bDisplayDownArrow && getManager() && getManager()->hasOptions())
		{
			m_size.cx += IcCuiSizes::Scale(16);
		}
	}

	return m_size;
}

void CDynamicStringPromptEdit::OnPaint()
{
	int textStartX = 5;
	int textStartY = 3;

	CRect rectUpdate;
	if (GetUpdateRect(rectUpdate) != 0)
	{
		OdCmColor colText;
		OdCmColor colBkg;
		int tilemode, cvport;
		icedGetVar(L"TILEMODE", tilemode);
		icedGetVar(L"CVPORT", cvport);
		if (!tilemode && cvport == 1)
		{
			icedGetVar(L"DYNTOOLTIPCOLORLAYOUTTEXT", colText);
			icedGetVar(L"DYNTOOLTIPCOLORLAYOUTBKG", colBkg);
		}
		else
		{
			icedGetVar(L"DYNTOOLTIPCOLORMODELTEXT", colText);
			icedGetVar(L"DYNTOOLTIPCOLORMODELBKG", colBkg);
		}
		CPaintDC dc(this);
		CRect srect;
		GetClientRect(srect);
		CMemDC memDC(dc, srect);
		// to make textout function write only inside text box
		CRgn region;
		region.CreateRectRgnIndirect(rectUpdate);
		memDC.GetDC().SelectClipRgn(&region);

		CBrush blackBrush;
		DWORD blackColor = 0;
		blackBrush.CreateSolidBrush(blackColor);
		memDC.GetDC().SelectObject(&blackBrush);
		memDC.GetDC().Rectangle(rectUpdate);

		DWORD BackColor;
		BackColor = RGB(colBkg.red(), colBkg.green(), colBkg.blue());// GetSysColor( COLOR_INACTIVECAPTION );
		memDC.GetDC().FillSolidRect(rectUpdate.left + 1, rectUpdate.top + 1, rectUpdate.Width() - 2, rectUpdate.Height() - 2, BackColor);

		// Rewrite the text since the backcolor paint
		// overwrote the existing text
		CString Text;
		GetWindowText(Text);
		Text.Replace(L"\n", L"");

		if (IsWindowEnabled())
			memDC.GetDC().SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		else
			memDC.GetDC().SetTextColor(RGB(colText.red(), colText.green(), colText.blue())); // GetSysColor(COLOR_INACTIVECAPTIONTEXT));

		CFont *pFont = GetFont();
		LOGFONT lf;
		if (pFont)
		    pFont->GetLogFont(&lf);
		lf.lfHeight = CDynamicEdit::getFontSize();
		CFont newFont;
		newFont.CreateFontIndirectW(&lf);

		CFont *oldFont = NULL;
		if (pFont)
			oldFont = memDC.GetDC().SelectObject(&newFont);
		textStartX = textStartX + rectUpdate.left;
		textStartY = textStartY + rectUpdate.top;
		memDC.GetDC().TextOut(textStartX, textStartY, Text.GetBuffer(Text.GetLength()));
		if (m_bDisplayDownArrow && getManager() && getManager()->hasOptions())
		{
			CSize fullTextSize = memDC.GetDC().GetTextExtent(Text);
			HICON hIcon = (HICON)::LoadImage(IcadSharedGlobals::GetIcadResourceInstance(), MAKEINTRESOURCE(IDI_DOWNARROW), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
			int x = fullTextSize.cx + 8;
			int y = textStartY + fullTextSize.cy / IcCuiSizes::Scale(2) - 5;
			DrawIconEx(memDC.GetDC().GetSafeHdc(), x, y, hIcon, IcCuiSizes::Scale(16), IcCuiSizes::Scale(16), 0, NULL, DI_NORMAL | DI_COMPAT);
			DestroyIcon(hIcon);

		}
		if (pFont)
			memDC.GetDC().SelectObject(oldFont);
	}
}

//**********************************************************************************************************************
//
//	Dynamic window parent for option list box
//

CDynamicListBoxInputWnd::CDynamicListBoxInputWnd(CWnd *pParent) : CDynamicInputWnd(pParent)
{
	m_bDisableMouse = true;
}

IMPLEMENT_DYNAMIC(CDynamicListBoxInputWnd, CDynamicInputWnd)

BEGIN_MESSAGE_MAP(CDynamicListBoxInputWnd, CDynamicInputWnd)
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CDynamicListBoxInputWnd::OnKeyDown(UINT nChar, UINT nRep, UINT flag)
{
	if (!m_hWnd)
		return;
	HWND hChild = ::GetWindow(m_hWnd, GW_CHILD);
	if (hChild && IsWindow(hChild) && ::IsWindowVisible(hChild))
	{
		CWnd *currentWnd = CWnd::FromHandle(hChild);
		CDynamicOptionListBox *pWnd = (CDynamicOptionListBox *)currentWnd;
		if (pWnd && pWnd->getManager())
			pWnd->getManager()->sendKey(nChar, MAKELPARAM(nRep, flag));
	}
}

void CDynamicListBoxInputWnd::OnSize(UINT nType, int cx, int cy)
{
	//CDynamicInputWnd::OnSize(nType, cx, cy);
	CWnd::OnSize(nType, cx, cy);
	if (!m_hWnd)
		return;
	HWND hChild = ::GetWindow(m_hWnd, GW_CHILD);
	if (hChild && IsWindow(hChild) && ::IsWindowVisible(hChild))
	{
		CWnd *currentWnd = CWnd::FromHandle(hChild);
		CDynamicOptionListBox *pWnd = (CDynamicOptionListBox *)currentWnd;

		CRect cr;
		GetClientRect(cr);
		if (pWnd)
		{
			pWnd->MoveWindow(0, 0, cr.right, cr.bottom);
			pWnd->UpdateWindow();

		}
	}
}

BOOL CDynamicListBoxInputWnd::ShowWindow(int nCmdShow)
{
	if (IcadSharedGlobals::IsMainThreadCurrent())
		CWnd::ShowWindow(nCmdShow);
	else
		PostMessage(WM_SHOWWINDOW, (WPARAM)nCmdShow == SW_SHOW ? TRUE : FALSE, 0);
	return TRUE;
}
//**********************************************************************************************************************
//
//	Dynamic option list control
//

CDynamicOptionListBox::CDynamicOptionListBox() : CListCtrl()
{
	m_pUiManager = NULL;
	m_optionMandatory = false;
	m_defaultIndex = -1;
	m_curIndex = -1;
}

IMPLEMENT_DYNAMIC(CDynamicOptionListBox, CListCtrl)

BEGIN_MESSAGE_MAP(CDynamicOptionListBox, CListCtrl)
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_KEYUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CDynamicOptionListBox::Show()
{
	int perc = 0;
	icedGetVar(L"TOOLTIPTRANSPARENCY", perc);
	if (GetParent() && GetParent()->m_hWnd && !GetParent()->IsWindowVisible())
	{
		GetParent()->ShowWindow(SW_SHOWNOACTIVATE);
	}
	SetTrasparentWindow(GetParent(), perc);
	if (m_hWnd && !IsWindowVisible())
	{
		ShowWindow(SW_SHOWNOACTIVATE);
	}
	SetTrasparentWindow(this, perc);
	isChanged = false;
	//this->SetFocus();
}

void CDynamicOptionListBox::Hide()
{
	if (GetParent() && GetParent()->m_hWnd && GetParent()->IsWindowVisible())
	{
		GetParent()->ShowWindow(SW_HIDE);
	}

	if (m_hWnd && IsWindowVisible())
	{
		ShowWindow(SW_HIDE);
	}
}

CSize CDynamicOptionListBox::getSize()
{
	CSize size(0, 0);
	CSize itemSize(0, 0);
	CString currentString;

	CDC *pDC = GetDC();

	CFont *pFont = GetFont();
	if (!pFont)
		pFont = pDC->GetCurrentFont();
	ASSERT(pFont);
	if (!pFont)
	{
		if (pDC)
		{
			ReleaseDC(pDC);
		}
		return size;
	}
	int tooltipsize = 0;
	icedGetVar(L"TOOLTIPSIZE", tooltipsize);
	LOGFONT lf;
	pFont->GetLogFont(&lf);

	lf.lfHeight = CDynamicEdit::getFontSize();
	CFont newFont;
	newFont.CreateFontIndirectW(&lf);

	CFont *pOldFont = pDC->SelectObject(&newFont);
	for (int i = 0; i != GetItemCount(); i++)
	{
		currentString = GetItemText(i, 0);
		itemSize = pDC->GetTextExtent(currentString);
		if (itemSize.cx> size.cx)
			size.cx = itemSize.cx;
		RECT rect;
		GetItemRect(i, &rect, LVIR_BOUNDS);
		size.cy = rect.bottom;
	}

	int offset = pDC->GetTextExtent(_T(" "), 1).cx * 2;

	pDC->SelectObject(pOldFont);
	bool hasDefault = m_defaultIndex >= 0;
	const int defaultValueOffset = IcCuiSizes::Scale((int)(hasDefault ? kDefaultValueOffset: 0));
	size.cx += 10 + offset + defaultValueOffset;
	//size.cy +=  6;
	ReleaseDC(pDC);

	return size;
}

void CDynamicOptionListBox::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);
	this->SetColumnWidth(0, cx);
}

void CDynamicOptionListBox::OnKillFocus(CWnd *pNewWnd)
{
	ASSERT(getManager());
	if (getManager())
		// if user have to choose between options don't hide option list
		if (!m_optionMandatory)
			getManager()->destroyOptionList();
}


void CDynamicOptionListBox::OnLButtonDown(UINT flags, CPoint point)
{
	BOOL bOutside = false;
	int item = HitTest(point);
	if (item>-1)
	{
		CString text, command;
		text = GetItemText(item, 0);
		if (!m_globalOptions.IsEmpty())
		{
			INT_PTR index = (INT_PTR)GetItemData(item);
			if (index >= 0 && index < m_globalOptions.GetSize())
			{
				if (text.CompareNoCase(ResourceString(IDC_ICADPROMPTMENU_CANCEL_6, "CANCEL")) == 0)
				{
					ASSERT(getManager());
					if (getManager())
						getManager()->destroyOptionList();
					IcCoreExternal::postIcInputEventToQueue(WM_MENUSELECT, 0, (LPARAM)L"^C");
					return;
				}
				else if (m_globalOptions.GetAt(index).CompareNoCase(L"`") == 0)
				{
					ASSERT(getManager());
					if (getManager())
						getManager()->destroyOptionList();
					IcCoreExternal::postIcInputEventToQueue(WM_MENUSELECT, 0, (LPARAM)L"");
					return;
				}
				else if (text.CompareNoCase(L"~") == 0)
				{
				}
				else
				{
					text = m_globalOptions.GetAt(index);
				}
			}
		}
		
		command = getCommandToSend(text, true);
		IIcadIOManager* pIO = getIOManagerInterface(getIcadInterfaces());
	//	pIO->sendMessage(WM_MENUSELECT, 0, (LPARAM)(LPCTSTR)command);
		for (int i = 0; i != command.GetLength(); i++)
		{
			pIO->sendMessage(WM_CHAR, command[i], 0);
		}

		ASSERT(getManager());
		if (getManager())
			getManager()->destroyOptionList();
		pIO->sendMessage(WM_CHAR, 13, 0); //enter
	}
}

void CDynamicOptionListBox::OnMouseMove(UINT flags, CPoint point)
{
	BOOL bOutside = false;
	int item = HitTest(point);
	if (item >= 0 && item != GetCurSel())
	{
		//SetSelectionMark( item );
		SetItem(item, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
		SetCurSel(item);
		UpdateWindow();
	}
}

void CDynamicOptionListBox::OnTimer(UINT_PTR timerId)
{
	KillTimer(timerId);
	if (timerId == kTimerId)
	{

		m_pressedChars = L"";
	}

}

void CDynamicOptionListBox::OnChar(UINT nChar, UINT nRep, UINT flag)
{
	wchar_t charPressed[2] = { (wchar_t)nChar , 0 };
	_wcsupr(charPressed);
	m_pressedChars += charPressed;
	SetTimer(kTimerId, 500, NULL);// set timer to 300 mseconds to store many keypressed

	if (m_pressedChars == VK_RETURN && (m_defaultIndex >= 0 || m_curIndex >= 0))
	{
		SetCurSel(m_curIndex >= 0? m_curIndex: m_defaultIndex);
		return;
	}

	int curSel = GetCurSel();
	// starting from current selected item search first item
	// 1. select item with equal capital letters
	for (int i = 0, index = curSel + 1; i != GetItemCount(); i++, index++)
	{
		CString itemCapitalLetters = L"";
		// restart search from the beg of list
		if (index >= GetItemCount())
			index = 0;
		CString text = GetItemText(index, 0);
		for (int j = 0; j != text.GetLength(); j++)
		{
			if (text[j] == L'?' || IcCommonExternal::isDigit(text[j]) || IcCommonExternal::isUpper(text[j]))
			{
				itemCapitalLetters += text[j];
			}
		}
		// if all capital letters are all equal
		if (itemCapitalLetters == m_pressedChars)
		{
			m_curIndex = index;
			SetCurSel(index);
			return;
		}
	}

	// 2. select item with starting capital letters
	for (int i = 0, index = curSel + 1; i != GetItemCount(); i++, index++)
	{
		CString itemCapitalLetters = L"";
		// restart search from the beg of list
		if (index >= GetItemCount())
			index = 0;
		CString text = GetItemText(index, 0);
		for (int j = 0; j != text.GetLength(); j++)
		{
			if (text[j] == L'?' || IcCommonExternal::isDigit(text[j]) || IcCommonExternal::isUpper(text[j]))
			{
				itemCapitalLetters += text[j];
			}
		}
		// if only a part of pressed chars are in item capital letters
		for (int j = 0; j != m_pressedChars.GetLength(); j++)
		{
			if ((itemCapitalLetters.GetLength()<j) || (itemCapitalLetters[j] != m_pressedChars[j]))
				break;
			if (j == m_pressedChars.GetLength() - 1)
			{
				m_curIndex = index;
				SetCurSel(index);
				return;
			}
		}
	}
}

void CDynamicOptionListBox::OnKeyUp(UINT nChar, UINT nRep, UINT flag)
{
	IIcadIOManager* pIO = getIOManagerInterface(getIcadInterfaces());
	int curSel = GetCurSel();
	if (nChar == VK_RETURN || nChar == VK_SPACE)
	{
		if (curSel >= 0 && isChanged)
		{
			CString text, command;
			text = GetItemText(curSel, 0);
			if (text.CompareNoCase(ResourceString(IDC_ICADPROMPTMENU_CANCEL_6, "CANCEL")) == 0)
			{
				ASSERT(getManager());
				if (getManager())
					getManager()->destroyOptionList();
				IcCoreExternal::postIcInputEventToQueue(WM_MENUSELECT, 0, (LPARAM)L"^C");
				return;
			}
			else if (text.CompareNoCase(L"`") == 0)
			{
				ASSERT(getManager());
				if (getManager())
					getManager()->destroyOptionList();
				IcCoreExternal::postIcInputEventToQueue(WM_MENUSELECT, 0, (LPARAM)L"");
				return;
			}
			else if (text.CompareNoCase(L"~") == 0)
			{
			}
			else
			{
				command = getCommandToSend(text, true);
			}

			for (int i = 0; i != command.GetLength(); i++)
			{
				pIO->sendMessage(WM_CHAR, command[i], 0);
			}

			ASSERT(getManager());
			if (getManager())
				getManager()->destroyOptionList();
			pIO->sendMessage(WM_CHAR, 13, 0); //enter
			//pIO->sendMessage(WM_MENUSELECT, 0, (LPARAM)(LPCTSTR)command);

		}
		else
		{
			// none selected, send enter
			pIO->sendMessage(WM_MENUSELECT, 0, (LPARAM)(LPCTSTR)L"");
		}
	}
}

int CDynamicOptionListBox::GetCurSel()
{
	for (int i = 0; i != GetItemCount(); i++)
	{
		if (GetItemState(i, LVIS_SELECTED) & LVIS_SELECTED)
			return i;
	}
	return -1;
}

void CDynamicOptionListBox::SetCurSel(int index)
{
	SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
}

int CDynamicOptionListBox::SelectString(const int startindex, const CString textToSelect)
{
	for (int i = startindex; i < GetItemCount(); i++)
	{
		if (GetItemText(i, 0) == textToSelect)
		{
			SetCurSel(i);
			return i;
		}
	}
	return -1;
}

void CDynamicOptionListBox::ResetContent()
{
	DeleteAllItems();
	m_globalOptions.RemoveAll();
	m_defaultIndex = -1;
	m_curIndex = - 1;
}

int CDynamicOptionListBox::AddString(CString str)
{
	return InsertItem(GetItemCount(), str);
}

int CDynamicOptionListBox::AddString(CString str, CString strGlobal)
{
	int indexList = InsertItem(GetItemCount(), str);
	INT_PTR indexData = m_globalOptions.Add(strGlobal);
	ASSERT(indexList == (int)indexData);
	BOOL st = SetItemData(indexList, (DWORD_PTR)indexData);
	ASSERT(st);
	return indexList;
}

void CDynamicOptionListBox::OnKeyDown(UINT nChar, UINT nRep, UINT flag)
{
	int i = 0;
	if (!m_hWnd)
		return;

	int curSel = GetCurSel();
	if (nChar == VK_DOWN)
	{
		if (curSel != GetItemCount() - 1)
		{
			m_curIndex = curSel + 1;
			while (GetItemText(m_curIndex, 0).Compare(L"~") == 0 )
			{
				m_curIndex++;
				if (m_curIndex == GetItemCount())
				{
					return;
				}
			}
			SetCurSel(m_curIndex);
			isChanged = true;
			if (m_defaultIndex >= 0)
				Update(m_defaultIndex);
		}
	}
	else if (nChar == VK_UP)
	{
		if (curSel > 0)
		{
			m_curIndex = curSel - 1;
			while (GetItemText(m_curIndex, 0).Compare(L"~") == 0)
			{
				m_curIndex--;
				if (m_curIndex == 0)
				{
					return;
				}
			}
			SetCurSel(m_curIndex);
			isChanged = true;
			if (m_defaultIndex >= 0)
				Update(m_defaultIndex);
		}
	}
}

void CDynamicOptionListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	// Save dc state
	int nSavedDC = pDC->SaveDC();

	CRect rcItem(lpDrawItemStruct->rcItem);
	int nItem = lpDrawItemStruct->itemID;

	// Get item image and state info
	LV_ITEM lvi;
	lvi.mask = LVIF_IMAGE | LVIF_STATE;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.stateMask = 0xFFFF;		// get all state flags
	GetItem(&lvi);

	// Should the item be highlighted
	BOOL bHighlight = (lvi.state & LVIS_DROPHILITED) || (lvi.state & LVIS_SELECTED);
	bool hasDefault = m_defaultIndex >= 0;
	const int defaultValueOffset = IcCuiSizes::Scale((int)(hasDefault ? kDefaultValueOffset: 0));

	// Get rectangles for drawing
	CRect rcBounds, rcLabel, rcIcon;
	GetItemRect(nItem, rcBounds, LVIR_BOUNDS);
	GetItemRect(nItem, rcLabel, LVIR_LABEL);
	GetItemRect(nItem, rcIcon, LVIR_ICON);
	POINT bottomRight = rcBounds.BottomRight();
	bottomRight.x += defaultValueOffset;
	rcBounds.SetRect(rcBounds.TopLeft(), bottomRight);
	CRect rcCol(rcBounds);

	CString sLabel = GetItemText(nItem, 0);

	CFont *pFont = GetFont();
	int tooltipsize = 0;
	icedGetVar(L"TOOLTIPSIZE", tooltipsize);
	LOGFONT lf;
	if (pFont)
	    pFont->GetLogFont(&lf);
	lf.lfHeight = CDynamicEdit::getFontSize();
	CFont newFont;
	newFont.CreateFontIndirectW(&lf);

	CFont *oldFont = NULL;
	if (pFont)
		oldFont = pDC->SelectObject(&newFont);

	// Labels are offset by a certain amount
	// This offset is related to the width of a space character
	int offset = pDC->GetTextExtent(_T(" "), 1).cx * 2;

	CRect rcHighlight;
	CRect rcWnd;
	//	int nExt;

	GetClientRect(&rcWnd);
	rcHighlight = rcBounds;
	rcHighlight.left = rcLabel.left;
	rcHighlight.right = rcWnd.right + defaultValueOffset;

	// Draw the background color
	if (bHighlight && sLabel.Compare(L"~")==-1)
	{
		pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
		pDC->FillRect(rcHighlight, &CBrush(::GetSysColor(COLOR_HIGHLIGHT)));
	}
	else
	{
		pDC->FillRect(rcHighlight, &CBrush(::GetSysColor(COLOR_WINDOW)));
	}

	// Set clip region
	rcCol.right = rcCol.left + GetColumnWidth(0) + defaultValueOffset;
	CRgn rgn;
	rgn.CreateRectRgnIndirect(&rcCol);
	pDC->SelectClipRgn(&rgn);
	rgn.DeleteObject();


	// Draw item label - Column 0
	rcLabel.left += offset / 2 + defaultValueOffset;
	rcLabel.right -= offset - defaultValueOffset;
	if (sLabel.Compare(L"~") == 0)
	{
		CRect rect(rcBounds);
		rect.left += 3;
		rect.right -= 3;
		CPen pen1(PS_SOLID, 1, afxGlobalData.clrBarShadow);
		CPen* pOldPen = pDC->SelectObject(&pen1);
		CPoint point;
		point.x = rect.left;
		point.y = (rect.top + ((rect.bottom - rect.top) / 2 - 2) + 2);
		pDC->MoveTo(point);
		point.x = rect.right + 1;
		pDC->LineTo(point);
		pDC->SelectObject(pOldPen);

		CPen pen2(PS_SOLID, 1, afxGlobalData.clrBarHilite);
		pOldPen = pDC->SelectObject(&pen2);
		point.x = rect.left;
		point.y++;
		pDC->MoveTo(point);
		point.x = rect.right + 1;
		pDC->LineTo(point);
		pDC->SelectObject(pOldPen);
	}
	else
	{
		pDC->DrawText(sLabel, -1, rcLabel, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS);
	}


	// Draw	default value marker
	int defaultIndex = m_curIndex >= 0? m_curIndex: m_defaultIndex;
	if (hasDefault && defaultIndex == nItem)
	{
		const int circleOffset = IcCuiSizes::Scale(6);
		CRect rcCircle(rcBounds);
		rcCircle.top += circleOffset;
		rcCircle.bottom -= circleOffset;
		rcCircle.left += circleOffset;
		rcCircle.right = rcCircle.left + rcCircle.Height();
		CBrush* oldBrush = pDC->GetCurrentBrush();
		CBrush markerBrush;
		markerBrush.CreateSolidBrush(RGB(0, 0, 0));

		pDC->SelectObject(markerBrush);
		pDC->Ellipse(rcCircle);
		pDC->SelectObject(oldBrush);
	}

	// Draw labels for remaining columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH;


	rcBounds.right = (rcHighlight.right > rcBounds.right ?
		rcHighlight.right : rcBounds.right) + defaultValueOffset;
	rgn.CreateRectRgnIndirect(&rcBounds);
	pDC->SelectClipRgn(&rgn);

	// Draw focus rectangle if item has focus
	if (lvi.state & LVIS_FOCUSED && (GetFocus() == this))
		pDC->DrawFocusRect(rcHighlight);

	pDC->SelectObject(oldFont);

	// Restore dc
	pDC->RestoreDC(nSavedDC);

}
BOOL CDynamicOptionListBox::ShowWindow(int nCmdShow)
{
	if (IcadSharedGlobals::IsMainThreadCurrent())
		CWnd::ShowWindow(nCmdShow);
	else
		PostMessage(WM_SHOWWINDOW, (WPARAM)nCmdShow == SW_SHOW ? TRUE : FALSE, 0);
	return TRUE;
}

bool CDynamicOptionListBox::MessageHandler(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ((!m_pUiManager->getActiveControl() || m_pUiManager->getActiveControl()->getStatus() != CDynamicEdit::eStateEditing) &&
		m_hWnd && IsWindowVisible())
	{
		if ((message == WM_KEYDOWN || message == WM_CHAR)&&
			(wParam == VK_UP || wParam == VK_DOWN || wParam == VK_RETURN || wParam == VK_SPACE))
		{
			SendMessage(WM_KEYDOWN, wParam, lParam);
			wchar_t ch = vkToChar((UINT)wParam, (UINT)lParam);
			if (ch)
				SendMessage(WM_CHAR, ch, lParam);
			SendMessage(WM_KEYUP, wParam, lParam);
			return true;
		}
		if (message == WM_KEYDOWN && wParam == VK_ESCAPE)
		{
			m_pUiManager->destroyOptionList();
			return false;
		}
	}
	return false;
}

void CDynamicOptionListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	TEXTMETRIC tm;
	HDC hDC = ::GetDC(NULL);
	CFont* pFont = GetFont();
	HFONT hFontOld = NULL;

	if (pFont)
        hFontOld = (HFONT)SelectObject(hDC, pFont->GetSafeHandle());
	GetTextMetrics(hDC, &tm);
	lpMeasureItemStruct->itemHeight = tm.tmHeight + tm.tmExternalLeading + 5;
	SelectObject(hDC, hFontOld);
	::ReleaseDC(NULL, hDC);
}

//**********************************************************************************************************************
//
//	Dynamic edit control for command string data
//

CDynamicCommandStringEdit::CDynamicCommandStringEdit() : CDynamicEdit()
{
	m_acceptMask = eAcceptAll;
	m_skipMask = eSkipOnTab;
	m_bCanGoToCoordinate = false;
}

IMPLEMENT_DYNAMIC(CDynamicCommandStringEdit, CDynamicEdit)

BEGIN_MESSAGE_MAP(CDynamicCommandStringEdit, CDynamicEdit)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE() // progesoft
	ON_WM_MOUSEWHEEL() // progesoft
END_MESSAGE_MAP()

bool CDynamicCommandStringEdit::validate(bool bAcceptEsnap)
{
	return true;
}

// progesoft [-
void	CDynamicCommandStringEdit::setStatus(State status)
{
	if (m_status == status)
		return;

	if (m_status == State::eStateEditing && ::GetFocus() == m_hWnd)
	{
		if (CWnd* currentView = icedGetIcadDwgView())
		{
			currentView->SetFocus();
		}
	}

	m_status = status;
	m_bSizeInvalid = true;

	if (m_status == State::eStateEditing && ::GetFocus() != m_hWnd)
	{
		// this is performed by timer...
		// SetFocus();
	}
}

void CDynamicCommandStringEdit::OnMouseMove(UINT flags, CPoint pt)
{
	ClientToScreen(&pt);
	CWnd* currentView = icedGetIcadDwgView();
	if (currentView)
	{
		currentView->ScreenToClient(&pt);
		currentView->SendMessage(WM_MOUSEMOVE, flags, MAKELPARAM(pt.x, pt.y));
	}
}
BOOL CDynamicCommandStringEdit::OnMouseWheel(UINT flags, short zDelta, CPoint pt)
{
	//ClientToScreen( &pt );
	CWnd* currentView = icedGetIcadDwgView();
	if (currentView)
	{
		//currentView->ScreenToClient( &pt );
		currentView->SendMessage(WM_MOUSEWHEEL, MAKEWPARAM(flags, zDelta), MAKELPARAM(pt.x, pt.y));
	}
	return FALSE;
}
// progesoft -]

void CDynamicCommandStringEdit::AutocompleteTextHighlighted(CString text)
{
	CString userText;
	GetWindowText(userText);
	int startSel, endSel;
	GetSel(startSel, endSel);
	int len = userText.GetLength();
	CString suggestion(text);
	setValue(text.GetBuffer());
	SetSel(min(startSel, len), max(endSel, text.GetLength()));
}

//**********************************************************************************************************************
//
//	Dynamic window parent for command list box
//

CDynamicCommandListBoxInputWnd::CDynamicCommandListBoxInputWnd(CWnd *pParent) : CDynamicInputWnd(pParent)
{
	m_bDisableMouse = true;
}

IMPLEMENT_DYNAMIC(CDynamicCommandListBoxInputWnd, CDynamicInputWnd)

BEGIN_MESSAGE_MAP(CDynamicCommandListBoxInputWnd, CDynamicInputWnd)
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CDynamicCommandListBoxInputWnd::OnKeyDown(UINT nChar, UINT nRep, UINT flag)
{
	if (!m_hWnd)
		return;

	HWND hChild = ::GetWindow(m_hWnd, GW_CHILD);
	if (hChild && IsWindow(hChild) && ::IsWindowVisible(hChild))
	{
		
		CWnd *currentWnd = CWnd::FromHandle(hChild);
		CDynamicCommandListBox *pWnd = (CDynamicCommandListBox *)currentWnd;
		if (pWnd && pWnd->getManager())
		{
			pWnd->getManager()->onKeyboardMessage(GetSafeHwnd(), WM_KEYDOWN, nChar, MAKELPARAM(nRep, flag));
			//pWnd->getManager()->sendKey(nChar, MAKELPARAM(nRep, flag));
		}
	}
}

void CDynamicCommandListBoxInputWnd::OnSize(UINT nType, int cx, int cy)
{
	/*CDynamicInputWnd::OnSize(nType, cx, cy);*/

	if (!m_hWnd)
		return;
	HWND hChild = ::GetWindow(m_hWnd, GW_CHILD);
	if (hChild && IsWindow(hChild) /*&& ::IsWindowVisible(hChild)*/)
	{
		CWnd *currentWnd = CWnd::FromHandle(hChild);
		CDynamicCommandListBox *pWnd = (CDynamicCommandListBox *)currentWnd;

		CRect cr;
		GetClientRect(cr);
		if (pWnd)
		{
			pWnd->MoveWindow(0, 0, cr.right, cr.bottom);
			pWnd->UpdateWindow();
		}
	}
}

BOOL CDynamicCommandListBoxInputWnd::ShowWindow(int nCmdShow)
{
	if (IcadSharedGlobals::IsMainThreadCurrent())
		CWnd::ShowWindow(nCmdShow);
	else
		PostMessage(WM_SHOWWINDOW, (WPARAM)nCmdShow == SW_SHOW ? TRUE : FALSE, 0);
	return TRUE;
}

//**********************************************************************************************************************
//
//	Dynamic command list control
//

CDynamicCommandListBox::CDynamicCommandListBox() : CListCtrl()
{
	m_pUiManager = NULL;
	m_optionMandatory = false;
	m_bPostPoneShow = false;

	m_icons.SetImageSize(IcCuiSizes::GetIconSize(FALSE));

	if(HBITMAP hBmp = IcIconUtils::LoadIcadBitmap(NULL, IDB_AUTOCOMP_SYSVARS, CUI_ICON_SIZE_TYPE::SMALL_SCALED))
	{
		VERIFY( IcIconUtils::AddBitmap(hBmp, &m_icons) == GetIconIndexByType(true) );
	}
	if(HBITMAP hBmp = IcIconUtils::LoadIcadBitmap(NULL, IDB_AUTOCOMP_ICCMD, CUI_ICON_SIZE_TYPE::SMALL_SCALED))
	{
		VERIFY( IcIconUtils::AddBitmap(hBmp, &m_icons) == GetIconIndexByType(false) );
	}

	if (HBITMAP hBmp = IcIconUtils::LoadIcadBitmap(NULL, IDB_AUTOCOMP_CMDHELP, CUI_ICON_SIZE_TYPE::SMALL_SCALED))
	{
		m_helpIconId = IcIconUtils::AddBitmap(hBmp, &m_icons);
	}
}

IMPLEMENT_DYNAMIC(CDynamicCommandListBox, CListCtrl)

BEGIN_MESSAGE_MAP(CDynamicCommandListBox, CListCtrl)
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_KEYUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_TIMER()
	ON_WM_NCCALCSIZE()
END_MESSAGE_MAP()

void CDynamicCommandListBox::Show()
{
	int perc = 0;
	icedGetVar(L"TOOLTIPTRANSPARENCY", perc);
	if (GetParent() && GetParent()->m_hWnd && !GetParent()->IsWindowVisible())
	{
		GetParent()->ShowWindow(SW_SHOWNOACTIVATE);
	}
	SetTrasparentWindow(GetParent(), perc);
	if (m_hWnd && !IsWindowVisible())
	{
		ShowWindow(SW_SHOWNOACTIVATE);
	}
	SetTrasparentWindow(this, perc);
	ShowScrollBar(SB_HORZ,true);
}

void CDynamicCommandListBox::Hide()
{
	if (GetParent() && GetParent()->m_hWnd && GetParent()->IsWindowVisible())
	{
		GetParent()->ShowWindow(SW_HIDE);
	}

	if (m_hWnd && IsWindowVisible())
	{
		ShowWindow(SW_HIDE);
	}
}

CSize CDynamicCommandListBox::getSize()
{
	CSize size(0, 0);
	CSize itemSize(0, 0);
	CString currentString;

	CDC *pDC = GetDC();

	CFont *pFont = GetFont();
	if (!pFont)
		pFont = pDC->GetCurrentFont();
	ASSERT(pFont);
	if (!pFont)
	{
		if (pDC)
		{
			ReleaseDC(pDC);
		}
		return size;
	}
	int tooltipsize = 0;
	icedGetVar(L"TOOLTIPSIZE", tooltipsize);
	LOGFONT lf;
	pFont->GetLogFont(&lf);

	lf.lfHeight = CDynamicEdit::getFontSize();
	CFont newFont;
	newFont.CreateFontIndirectW(&lf);

	CFont *pOldFont = pDC->SelectObject(&newFont);
	int j = 0;
	for (int i = 0; i != GetItemCount(); i++)
	{
		RECT rect;
		GetItemRect(i, &rect, LVIR_BOUNDS);
		currentString = GetItemText(i, 0);
		itemSize = pDC->GetTextExtent(currentString);
		if (itemSize.cx > size.cx)
		{
			size.cx = itemSize.cx;
		}

		if (rect.top >= 0 && j < 7)
		{
			size.cy = rect.bottom;
			j++;
		}
	}

	int offset = pDC->GetTextExtent(_T(" "), 1).cx * 2 + pDC->GetTextExtent(_T(" "), 1).cy;

	pDC->SelectObject(pOldFont);
	size.cx += IcCuiSizes::Scale(33) +offset;
	//size.cy +=  6;
	ReleaseDC(pDC);

	return size;
}

void CDynamicCommandListBox::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);
	this->SetColumnWidth(0, cx);
}

void CDynamicCommandListBox::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	ModifyStyle(WS_HSCROLL, 0);
	CListCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);
}

void CDynamicCommandListBox::OnKillFocus(CWnd *pNewWnd)
{
	ASSERT(getManager());
	if (getManager())
		// if user have to choose between options don't hide option list
		if (!m_optionMandatory)
			getManager()->destroyCommandList();
}

void CDynamicCommandListBox::OnLButtonDown(UINT flags, CPoint point)
{
	BOOL bOutside = false;
	int item = HitTest(point);
	if (item>-1)
	{
		auto destroyCommandList = [&]()
		{
			ASSERT(getManager());
			if (getManager())
			{
				getManager()->destroyCommandList();
				getManager()->getBoxContainer()->getCommandStringBox()->Hide();
				getManager()->disableActiveControl();
			}
		};
		CString text, command;
		text = GetItemText(item, 0);
		command = getCommandToSend(text, false);

		CRect rect(0, 0, 0, 0);
		GetItemRect(item, rect, LVIR_BOUNDS);
		rect.left = rect.right - IcCuiSizes::Scale(17);

		if (rect.PtInRect(point))
		{
			IIcadSysvarManager* pISysvar = getSysvarManagerInterface();
			IIcadIOManager* pIO = getIOManagerInterface();

			pISysvar->setVar(L"ISMULTIPLEMODE", 0);

			IcString helpFileName;
			if (pIO->findFile(IcadHelpData::getIcadHelpFileName(), helpFileName) != RTNORM)
			{
				wchar_t hlp[MAX_BUF_LEN];
				if (LocalSearch::SearchHelpFile(IcadHelpData::getIcadHelpFileName(), hlp))
				{
					helpFileName = hlp;
				}
			}

			destroyCommandList();
			IcadHelp(helpFileName.c_str(), command, 0);
		}
		else
		{
			IIcadIOManager* pIO = getIOManagerInterface(getIcadInterfaces());
			//pIO->sendMessage(WM_MENUSELECT, 0, (LPARAM)(LPCTSTR)command);
			for (int i = 0; i != command.GetLength(); i++)
			{
				pIO->sendMessage(WM_CHAR, command[i], 0);
			}

			pIO->sendMessage(WM_CHAR, 13, 0); //enter
			destroyCommandList();
		}
	}
}

void CDynamicCommandListBox::OnMouseMove(UINT flags, CPoint point)
{
	BOOL bOutside = false;
	int item = HitTest(point);
	if (item >= 0 && item != GetCurSel())
	{
		//SetSelectionMark( item );
		SetItem(item, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
		SetCurSel(item);
		UpdateWindow();
	}
}

void CDynamicCommandListBox::OnTimer(UINT_PTR timerId)
{
	KillTimer(timerId);
	if (timerId == kTimerId)
	{

		m_pressedChars = L"";
	}
	// progesoft
	else if (timerId == kFillList)
	{
		getManager()->displayCommandList(true , true);
		getManager()->resizeControls();
	}
}

void CDynamicCommandListBox::OnChar(UINT nChar, UINT nRep, UINT flag)
{

}

void CDynamicCommandListBox::OnKeyUp(UINT nChar, UINT nRep, UINT flag)
{
	IIcadIOManager* pIO = getIOManagerInterface(getIcadInterfaces());
	int curSel = GetCurSel();
	if (nChar == VK_RETURN || nChar == VK_SPACE)
	{
		if (curSel >= 0)
		{
			CString text, command;
			text = GetItemText(curSel, 0);
			command = getCommandToSend(text, false);
			pIO->sendMessage(WM_MENUSELECT, 0, (LPARAM)(LPCTSTR)command);
			if (getManager())
				getManager()->destroyCommandList();
		}
		else
		{
			// none selected, send enter
			pIO->sendMessage(WM_MENUSELECT, 0, (LPARAM)(LPCTSTR)L"");
		}
	}
}

int CDynamicCommandListBox::GetCurSel()
{
	for (int i = 0; i != GetItemCount(); i++)
	{
		if (GetItemState(i, LVIS_SELECTED) & LVIS_SELECTED)
			return i;
	}
	return -1;
}

void CDynamicCommandListBox::SetCurSel(int index)
{
	SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
	EnsureVisible(index, FALSE);
}

int CDynamicCommandListBox::SelectString(const int startindex, const CString textToSelect)
{
	for (int i = startindex; i < GetItemCount(); i++)
	{
		if (GetItemText(i, 0) == textToSelect)
		{
			SetCurSel(i);
			return i;
		}
	}
	return -1;
}

void CDynamicCommandListBox::ResetContent()
{
	DeleteAllItems();
}

int CDynamicCommandListBox::AddString(CString str)
{
	return InsertItem(GetItemCount(), str);
}

void CDynamicCommandListBox::OnKeyDown(UINT nChar, UINT nRep, UINT flag)
{
	int i = 0;
	if (!m_hWnd)
		return;

	int curSel = GetCurSel();
	if (nChar == VK_DOWN)
	{
		if (curSel != GetItemCount() - 1)
		{
			SetCurSel(curSel + 1);
			CDynamicEdit *commandBox = m_pUiManager->getBoxContainer()->getCommandStringBox(true);
			CString command = GetItemText(curSel + 1, 0);
			int pos = command.Find(L'(');
			if (pos > 0)
			{
				command = command.Mid(pos + 1, command.GetLength() - pos - 2);
			}
			((CDynamicCommandStringEdit*)commandBox)->AutocompleteTextHighlighted(command.GetBuffer());
			//commandBox->setValue(command.GetBuffer());
		}
	}
	else if (nChar == VK_UP)
	{
		if (curSel > 0)
		{
			SetCurSel(curSel - 1);
			CDynamicEdit *commandBox = m_pUiManager->getBoxContainer()->getCommandStringBox(true);
			CString command = GetItemText(curSel - 1, 0);
			int pos = command.Find(L'(');
			if (pos > 0)
			{
				command = command.Mid(pos + 1, command.GetLength() - pos - 2);
			}
			((CDynamicCommandStringEdit*)commandBox)->AutocompleteTextHighlighted(command.GetBuffer());
			//commandBox->setValue(command.GetBuffer());
		}
	}
}

void CDynamicCommandListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	IIcadMainInternal* pMainManager = getMainInternalInterface(getIcadInterfaces());
	ASSERT(pMainManager != NULL);
	if (pMainManager == NULL)
		return;

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	// Save dc state
	int nSavedDC = pDC->SaveDC();

	CRect rcItem(lpDrawItemStruct->rcItem);
	int nItem = lpDrawItemStruct->itemID;

	// Get item image and state info
	LV_ITEM lvi;
	lvi.mask = LVIF_IMAGE | LVIF_STATE;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.stateMask = 0xFFFF;		// get all state flags
	GetItem(&lvi);

	// Should the item be highlighted
	BOOL bHighlight = (lvi.state & LVIS_DROPHILITED) || (lvi.state & LVIS_SELECTED);

	// Get rectangles for drawing
	CRect rcBounds, rcLabel, rcIcon;
	GetItemRect(nItem, rcBounds, LVIR_BOUNDS);
	GetItemRect(nItem, rcLabel, LVIR_LABEL);
	GetItemRect(nItem, rcIcon, LVIR_ICON);
	CRect rcCol(rcBounds);

	CString sLabel = GetItemText(nItem, 0);

	CFont *pFont = GetFont();
	LOGFONT lf;
	if (pFont)
	    pFont->GetLogFont(&lf);
	lf.lfHeight = CDynamicEdit::getFontSize();
	CFont newFont;
	newFont.CreateFontIndirectW(&lf);

	CFont *oldFont = NULL;
	if (pFont)
		oldFont = pDC->SelectObject(&newFont);

	// Labels are offset by a certain amount
	// This offset is related to the width of a space character
	int offset = pDC->GetTextExtent(_T(" "), 1).cx * 2;

	CRect rcHighlight;
	CRect rcWnd;
	//	int nExt;

	GetClientRect(&rcWnd);
	rcHighlight = rcBounds;
	rcHighlight.left = rcLabel.left;
	rcHighlight.right = rcWnd.right;

	// Draw the background color
	if (bHighlight)
	{
		pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
		pDC->FillRect(rcHighlight, &CBrush(::GetSysColor(COLOR_HIGHLIGHT)));
	}
	else
	{
		pDC->FillRect(rcHighlight, &CBrush(::GetSysColor(COLOR_WINDOW)));
	}

	// Set clip region
	rcCol.right = rcCol.left + GetColumnWidth(0);
	CRgn rgn;
	rgn.CreateRectRgnIndirect(&rcCol);
	pDC->SelectClipRgn(&rgn);
	rgn.DeleteObject();


	// Draw item label - Column 0
	//rcLabel.left += offset / 2;
	rcLabel.right -= offset;

	CString command = sLabel;
	int pos = command.Find(L'(');
	if (pos > 0)
	{
		command = command.Mid(pos + 1, command.GetLength() - pos - 2);
	}

	if(auto itemCommand = (ItemsCommand* )GetItemData(nItem))
	{
		auto pImages = CMFCToolBar::GetMenuImages();
		int iconId = -1;
		if (!itemCommand->m_isSysVar)
		{
			iconId = pMainManager->getCommandImageManager().GetCommandImageId(command);
		}

		if(iconId < 0)
		{
			pImages = &m_icons;
			iconId = GetIconIndexByType(itemCommand->m_isSysVar);
		}

		pImages->DrawEx(pDC, rcLabel, iconId);
	}

	rcLabel.left += rcLabel.Height();

	pDC->DrawText(sLabel, -1, rcLabel, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP
		| DT_VCENTER | DT_END_ELLIPSIS);

	CRect rRect(rcBounds);
	rRect.left = rcBounds.Width() - IcCuiSizes::Scale(17);
	m_icons.DrawEx(pDC, rRect, m_helpIconId);

	// Draw labels for remaining columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH;


	rcBounds.right = rcHighlight.right > rcBounds.right ? rcHighlight.right : rcBounds.right;
	rgn.CreateRectRgnIndirect(&rcBounds);
	pDC->SelectClipRgn(&rgn);

	// Draw focus rectangle if item has focus
	if (lvi.state & LVIS_FOCUSED && (GetFocus() == this))
		pDC->DrawFocusRect(rcHighlight);

	pDC->SelectObject(oldFont);

	// Restore dc
	pDC->RestoreDC(nSavedDC);
}

BOOL CDynamicCommandListBox::ShowWindow(int nCmdShow)
{
	if (IcadSharedGlobals::IsMainThreadCurrent())
		CWnd::ShowWindow(nCmdShow);
	else
		PostMessage(WM_SHOWWINDOW, (WPARAM)nCmdShow == SW_SHOW ? TRUE : FALSE, 0);
	return TRUE;
}

void CDynamicCommandListBox::postPoneShow()
{
	m_bPostPoneShow = true;
}

void CDynamicCommandListBox::ShowIfRequired()
{
	if (m_bPostPoneShow)
		Show();

	UpdateWindow();
	RedrawWindow();
	SetCurSel(GetCurSel());
	m_bPostPoneShow = false;
}

bool CDynamicCommandListBox::ShowIsRequired()
{
	return m_bPostPoneShow;
}

bool CDynamicCommandListBox::MessageHandler(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_hWnd && IsWindowVisible())
	{
		if (message == WM_KEYDOWN &&
			(wParam == VK_UP || wParam == VK_DOWN))
		{
			SendMessage(WM_KEYDOWN, wParam, lParam);
			wchar_t ch = vkToChar((UINT)wParam, (UINT)lParam);
			if (ch)
				SendMessage(WM_CHAR, ch, lParam);
			SendMessage(WM_KEYUP, wParam, lParam);
			return true;
		}
	}
	return false;
}

void CDynamicCommandListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	TEXTMETRIC tm;
	HDC hDC = ::GetDC(NULL);
	CFont* pFont = GetFont();
	HFONT hFontOld = NULL;
	CFont newFont;
	if (pFont)
	{
		LOGFONT lf = { 0 };
		pFont->GetLogFont(&lf);
		lf.lfHeight = CDynamicEdit::getFontSize();
		newFont.CreateFontIndirectW(&lf);
		hFontOld = (HFONT)SelectObject(hDC, newFont.GetSafeHandle());
	}
	GetTextMetrics(hDC, &tm);
	lpMeasureItemStruct->itemHeight = tm.tmHeight + tm.tmExternalLeading + 5;
	SelectObject(hDC, hFontOld);
	::ReleaseDC(NULL, hDC);
}

constexpr int CDynamicCommandListBox::GetIconIndexByType(bool bIsSysvar)
{
	return bIsSysvar ? 0 : 1;
}

IMPLEMENT_DYNAMIC(CDynamicPrompt,CDynamicStringPromptEdit)
BEGIN_MESSAGE_MAP(CDynamicPrompt,CDynamicStringPromptEdit)
	ON_WM_TIMER()
END_MESSAGE_MAP()

CDynamicPrompt::CDynamicPrompt():m_bNeedUpdatePrompt(false)
{
}

void CDynamicPrompt::setValue(const OdString val)
{
	m_text = val.c_str();
	m_bNeedUpdatePrompt = true;
	SetTimer(kTimerId, kDefaultValueOffset, NULL);
}

void CDynamicPrompt::OnTimer(UINT_PTR timerId)
{
	if (m_bNeedUpdatePrompt)
	{
		if (timerId == kTimerId)
		{
			KillTimer(timerId);
			m_bNeedUpdatePrompt = false;

			if (!m_text.IsEmpty() && m_text[0] == L'\n')
			{
				m_text.Right(m_text.GetLength()-1);
			}
			OdString val = m_text;
			getManager()->getOptionListAndDefault(val, !IcadSharedGlobals::IsMainThreadCurrent());
			SetWindowTextW((LPCTSTR)val);
			m_bDisplayDownArrow = true;
		}
	}
}

void CDynamicPrompt::SetWindowTextW(LPCTSTR lpszString)
{
	m_text = lpszString;
	if (IcadSharedGlobals::IsMainThreadCurrent())
		CEdit::SetWindowText(lpszString);
	else
		PostMessage(WM_CUSTOMSETTEXT, 0, (LPARAM)(LPCTSTR)m_text);

	m_bSizeInvalid = true;
}
